/**
  ******************************************************************************
  * @file    trace_emb.h
  * @author  MCD Application Team
  * @brief   This file contains the headers of usbpd_cad.h for Cable Attach-Detach
  *          controls.
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
#ifndef __TRACER_EMB_H_
#define __TRACER_EMB_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "tracer_emb_conf.h"

/** @addtogroup STM32_USBPD_LIBRARY
  * @{
  */

/** @addtogroup USBPD_CORE
  * @{
  */

/** @addtogroup USBPD_CORE_TRACE
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/**
  * @}
  */

/* Exported define -----------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @defgroup USBPD_CORE_TRACER_EMB_Exported_Functions USBPD CORE TRACE Exported Functions
  * @{
  */
/**
  * @brief  Initialize the TRACE module
  * @retval None
  */
void            TRACER_EMB_Init(void);

/**
  * @brief  Add information in debug trace buffer
  * @param  Type    Trace Type based on @ref TRACER_EMB_EVENT
  * @param  PortNum Port number value
  * @param  Sop     SOP type
  * @param  Ptr     Pointer on the data to send
  * @param  Size    Size of the data to send
  * @retval None.
  */
void            TRACER_EMB_Add(uint8_t *Ptr, uint32_t Size);

/**
  * @brief  Add one data into the trace buffer
  * @param  pos position
  * @param  data data value
  */
void TRACER_EMB_WriteData(uint16_t pos, uint8_t data);

/**
  * @brief  Lock the trace buffer.
  * @param  None.
  * @retval None.
  */
void TRACER_EMB_Lock(void);

/**
  * @brief  UnLock the trace buffer.
  * @param  None.
  * @retval None.
  */
void TRACER_EMB_UnLock(void);


/**
  * @brief  allocate space inside the buffer to push data
  * @param  data size
  * @retval write position inside the buffer is -1 no space available.
  */
int32_t TRACER_EMB_AllocateBufer(uint32_t Size);

/**
  * @brief  Main Trace TX process to push data on the media.
  * @retval Timing
  */
uint32_t TRACER_EMB_TX_Process(void);


void TRACER_EMB_CALLBACK_TX(void);

#if TRACER_EMB_DMA_MODE == 1UL
void TRACER_EMB_IRQHandlerDMA(void);
#endif

void TRACER_EMB_IRQHandlerUSART(void);


void TRACER_EMB_StartRX(void (*callbackRX)(uint8_t, uint8_t));

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __USBPD_CAD_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
