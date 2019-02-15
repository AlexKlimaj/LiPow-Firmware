/**
 ******************************************************************************
 * @file           : battery.h
 * @brief          : Header for battery.c file.
 ******************************************************************************
 */

#ifndef BATTERY_H_
#define BATTERY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32g0xx_hal.h"
#include "FreeRTOS.h"
#include "main.h"

#define VOLTAGE_CONNECTED_THRESHOLD		(uint32_t)( 0.5 * BATTERY_ADC_MULTIPLIER )
#define CELL_DELTA_V_ENABLE_BALANCING	(uint32_t)( 0.08 * BATTERY_ADC_MULTIPLIER )
#define CELL_BALANCING_HYSTERESIS_V		(uint32_t)( 0.04 * BATTERY_ADC_MULTIPLIER )
#define MIN_CELL_V_FOR_BALANCING		(uint32_t)( 3.0 * BATTERY_ADC_MULTIPLIER )

#define CONNECTED				(uint8_t)1
#define NOT_CONNECTED			(uint8_t)0

#define THREE_S_BITMASK 		0b0111
#define TWO_S_BITMASK			0b0011
#define ONE_S_BITMASK			0b0001

#define NO_ERROR				(uint8_t)0
#define CELL_CONNECTION_ERROR 	(uint8_t)1
#define CELL_VOLTAGE_ERROR		(uint8_t)2
#define XT60_VOLTAGE_ERROR		(uint8_t)3

void Battery_Connection_State();

uint8_t Get_XT60_Connection_State(void);

uint8_t Get_Balance_Connection_State(void);

uint8_t Get_Number_Of_Cells(void);

uint8_t Get_Balancing_State(void);

uint8_t Get_Charging_State(void);

uint8_t Get_Battery_Error_State(void);

#endif /* BATTERY_H_ */
