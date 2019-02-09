/**
 ******************************************************************************
 * @file           : battery.c
 * @brief          : Handles battery state information
 ******************************************************************************
 */


#include "battery.h"

#include "adc_interface.hpp"
#include "task.h"

/* Private typedef -----------------------------------------------------------*/
struct Battery {
	uint8_t xt60_connected;
	uint8_t balance_port_connected;
	uint8_t number_of_cells;
	uint8_t balancing_enabled;
	uint8_t charging_enabled;
	uint8_t battery_error_state;
};

/* Private variables ---------------------------------------------------------*/
struct Battery battery_state;

uint8_t cell_connected_bitmask = 0;

/* Private function prototypes -----------------------------------------------*/
void vBattery_Connection_State(void *pvParameters);

/**
 * @brief Creates the battery task
 */
void vCreateBatteryTask( uint16_t usStackSize, unsigned portBASE_TYPE uxPriority )
{
	/* Create the task, storing the handle. */
	xTaskCreate(vBattery_Connection_State, /* Function that implements the task. */
		(const char* const ) "battery connection", /* Text name for the task. */
		usStackSize, /* Stack size in words, not bytes. */
		0, /* Parameter passed into the task. */
		uxPriority, /* Priority at which the task is created. */
		0); /* Used to pass out the created task's handle. */
}

uint8_t Get_XT60_Connection_State(void)
{
	return battery_state.xt60_connected;
}

uint8_t Get_Balance_Connection_State(void)
{
	return battery_state.balance_port_connected;
}

uint8_t Get_Number_Of_Cells(void)
{
	return battery_state.number_of_cells;
}

uint8_t Get_Balancing_State(void)
{
	return battery_state.balancing_enabled;
}

uint8_t Get_Charging_State(void)
{
	return battery_state.charging_enabled;
}

void vBattery_Connection_State(void *pvParameters)
{
	TickType_t xDelay = 100 / portTICK_PERIOD_MS;

	for (;;) {
		if ( Get_Battery_Voltage() > VOLTAGE_CONNECTED_THRESHOLD ) {
			battery_state.xt60_connected = CONNETED;
		}
		else {
			battery_state.xt60_connected = NOT_CONNECTED;
		}

		if ( Get_Cell_Four_Voltage() > VOLTAGE_CONNECTED_THRESHOLD ) {
			cell_connected_bitmask = 0b1000;
		}
		if ( Get_Cell_Three_Voltage() > VOLTAGE_CONNECTED_THRESHOLD ) {
			cell_connected_bitmask |= 0b0100;
		}
		if ( Get_Cell_Two_Voltage() > VOLTAGE_CONNECTED_THRESHOLD ) {
			cell_connected_bitmask |= 0b0010;
		}
		if ( Get_Cell_One_Voltage() > VOLTAGE_CONNECTED_THRESHOLD ) {
			cell_connected_bitmask |= 0b0001;
		}

		if ( cell_connected_bitmask & (1<<4) ) {
			if ( cell_connected_bitmask & THREE_S_BITMASK ) {
				battery_state.number_of_cells = 4;
				battery_state.battery_error_state = 0;
			}
			else {
				battery_state.number_of_cells = 0;
				battery_state.battery_error_state = CELL_CONNECTION_ERROR;
			}
		}
		else if ( cell_connected_bitmask & (1<<3) ) {
			if ( cell_connected_bitmask & TWO_S_BITMASK ) {
				battery_state.number_of_cells = 3;
				battery_state.battery_error_state = 0;
			}
			else {
				battery_state.number_of_cells = 0;
				battery_state.battery_error_state = CELL_CONNECTION_ERROR;
			}
		}
		else if ( cell_connected_bitmask & (1<<2) ) {
			if ( cell_connected_bitmask & ONE_S_BITMASK ) {
				battery_state.number_of_cells = 2;
				battery_state.battery_error_state = 0;
			}
			else {
				battery_state.number_of_cells = 0;
				battery_state.battery_error_state = CELL_CONNECTION_ERROR;
			}
		}
		else {
			battery_state.number_of_cells = 0;
		}

		if ( battery_state.number_of_cells > 1 ) {
			battery_state.balance_port_connected = CONNETED;
		}
		else {
			battery_state.balance_port_connected = NOT_CONNECTED;
		}

		vTaskDelay(xDelay);
	}
}
