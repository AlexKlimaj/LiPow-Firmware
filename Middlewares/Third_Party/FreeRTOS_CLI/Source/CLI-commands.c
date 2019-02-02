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

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+CLI includes. */
#include "FreeRTOS_CLI.h"

#include "string.h"
#include "printf.h"

#include "adc_interface.h"
#include "UARTCommandConsole.h"

/*
 * Defines a command that returns a table showing the state of each task at the
 * time the command is called.
 */
static BaseType_t prvTaskStatsCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

/* Structure that defines the "task-stats" command line command. */
static const CLI_Command_Definition_t xTaskStats = { "stats", /* The command string to type. */
"\r\nstats:\r\n Displays a table showing the system stats\r\n\r\n", prvTaskStatsCommand, /* The function to run. */
0 /* No parameters are expected. */
};

/*-----------------------------------------------------------*/

void vRegisterCLICommands(void) {
	/* Register all the command line commands defined immediately above. */
	FreeRTOS_CLIRegisterCommand(&xTaskStats);
}
/*-----------------------------------------------------------*/

static BaseType_t prvTaskStatsCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	/* Remove compile time warnings about unused parameters, and check the
	 write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	 write buffer length is adequate, so does not check for buffer overflows. */
	(void) pcCommandString;
	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	/* Generate a table of stats. */
	sprintf(pcWriteBuffer, "Variable                Value\r\n************************************************\r\n"
			"Battery Voltage (V)     %f\r\n"
			"Cell One Voltage (V)    %f\r\n"
			"Cell Two Voltage (V)    %f\r\n"
			"Cell Three Voltage (V)  %f\r\n"
			"Cell Four Voltage (V)   %f\r\n"
			"MCU Temperature (C)     %f\r\n", Get_Battery_Voltage(), Get_Cell_One_Voltage(), Get_Cell_Two_Voltage(), Get_Cell_Three_Voltage(), Get_Cell_Four_Voltage(), Get_MCU_Temperature());

	/* There is no more data to return after this single string, so return
	 pdFALSE. */
	return pdFALSE;
}
/*-----------------------------------------------------------*/
