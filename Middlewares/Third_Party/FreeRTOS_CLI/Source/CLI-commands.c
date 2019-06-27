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
#include "usbpd.h"
#include <stdlib.h>

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
 * Implements the cal command.
 */
static BaseType_t prvCalibrateCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

/*
 * Implements the write_otp command.
 */
static BaseType_t prvWriteOTPFlashCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

/*
 * Implements the run-time-stats command.
 */
#if( configGENERATE_RUN_TIME_STATS == 1 )
	static BaseType_t prvRunTimeStatsCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
#endif /* configGENERATE_RUN_TIME_STATS */

/* Structure that defines the "stats" command line command. */
static const CLI_Command_Definition_t xStats =
{
	"stats", /* The command string to type. */
	"\r\nstats:\r\n Displays a table showing the system stats\r\n",
	prvStatsCommand, /* The function to run. */
	0 /* No parameters are expected. */
};

/* Structure that defines the "cal" command line command. */
static const CLI_Command_Definition_t xCal =
{
	"cal", /* The command string to type. */
	"\r\ncal:\r\n Calibrates the ADC based on a known input voltage. Expects one argument as a float in milivolts. Connect input voltage to cells 1-4 and the XT60 battery output.\r\n",
	prvCalibrateCommand, /* The function to run. */
	1 /* One parameter are expected. */
};

/* Structure that defines the "write_otp_scalars_to_flash" command line command. */
static const CLI_Command_Definition_t xOTP =
{
	"write_otp", /* The command string to type. */
	"\r\nwrite_otp:\r\n Writes the calibration scalars to OTP flash. Will fail if scalars not set or out of range. Must run cal first with known accurate voltage. Can run up to ~32 times.\r\n",
	prvWriteOTPFlashCommand, /* The function to run. */
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

	FreeRTOS_CLIRegisterCommand(&xCal);

	FreeRTOS_CLIRegisterCommand(&xOTP);

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

	float cell_voltage_float[4];
	for (int i = 0; i < 4; i++) {
		cell_voltage_float[i] = ((float)Get_Cell_Voltage(i)/BATTERY_ADC_MULTIPLIER);
	}

	float vdda_float = (float)Get_VDDa()/BATTERY_ADC_MULTIPLIER;

	float battery_voltage = ((float)Get_Battery_Voltage()/BATTERY_ADC_MULTIPLIER);
	float charge_current = ((float)Get_Charge_Current_ADC_Reading()/REG_ADC_MULTIPLIER);
	float output_power = battery_voltage * charge_current;

	float regulator_vbat_voltage = ((float)Get_VBAT_ADC_Reading()/REG_ADC_MULTIPLIER);
	float vbus_voltage = ((float)Get_VBUS_ADC_Reading()/REG_ADC_MULTIPLIER);
	float input_current = ((float)Get_Input_Current_ADC_Reading()/REG_ADC_MULTIPLIER);
	float input_power = vbus_voltage * input_current;

	float efficiency = output_power/input_power;

	float max_charge_current = (float)Get_Max_Charge_Current()/1000.0f;

	/* Generate a table of stats. */
	sprintf(pcWriteBuffer,
			"Variable                    Value\r\n"
			"************************************************\r\n"
			"Battery Voltage MCU(V)       %.3f\r\n"
			"Battery Voltage Reg (V)      %.3f\r\n"
			"Charging Current (A)         %.3f\r\n"
			"Charging Power (W)           %.3f\r\n"
			"Cell One Voltage (V)         %.3f\r\n"
			"Cell Two Voltage (V)         %.3f\r\n"
			"Cell Three Voltage (V)       %.3f\r\n"
			"Cell Four Voltage (V)        %.3f\r\n"
			"2 Series Voltage (V)         %.3f\r\n"
			"3 Series Voltage (V)         %.3f\r\n"
			"4 Series Voltage (V)         %.3f\r\n"
			"MCU Temperature (C)          %d\r\n"
			"VDDa (V)                     %.3f\r\n"
			"XT60 Connected               %u\r\n"
			"Balance Connection State     %u\r\n"
			"Number of Cells              %u\r\n"
			"Battery Requires Charging    %u\r\n"
			"Balancing State/Bitmask      %b\r\n"
			"Regulator Connection State   %d\r\n"
			"Charging State               %u\r\n"
			"Max Charge Current           %.3f\r\n"
			"Vbus Voltage (V)             %.3f\r\n"
			"Input Current (A)            %.3f\r\n"
			"Input Power (W)              %.3f\r\n"
			"Efficiency (OutputW/InputW)  %.3f\r\n"
			"Battery Error State          %u\r\n",
			battery_voltage,
			regulator_vbat_voltage,
			charge_current,
			output_power,
			cell_voltage_float[0],
			cell_voltage_float[1],
			cell_voltage_float[2],
			cell_voltage_float[3],
			(float)Get_Two_S_Voltage()/BATTERY_ADC_MULTIPLIER,
			(float)Get_Three_S_Voltage()/BATTERY_ADC_MULTIPLIER,
			(float)Get_Four_S_Voltage()/BATTERY_ADC_MULTIPLIER,
			Get_MCU_Temperature(),
			vdda_float,
			Get_XT60_Connection_State(),
			Get_Balance_Connection_State(),
			Get_Number_Of_Cells(),
			Get_Requires_Charging_State(),
			Get_Balancing_State(),
			Get_Regulator_Connection_State(),
			Get_Regulator_Charging_State(),
			max_charge_current,
			vbus_voltage,
			input_current,
			input_power,
			efficiency,
			Get_Error_State());

	/* There is no more data to return after this single string, so return
	 pdFALSE. */
	return pdFALSE;
}
/*-----------------------------------------------------------*/

static BaseType_t prvCalibrateCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	/* Remove compile time warnings about unused parameters, and check the
	 write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	 write buffer length is adequate, so does not check for buffer overflows. */
	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	const char *pcParameter1;
	BaseType_t xParameter1StringLength;

    pcParameter1 = FreeRTOS_CLIGetParameter
                        (
                          /* The command string itself. */
                          pcCommandString,
                          /* Return the first parameter. */
                          1,
                          /* Store the parameter string length. */
                          &xParameter1StringLength
                        );

	float input_voltage_mv = strtof(pcParameter1, NULL);

	/* Print the ADC reading. */
	uint8_t result = Calibrate_ADC(input_voltage_mv);

	sprintf(pcWriteBuffer, "Calibration Result: %u\r\n", result);

	/* There is no more data to return after this single string, so return
	 pdFALSE. */
	return pdFALSE;
}
/*-----------------------------------------------------------*/

static BaseType_t prvWriteOTPFlashCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	/* Remove compile time warnings about unused parameters, and check the
	 write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	 write buffer length is adequate, so does not check for buffer overflows. */
	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	/* Print the result */
	uint8_t result = Write_Cal_To_OTP_Flash();

	sprintf(pcWriteBuffer, "OTP Write Result: %u\r\n", result);

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
