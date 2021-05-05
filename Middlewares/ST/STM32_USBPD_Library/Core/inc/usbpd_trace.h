/**
  ******************************************************************************
  * @file    usbpd_trace.h
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
#ifndef __USBPD_TRACE_H_
#define __USBPD_TRACE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbpd_core.h"
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
/* Exported define -----------------------------------------------------------*/
typedef enum {
  TCPM_TRACE_CORE_ALERT,    /*!< ALERT_TASK */
  TCPM_TRACE_ALERT,         /*!< USBPD_TCPM_alert */
  TCPM_TRACE_ALERT_CLEAR,   /*!< USBPD_TCPM_alert: tcpc_clear_alert */
  TCPM_TRACE_ALERT_GET_POWER_STATUS,  /*!< USBPD_TCPM_alert: get_power_status */
  TCPM_TRACE_ALERT_GET_FAULT_STATUS,  /*!< USBPD_TCPM_alert: get_fault_status */
  TCPM_TRACE_ALERT_SET_FAULT_STATUS,  /*!< USBPD_TCPM_alert: set_fault_status */
  TCPM_TRACE_ALERT_READ_ALERT,  /*!< USBPD_TCPM_alert: set_fault_status */
  TCPM_TRACE_HW_INIT,  /*!< USBPD_TCPM_HWInit */
  TCPM_TRACE_SET_CC,  /*!< USBPD_TCPM_set_cc */
  TCPM_TRACE_VBUS_GET_VOLTAGE,  /*!< USBPD_TCPM_VBUS_GetVoltage */
  TCPM_TRACE_VBUS_IS_VSAFE0V,  /*!< USBPD_TCPM_VBUS_IsVsafe0V */
  TCPM_TRACE_VBUS_IS_VSAFEEV,  /*!< USBPD_TCPM_VBUS_IsVsafe5V */
  TCPM_TRACE_VBUS_ENABLE,  /*!< USBPD_TCPM_VBUS_Enable */
  TCPM_TRACE_VBUS_DISABLE,  /*!< USBPD_TCPM_VBUS_Disable */
  TCPM_TRACE_INIT,          /*USBPD_PHY_Init*/
  TCPM_TRACE_DEINIT,        /*USBPD_PHY_Deinit*/
  TCPM_TRACE_RESET,         /*USBPD_PHY_Reset*/
  TCPM_TRACE_SOP_SUPPORTED, /*USBPD_PHY_SOPSupported*/
  TCPM_TRACE_GET_CC,        /*USBPD_TCPM_get_cc*/
  TCPM_TRACE_SET_POLARITY, /*USBPD_TCPM_set_polarity*/
  TCPM_TRACE_SET_VCONN,     /*USBPD_TCPM_set_vconn*/
  TCPM_TRACE_SET_MSG_HEADER, /*USBPD_TCPM_set_msg_header*/
  TCPM_TRACE_SET_RX_STATE,  /*USBPD_TCPM_set_rx_state*/
  TCPM_TRACE_SET_GET_MESSAGE, /*USBPD_TCPM_get_message*/
  TCPM_TRACE_TRANSMIT,      /*USBPD_TCPM_transmit*/
  TCPM_TRACE_RESET_REQUEST, /*USBPD_PHY_ResetRequest*/
  TCPM_TRACE_SEND_BIST,     /*USBPD_PHY_Send_BIST_Pattern*/
  TCPM_TRACE_SEND_BIST_PATTERN, /*USBPD_TCPM_Send_BIST_Pattern*/
  TCPM_TRACE_SINK_TXNG,     /*USBPD_PHY_SetResistor_SinkTxNG*/
  TCPM_TRACE_SINK_TXOK,     /*USBPD_PHY_SetResistor_SinkTxOK*/
  TCPM_TRACE_SINK_IF_TXOK,  /*USBPD_PHY_IsResistor_SinkTxOk*/
  TCPM_TRACE_FAST_ROLE_SWAP, /*USBPD_PHY_FastRoleSwapSignalling*/
  TCPM_TRACE_ENABLE_RX,     /* tcpc_EnableRx */
  TCPM_TRACE_DISABLE_RX,    /* tcpc_DisableRx */
  TCPM_TRACE_SET_PIN_ROLE,  /* tcpc_tcpc_set_cc */
} USBPD_TCPM_TRACE;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern TRACE_ENTRY_POINT USBPD_Trace;
/* Exported functions --------------------------------------------------------*/

/** @defgroup USBPD_CORE_TRACE_Exported_Functions USBPD CORE TRACE Exported Functions
  * @{
  */
/**
  * @brief  Initialize the TRACE module
  * @retval None
  */
void            USBPD_TRACE_Init(void);

/**
  * @brief  DeInitialize the TRACE module
  * @retval None
  */
void            USBPD_TRACE_DeInit(void);

/**
  * @brief  Add information in debug trace buffer
  * @param  Type    Trace Type based on @ref TRACE_EVENT
  * @param  PortNum Port number value
  * @param  Sop     SOP type
  * @param  Ptr     Pointer on the data to send
  * @param  Size    Size of the data to send
  * @retval None.
  */
void            USBPD_TRACE_Add(TRACE_EVENT Type, uint8_t PortNum, uint8_t Sop, uint8_t *Ptr, uint32_t Size);

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
