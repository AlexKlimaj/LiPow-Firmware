/**
  ******************************************************************************
  * @file    usbpd_pwr_hw_if.c
  * @author  MCD Application Team
  * @brief   This file contains power hardware interface functions.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usbpd_hw_if.h"


#define _USBPD_POWER_DEBUG
#if  defined(_TRACE)
#include "usbpd_core.h"
#include "usbpd_trace.h"
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#if  defined(_TRACE)
#define POWER_DEBUG(__MSG__,__SIZE__)   USBPD_TRACE_Add(USBPD_TRACE_DEBUG, PortNum, 0,__MSG__,__SIZE__);
#else
#define POWER_DEBUG(__MSG__,__SIZE__)
#endif

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
USBPD_StatusTypeDef HW_IF_PWR_SetVoltage(uint8_t PortNum, uint16_t voltage)
{
  UNUSED(PortNum);
  UNUSED(voltage);
  return USBPD_OK;
}

uint16_t HW_IF_PWR_GetVoltage(uint8_t PortNum)
{
  uint32_t _voltage;
  BSP_USBPD_PWR_VBUSGetVoltage(PortNum, &_voltage);
  return (uint16_t)_voltage;
}

int16_t HW_IF_PWR_GetCurrent(uint8_t PortNum)
{
  int32_t _current;
  BSP_USBPD_PWR_VBUSGetCurrent(PortNum, &_current);
  return (int16_t)_current;
}

#if defined(_SRC) || defined(_DRP)
USBPD_StatusTypeDef HW_IF_PWR_Enable(uint8_t PortNum, USBPD_FunctionalState state, CCxPin_TypeDef Cc, uint32_t VconnState, USBPD_PortPowerRole_TypeDef role)
{
  UNUSED(role);
  int32_t status;
  if (USBPD_ENABLE == state)
  {
#if defined(_VCONN_SUPPORT)
    if (USBPD_TRUE == VconnState)
    {
      POWER_DEBUG((uint8_t *)"VCONN ON", 8);
      (void)BSP_USBPD_PWR_VCONNOn(PortNum, Cc);
    }
#endif /* _VCONN_SUPPORT */
    POWER_DEBUG((uint8_t *)"VBUS ON", 7);
    status = BSP_USBPD_PWR_VBUSOn(PortNum);
  }
  else
  {
#if defined(_VCONN_SUPPORT)
    if (VconnState == USBPD_TRUE)
    {
      POWER_DEBUG((uint8_t *)"VCONN OFF", 9);
      (void)BSP_USBPD_PWR_VCONNOff(PortNum, Cc);
    }
#endif /* _VCONN_SUPPORT */
    POWER_DEBUG((uint8_t *)"VBUS OFF", 8);
    status = BSP_USBPD_PWR_VBUSOff(PortNum);
  }
  return (status == BSP_ERROR_NONE) ? USBPD_OK : USBPD_FAIL;
}
#endif /* _SRC || _DRP */

USBPD_FunctionalState HW_IF_PWR_VBUSIsEnabled(uint8_t PortNum)
{
#if defined(_SRC)||defined(_DRP)
  uint8_t _state;
  BSP_USBPD_PWR_VBUSIsOn(PortNum, &_state);
  return (_state == BSP_ERROR_NONE) ? USBPD_DISABLE : USBPD_ENABLE;
#else
  return USBPD_DISABLE;
#endif /* _SRC || _DRP */
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
