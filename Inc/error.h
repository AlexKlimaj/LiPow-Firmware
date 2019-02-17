/**
 ******************************************************************************
 * @file           : error.h
 * @brief          : Header for error.c file.
 ******************************************************************************
 */

#ifndef ERROR_H_
#define ERROR_H_

#include "stm32g0xx_hal.h"
#include "FreeRTOS.h"

#define CONNECTED				1
#define NOT_CONNECTED			0

#define NO_ERROR						0b000000
#define CELL_CONNECTION_ERROR 			0b000001
#define CELL_VOLTAGE_ERROR				0b000010
#define XT60_VOLTAGE_ERROR				0b000100
#define MCU_OVER_TEMP					0b001000
#define REGULATOR_COMMUNICATION_ERROR	0b010000
#define VOLTAGE_INPUT_ERROR				0b100000

uint32_t Get_Error_State(void);

void Set_Error_State(uint32_t error_bitmask);

void Clear_Error_State(uint32_t error_bitmask);

#endif /* ERROR_H_ */
