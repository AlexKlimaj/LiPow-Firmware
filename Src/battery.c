/**
 ******************************************************************************
 * @file           : battery.c
 * @brief          : Handles battery state information
 ******************************************************************************
 */


#include "battery.h"

#include "adc_interface.h"
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

static uint8_t cell_connected_bitmask = 0;

/* Private function prototypes -----------------------------------------------*/
void Balance_Battery(void);
void Balance_Connection_State(void);
void Balancing_GPIO_Control(uint8_t cell_balancing_gpio_bitmask);
void MCU_Temperature_Safety_Check(void);

/**
 * @brief Based on ADC readings, determine if balancing is needed, if so, balance battery
 */
void Balance_Battery()
{
	if ( (battery_state.balance_port_connected == CONNECTED) && (battery_state.battery_error_state == 0) ) {

		uint32_t min_cell_voltage = Get_Cell_Voltage(0);
		uint32_t max_cell_voltage = Get_Cell_Voltage(0);
		for(int i = 1; i < battery_state.number_of_cells; i++) {
			if (Get_Cell_Voltage(i) < min_cell_voltage) {
				min_cell_voltage = Get_Cell_Voltage(i);
			}
			if (Get_Cell_Voltage(i) > max_cell_voltage) {
				max_cell_voltage = Get_Cell_Voltage(i);
			}
		}

		if ( ((max_cell_voltage - min_cell_voltage) > CELL_DELTA_V_ENABLE_BALANCING) && (min_cell_voltage > MIN_CELL_V_FOR_BALANCING) && (battery_state.balancing_enabled == 0)) {
			battery_state.balancing_enabled = 1;
		}
		else if ( (((max_cell_voltage - min_cell_voltage) < (CELL_DELTA_V_ENABLE_BALANCING - CELL_BALANCING_HYSTERESIS_V)) && (battery_state.balancing_enabled == 1)) || (min_cell_voltage < MIN_CELL_V_FOR_BALANCING) ) {
			battery_state.balancing_enabled = 0;
		}

		if (battery_state.balancing_enabled == 1) {

			uint8_t cell_balance_bitmask = 0;
			for(int i = 1; i < battery_state.number_of_cells; i++) {
				if ( (Get_Cell_Voltage(i) - min_cell_voltage) > (CELL_DELTA_V_ENABLE_BALANCING - CELL_BALANCING_HYSTERESIS_V)) {
					cell_balance_bitmask |= (1<<i);
				}
				else {
					cell_balance_bitmask &= ~(1<<i);
				}
			}
			Balancing_GPIO_Control(cell_balance_bitmask);
		}
		else {
			Balancing_GPIO_Control(0);
		}
	}
	else {
		Balancing_GPIO_Control(0);
		battery_state.balancing_enabled = 0;
	}
}

/**
 * @brief Determines the state of the balance connection based on ADC readings
 */
void Balance_Connection_State()
{
	if (( Get_Four_S_Voltage() > VOLTAGE_CONNECTED_THRESHOLD ) && ( Get_Cell_Voltage(3) > VOLTAGE_CONNECTED_THRESHOLD )) {
		cell_connected_bitmask |= 0b1000;
	}
	else {
		cell_connected_bitmask &= ~0b1000;
	}
	if (( Get_Three_S_Voltage() > VOLTAGE_CONNECTED_THRESHOLD ) && ( Get_Cell_Voltage(2) > VOLTAGE_CONNECTED_THRESHOLD )) {
		cell_connected_bitmask |= 0b0100;
	}
	else {
		cell_connected_bitmask &= ~0b0100;
	}
	if (( Get_Two_S_Voltage() > VOLTAGE_CONNECTED_THRESHOLD ) && ( Get_Cell_Voltage(1) > VOLTAGE_CONNECTED_THRESHOLD )) {
		cell_connected_bitmask |= 0b0010;
	}
	else {
		cell_connected_bitmask &= ~0b0010;
	}
	if ( Get_Cell_Voltage(0) > VOLTAGE_CONNECTED_THRESHOLD ) {
		cell_connected_bitmask |= 0b0001;
	}
	else {
		cell_connected_bitmask &= ~0b0001;
	}

	if ( cell_connected_bitmask & (1<<3) ) {
		if ( (cell_connected_bitmask & THREE_S_BITMASK) == THREE_S_BITMASK ) {
			battery_state.number_of_cells = 4;
			battery_state.battery_error_state &= ~CELL_CONNECTION_ERROR;
		}
		else {
			battery_state.number_of_cells = 0;
			battery_state.battery_error_state |= CELL_CONNECTION_ERROR;
		}
	}
	else if ( cell_connected_bitmask & (1<<2) ) {
		if ( (cell_connected_bitmask & TWO_S_BITMASK) ==  TWO_S_BITMASK ) {
			battery_state.number_of_cells = 3;
			battery_state.battery_error_state &= ~CELL_CONNECTION_ERROR;
		}
		else {
			battery_state.number_of_cells = 0;
			battery_state.battery_error_state |= CELL_CONNECTION_ERROR;
		}
	}
	else if ( cell_connected_bitmask & (1<<1) ) {
		if ( (cell_connected_bitmask & ONE_S_BITMASK) == ONE_S_BITMASK ) {
			battery_state.number_of_cells = 2;
			battery_state.battery_error_state &= ~CELL_CONNECTION_ERROR;
		}
		else {
			battery_state.number_of_cells = 0;
			battery_state.battery_error_state |= CELL_CONNECTION_ERROR;
		}
	}
	else {
		battery_state.number_of_cells = 0;
		battery_state.battery_error_state &= ~CELL_CONNECTION_ERROR;
	}

	if ( battery_state.number_of_cells > 1 ) {
		battery_state.balance_port_connected = CONNECTED;
	}
	else {
		battery_state.balance_port_connected = NOT_CONNECTED;
	}
}

/**
 * @brief Determines the state of connections based on ADC readings
 */
void Battery_Connection_State()
{
	if ( Get_Battery_Voltage() > VOLTAGE_CONNECTED_THRESHOLD ) {
		battery_state.xt60_connected = CONNECTED;
	}
	else {
		battery_state.xt60_connected = NOT_CONNECTED;
	}

	Balance_Connection_State();

	MCU_Temperature_Safety_Check();

	Balance_Battery();
}

/**
 * @brief Controls the GPIO outputs of the balancing circuit
 * @param  cell_balancing_gpio_bitmask: Four bit bitmask for cells 1-4. 1 balancing enabled, 0 disabled. Position 0 - cell 1, 1 - cell 2, etc.
 */
void Balancing_GPIO_Control(uint8_t cell_balancing_gpio_bitmask)
{
	if ( cell_balancing_gpio_bitmask & (1<<3) ) {
		HAL_GPIO_WritePin(CELL_4S_DIS_EN_GPIO_Port, CELL_4S_DIS_EN_Pin, GPIO_PIN_SET);
	}
	else {
		HAL_GPIO_WritePin(CELL_4S_DIS_EN_GPIO_Port, CELL_4S_DIS_EN_Pin, GPIO_PIN_RESET);
	}

	if ( cell_balancing_gpio_bitmask & (1<<2) ) {
		HAL_GPIO_WritePin(CELL_3S_DIS_EN_GPIO_Port, CELL_3S_DIS_EN_Pin, GPIO_PIN_SET);
	}
	else {
		HAL_GPIO_WritePin(CELL_3S_DIS_EN_GPIO_Port, CELL_3S_DIS_EN_Pin, GPIO_PIN_RESET);
	}

	if ( cell_balancing_gpio_bitmask & (1<<1) ) {
		HAL_GPIO_WritePin(CELL_2S_DIS_EN_GPIO_Port, CELL_2S_DIS_EN_Pin, GPIO_PIN_SET);
	}
	else {
		HAL_GPIO_WritePin(CELL_2S_DIS_EN_GPIO_Port, CELL_2S_DIS_EN_Pin, GPIO_PIN_RESET);
	}

	if ( cell_balancing_gpio_bitmask & (1<<0) ) {
		HAL_GPIO_WritePin(CELL_1S_DIS_EN_GPIO_Port, CELL_1S_DIS_EN_Pin, GPIO_PIN_SET);
	}
	else {
		HAL_GPIO_WritePin(CELL_1S_DIS_EN_GPIO_Port, CELL_1S_DIS_EN_Pin, GPIO_PIN_RESET);
	}
}

/**
 * @brief Checks if the MCU temperature is unsafe and sets battery error flag if so. Clears flag if temperature falls below recovery threshold.
 */
void MCU_Temperature_Safety_Check() {
	if (Get_MCU_Temperature() > MAX_MCU_TEMP_C_FOR_OPERATION) {
		battery_state.battery_error_state |= MCU_OVER_TEMP;
	}
	else if ( ((battery_state.battery_error_state & MCU_OVER_TEMP) == MCU_OVER_TEMP) && (Get_MCU_Temperature() < MCU_TEMP_C_RECOVERY) ) {
		battery_state.battery_error_state &= ~MCU_OVER_TEMP;
	}
}

/**
 * @brief Returns the balance connection state
 * @retval uint8_t CONNECTED or NOT_CONNECTED
 */
uint8_t Get_Balance_Connection_State()
{
	return battery_state.balance_port_connected;
}

/**
 * @brief Returns the state of balancing
 * @retval uint8_t 1 if balancing enabled or 0 if not enabled
 */
uint8_t Get_Balancing_State()
{
	return battery_state.balancing_enabled;
}

/**
 * @brief Returns the error state
 * @retval uint8_t NO_ERROR if no error. Other errors are defined in battery.h
 */
uint8_t Get_Battery_Error_State()
{
	return battery_state.battery_error_state;
}

/**
 * @brief Returns the state of charging
 * @retval uint8_t 1 if charging enabled or 0 if not enabled
 */
uint8_t Get_Charging_State()
{
	return battery_state.charging_enabled;
}

/**
 * @brief Returns the number of cells connected to the balance port
 * @retval uint8_t 2, 3, or 4
 */
uint8_t Get_Number_Of_Cells()
{
	return battery_state.number_of_cells;
}

/**
 * @brief Returns the XT60 connection state
 * @retval uint8_t CONNECTED or NOT_CONNECTED
 */
uint8_t Get_XT60_Connection_State()
{
	return battery_state.xt60_connected;
}

