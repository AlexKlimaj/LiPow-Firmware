/**
 ******************************************************************************
 * @file           : bq25703a_regulator.h
 * @brief          : Header for bq25703a_regulator.c file.
 ******************************************************************************
 */

#ifndef BQ25703A_REGULATOR_H_
#define BQ25703A_REGULATOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32g0xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "cmsis_os.h"


#define I2C_TIMEOUT					(500 / portTICK_PERIOD_MS)

#define BQ26703A_I2C_ADDRESS		0xD6
#define BQ26703A_MANUFACTURER_ID	0x40
#define BQ26703A_DEVICE_ID			0x78

#define MANUFACTURER_ID_ADDR		0x2E
#define DEVICE_ID_ADDR				0x2F
#define MAX_CHARGE_VOLTAGE_ADDR		0x04
#define CHARGE_CURRENT_ADDR			0x02
#define CHARGE_OPTION_0_ADDR_1		0x01
#define CHARGE_OPTION_0_ADDR_2		0x00
#define MINIMUM_SYSTEM_VOLTAGE_ADDR	0x0D

#define EN_LWPWR					0b0
#define EN_OOA						0b1

uint8_t Get_Regulator_Connection_State(void);
void vRegulator(void const *pvParameters);

/* Used to guard access to the I2C in case messages are sent to the UART from
 more than one task. */
extern SemaphoreHandle_t xTxMutex_Regulator;
osThreadId regulatorTaskHandle;

#endif /* BQ25703A_REGULATOR_H_ */
