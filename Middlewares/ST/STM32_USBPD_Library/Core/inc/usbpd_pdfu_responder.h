/**
  ******************************************************************************
  * @file    usbpd_pdfu_responder.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for USBPD PDFU
  *          interface.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics. 
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _USBPD_PDURESPONDER_H
#define _USBPD_PDURESPONDER_H

/* Includes ------------------------------------------------------------------*/
#include "usbpd_core.h"
#ifdef _RTOS
#include "cmsis_os.h"
#endif /* _RTOS */

/** @addtogroup STM32_USBPD_PDFU
  * @{
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup USBPD_PDFU_RESPONDER_Exported_Constants USBPD PDFU RESPONDER exported Constants
  * @{
  */
/**
  * @brief  PDFU Constants used for PDFU action requested by user
  */
#define PDFU_EVENT_TIMER         (0u)
#define PDFU_EVENT_NONE          (1u)
#define PDFU_EVENT_RESPONDER     (2u)
/**
  * @}
  */

/* Exported variables --------------------------------------------------------*/
/** @defgroup USBPD_PDFU_RESPONDER_Exported_Varialbes USBPD PDFU RESPONDER exported Variables
  * @{
  */
#if defined(_RTOS)
#if defined(USBPD_PDFU_RESPONDER_C)
osMessageQId PDFUQueueId[USBPD_PORT_COUNT];
#else
extern osMessageQId PDFUQueueId[USBPD_PORT_COUNT];
#endif /* USBPD_PDFU_RESPONDER_C */
#endif /* _RTOS */

/**
  * @}
  */


/* Exported types ------------------------------------------------------------*/
/** @defgroup USBPD_PDFU_RESPONDER_Exported_Types USBPD PDFU RESPONDER exported Types
  * @{
  */
/** @defgroup USBPD_PDFU_CallBacks_structure_definition PDFU CallBacks structure definition
  * @brief  PDFU CallBacks exposed by the PDFU to the DPM
  * @{
  */
typedef struct
{
  /**
    * @brief  Retrieve information from DPM.
    * @param  PortNum Port number
    * @param  MsgId   FWUPD Msg ID based on @ref USBPD_FWUPD_MsgType_TypeDef
    * @param  Ptr Pointer on address where DPM data should be written
    * @param  Size    Pointer on nb of bytes written by DPM
    * @retval USBPD Status
    */
  USBPD_StatusTypeDef (*USBPD_PDFU_GetDataInfo)(uint8_t PortNum, USBPD_FWUPD_MsgType_TypeDef MsgId, uint8_t *Ptr, uint32_t *Size);

  /**
    * @brief  Update FW Update information in DPM.
    * @param  PortNum Port number
    * @param  MsgId   FWUPD Msg ID based on @ref USBPD_FWUPD_MsgType_TypeDef
    * @param  Ptr Pointer on address where DPM data to be updated could be read
    * @param  Size    Nb of byte to be updated in DPM
    * @retval USBPD status
    */
  USBPD_StatusTypeDef (*USBPD_PDFU_SetDataInfo)(uint8_t PortNum, USBPD_FWUPD_MsgType_TypeDef MsgId, uint8_t *Ptr, uint32_t Size);

  /**
    * @brief  Inform DPM than PDFU responder has entered in manifestation flow
    * @param  PortNum Port number
    * @retval None
    */
  void (*USBPD_PDFU_InformManifestation)(uint8_t PortNum);

}USBPD_PDFU_Callbacks;

/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/** @defgroup USBPD_PDFU_RESPONDER_Exported_Functions USBPD PDFU RESPONDER exported functions
  * @{
  */

/**
  * @brief  Initialization function
  * @param  PortNum       Index of Current used port
  * @param  pParams       Pointer on @ref USBPD_ParamsTypeDef structure
  * @param  PDFUCallbacks PDFU callbacks linked to DPM functions (based on @ref USBPD_PDFU_Callbacks)
  * @retval None
  */
void      USBPD_PDFU_init(uint8_t PortNum, USBPD_ParamsTypeDef *pParams, const USBPD_PDFU_Callbacks *PDFUCallbacks);
void      USBPD_PDFU_CreateTask(uint8_t PortNum);
void      USBPD_PDFU_TerminateTask(uint8_t PortNum);
void      USBPD_PDFU_TimerCounter(uint8_t PortNum);
#if !defined(_RTOS)
uint32_t  PDFU_Task(uint8_t PortNum, uint8_t Event);
#endif /* !_RTOS */

/**
  * @brief  FW UPDATE Request received
  * @param  PortNum   Index of Current used port
  * @param  ptrData   Pointer on data received in FW UPDATE REQ message
  * @param  DataSize  Size of the data
  * @retval none
  */
void USBPD_PDFU_FWUpdateRequestReceived(uint8_t PortNum, uint8_t *ptrData, uint16_t DataSize);

/**
  * @}
  */

/**
  * @}
  */

#endif /* _USBPD_PDURESPONDER_H */

