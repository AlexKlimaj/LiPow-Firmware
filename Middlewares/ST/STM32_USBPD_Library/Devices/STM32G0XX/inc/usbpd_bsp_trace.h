/**
  ******************************************************************************
  * @file    usbpd_bsp_trace.h
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

#ifndef USBPD_BSP_TRACE_H
#define USBPD_BSP_TRACE_H
/* Includes ------------------------------------------------------------------*/

/* Exported functions ---------------------------------------------------------*/

void                    BSP_TRACE_Init(void (*callbackTX)(void), void (*callbackRX)(uint8_t, uint8_t));
void                    BSP_TRACE_DeInit(void);
void                    BSP_TRACE_RegisterRxCallback(void (*callbackRX)(uint8_t, uint8_t));
void                    BSP_TRACE_IRQHandlerDMA(void);
void                    BSP_TRACE_StartRX(void);
void                    BSP_TRACE_IRQHandlerUSART(void);
void                    BSP_TRACE_SendData(uint8_t *data, uint32_t size);

#endif /* USBPD_BSP_TRACE_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
