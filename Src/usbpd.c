/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usbpd.c
  * @author  MCD Application Team
  * @brief   This file contains the device define.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbpd.h"

/* USER CODE BEGIN 0 */

#include "usbpd_core.h"
#include "usbpd_dpm_core.h"
#include "usbpd_dpm_conf.h"
#include "usbpd_dpm_user.h"
#include "usbpd_pwr_if.h"

#include "printf.h"

/* USER CODE END 0 */

/* USER CODE BEGIN 1 */
/* USER CODE END 1 */

/* Global variables ---------------------------------------------------------*/

/* USER CODE BEGIN 2 */
/* USER CODE END 2 */

/* USBPD init function */
void MX_USBPD_Init(void)
{

  /* Global Init of USBPD HW */
  USBPD_HW_IF_GlobalHwInit();

  /* Initialize the Device Policy Manager */
  if(USBPD_OK != USBPD_DPM_InitCore())
  {
    while(1);
  }

  /* Initialise the DPM application */
  if (USBPD_OK != USBPD_DPM_UserInit())
  {
    while(1);
  }

  /* USER CODE BEGIN 3 */

  TickType_t xDelay = 500 / portTICK_PERIOD_MS;

  vTaskDelay(xDelay);

  USBPD_DPM_RequestSourceCapability(0);

  /* USER CODE END 3 */

}

/* USER CODE BEGIN 4 */

/**
  * @brief  Gets the source's max voltage
  * @retval None
  */
void Get_source_capabilities()
{
  uint8_t _str[25];
  uint8_t PortNum = 0;
  uint8_t _max = DPM_Ports[PortNum].DPM_NumberOfRcvSRCPDO;

  for(uint8_t index=0; index < _max; index++)
  {
    switch((DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[index] & USBPD_PDO_TYPE_Msk) >> USBPD_PDO_TYPE_Pos)
    {
    case USBPD_CORE_PDO_TYPE_FIXED :
      {
        uint32_t maxcurrent = ((DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[index] & USBPD_PDO_SRC_FIXED_MAX_CURRENT_Msk) >> USBPD_PDO_SRC_FIXED_MAX_CURRENT_Pos)*10;
        uint32_t maxvoltage = ((DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[index] & USBPD_PDO_SRC_FIXED_VOLTAGE_Msk) >> USBPD_PDO_SRC_FIXED_VOLTAGE_Pos)*50;
        sprintf((char*)_str, " FIXED:%2dV %2d.%dA ", (maxvoltage/1000), (maxcurrent/1000), ((maxcurrent % 1000) /100));
      }
      break;
    case USBPD_CORE_PDO_TYPE_BATTERY :
      {
        uint32_t maxvoltage = ((DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[index] & USBPD_PDO_SRC_BATTERY_MAX_VOLTAGE_Msk) >> USBPD_PDO_SRC_BATTERY_MAX_VOLTAGE_Pos) * 50;
        uint32_t minvoltage = ((DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[index] & USBPD_PDO_SRC_BATTERY_MIN_VOLTAGE_Msk) >> USBPD_PDO_SRC_BATTERY_MIN_VOLTAGE_Pos) * 50;
        uint32_t maxpower = ((DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[index] & USBPD_PDO_SRC_BATTERY_MAX_POWER_Msk) >> USBPD_PDO_SRC_BATTERY_MAX_POWER_Pos) * 250;
        sprintf((char*)_str, " BATT:%2d.%1d-%2d.%1dV %2d.%dW", (minvoltage/1000),(minvoltage/100)%10, (maxvoltage/1000),(maxvoltage/100)%10, (maxpower/1000), (maxpower%1000)/100);
      }
      break;
    case USBPD_CORE_PDO_TYPE_VARIABLE :
      {
        uint32_t maxvoltage = ((DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[index] & USBPD_PDO_SRC_VARIABLE_MAX_VOLTAGE_Msk) >> USBPD_PDO_SRC_VARIABLE_MAX_VOLTAGE_Pos) * 50;
        uint32_t minvoltage = ((DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[index] & USBPD_PDO_SRC_VARIABLE_MIN_VOLTAGE_Msk) >> USBPD_PDO_SRC_VARIABLE_MIN_VOLTAGE_Pos) * 50;
        uint32_t maxcurrent = ((DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[index] & USBPD_PDO_SRC_VARIABLE_MAX_CURRENT_Msk) >> USBPD_PDO_SRC_VARIABLE_MAX_CURRENT_Pos) * 10;
        sprintf((char*)_str, " VAR: %2d.%1d-%2d.%1dV %2d.%dA", (minvoltage/1000),(minvoltage/100)%10, (maxvoltage/1000),(maxvoltage/100)%10, (maxcurrent/1000), ((maxcurrent % 1000) /100));
      }
      break;
    case USBPD_CORE_PDO_TYPE_APDO :
      {
        uint32_t minvoltage = ((DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[index] & USBPD_PDO_SRC_APDO_MIN_VOLTAGE_Msk) >> USBPD_PDO_SRC_APDO_MIN_VOLTAGE_Pos) * 100;
        uint32_t maxvoltage = ((DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[index] & USBPD_PDO_SRC_APDO_MAX_VOLTAGE_Msk) >> USBPD_PDO_SRC_APDO_MAX_VOLTAGE_Pos) * 100;
        uint32_t maxcurrent = ((DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[index] & USBPD_PDO_SRC_APDO_MAX_CURRENT_Msk) >> USBPD_PDO_SRC_APDO_MAX_CURRENT_Pos) * 50;
        sprintf((char*)_str, " APDO:%2d.%1d-%2d.%1dV %2d.%dA", (minvoltage/1000),(minvoltage/100)%10, (maxvoltage/1000),(maxvoltage/100)%10, (maxcurrent/1000), ((maxcurrent % 1000) /100));
      }
      break;
    default :
      sprintf((char*)_str,"Not yet managed by demo");
      break;
    }
  }
}

/* USER CODE END 4 */

/**
  * @}
  */
 
/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
