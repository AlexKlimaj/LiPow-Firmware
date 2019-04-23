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

#ifndef HW_TRACE_H
#define HW_TRACE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported functions ---------------------------------------------------------*/

void  HW_TRACER_EMB_Init(void);
void  HW_TRACER_EMB_DeInit(void);
void  HW_TRACER_EMB_RegisterRxCallback(void (*callbackRX)(uint8_t, uint8_t));
#if TRACER_EMB_DMA_MODE == 1UL
void  HW_TRACER_EMB_IRQHandlerDMA(void);
#endif
void  HW_TRACER_EMB_IRQHandlerUSART(void);
void  HW_TRACER_EMB_StartRX(void);
void  HW_TRACER_EMB_SendData(uint8_t *data, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif /* HW_TRACE_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
