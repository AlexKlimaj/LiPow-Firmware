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

#define BATTERY_ADC_OFFSET 4.48f
#define BATTERY_ADC_SCALAR 0.004237f

#define CELL_ONE_ADC_OFFSET 9.28f
#define CELL_ONE_ADC_SCALAR 0.001111f


float Get_Battery_Voltage(void);
uint8_t Set_Battery_Voltage(float voltage_v);

float Get_Cell_One_Voltage(void);
uint8_t Set_Cell_One_Voltage(float voltage_v);

float Get_Cell_Two_Voltage(void);
uint8_t Set_Cell_Two_Voltage(float voltage_v);

float Get_Cell_Three_Voltage(void);
uint8_t Set_Cell_Three_Voltage(float voltage_v);

float Get_Cell_Four_Voltage(void);
uint8_t Set_Cell_Four_Voltage(float voltage_v);

float Get_MCU_Temperature(void);
uint8_t Set_MCU_Temperature(float temperature_c);


#ifdef __cplusplus
}
#endif

#endif /* ADC_INTERFACE_H_ */
