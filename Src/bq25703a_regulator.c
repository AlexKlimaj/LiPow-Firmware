/**
 ******************************************************************************
 * @file           : bq25703a_regulator.c
 * @brief          : Handles battery state information
 ******************************************************************************
 */


#include "bq25703a_regulator.h"
#include "battery.h"
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

extern I2C_HandleTypeDef hi2c1;

/* Private typedef -----------------------------------------------------------*/
struct Regulator {
	uint8_t connected;
	uint8_t status;
	uint32_t input_current;
	uint32_t output_current;
	uint16_t max_charge_voltage;
	uint8_t input_current_limit;
	uint16_t min_input_voltage_limit;
};

/* Private variables ---------------------------------------------------------*/
struct Regulator regulator;

/* Used to guard access to the I2C in case messages are sent to the UART from
 more than one task. */
static SemaphoreHandle_t xTxMutex = NULL;

/* The maximum time to wait for the mutex that guards the UART to become
 available. */
#define cmdMAX_MUTEX_WAIT	pdMS_TO_TICKS( 300 )

/* Private function prototypes -----------------------------------------------*/
void I2C_Transfer(uint8_t *pData, uint16_t size);
void vRegulator(void *pvParameters);

/**
 * @brief Performs an I2C transfer
 */
void I2C_Transfer(uint8_t *pData, uint16_t size) {

	if ( xSemaphoreTake( xTxMutex, cmdMAX_MUTEX_WAIT ) == pdPASS) {
		do
		{
			while (HAL_I2C_Master_Transmit_DMA(&hi2c1, (uint16_t)BQ26703A_I2C_ADDRESS, pData, size) != HAL_OK);
		    while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY);
		}
		while(HAL_I2C_GetError(&hi2c1) == HAL_I2C_ERROR_AF);
		xSemaphoreGive(xTxMutex);
	}
}

void I2C_Receive(uint8_t *pData, uint16_t size) {
	if ( xSemaphoreTake( xTxMutex, cmdMAX_MUTEX_WAIT ) == pdPASS) {
		do
		{
			while(HAL_I2C_Master_Receive_DMA(&hi2c1, (uint16_t)BQ26703A_I2C_ADDRESS, pData, size) != HAL_OK);
			while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY);
		}
		while(HAL_I2C_GetError(&hi2c1) == HAL_I2C_ERROR_AF);
		xSemaphoreGive(xTxMutex);
	}
}

/**
 * @brief Creates the regulator task
 */
void vCreateRegulatorTask( uint16_t usStackSize, unsigned portBASE_TYPE uxPriority )
{
	/* Create the semaphore used to access the I2C TX. */
	xTxMutex = xSemaphoreCreateMutex();
	configASSERT(xTxMutex);

	/* Create the task, storing the handle. */
	xTaskCreate(vRegulator, /* Function that implements the task. */
		(const char* const ) "regulator", /* Text name for the task. */
		usStackSize, /* Stack size in words, not bytes. */
		0, /* Parameter passed into the task. */
		uxPriority, /* Priority at which the task is created. */
		0); /* Used to pass out the created task's handle. */
}


void vRegulator(void *pvParameters) {

	TickType_t xDelay = 500 / portTICK_PERIOD_MS;

	/* Get the manufacturer id */
	uint8_t manufacturer_id_address = MANUFACTURER_ID_ADDR;
	I2C_Transfer((uint8_t *) &manufacturer_id_address, 1);
	uint8_t manufacturer_id;
	I2C_Receive((uint8_t *) &manufacturer_id, sizeof(manufacturer_id));

	/* Get the device id */
	uint8_t device_id_address = DEVICE_ID_ADDR;
	I2C_Transfer((uint8_t *) &device_id_address, 1);
	uint8_t device_id;
	I2C_Receive((uint8_t *) &device_id, sizeof(device_id));

	if ( (device_id == BQ26703A_DEVICE_ID) && (manufacturer_id == BQ26703A_MANUFACTURER_ID) ) {
		regulator.connected = CONNECTED;
	}
	else {
		regulator.connected = NOT_CONNECTED;
	}

	for (;;) {



		vTaskDelay(xDelay);
	}
}
