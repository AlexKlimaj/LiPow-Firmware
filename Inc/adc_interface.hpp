/**
 ******************************************************************************
 * @file           : adc_interface.h
 * @brief          : Header for adc_interface.c file.
 ******************************************************************************
 */

#ifndef ADC_INTERFACE_HPP_
#define ADC_INTERFACE_HPP_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "stm32g0xx_hal.h"
#include "FreeRTOS.h"

// Calculate the voltages based on the following data:
// https://docs.google.com/spreadsheets/d/1uXEK7AOaoLDN3ATuCtUCRYjI7-zBhgmvHDWbn-7-k90/edit?usp=sharing

#define ADC_FILTER_SUM_COUNT		380

#define BATTERY_ADC_MULTIPLIER 		100000

#define BATTERY_ADC_OFFSET 			(int32_t)( 5424 )
#define BATTERY_ADC_SCALAR 			(uint32_t)( 422 )
#define BATTERY_MIN_ADC_READING 	20

#define CELL_ONE_ADC_OFFSET 		(int32_t)( -1023 )
#define CELL_ONE_ADC_SCALAR 		(uint32_t)( 111 )
#define CELL_ONE_MIN_ADC_READING 	20

#define CELL_TWO_ADC_OFFSET 		(int32_t)( -2157 )
#define CELL_TWO_ADC_SCALAR 		(uint32_t)( 214 )
#define CELL_TWO_MIN_ADC_READING 	20

#define CELL_THREE_ADC_OFFSET 		(int32_t)( -3749 )
#define CELL_THREE_ADC_SCALAR 		(uint32_t)( 307 )
#define CELL_THREE_MIN_ADC_READING 	20

#define CELL_FOUR_ADC_OFFSET 		(int32_t)( 5298 )
#define CELL_FOUR_ADC_SCALAR 		(uint32_t)( 423 )
#define CELL_FOUR_MIN_ADC_READING 	20

void vCreateADCTask( uint16_t usStackSize, unsigned portBASE_TYPE uxPriority );

uint32_t Get_Battery_Voltage(void);

uint32_t Get_Cell_One_Voltage(void);

uint32_t Get_Cell_Two_Voltage(void);

uint32_t Get_Cell_Three_Voltage(void);

uint32_t Get_Cell_Four_Voltage(void);

uint32_t Get_MCU_Temperature(void);

#ifdef __cplusplus
}
#endif

#endif /* ADC_INTERFACE_HPP_ */
