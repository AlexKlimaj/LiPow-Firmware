/**
 ******************************************************************************
 * @file           : adc_interface.c
 * @brief          : Interface for setting and getting information read in from the ADC
 ******************************************************************************
 */

#include "adc_interface.h"
#include "battery.h"

#include "stm32g0xx_hal_flash.h"

#include "task.h"
#include "printf.h"
#include "string.h"

extern ADC_HandleTypeDef hadc1;

/* Private typedef -----------------------------------------------------------*/
struct Adc {
	uint32_t bat_voltage;
	uint32_t cell_voltage[4];
	uint32_t vrefint;
	uint32_t vdda;
	int32_t temperature;
	uint32_t two_s_battery_voltage;
	uint32_t three_s_battery_voltage;
	uint32_t four_s_battery_voltage;
};

/* Private variables ---------------------------------------------------------*/
struct Adc adc_values;
uint32_t adc_buffer[7];
static volatile uint32_t adc_scalars[5], adc_offset[5], adc_buffer_filtered[7], adc_filtered_output[7];
static volatile uint32_t adc_sum_count;
static volatile uint16_t vrefint_cal;
static volatile uint8_t cal_present;

/* Private function prototypes -----------------------------------------------*/
uint8_t Set_Battery_Voltage(uint32_t adc_reading);
uint8_t Set_Cell_Voltage(uint8_t cell_number, uint32_t adc_reading);
uint8_t Set_MCU_Temperature(uint32_t adc_reading);
uint8_t Set_VDDa(uint32_t adc_reading);
uint8_t Read_Scalars_From_Flash(void);

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

	adc_values.bat_voltage = adc_reading * adc_scalars[0];

	return 1;
}

/**
 * @brief Gets cell X voltage that was read in from the ADC
 * @param  cell_number: Cell number 0-3 to get voltage
 * @retval Cell 1 voltage in volts * BATTERY_ADC_MULTIPLIER
 */
uint32_t Get_Cell_Voltage(uint8_t cell_number) {
	if (cell_number > 3) {
		return UINT32_MAX;
	}
	return adc_values.cell_voltage[cell_number];
}

/**
 * @brief  Sets the cell 1 voltage that was read in from the ADC
 * @param  cell_number: Cell number 0-3 to set voltage
 * @param  adc_reading: Raw reading from ADC
 * @retval uint8_t 1 if successful, 0 if error
 */
uint8_t Set_Cell_Voltage(uint8_t cell_number, uint32_t adc_reading) {

	if (cell_number > 3) {
		return 0;
	}

	if (cell_number == 0) {
		if ((adc_reading < CELL_ONE_MIN_ADC_READING) || (adc_reading > 4095)) {
			adc_values.cell_voltage[0] = 0;
			return 0;
		}
		else {
			adc_values.cell_voltage[0] = adc_reading * adc_scalars[1];

			if (adc_values.cell_voltage[0] > CELL_MAX_VOLTAGE) {
				adc_values.cell_voltage[0] = 0;
				return 0;
			}
		}
	}

	if (cell_number == 1) {
		if ((adc_reading < CELL_TWO_MIN_ADC_READING) || (adc_reading > 4095)) {
			adc_values.cell_voltage[1] = 0;
			adc_values.two_s_battery_voltage = 0;
			return 0;
		}
		else {
			adc_values.two_s_battery_voltage = adc_reading * adc_scalars[2];

			if (adc_values.two_s_battery_voltage > TWO_S_MAX_VOLTAGE) {
				adc_values.two_s_battery_voltage = 0;
				return 0;
			}

			if ( adc_values.two_s_battery_voltage > adc_values.cell_voltage[0] ) {
				adc_values.cell_voltage[1] = adc_values.two_s_battery_voltage - adc_values.cell_voltage[0];
			}
			else {
				adc_values.cell_voltage[1] = 0;
				return 0;
			}
		}
	}

	if (cell_number == 2) {
		if ((adc_reading < CELL_THREE_MIN_ADC_READING) || (adc_reading > 4095)) {
			adc_values.cell_voltage[2] = 0;
			return 0;
		}
		else {
			adc_values.three_s_battery_voltage = adc_reading * adc_scalars[3];

			if (adc_values.three_s_battery_voltage > THREE_S_MAX_VOLTAGE) {
				adc_values.three_s_battery_voltage = 0;
				return 0;
			}

			if ( adc_values.three_s_battery_voltage > adc_values.two_s_battery_voltage ) {
				adc_values.cell_voltage[2] = adc_values.three_s_battery_voltage - adc_values.two_s_battery_voltage;
			}
			else {
				adc_values.cell_voltage[2] = 0;
				return 0;
			}
		}
	}

	if (cell_number == 3) {
		if ((adc_reading < CELL_FOUR_MIN_ADC_READING) || (adc_reading > 4095)) {
			adc_values.cell_voltage[3] = 0;
			return 0;
		}
		else {
			adc_values.four_s_battery_voltage = adc_reading * adc_scalars[4];

			if (adc_values.four_s_battery_voltage > FOUR_S_MAX_VOLTAGE) {
				adc_values.four_s_battery_voltage = 0;
				return 0;
			}

			if ( adc_values.four_s_battery_voltage > adc_values.three_s_battery_voltage ) {
				adc_values.cell_voltage[3] = adc_values.four_s_battery_voltage - adc_values.three_s_battery_voltage;
			}
			else {
				adc_values.cell_voltage[3] = 0;
				return 0;
			}
		}
	}

	return 1;
}

/**
 * @brief Gets mcu junction temperature that was read in from the ADC
 * @retval MCU junction temperature in celcius
 */
int32_t Get_MCU_Temperature(void) {
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
		adc_values.vrefint = __HAL_ADC_CALC_VREFANALOG_VOLTAGE(adc_filtered_output[6], ADC_RESOLUTION_12B);
		adc_values.temperature = __HAL_ADC_CALC_TEMPERATURE(adc_values.vrefint, adc_reading, ADC_RESOLUTION_12B);
	}
	return 1;
}

/**
 * @brief Gets the value of VDDa read in from the ADC
 * @retval VDDa in volts * BATTERY_ADC_MULTIPLIER
 */
uint32_t Get_VDDa() {
	return adc_values.vdda;
}

/**
 * @brief  Sets the value of VDDa read in from the ADC
 * @param  adc_reading: Raw reading from ADC
 * @retval uint8_t 1 if successful, 0 if error
 */
uint8_t Set_VDDa(uint32_t adc_reading) {
	if ((adc_reading < 0) || (adc_reading > 4095)) {
		return 0;
	} else {
		adc_values.vdda = 3 * ((vrefint_cal * BATTERY_ADC_MULTIPLIER) / adc_reading);
	}
	return 1;
}

/**
 * @brief  Calculates and sets the ADC scalars based on a reference voltage input
 * @param  reference_voltage: Reference voltage in milivolts
 * @retval uint8_t 1 if successful, 0 if error
 */
uint8_t Calibrate_ADC(float reference_voltage_mv) {

	if (reference_voltage_mv > 4200.0f) {
		return 0;
	}

	printf("Input Reference Voltage in mv: %.3f\r\n", reference_voltage_mv);

	if (reference_voltage_mv < 0.01f) {
		for (int i = 0; i < 5; i++) {
			adc_offset[i] = adc_filtered_output[i];

			printf("ADC Channel %u offset: %u\r\n", i, adc_offset[i]);
		}
	}
	else {
		for (int i = 0; i < 5; i++) {
			float scale = (reference_voltage_mv * BATTERY_ADC_MULTIPLIER) / (adc_filtered_output[i] * 1000);
			adc_scalars[i] = (uint32_t)scale;

			printf("ADC Channel %u scalar: %u\r\n", i, adc_scalars[i]);
		}
	}

	return 1;
}

void vRead_ADC(void const *pvParameters) {
	// calibrate ADC
	vTaskDelay(500 / portTICK_PERIOD_MS);
	while (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK);
	vTaskDelay(500 / portTICK_PERIOD_MS);
	vrefint_cal = (uint32_t)(*VREFINT_CAL_ADDR); // VREFINT calibration value

	//Read the scalars out of OTP flash
	Read_Scalars_From_Flash();

	adc_sum_count = 0;

	static uint32_t thread_notification;
	const TickType_t xMaxBlockTime = pdMS_TO_TICKS(500);

	// Start the DMA ADC
	HAL_ADC_Start_DMA(&hadc1, adc_buffer, hadc1.Init.NbrOfConversion);

	for (;;) {
		/* Wait to be notified of an interrupt. */
		thread_notification = ulTaskNotifyTake(pdTRUE, xMaxBlockTime);

		if (thread_notification) {

			/* A notification was received. */
			Set_Battery_Voltage(adc_filtered_output[0]);

			for (int i = 0; i < 4; i++) {
				Set_Cell_Voltage(i, adc_filtered_output[i+1]);
			}

			Set_MCU_Temperature(adc_filtered_output[5]);

			Set_VDDa(adc_filtered_output[6]);

			/* Determines battery connection state and performs balancing */
			Battery_Connection_State();

		} else {
			/* Did not receive a notification within the expected time. */
			printf("Did Not Receive an ADC Notification\r\n");
		}
	}
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	/* tCONV = Sampling time + 12.5 x ADC clock cycles
	 For 160 sample time and 16MHz clock divided by 4
	 tCONV = (160 + 12.5) x 1/(16MHz/4) = 43.125us
	 For 6 reads = 258.75us or 3.864kHz */

	unsigned length = sizeof(adc_buffer_filtered)/sizeof(adc_buffer_filtered[0]);

	for(unsigned i = 0; i < length; i++) {
		adc_buffer_filtered[i] += adc_buffer[i];
	}

	adc_sum_count++;

	if (adc_sum_count == ADC_FILTER_SUM_COUNT) {

		length = sizeof(adc_filtered_output)/sizeof(adc_filtered_output[0]);

		for(unsigned i = 0; i < length; i++) {
			adc_filtered_output[i] = ( adc_buffer_filtered[i] / ADC_FILTER_SUM_COUNT );
		}

		adc_sum_count = 0;

		// Clear the buffer
		memset((uint32_t *)adc_buffer_filtered, 0, sizeof(adc_buffer_filtered));

		BaseType_t should_context_switch = pdFALSE;
		vTaskNotifyGiveFromISR(adcTaskHandle, &should_context_switch);
		portYIELD_FROM_ISR(should_context_switch);
	}
}

uint32_t Get_Two_S_Voltage() {
	return adc_values.two_s_battery_voltage;
}

uint32_t Get_Three_S_Voltage() {
	return adc_values.three_s_battery_voltage;
}

uint32_t Get_Four_S_Voltage() {
	return adc_values.four_s_battery_voltage;
}

/**
 * @brief  Writes the cal result to OTP Flash
 * @retval uint8_t 1 if successful, 0 if error
 */
uint8_t Write_Cal_To_OTP_Flash() {
	HAL_StatusTypeDef status;

	/* Check input parameters */
	for (int i = 0; i < 5; i++) {
		if ((adc_scalars[i] < 750) || (adc_scalars[i] > 5000)) {
			printf("ERROR: ADC Scalar %u Not Set or Out of Range\r\n", i);
			/* Return error */
			return 1;
		}
	}

	if (HAL_FLASH_Unlock() != HAL_OK) {
		printf("ERROR: Could not unlock Flash\r\n");
		return 1;
	}

	uint32_t address = OTP_START_ADDR;
	uint32_t temp_address = OTP_START_ADDR;

	for (int i = 0; i < OTP_SIZE; i++) {

		for (int x = 0; x < 5; x++) {
			uint32_t value = *(uint32_t *)(temp_address + (i * BYTES_IN_UINT64) + (x * BYTES_IN_UINT32));
			if ((value > 750) && (value < 5000)) {
				printf("OTP Memory Value: %u\r\n", value);
				address = temp_address + ((i + 1) * BYTES_IN_UINT64);
			}
			else {
				break;
			}
		}
	}

	printf("OPT Scalar Start Address: 0x%08x\r\n", address);

	uint64_t data_in_64;

	//Write Calibration Values
	for (int i = 0; i < 3; i++) {
		/* Write double */
		if (i < 2) {
			data_in_64 = adc_scalars[i*2] | (uint64_t)adc_scalars[(i*2)+1] << 32;
		}
		else {
			uint32_t mask = 0xFFFFFFFF;
			data_in_64 = adc_scalars[i*2] | (uint64_t)mask << 32;
		}

		printf("Writing 0x%016llx to address: 0x%08x\r\n", (uint64_t)data_in_64, (uint32_t)(address + (i * BYTES_IN_UINT64)));

		status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (address + (i * BYTES_IN_UINT64)), data_in_64);

		if (status != HAL_OK) {
			printf("ERROR: Write scalar #%u to OTP Flash Failed\r\n", i);

			if (HAL_FLASH_Lock() != HAL_OK) {
				printf("ERROR: Could not lock Flash\r\n");
				return 1;
			}

			/* Return error */
			return 1;
		}
	}

	if (HAL_FLASH_Lock() != HAL_OK) {
		printf("ERROR: Could not lock Flash\r\n");
		return 1;
	}

	return 0;
}

/**
 * @brief  Read the cal values from OTP Flash
 * @retval uint8_t 1 if successful, 0 if error
 */
uint8_t Read_Scalars_From_Flash() {
	uint32_t address = OTP_START_ADDR;

	uint32_t temp_scalars[5] = {0};

	uint8_t t = 4;

	for (int i = OTP_SIZE; i >= 0; i--) {

		uint32_t value = *(uint32_t *)(address + (i * BYTES_IN_UINT32));

		if ((value > 750) && (value < 5000)) {
			printf("OTP Value %u at address: 0x%08x\r\n", value, (uint32_t)(address + (i * BYTES_IN_UINT32)));

			if (cal_present == 0) {
				temp_scalars[t] = value;
			}

			if(t == 0) {
				cal_present = cal_present + 1;
				t = 5;
			}

			t--;
		}
	}

	if (cal_present != 0) {

		for (int y = 0; y < 5; y++) {
			adc_scalars[y] = temp_scalars[y];
		}

		printf("Calibration values already present. 32 total calibrations can be performed. Number of calibrations performed: %u\r\n", cal_present);
		printf("Using these calibration values:\r\n");

		for (int i = 0; i < 5; i++) {
			printf("Scalar: %u  Value: %u\r\n", i, adc_scalars[i]);
		}
		return 0;
	}

	cal_present = 0;
	printf("NOT CALIBRATED. Connect known good voltage to cells 1-4 and XT60 in the range of 3.3V - 4V and run cal command.\r\nThen write then to flash with write_otp_scalars_to_flash\r\n");

	return 1;
}
