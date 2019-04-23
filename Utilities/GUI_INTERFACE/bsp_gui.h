/**
  ******************************************************************************
  * @file    bsp_gui.h
  * @author  MCD Application Team
  * @brief   This file contains bsp interface control functions.
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

#ifndef BSP_GUI_H
#define BSP_GUI_H
/* Includes ------------------------------------------------------------------*/
#if defined(_GUI_INTERFACE)
#include "usbpd_def.h"
#endif /* _GUI_INTERFACE */

/* Private typedef -----------------------------------------------------------*/
typedef enum {
  GUI_OK,
  GUI_ERASE_ERROR,
  GUI_WRITE_ERROR,
  GUI_ERROR
} GUI_StatusTypeDef;

/* Variable containing ADC conversions results */
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

GUI_StatusTypeDef     BSP_GUI_LoadDataFromFlash(void);
GUI_StatusTypeDef     BSP_GUI_SaveDataInFlash(void);

#endif /* BSP_GUI_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

