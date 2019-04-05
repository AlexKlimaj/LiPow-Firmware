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
#include "cmsis_os.h"

#define VOLTAGE_CONNECTED_THRESHOLD			(uint32_t)( 0.5 * BATTERY_ADC_MULTIPLIER )
#define CELL_DELTA_V_ENABLE_BALANCING		(uint32_t)( 0.06 * BATTERY_ADC_MULTIPLIER )
#define CELL_BALANCING_HYSTERESIS_V			(uint32_t)( 0.03 * BATTERY_ADC_MULTIPLIER )
#define MIN_CELL_V_FOR_BALANCING			(uint32_t)( 3.0 * BATTERY_ADC_MULTIPLIER )
#define MAX_CELL_VOLTAGE_TO_ENABLE_CHARGING	(uint32_t)( 4.185 * BATTERY_ADC_MULTIPLIER )
#define CELL_CHARGING_HYSTERESIS_V			(uint32_t)( 4.195 * BATTERY_ADC_MULTIPLIER )

#define MAX_MCU_TEMP_C_FOR_OPERATION	75
#define MCU_TEMP_C_RECOVERY				65

#define THREE_S_BITMASK 		0b0111
#define TWO_S_BITMASK			0b0011
#define ONE_S_BITMASK			0b0001

void Battery_Connection_State();

uint8_t Get_XT60_Connection_State(void);

uint8_t Get_Balance_Connection_State(void);

uint8_t Get_Number_Of_Cells(void);

uint8_t Get_Balancing_State(void);

uint8_t Get_Charging_State(void);

#endif /* BATTERY_H_ */
