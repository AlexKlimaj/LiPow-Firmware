/**
  ******************************************************************************
  * @file    usbpd_dpm_core.h
  * @author  MCD Application Team
  * @brief   Header file for usbpd_dpm_core.c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#ifndef __USBPD_DPM_CORE_H_
#define __USBPD_DPM_CORE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported typedef ----------------------------------------------------------*/

/* Exported define -----------------------------------------------------------*/
#ifdef _RTOS
#if defined(USBPD_TCPM_MODULE_ENABLED)
#define TCPM_ALARMBOX_MESSAGES_MAX      (2U * USBPD_PORT_COUNT)
#endif /* USBPD_TCPM_MODULE_ENABLED */
#endif /* _RTOS */

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern USBPD_ParamsTypeDef DPM_Params[USBPD_PORT_COUNT];

/* Exported functions --------------------------------------------------------*/
USBPD_StatusTypeDef USBPD_DPM_Init(void);
#ifdef _SIMULATOR /* defined only for simulation mode */
void USBPD_DPM_Run(void);
#endif
void                USBPD_DPM_TimerCounter(void);

#ifdef __cplusplus
}
#endif

#endif /* __USBPD_DPM_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
