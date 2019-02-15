/**
 ******************************************************************************
 * @file           : adc_interface.c
 * @brief          : Interface for setting and getting information read in from the ADC
 ******************************************************************************
 */

#include "adc_interface.h"
#include "battery.h"

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
TaskHandle_t adcTaskHandle = NULL;
struct Adc adc_values;
uint32_t adc_buffer[7];
static volatile uint32_t adc_buffer_filtered[7], adc_filtered_output[7];
static volatile uint32_t adc_sum_count;
static volatile uint16_t vrefint_cal;

/* Private function prototypes -----------------------------------------------*/
uint8_t Set_Battery_Voltage(uint32_t adc_reading);
uint8_t Set_Cell_Voltage(uint8_t cell_number, uint32_t adc_reading);
uint8_t Set_MCU_Temperature(uint32_t adc_reading);
uint8_t Set_VDDa(uint32_t adc_reading);
void vRead_ADC(void *pvParameters);

/**
 * @brief Creates the adc task
 */
void vCreateADCTask( uint16_t usStackSize, unsigned portBASE_TYPE uxPriority )
{
	/* Create the task, storing the handle. */
	xTaskCreate(vRead_ADC, /* Function that implements the task. */
		(const char* const ) "read_adc", /* Text name for the task. */
		usStackSize, /* Stack size in words, not bytes. */
		0, /* Parameter passed into the task. */
		uxPriority, /* Priority at which the task is created. */
		&adcTaskHandle); /* Used to pass out the created task's handle. */
}

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

		adc_values.cell_voltage[0] = (adc_reading * CELL_ONE_ADC_SCALAR) + CELL_ONE_ADC_OFFSET;
		if (adc_values.cell_voltage[0] > CELL_MAX_VOLTAGE) {
			adc_values.cell_voltage[0] = 0;
		}
	}

	if (cell_number == 1) {
		if ((adc_reading < CELL_TWO_MIN_ADC_READING) || (adc_reading > 4095)) {
			adc_values.cell_voltage[1] = 0;
			adc_values.two_s_battery_voltage = 0;
			return 0;
		}

		adc_values.two_s_battery_voltage = ((adc_reading * CELL_TWO_ADC_SCALAR) + CELL_TWO_ADC_OFFSET);
		if (adc_values.two_s_battery_voltage > TWO_S_MAX_VOLTAGE) {
			adc_values.two_s_battery_voltage = 0;
		}

		if ( adc_values.two_s_battery_voltage > adc_values.cell_voltage[0] ) {
			adc_values.cell_voltage[1] = adc_values.two_s_battery_voltage - adc_values.cell_voltage[0];
		}
		else {
			adc_values.cell_voltage[1] = 0;
		}
	}

	if (cell_number == 2) {
		if ((adc_reading < CELL_THREE_MIN_ADC_READING) || (adc_reading > 4095)) {
			adc_values.cell_voltage[2] = 0;
			return 0;
		}

		adc_values.three_s_battery_voltage = ((adc_reading * CELL_THREE_ADC_SCALAR) + CELL_THREE_ADC_OFFSET);
		if (adc_values.three_s_battery_voltage > THREE_S_MAX_VOLTAGE) {
			adc_values.three_s_battery_voltage = 0;
		}

		if ( adc_values.three_s_battery_voltage > adc_values.two_s_battery_voltage ) {
			adc_values.cell_voltage[2] = adc_values.three_s_battery_voltage - adc_values.two_s_battery_voltage;
		}
		else {
			adc_values.cell_voltage[2] = 0;
		}
	}

	if (cell_number == 3) {
		if ((adc_reading < CELL_FOUR_MIN_ADC_READING) || (adc_reading > 4095)) {
			adc_values.cell_voltage[3] = 0;
			return 0;
		}

		adc_values.four_s_battery_voltage = ((adc_reading * CELL_FOUR_ADC_SCALAR) + CELL_FOUR_ADC_OFFSET);
		if (adc_values.four_s_battery_voltage > FOUR_S_MAX_VOLTAGE) {
			adc_values.four_s_battery_voltage = 0;
		}

		if ( adc_values.four_s_battery_voltage > adc_values.three_s_battery_voltage ) {
			adc_values.cell_voltage[3] = adc_values.four_s_battery_voltage - adc_values.three_s_battery_voltage;
		}
		else {
			adc_values.cell_voltage[3] = 0;
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

void vRead_ADC(void *pvParameters) {
	// calibrate ADC
	while (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK);
	vTaskDelay(100 / portTICK_PERIOD_MS);
	vrefint_cal = (uint32_t)(*VREFINT_CAL_ADDR); // VREFINT calibration value

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

			//printf("adc 4 value = %d\r\n", adc_filtered_output[4]);
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

