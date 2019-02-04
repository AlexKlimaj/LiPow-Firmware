/**
 ******************************************************************************
 * @file           : adc_interface.h
 * @brief          : Header for adc_interface.c file.
 ******************************************************************************
 */

#ifndef ADC_INTERFACE_H_
#define ADC_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32g0xx_hal.h"

// Calculate the voltages based on the following data:
// https://docs.google.com/spreadsheets/d/1uXEK7AOaoLDN3ATuCtUCRYjI7-zBhgmvHDWbn-7-k90/edit?usp=sharing

#define ADC_FILTER_SUM_COUNT		(unsigned)380

#define BATTERY_ADC_MULTIPLIER 		(unsigned)100000

#define BATTERY_ADC_OFFSET 			(unsigned)( 0.0542 * BATTERY_ADC_MULTIPLIER )
#define BATTERY_ADC_SCALAR 			(unsigned)( 0.00422 * BATTERY_ADC_MULTIPLIER )
#define BATTERY_MIN_ADC_READING 	(unsigned)10

#define CELL_ONE_ADC_OFFSET 		(unsigned)( 0.0102 * BATTERY_ADC_MULTIPLIER )
#define CELL_ONE_ADC_SCALAR 		(unsigned)( 0.00111 * BATTERY_ADC_MULTIPLIER )
#define CELL_ONE_MIN_ADC_READING 	(unsigned)10

#define CELL_TWO_ADC_OFFSET 		(unsigned)( 0.00573 * BATTERY_ADC_MULTIPLIER )
#define CELL_TWO_ADC_SCALAR 		(unsigned)( 0.00206 * BATTERY_ADC_MULTIPLIER )
#define CELL_TWO_MIN_ADC_READING 	(unsigned)10

#define CELL_THREE_ADC_OFFSET 		(unsigned)( 0.0375 * BATTERY_ADC_MULTIPLIER )
#define CELL_THREE_ADC_SCALAR 		(unsigned)( 0.00307 * BATTERY_ADC_MULTIPLIER )
#define CELL_THREE_MIN_ADC_READING 	(unsigned)10

#define CELL_FOUR_ADC_OFFSET 		(unsigned)( 0.0542 * BATTERY_ADC_MULTIPLIER )
#define CELL_FOUR_ADC_SCALAR 		(unsigned)( 0.00422 * BATTERY_ADC_MULTIPLIER )
#define CELL_FOUR_MIN_ADC_READING 	(unsigned)10

void vCreateADCTask(void);

uint32_t Get_Battery_Voltage(void);

uint32_t Get_Cell_One_Voltage(void);

uint32_t Get_Cell_Two_Voltage(void);

uint32_t Get_Cell_Three_Voltage(void);

uint32_t Get_Cell_Four_Voltage(void);

uint32_t Get_MCU_Temperature(void);

#ifdef __cplusplus
}
#endif

#endif /* ADC_INTERFACE_H_ */
