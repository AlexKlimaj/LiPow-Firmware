/**
 ******************************************************************************
 * @file           : battery.c
 * @brief          : Handles battery state information
 ******************************************************************************
 */


#include "battery.h"

#include "FreeRTOS.h"
#include "task.h"

/* Private typedef -----------------------------------------------------------*/
struct Battery {
	uint8_t xt60_connected;
	uint8_t balance_port_connected;
	uint8_t number_of_cells;
	uint8_t balancing_enabled;
	uint8_t charging_enabled;
};

/* Private variables ---------------------------------------------------------*/
TaskHandle_t batteryTaskHandle = NULL;
struct Battery battery_state;

/* Private function prototypes -----------------------------------------------*/
void vBattery_State(void *pvParameters);

/**
 * @brief Creates the battery task
 */
void vCreateBatteryTask( void )
{
	/* Create the task, storing the handle. */
	xTaskCreate(vBattery_State, /* Function that implements the task. */
		(const char* const ) "battery", /* Text name for the task. */
		vBattery_State_STACK_SIZE, /* Stack size in words, not bytes. */
		0, /* Parameter passed into the task. */
		BATTERY_TASK_PRIORITY, /* Priority at which the task is created. */
		&batteryTaskHandle); /* Used to pass out the created task's handle. */
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

void vBattery_State(void *pvParameters)
{

}
