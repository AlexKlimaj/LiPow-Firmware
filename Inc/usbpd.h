/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usbpd.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __usbpd_H
#define __usbpd_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbpd_core.h"
#include "usbpd_dpm_core.h"
#include "usbpd_dpm_conf.h"
#include "usbpd_hw_if.h"

/* USER CODE BEGIN 0 */
#define VOLTAGE_CHOICE_ARRAY_SIZE 5
#define INPUT_VOLTAGE_VALID_THRESH_MV 1000

#define NO_USB_PD_SUPPLY 2
#define READY 1
#define NOT_READY 0

/* USER CODE END 0 */

/* Global variables ---------------------------------------------------------*/

/* USER CODE BEGIN 1 */
/* USER CODE END 1 */

/* USBPD init function */
void            MX_USBPD_Init(void);

/* USER CODE BEGIN 2 */

uint8_t Get_Input_Power_Ready(void);
uint32_t Get_Max_Input_Power(void);
uint32_t Get_Max_Input_Current(void);
uint32_t Get_Input_Voltage(void);

/* USER CODE END 2 */

#ifdef __cplusplus
}
#endif
#endif /*__usbpd_H */

/**
  * @}
  */

/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
