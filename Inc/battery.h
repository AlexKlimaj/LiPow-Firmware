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

void vCreateBatteryTask(void);

uint8_t Get_XT60_Connection_State(void);

uint8_t Get_Balance_Connection_State(void);

uint8_t Get_Number_Of_Cells(void);

uint8_t Get_Balancing_State(void);

uint8_t Get_Charging_State(void);

#endif /* BATTERY_H_ */
