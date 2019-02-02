/**
  ******************************************************************************
  * @file           : adc_interface.c
  * @brief          : Interface for setting and getting information read in from the ADC
  ******************************************************************************
  */

#include "adc_interface.h"

struct Adc {
  float bat_voltage;
  float cell_1_voltage;
  float cell_2_voltage;
  float cell_3_voltage;
  float cell_4_voltage;
  float temperature;
};

struct Adc adc_values;

/**
* @brief Gets the battery voltage that was read in from the ADC
* @retval Battery voltage in volts
*/
float Get_Battery_Voltage(void)
{
  return adc_values.bat_voltage;
}

/**
  * @brief  Sets the battery voltage that was read in from the ADC
  * @param  voltage_v: Battery voltage in volts
  * @retval uint8_t 1 if successful, 0 if error
  */
uint8_t Set_Battery_Voltage(float voltage_v)
{
  if ((voltage_v < 0.0f) || (voltage_v > 18.0f))
    {
      return 0;
    }
  else
    {
      adc_values.bat_voltage = voltage_v;
      return 1;
    }
}

/**
* @brief Gets cell 1 voltage that was read in from the ADC
* @retval Cell 1 voltage in volts
*/
float Get_Cell_One_Voltage(void)
{
  return adc_values.cell_1_voltage;
}

/**
  * @brief  Sets the cell 1 voltage that was read in from the ADC
  * @param  voltage_v: cell 1 voltage in volts
  * @retval uint8_t 1 if successful, 0 if error
  */
uint8_t Set_Cell_One_Voltage(float voltage_v)
{
  if ((voltage_v < 0.0f) || (voltage_v > 18.0f))
    {
      return 0;
    }
  else
    {
      adc_values.cell_1_voltage = voltage_v;
      return 1;
    }
}

/**
* @brief Gets cell 2 voltage that was read in from the ADC
* @retval Cell 2 voltage in volts
*/
float Get_Cell_Two_Voltage(void)
{
  return adc_values.cell_2_voltage;
}

/**
  * @brief  Sets the cell 2 voltage that was read in from the ADC
  * @param  voltage_v: cell 2 voltage in volts
  * @retval uint8_t 1 if successful, 0 if error
  */
uint8_t Set_Cell_Two_Voltage(float voltage_v)
{
  if ((voltage_v < 0.0f) || (voltage_v > 18.0f))
    {
      return 0;
    }
  else
    {
      adc_values.cell_2_voltage = voltage_v;
      return 1;
    }
}

/**
* @brief Gets cell 3 voltage that was read in from the ADC
* @retval Cell 3 voltage in volts
*/
float Get_Cell_Three_Voltage(void)
{
  return adc_values.cell_3_voltage;
}

/**
  * @brief  Sets the cell 3 voltage that was read in from the ADC
  * @param  voltage_v: cell 3 voltage in volts
  * @retval uint8_t 1 if successful, 0 if error
  */
uint8_t Set_Cell_Three_Voltage(float voltage_v)
{
  if ((voltage_v < 0.0f) || (voltage_v > 18.0f))
    {
      return 0;
    }
  else
    {
      adc_values.cell_3_voltage = voltage_v;
      return 1;
    }
}

/**
* @brief Gets cell 4 voltage that was read in from the ADC
* @retval Cell 4 voltage in volts
*/
float Get_Cell_Four_Voltage(void)
{
  return adc_values.cell_4_voltage;
}

/**
  * @brief  Sets the cell 4 voltage that was read in from the ADC
  * @param  voltage_v: cell 4 voltage in volts
  * @retval uint8_t 1 if successful, 0 if error
  */
uint8_t Set_Cell_Four_Voltage(float voltage_v)
{
  if ((voltage_v < 0.0f) || (voltage_v > 18.0f))
    {
      return 0;
    }
  else
    {
      adc_values.cell_4_voltage = voltage_v;
      return 1;
    }
}

/**
* @brief Gets mcu junction temperature that was read in from the ADC
* @retval MCU junction temperature in celcius
*/
float Get_MCU_Temperature(void)
{
  return adc_values.temperature;
}

/**
  * @brief  Sets the mcu junction temperature that was read in from the ADC
  * @param  temperature_c: MCU junction temperature in celcius
  * @retval uint8_t 1 if successful, 0 if error
  */
uint8_t Set_MCU_Temperature(float temperature_c)
{
  if ((temperature_c < -200.0f) || (temperature_c > 200.0f))
    {
      return 0;
    }
  else
    {
      adc_values.temperature = temperature_c;
      return 1;
    }
}
