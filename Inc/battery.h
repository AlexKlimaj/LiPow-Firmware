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

#define VOLTAGE_CONNECTED_THRESHOLD		(uint32_t)( 0.5 * BATTERY_ADC_MULTIPLIER )

#define CONNETED		1
#define NOT_CONNECTED	0

#define THREE_S_BITMASK 0b0111
#define TWO_S_BITMASK	0b0011
#define ONE_S_BITMASK	0b0001

#define CELL_CONNECTION_ERROR 1

void vCreateBatteryTask( uint16_t usStackSize, unsigned portBASE_TYPE uxPriority );

uint8_t Get_XT60_Connection_State(void);

uint8_t Get_Balance_Connection_State(void);

uint8_t Get_Number_Of_Cells(void);

uint8_t Get_Balancing_State(void);

uint8_t Get_Charging_State(void);

#endif /* BATTERY_H_ */
