/**
  ******************************************************************************
  * @file    usbpd_porthandle.h
  * @author  MCD Application Team
  * @brief   This file contains the headers of usbpd_porthandle.h.
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

#ifndef __USBPD_PORTHANDLE_H_
#define __USBPD_PORTHANDLE_H_

#ifdef __cplusplus
extern "C" {
#endif 
  
/* Includes ------------------------------------------------------------------*/
  
/**
* @addtogroup USBPD_PHY
* @{
* */
  
/* Exported typedef ----------------------------------------------------------*/

/** @defgroup USBPD_PORT_HandleTypeDef USB PD handle Structure definition for USBPD_PHY_HW_IF
  * @brief  USBPD PORT handle Structure definition
  * @{
  */
typedef struct
{
  UCPD_TypeDef                *husbpd;          /*!< UCPD Handle parameters             */
  DMA_Channel_TypeDef         *hdmatx;          /*!< Tx DMA Handle parameters           */
  DMA_Channel_TypeDef         *hdmarx;          /*!< Rx DMA Handle parameters           */
  
  USBPD_SettingsTypeDef       *settings;
  USBPD_ParamsTypeDef         *params;
  USBPD_HW_IF_Callbacks        cbs;             /*!< USBPD_PHY_HW_IF callbacks         */

  __IO uint32_t                CC1;             /*!< CC1 detection state               */
  __IO uint32_t                CC2;             /*!< CC2 detection state               */
  
  void (*USBPD_CAD_WakeUp)(void);               /*!< function used to wakeup cad task   */

  uint8_t                     *ptr_RxBuff;     /*!< Pointer to Raw Rx transfer Buffer  */
  
  CCxPin_TypeDef               CCx;             /*!< CC pin used for communication      */ 
} USBPD_PORT_HandleTypeDef;
/** 
  * @}
  */

/* Exported define -----------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
  
/**
* @}
*/
  
#ifdef __cplusplus
}
#endif

#endif /* __USBPD_PORTHANDLE_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
