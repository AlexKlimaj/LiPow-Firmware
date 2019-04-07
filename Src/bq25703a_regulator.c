/**
 ******************************************************************************
 * @file           : bq25703a_regulator.c
 * @brief          : Handles battery state information
 ******************************************************************************
 */


#include "bq25703a_regulator.h"
#include "battery.h"
#include "error.h"
#include "main.h"
#include "string.h"
#include "printf.h"

extern I2C_HandleTypeDef hi2c1;

/* Private typedef -----------------------------------------------------------*/
struct Regulator {
	uint8_t connected;
	uint8_t charging_status;
	uint16_t max_charge_voltage;
	uint8_t input_current_limit;
	uint16_t min_input_voltage_limit;
	uint32_t vbus_voltage;
	uint32_t vbat_voltage;
	uint32_t vsys_voltage;
	uint32_t charge_current;
	uint32_t input_current;
};

/* Private variables ---------------------------------------------------------*/
struct Regulator regulator;

/* The maximum time to wait for the mutex that guards the UART to become
 available. */
#define cmdMAX_MUTEX_WAIT	pdMS_TO_TICKS( 300 )

/* Private function prototypes -----------------------------------------------*/
void I2C_Transfer(uint8_t *pData, uint16_t size);
void I2C_Receive(uint8_t *pData, uint16_t size);
void I2C_Write_Register(uint8_t addr_to_write, uint8_t *pData);
void I2C_Write_Two_Byte_Register(uint8_t addr_to_write, uint8_t lsb_data, uint8_t msb_data);
void I2C_Read_Register(uint8_t addr_to_read, uint8_t *pData, uint16_t size);
uint8_t Query_Regulator_Connection(void);
uint8_t Read_Charge_Okay(void);
void Read_Charge_Status(void);
void Regulator_Set_ADC_Option(void);
void Regulator_Read_ADC(void);
void Regulator_HI_Z(uint8_t hi_z_en);
void Regulator_OTG_EN(uint8_t otg_en);
void Regulator_Set_Charge_Option_0(void);
void Set_Charge_Voltage(uint8_t number_of_cells);

/**
 * @brief Returns whether the regulator is connected over I2C
 * @retval uint8_t CONNECTED or NOT_CONNECTED
 */
uint8_t Get_Regulator_Connection_State() {
	return regulator.connected;
}

/**
 * @brief Returns whether the regulator is charging
 * @retval uint8_t 1 if charging, 0 if not charging
 */
uint8_t Get_Regulator_Charging_State() {
	return regulator.charging_status;
}

/**
 * @brief Gets VBUS voltage that was read in from the ADC on the regulator
 * @retval VBUS voltage in volts * BATTERY_ADC_MULTIPLIER
 */
uint32_t Get_VBUS_ADC_Reading() {
	return regulator.vbus_voltage;
}

/**
 * @brief Gets Input Current that was read in from the ADC on the regulator
 * @retval Input Current in amps * BATTERY_ADC_MULTIPLIER
 */
uint32_t Get_Input_Current_ADC_Reading() {
	return regulator.input_current;
}

/**
 * @brief Gets Charge Current that was read in from the ADC on the regulator
 * @retval Charge Current in amps * BATTERY_ADC_MULTIPLIER
 */
uint32_t Get_Charge_Current_ADC_Reading() {
	return regulator.charge_current;
}

/**
 * @brief Performs an I2C transfer
 * @param pData Pointer to location of data to transfer
 * @param size Size of data to be transferred
 */
void I2C_Transfer(uint8_t *pData, uint16_t size) {

	if ( xSemaphoreTake( xTxMutex_Regulator, cmdMAX_MUTEX_WAIT ) == pdPASS) {
		do
		{
			TickType_t xtimeout_start = xTaskGetTickCount();
			while (HAL_I2C_Master_Transmit_DMA(&hi2c1, (uint16_t)BQ26703A_I2C_ADDRESS, pData, size) != HAL_OK) {
				if (((xTaskGetTickCount()-xtimeout_start)/portTICK_PERIOD_MS) > I2C_TIMEOUT) {
					Set_Error_State(REGULATOR_COMMUNICATION_ERROR);
					break;
				}
			}
		    while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) {
				if (((xTaskGetTickCount()-xtimeout_start)/portTICK_PERIOD_MS) > I2C_TIMEOUT) {
					Set_Error_State(REGULATOR_COMMUNICATION_ERROR);
					break;
				}
		    }
		}
		while(HAL_I2C_GetError(&hi2c1) == HAL_I2C_ERROR_AF);
		xSemaphoreGive(xTxMutex_Regulator);
	}
}

/**
 * @brief Performs an I2C transfer
 * @param pData Pointer to location to store received data
 * @param size Size of data to be received
 */
void I2C_Receive(uint8_t *pData, uint16_t size) {
	if ( xSemaphoreTake( xTxMutex_Regulator, cmdMAX_MUTEX_WAIT ) == pdPASS) {
		do
		{
			TickType_t xtimeout_start = xTaskGetTickCount();
			while (HAL_I2C_Master_Receive_DMA(&hi2c1, (uint16_t)BQ26703A_I2C_ADDRESS, pData, size) != HAL_OK) {
				if (((xTaskGetTickCount()-xtimeout_start)/portTICK_PERIOD_MS) > I2C_TIMEOUT) {
					Set_Error_State(REGULATOR_COMMUNICATION_ERROR);
					break;
				}
			}
			while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) {
				if (((xTaskGetTickCount()-xtimeout_start)/portTICK_PERIOD_MS) > I2C_TIMEOUT) {
					Set_Error_State(REGULATOR_COMMUNICATION_ERROR);
					break;
				}
			}
		}
		while(HAL_I2C_GetError(&hi2c1) == HAL_I2C_ERROR_AF);
		xSemaphoreGive(xTxMutex_Regulator);
	}
}

/**
 * @brief Automatically performs two I2C writes to write a register on the regulator
 * @param pData Pointer to data to be transferred
 */
void I2C_Write_Register(uint8_t addr_to_write, uint8_t *pData) {
	uint8_t data[2];
	data[0] = addr_to_write;
	data[1] = *pData;
	I2C_Transfer(data, 2);
}

/**
 * @brief Automatically performs three I2C writes to write a two byte register on the regulator
 * @param lsb_data Pointer to least significant byte of data to be transferred
 * @param msb_data Pointer to most significant byte of data to be transferred
 */
void I2C_Write_Two_Byte_Register(uint8_t addr_to_write, uint8_t lsb_data, uint8_t msb_data) {

	uint8_t data[3];
	data[0] = addr_to_write;
	data[1] = lsb_data;
	data[2] = msb_data;

	I2C_Transfer(data, 3);
}

/**
 * @brief Automatically performs one I2C write and an I2C read to get the value of a register
 * @param pData Pointer to where to store data
 */
void I2C_Read_Register(uint8_t addr_to_read, uint8_t *pData, uint16_t size) {
		I2C_Transfer((uint8_t *)&addr_to_read, 1);
		I2C_Receive(pData, size);
}

/**
 * @brief Checks if the regulator is connected over I2C
 * @retval uint8_t CONNECTED or NOT_CONNECTED
 */
uint8_t Query_Regulator_Connection() {
	/* Get the manufacturer id */
	uint8_t manufacturer_id;
	I2C_Read_Register(MANUFACTURER_ID_ADDR, (uint8_t *) &manufacturer_id, 1);

	/* Get the device id */
	uint8_t device_id;
	I2C_Read_Register(DEVICE_ID_ADDR, (uint8_t *) &device_id, 1);

	if ( (device_id == BQ26703A_DEVICE_ID) && (manufacturer_id == BQ26703A_MANUFACTURER_ID) ) {
		Clear_Error_State(REGULATOR_COMMUNICATION_ERROR);
		return CONNECTED;
	}
	else {
		Set_Error_State(REGULATOR_COMMUNICATION_ERROR);
		return NOT_CONNECTED;
	}
}

/**
 * @brief Checks the state of the Charge okay pin and returns the value
 * @retval 0 if VBUS falls below 3.2 V or rises above 26 V, 1 if VBUS is between 3.5V and 24.5V
 */
uint8_t Read_Charge_Okay() {
	return HAL_GPIO_ReadPin(CHRG_OK_GPIO_Port, CHRG_OK_Pin);
}

/**
 * @brief Reads ChargeStatus register and sets status
 */
void Read_Charge_Status() {
	uint8_t data[2];
	I2C_Read_Register(CHARGE_STATUS_ADDR, data, 2);

	if (data[1] & CHARGING_ENABLED_MASK) {
		regulator.charging_status = 1;
	}
	else {
		regulator.charging_status = 0;
	}
}

/**
 * @brief Sets the Regulators ADC settings
 */
void Regulator_Set_ADC_Option() {

	uint8_t ADC_lsb_3A = ADC_ENABLED_BITMASK;

	I2C_Write_Register(ADC_OPTION_ADDR, (uint8_t *) &ADC_lsb_3A);
}

/**
 * @brief Initiates and reads a single ADC conversion on the regulator
 */
void Regulator_Read_ADC() {
	TickType_t xDelay = 80 / portTICK_PERIOD_MS;

	uint8_t ADC_msb_3B = ADC_START_CONVERSION_MASK;

	I2C_Write_Register((ADC_OPTION_ADDR+1), (uint8_t *) &ADC_msb_3B);

	/* Wait for the conversion to finish */
	while (ADC_msb_3B & (1<<6)) {
		vTaskDelay(xDelay);
		I2C_Read_Register((ADC_OPTION_ADDR+1), (uint8_t *) &ADC_msb_3B, 1);
	}

	uint8_t temp = 0;

	I2C_Read_Register(VBAT_ADC_ADDR, (uint8_t *) &temp, 1);
	regulator.vbat_voltage = (temp * VBAT_ADC_SCALE) + VBAT_ADC_OFFSET;

	I2C_Read_Register(VSYS_ADC_ADDR, (uint8_t *) &temp, 1);
	regulator.vsys_voltage = (temp * VSYS_ADC_SCALE) + VSYS_ADC_OFFSET;

	I2C_Read_Register(ICHG_ADC_ADDR, (uint8_t *) &temp, 1);
	regulator.charge_current = temp * ICHG_ADC_SCALE;

	I2C_Read_Register(IIN_ADC_ADDR, (uint8_t *) &temp, 1);
	regulator.input_current = temp * IIN_ADC_SCALE;

	I2C_Read_Register(VBUS_ADC_ADDR, (uint8_t *) &temp, 1);
	regulator.vbus_voltage = (temp * VBUS_ADC_SCALE) + VBUS_ADC_OFFSET;
}

/**
 * @brief Enables or disables high impedance mode on the output of the regulator
 * @param hi_z_en 1 puts the output of the regulator in hiz mode. 0 takes the regulator out of hi_z and allows charging
 */
void Regulator_HI_Z(uint8_t hi_z_en) {
	if (hi_z_en == 1) {
		HAL_GPIO_WritePin(ILIM_HIZ_GPIO_Port, ILIM_HIZ_Pin, GPIO_PIN_RESET);
	}
	else {
		HAL_GPIO_WritePin(ILIM_HIZ_GPIO_Port, ILIM_HIZ_Pin, GPIO_PIN_SET);
	}
}

/**
 * @brief Enables or disables On the Go Mode
 * @param otg_en 0 disables On the GO Mode. 1 Enables.
 */
void Regulator_OTG_EN(uint8_t otg_en) {
	if (otg_en == 0) {
		HAL_GPIO_WritePin(EN_OTG_GPIO_Port, EN_OTG_Pin, GPIO_PIN_RESET);
	}
	else {
		HAL_GPIO_WritePin(EN_OTG_GPIO_Port, EN_OTG_Pin, GPIO_PIN_SET);
	}
}

/**
 * @brief Sets Charge Option 0 Based on #defines in header
 */
void Regulator_Set_Charge_Option_0() {

	uint8_t charge_option_0_register_1_value = 0b00100110;
	uint8_t charge_option_0_register_2_value = 0b00001110;

	I2C_Write_Two_Byte_Register(CHARGE_OPTION_0_ADDR, charge_option_0_register_2_value, charge_option_0_register_1_value);

	return;
}

/**
 * @brief Sets the charging current limit. From 64mA to 8.128A in 64mA steps. Maps from 0 - 128. 7 bit value.
 * @param charge_current_limit 0-128 which remaps from 64mA-8.128A. 7 bit value.
 */
void Set_Charge_Current(uint8_t charge_current_limit) {

	uint8_t charge_current_register_1_value = 0;
	uint8_t charge_current_register_2_value = 0;

	if ((charge_current_limit >= 0) || (charge_current_limit <= 128)) {
		charge_current_register_1_value = (charge_current_limit >> 3);
		charge_current_register_2_value = (charge_current_limit << 6);
	}

	I2C_Write_Two_Byte_Register(CHARGE_CURRENT_ADDR, charge_current_register_2_value, charge_current_register_1_value);

	return;
}

/**
 * @brief Sets the charging voltage based on the number of cells. 1 - 4.192V, 2 - 8.400V, 3 - 12.592V, 4 - 16.800V
 * @param number_of_cells number of cells connected
 */
void Set_Charge_Voltage(uint8_t number_of_cells) {

	uint8_t max_charge_register_1_value = 0;
	uint8_t max_charge_register_2_value = 0;

	uint8_t	minimum_system_voltage_value = 0b00000100; //1.024V

	if ((number_of_cells > 0) || (number_of_cells < 5)) {
		switch (number_of_cells) {
			case 1:
				max_charge_register_1_value = 0b00010000;
				max_charge_register_2_value = 0b01100000;
				minimum_system_voltage_value = 0b00001011; //2.816V
				break;
			case 2:
				max_charge_register_1_value = 0b00100000;
				max_charge_register_2_value = 0b11010000;
				minimum_system_voltage_value = 0b00010110; //5.632V
				break;
			case 3:
				max_charge_register_1_value = 0b00110001;
				max_charge_register_2_value = 0b00110000;
				minimum_system_voltage_value = 0b00100001; //8.448V
				break;
			case 4:
				max_charge_register_1_value = 0b01000001;
				max_charge_register_2_value = 0b10100000;
				minimum_system_voltage_value = 0b00101100; //11.264V
				break;
			default:
				max_charge_register_1_value = 0;
				max_charge_register_2_value = 0;
				minimum_system_voltage_value = 0b00000100; //1.024V
				break;
			}
	}

	I2C_Write_Register(MINIMUM_SYSTEM_VOLTAGE_ADDR, (uint8_t *) &minimum_system_voltage_value);

	I2C_Write_Two_Byte_Register(MAX_CHARGE_VOLTAGE_ADDR, max_charge_register_2_value, max_charge_register_1_value);

	return;
}

/**
 * @brief Main regulator task
 */
void vRegulator(void const *pvParameters) {

	TickType_t xDelay = 500 / portTICK_PERIOD_MS;

	/* Disable the output of the regulator for safety */
	Regulator_HI_Z(1);

	/* Disable OTG mode */
	Regulator_OTG_EN(0);

	/* Check if the regulator is connected */
	regulator.connected = Query_Regulator_Connection();

	/* Set Charge Option 0 */
	Regulator_Set_Charge_Option_0();

	/* Setup the ADC on the Regulator */
	Regulator_Set_ADC_Option();

	for (;;) {

		if (Read_Charge_Okay() != 1) {
			Set_Error_State(VOLTAGE_INPUT_ERROR);
		}
		else if ((Get_Error_State() & VOLTAGE_INPUT_ERROR) == VOLTAGE_INPUT_ERROR) {
			Clear_Error_State(VOLTAGE_INPUT_ERROR);
		}

		if ((Get_Error_State() & REGULATOR_COMMUNICATION_ERROR) == REGULATOR_COMMUNICATION_ERROR) {
			regulator.connected = 0;
		}

		if ((Get_XT60_Connection_State() == CONNECTED) && (Get_Balance_Connection_State() == CONNECTED) && (Get_Error_State() == 0)) {

			if (regulator.charging_status == 0) {
				Set_Charge_Voltage(Get_Number_Of_Cells());
				Set_Charge_Current(20); //Hardcoded for now. Will need to be determined with an algorithm once USB PD is implemented
				vTaskDelay(xDelay*4);
				Regulator_HI_Z(0);
			}
			else {
				Set_Charge_Voltage(Get_Number_Of_Cells());
				Set_Charge_Current(50); //Hardcoded for now. Will need to be determined with an algorithm once USB PD is implemented
			}

		}
		else {
			Regulator_HI_Z(1);
			Set_Charge_Voltage(0);
			Set_Charge_Current(0);
		}

		Read_Charge_Status();
		Regulator_Read_ADC();

		vTaskDelay(xDelay);
	}
}
