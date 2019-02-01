/*
 * FreeRTOS Kernel V10.0.1
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* Standard includes. */
#include "string.h"
#include "stdio.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "FreeRTOS_CLI.h"
#include "UARTCommandConsole.h"

/* Dimensions the buffer into which input characters are placed. */
#define cmdMAX_INPUT_SIZE		50

/* DEL acts as a backspace. */
#define cmdASCII_DEL		( 0x7F )

/* The maximum time to wait for the mutex that guards the UART to become
available. */
#define cmdMAX_MUTEX_WAIT		pdMS_TO_TICKS( 300 )

/* Flag to indicate if DMA transfer has completed */
uint8_t uart_tx_ready = 0;

/*-----------------------------------------------------------*/

/*
 * The task that implements the command console processing.
 */
static void prvUARTCommandConsoleTask( void *pvParameters );
void vUARTCommandConsoleStart( uint16_t usStackSize, UBaseType_t uxPriority );

/*-----------------------------------------------------------*/

/* Const messages output by the command console. */
static const char * const pcWelcomeMessage = "\r\nStarting LiPow.\r\nType Help to view a list of registered commands.\r\n\r\n>";
static const char * const pcEndOfOutputMessage = "\r\n[Press ENTER to execute the previous command again]\r\n>";
static const char * const pcNewLine = "\r\n";

/* Used to guard access to the UART in case messages are sent to the UART from
more than one task. */
static SemaphoreHandle_t xTxMutex = NULL;

/* The handle to the UART port, which is not used by all ports. */
extern UART_HandleTypeDef huart1;

/*-----------------------------------------------------------*/

void vUARTCommandConsoleStart( uint16_t usStackSize, UBaseType_t uxPriority )
{
	/* Create the semaphore used to access the UART Tx. */
	xTxMutex = xSemaphoreCreateMutex();
	configASSERT( xTxMutex );

	/* Create that task that handles the console itself. */
	xTaskCreate( 	prvUARTCommandConsoleTask,	/* The task that implements the command console. */
					"CLI",						/* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
					usStackSize,				/* The size of the stack allocated to the task. */
					NULL,						/* The parameter is not used, so NULL is passed. */
					uxPriority,					/* The priority allocated to the task. */
					NULL );						/* A handle is not required, so just pass NULL. */
}
/*-----------------------------------------------------------*/

static void prvUARTCommandConsoleTask( void *pvParameters )
{
signed char cRxedChar = '\0';
uint8_t ucInputIndex = 0;
char *pcOutputString;
static char cInputString[ cmdMAX_INPUT_SIZE ], cLastInputString[ cmdMAX_INPUT_SIZE ];
BaseType_t xReturned;

	( void ) pvParameters;

	/* Obtain the address of the output buffer.  Note there is no mutual
	exclusion on this buffer as it is assumed only one command console interface
	will be used at any one time. */
	pcOutputString = FreeRTOS_CLIGetOutputBuffer();

	/* Send the welcome message. */
	UART_DMA_Transfer(&huart1, (uint8_t *)pcWelcomeMessage, ( unsigned short ) strlen(pcWelcomeMessage));

	for( ;; )
	{
		/* Wait for the next character.  The while loop is used in case
		INCLUDE_vTaskSuspend is not set to 1 - in which case portMAX_DELAY will
		be a genuine block time rather than an infinite block time. */
		//while( xSerialGetChar( xPort, &cRxedChar, portMAX_DELAY ) != pdPASS );
		while(HAL_UART_Receive_DMA(&huart1, (uint8_t *)&cRxedChar, 1) != HAL_OK);

		/* Ensure exclusive access to the UART Tx. */
		if( xSemaphoreTake( xTxMutex, cmdMAX_MUTEX_WAIT ) == pdPASS )
		{
			/* Echo the character back. */
			//xSerialPutChar( xPort, cRxedChar, portMAX_DELAY );
			UART_DMA_Transfer(&huart1, (uint8_t *)&cRxedChar, 1);

			/* Was it the end of the line? */
			if( cRxedChar == '\n' || cRxedChar == '\r' )
			{
				/* Just to space the output from the input. */
				UART_DMA_Transfer(&huart1, (uint8_t *)pcNewLine, ( unsigned short ) strlen(pcNewLine));

				/* See if the command is empty, indicating that the last command
				is to be executed again. */
				if( ucInputIndex == 0 )
				{
					/* Copy the last command back into the input string. */
					strcpy( cInputString, cLastInputString );
				}

				/* Pass the received command to the command interpreter.  The
				command interpreter is called repeatedly until it returns
				pdFALSE	(indicating there is no more output) as it might
				generate more than one string. */
				do
				{
					/* Get the next output string from the command interpreter. */
					xReturned = FreeRTOS_CLIProcessCommand( cInputString, pcOutputString, configCOMMAND_INT_MAX_OUTPUT_SIZE );

					/* Write the generated string to the UART. */

					UART_DMA_Transfer(&huart1, (uint8_t *)pcOutputString, ( unsigned short ) strlen(pcOutputString));

				} while( xReturned != pdFALSE );

				/* All the strings generated by the input command have been
				sent.  Clear the input string ready to receive the next command.
				Remember the command that was just processed first in case it is
				to be processed again. */
				strcpy( cLastInputString, cInputString );
				ucInputIndex = 0;
				memset( cInputString, 0x00, cmdMAX_INPUT_SIZE );

				UART_DMA_Transfer(&huart1, (uint8_t *)pcEndOfOutputMessage, ( unsigned short ) strlen(pcEndOfOutputMessage));
			}
			else
			{
				if( cRxedChar == '\r' )
				{
					/* Ignore the character. */
				}
				else if( ( cRxedChar == '\b' ) || ( cRxedChar == cmdASCII_DEL ) )
				{
					/* Backspace was pressed.  Erase the last character in the
					string - if any. */
					if( ucInputIndex > 0 )
					{
						ucInputIndex--;
						cInputString[ ucInputIndex ] = '\0';
					}
				}
				else
				{
					/* A character was entered.  Add it to the string entered so
					far.  When a \n is entered the complete	string will be
					passed to the command interpreter. */
					if( ( cRxedChar >= ' ' ) && ( cRxedChar <= '~' ) )
					{
						if( ucInputIndex < cmdMAX_INPUT_SIZE )
						{
							cInputString[ ucInputIndex ] = cRxedChar;
							ucInputIndex++;
						}
					}
				}
			}

			/* Must ensure to give the mutex back. */
			xSemaphoreGive( xTxMutex );
		}
	}
}
/*-----------------------------------------------------------*/

void vOutputString( const char * const pcMessage )
{
	if( xSemaphoreTake( xTxMutex, cmdMAX_MUTEX_WAIT ) == pdPASS )
	{
		while(HAL_UART_Transmit_DMA(&huart1, (uint8_t *)pcMessage, ( unsigned short ) strlen(pcMessage)) != HAL_OK);
		xSemaphoreGive( xTxMutex );
	}
}
/*-----------------------------------------------------------*/

void UART_DMA_Transfer(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
	//Set the flag to not ready
	uart_tx_ready = 0;
	while(HAL_UART_Transmit_DMA(huart, pData, Size) != HAL_OK);
	// Wait for the transfer to finish
	while(uart_tx_ready != 1);
}
/*-----------------------------------------------------------*/

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
    	uart_tx_ready = 1;
    }
}

