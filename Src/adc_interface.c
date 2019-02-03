/**
 ******************************************************************************
 * @file           : adc_interface.c
 * @brief          : Interface for setting and getting information read in from the ADC
 ******************************************************************************
 */

#include "adc_interface.h"

struct Adc {
	int32_t bat_voltage;
	int32_t cell_1_voltage;
	int32_t cell_2_voltage;
	int32_t cell_3_voltage;
	int32_t cell_4_voltage;
	int32_t temperature;
};

struct Adc adc_values;

/**
 * @brief Gets the battery voltage that was read in from the ADC
 * @retval Battery voltage in volts * BATTERY_ADC_MULTIPLIER
 */
uint32_t Get_Battery_Voltage(void) {
	return adc_values.bat_voltage;
}

/**
 * @brief  Sets the battery voltage that was read in from the ADC
 * @param  adc_reading: Raw reading from ADC
 * @retval uint8_t 1 if successful, 0 if error
 */
uint8_t Set_Battery_Voltage(uint32_t adc_reading) {
	if ((adc_reading < BATTERY_MIN_ADC_READING) || (adc_reading > 4095)) {
		adc_values.bat_voltage = 0;
		return 0;
	}

	adc_values.bat_voltage = (adc_reading * BATTERY_ADC_SCALAR) + BATTERY_ADC_OFFSET;

	return 1;
}

/**
 * @brief Gets cell 1 voltage that was read in from the ADC
 * @retval Cell 1 voltage in volts * BATTERY_ADC_MULTIPLIER
 */
uint32_t Get_Cell_One_Voltage(void) {
	return adc_values.cell_1_voltage;
}

/**
 * @brief  Sets the cell 1 voltage that was read in from the ADC
 * @param  adc_reading: Raw reading from ADC
 * @retval uint8_t 1 if successful, 0 if error
 */
uint8_t Set_Cell_One_Voltage(uint32_t adc_reading) {
	if ((adc_reading < CELL_ONE_MIN_ADC_READING) || (adc_reading > 4095)) {
		adc_values.cell_1_voltage = 0;
		return 0;
	}

	adc_values.cell_1_voltage = (adc_reading * CELL_ONE_ADC_SCALAR) + CELL_ONE_ADC_OFFSET;

	return 1;
}

/**
 * @brief Gets cell 2 voltage that was read in from the ADC
 * @retval Cell 2 voltage in volts * BATTERY_ADC_MULTIPLIER
 */
uint32_t Get_Cell_Two_Voltage(void) {
	return adc_values.cell_2_voltage;
}

/**
 * @brief  Sets the cell 2 voltage that was read in from the ADC
 * @param  adc_reading: Raw reading from ADC
 * @retval uint8_t 1 if successful, 0 if error
 */
uint8_t Set_Cell_Two_Voltage(uint32_t adc_reading) {
	if ((adc_reading < CELL_TWO_MIN_ADC_READING) || (adc_reading > 4095)) {
		adc_values.cell_2_voltage = 0;
		return 0;
	}

	adc_values.cell_2_voltage = ((adc_reading * CELL_TWO_ADC_SCALAR) + CELL_TWO_ADC_OFFSET) - adc_values.cell_1_voltage;

	return 1;
}

/**
 * @brief Gets cell 3 voltage that was read in from the ADC
 * @retval Cell 3 voltage in volts * BATTERY_ADC_MULTIPLIER
 */
uint32_t Get_Cell_Three_Voltage(void) {
	return adc_values.cell_3_voltage;
}

/**
 * @brief  Sets the cell 3 voltage that was read in from the ADC
 * @param  adc_reading: Raw reading from ADC
 * @retval uint8_t 1 if successful, 0 if error
 */
uint8_t Set_Cell_Three_Voltage(uint32_t adc_reading) {
	if ((adc_reading < CELL_THREE_MIN_ADC_READING) || (adc_reading > 4095)) {
		adc_values.cell_3_voltage = 0;
		return 0;
	}

	adc_values.cell_3_voltage = ((adc_reading * CELL_THREE_ADC_SCALAR) + CELL_THREE_ADC_OFFSET)
			- adc_values.cell_2_voltage - adc_values.cell_1_voltage;

	return 1;
}

/**
 * @brief Gets cell 4 voltage that was read in from the ADC
 * @retval Cell 4 voltage in volts * BATTERY_ADC_MULTIPLIER
 */
uint32_t Get_Cell_Four_Voltage(void) {
	return adc_values.cell_4_voltage;
}

/**
 * @brief  Sets the cell 4 voltage that was read in from the ADC
 * @param  adc_reading: Raw reading from ADC
 * @retval uint8_t 1 if successful, 0 if error
 */
uint8_t Set_Cell_Four_Voltage(uint32_t adc_reading) {
	if ((adc_reading < CELL_FOUR_MIN_ADC_READING) || (adc_reading > 4095)) {
		adc_values.cell_4_voltage = 0;
		return 0;
	}

	adc_values.cell_4_voltage = ((adc_reading * CELL_FOUR_ADC_SCALAR) + CELL_FOUR_ADC_OFFSET)
			- adc_values.cell_3_voltage - adc_values.cell_2_voltage - adc_values.cell_1_voltage;

	return 1;
}

/**
 * @brief Gets mcu junction temperature that was read in from the ADC
 * @retval MCU junction temperature in celcius
 */
uint32_t Get_MCU_Temperature(void) {
	return adc_values.temperature;
}

/**
 * @brief  Sets the mcu junction temperature that was read in from the ADC
 * @param  temperature_c: MCU junction temperature in celcius
 * @retval uint8_t 1 if successful, 0 if error
 */
uint8_t Set_MCU_Temperature(uint32_t adc_reading) {
	if ((adc_reading < 0) || (adc_reading > 4095)) {
		return 0;
	} else {
		adc_values.temperature = adc_reading;
		return 1;
	}
}
