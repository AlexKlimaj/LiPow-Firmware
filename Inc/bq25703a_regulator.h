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

#define BQ26703A_I2C_ADDRESS		0xD6
#define BQ26703A_MANUFACTURER_ID	0x40
#define BQ26703A_DEVICE_ID			0x78

#define MANUFACTURER_ID_ADDR		0x2E
#define DEVICE_ID_ADDR				0x2F

void vCreateRegulatorTask(uint16_t usStackSize, unsigned portBASE_TYPE uxPriority);

#endif /* BQ25703A_REGULATOR_H_ */
