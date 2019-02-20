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
#include "battery.h"
#include "bq25703a_regulator.h"
#include "error.h"
#include "UARTCommandConsole.h"

/*
 * Defines a command that returns a table showing the state of each task at the
 * time the command is called.
 */
static BaseType_t prvStatsCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

/*
 * Implements the task-stats command.
 */
static BaseType_t prvTaskStatsCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

/*
 * Implements the run-time-stats command.
 */
#if( configGENERATE_RUN_TIME_STATS == 1 )
	static BaseType_t prvRunTimeStatsCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
#endif /* configGENERATE_RUN_TIME_STATS */

/* Structure that defines the "task-stats" command line command. */
static const CLI_Command_Definition_t xStats =
{
	"stats", /* The command string to type. */
	"\r\nstats:\r\n Displays a table showing the system stats\r\n",
	prvStatsCommand, /* The function to run. */
	0 /* No parameters are expected. */
};

/* Structure that defines the "task-stats" command line command.  This generates
a table that gives information on each task in the system. */
static const CLI_Command_Definition_t xTaskStats =
{
	"task-stats", /* The command string to type. */
	"\r\ntask-stats:\r\n Displays a table showing the state of each FreeRTOS task\r\n",
	prvTaskStatsCommand, /* The function to run. */
	0 /* No parameters are expected. */
};

#if( configGENERATE_RUN_TIME_STATS == 1 )
	/* Structure that defines the "run-time-stats" command line command.   This
	generates a table that shows how much run time each task has */
	static const CLI_Command_Definition_t xRunTimeStats =
	{
		"run-time-stats", /* The command string to type. */
		"\r\nrun-time-stats:\r\n Displays a table showing how much processing time each FreeRTOS task has used\r\n",
		prvRunTimeStatsCommand, /* The function to run. */
		0 /* No parameters are expected. */
	};
#endif /* configGENERATE_RUN_TIME_STATS */

/*-----------------------------------------------------------*/

void vRegisterCLICommands(void) {
	/* Register all the command line commands defined immediately above. */
	FreeRTOS_CLIRegisterCommand(&xStats);
	FreeRTOS_CLIRegisterCommand(&xTaskStats);

	#if( configGENERATE_RUN_TIME_STATS == 1 )
	{
		FreeRTOS_CLIRegisterCommand( &xRunTimeStats );
	}
	#endif
}
/*-----------------------------------------------------------*/

static BaseType_t prvStatsCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	/* Remove compile time warnings about unused parameters, and check the
	 write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	 write buffer length is adequate, so does not check for buffer overflows. */
	(void) pcCommandString;
	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	float battery_voltage_float = ((float)Get_Battery_Voltage()/BATTERY_ADC_MULTIPLIER);

	float cell_voltage_float[4];
	for (int i = 0; i < 4; i++) {
		cell_voltage_float[i] = ((float)Get_Cell_Voltage(i)/BATTERY_ADC_MULTIPLIER);
	}

	float vdda_float = (float)Get_VDDa()/BATTERY_ADC_MULTIPLIER;

	/* Generate a table of stats. */
	sprintf(pcWriteBuffer,
			"Variable                    Value\r\n"
			"************************************************\r\n"
			"Battery Voltage (V)         %f\r\n"
			"Cell One Voltage (V)        %f\r\n"
			"Cell Two Voltage (V)        %f\r\n"
			"Cell Three Voltage (V)      %f\r\n"
			"Cell Four Voltage (V)       %f\r\n"
			"MCU Temperature (C)         %d\r\n"
			"VDDa (V)                    %f\r\n"
			"XT60 Connected              %u\r\n"
			"Balance Connection State    %u\r\n"
			"Number of Cells             %u\r\n"
			"Balancing State             %u\r\n"
			"Regulator Connection State  %d\r\n"
			"Charging State              %u\r\n"
			"Vbus Voltage                %f\r\n"
			"Psys Voltage                %f\r\n"
			"Battery Error State         %u\r\n",
			battery_voltage_float,
			cell_voltage_float[0],
			cell_voltage_float[1],
			cell_voltage_float[2],
			cell_voltage_float[3],
			Get_MCU_Temperature(),
			vdda_float,
			Get_XT60_Connection_State(),
			Get_Balance_Connection_State(),
			Get_Number_Of_Cells(),
			Get_Balancing_State(),
			Get_Regulator_Connection_State(),
			Get_Regulator_Charging_State(),
			((float)Get_VBUS_ADC_Reading()/BATTERY_ADC_MULTIPLIER),
			((float)Get_PSYS_ADC_Reading()/BATTERY_ADC_MULTIPLIER),
			Get_Error_State());

	/* There is no more data to return after this single string, so return
	 pdFALSE. */
	return pdFALSE;
}
/*-----------------------------------------------------------*/

static BaseType_t prvTaskStatsCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
const char *const pcHeader = "State   Priority  Stack    #\r\n************************************************\r\n";
BaseType_t xSpacePadding;

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	/* Generate a table of task stats. */
	strcpy( pcWriteBuffer, "Task" );
	pcWriteBuffer += strlen( pcWriteBuffer );

	/* Minus three for the null terminator and half the number of characters in
	"Task" so the column lines up with the centre of the heading. */
	configASSERT( configMAX_TASK_NAME_LEN > 3 );
	for( xSpacePadding = strlen( "Task" ); xSpacePadding < ( configMAX_TASK_NAME_LEN - 3 ); xSpacePadding++ )
	{
		/* Add a space to align columns after the task's name. */
		*pcWriteBuffer = ' ';
		pcWriteBuffer++;

		/* Ensure always terminated. */
		*pcWriteBuffer = 0x00;
	}
	strcpy( pcWriteBuffer, pcHeader );
	vTaskList( pcWriteBuffer + strlen( pcHeader ) );

	/* There is no more data to return after this single string, so return
	pdFALSE. */
	return pdFALSE;
}
/*-----------------------------------------------------------*/

#if( configGENERATE_RUN_TIME_STATS == 1 )

	static BaseType_t prvRunTimeStatsCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
	{
	const char * const pcHeader = "  Abs Time      % Time\r\n****************************************\r\n";
	BaseType_t xSpacePadding;

		/* Remove compile time warnings about unused parameters, and check the
		write buffer is not NULL.  NOTE - for simplicity, this example assumes the
		write buffer length is adequate, so does not check for buffer overflows. */
		( void ) pcCommandString;
		( void ) xWriteBufferLen;
		configASSERT( pcWriteBuffer );

		/* Generate a table of task stats. */
		strcpy( pcWriteBuffer, "Task" );
		pcWriteBuffer += strlen( pcWriteBuffer );

		/* Pad the string "task" with however many bytes necessary to make it the
		length of a task name.  Minus three for the null terminator and half the
		number of characters in	"Task" so the column lines up with the centre of
		the heading. */
		for( xSpacePadding = strlen( "Task" ); xSpacePadding < ( configMAX_TASK_NAME_LEN - 3 ); xSpacePadding++ )
		{
			/* Add a space to align columns after the task's name. */
			*pcWriteBuffer = ' ';
			pcWriteBuffer++;

			/* Ensure always terminated. */
			*pcWriteBuffer = 0x00;
		}

		strcpy( pcWriteBuffer, pcHeader );
		vTaskGetRunTimeStats( pcWriteBuffer + strlen( pcHeader ) );

		/* There is no more data to return after this single string, so return
		pdFALSE. */
		return pdFALSE;
	}

#endif /* configGENERATE_RUN_TIME_STATS */
/*-----------------------------------------------------------*/
