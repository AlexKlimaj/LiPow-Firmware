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
#include "FreeRTOS.h"
#include "cmsis_os.h"

#define ADC_FILTER_SUM_COUNT		380

#define BATTERY_ADC_MULTIPLIER 		1000000

#define BATTERY_ADC_OFFSET 			(int32_t)( 0 )
#define BATTERY_ADC_SCALAR 			(uint32_t)( 4500 )
#define BATTERY_MIN_ADC_READING 	20

#define CELL_ONE_ADC_OFFSET 		(int32_t)( 0 )
#define CELL_ONE_ADC_SCALAR 		(uint32_t)( 1110 )
#define CELL_ONE_MIN_ADC_READING 	20

#define CELL_TWO_ADC_OFFSET 		(int32_t)( 0 )
#define CELL_TWO_ADC_SCALAR 		(uint32_t)( 2150 )
#define CELL_TWO_MIN_ADC_READING 	20

#define CELL_THREE_ADC_OFFSET 		(int32_t)( 0 )
#define CELL_THREE_ADC_SCALAR 		(uint32_t)( 3270 )
#define CELL_THREE_MIN_ADC_READING 	20

#define CELL_FOUR_ADC_OFFSET 		(int32_t)( 0 )
#define CELL_FOUR_ADC_SCALAR 		(uint32_t)( 4500 )
#define CELL_FOUR_MIN_ADC_READING 	20

#define CELL_MAX_VOLTAGE			(uint32_t)( 5 * BATTERY_ADC_MULTIPLIER )
#define TWO_S_MAX_VOLTAGE			(uint32_t)( 10 * BATTERY_ADC_MULTIPLIER )
#define THREE_S_MAX_VOLTAGE			(uint32_t)( 15 * BATTERY_ADC_MULTIPLIER )
#define FOUR_S_MAX_VOLTAGE			(uint32_t)( 20 * BATTERY_ADC_MULTIPLIER )

void vRead_ADC(void const *pvParameters);

uint8_t Calibrate_ADC(float reference_voltage_mv);

uint32_t Get_Battery_Voltage(void);

uint32_t Get_Cell_Voltage(uint8_t cell_number);

uint32_t Get_Two_S_Voltage(void);

uint32_t Get_Three_S_Voltage(void);

uint32_t Get_Four_S_Voltage(void);

int32_t Get_MCU_Temperature(void);

uint32_t Get_VDDa(void);

osThreadId adcTaskHandle;

#ifdef __cplusplus
}
#endif

#endif /* ADC_INTERFACE_H_ */
