/**
  ******************************************************************************
  * @file    usbpd_phy.c
  * @author  MCD Application Team
  * @brief   This file contains PHY layer functions.
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
#include "usbpd_def.h"
#include "usbpd_phy.h"
#include "usbpd_hw_if.h"
#include "usbpd_pwr_if.h"
#include "usbpd_porthandle.h"

/** @addtogroup STM32_USBPD_LIBRARY
  * @{
  */

/** @addtogroup USBPD_DEVICE
  * @{
  */

/** @addtogroup USBPD_DEVICE_PHY
  * @{
  */

/* Private defines -----------------------------------------------------------*/
/** @defgroup USBPD_DEVICE_PHY_Private_defines USBPD DEVICE PHY Private defines
  * @{
  */
/**
  * @}
  */

/* Private typedef -----------------------------------------------------------*/
/** @defgroup USBPD_DEVICE_PHY_Private_typedef USBPD DEVICE PHY Private typedef
  * @{
  */


/**
  * @brief Handle to support the data of the layer
  */
typedef struct
{
  USBPD_PHY_Callbacks  *cbs;
  uint32_t  SupportedSOP;        /*!<bit field SOP"Debug SOP'Debug SOP" SOP' SOP */
} PHY_HandleTypeDef;

typedef void (*PHY_CB_t)(uint8_t PortNum, USBPD_SOPType_TypeDef Type); /*!<  Common callback definition */

/**
  * @}
  */

/* Private define and macro --------------------------------------------------*/
/** @defgroup USBPD_DEVICE_PHY_Private_macros USBPD DEVICE PHY Private macros
  * @{
  */

/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/
/** @defgroup USBPD_DEVICE_PHY_Private_variables USBPD DEVICE PHY Private variables
  * @{
  */

/** Internal struct for RXTX */
static PHY_HandleTypeDef PHY_Ports[USBPD_PORT_COUNT];
extern USBPD_PORT_HandleTypeDef Ports[USBPD_PORT_COUNT];
/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
/** @defgroup USBPD_DEVICE_PHY_Private_functions USBPD DEVICE PHY Private functions
  * @{
  */
USBPD_StatusTypeDef         PHY_PortInit(uint8_t PortNum, USBPD_PHY_Callbacks *cbs, uint8_t *pRxBuffer, uint32_t SupportedSOP);
void                        PHY_BistCompleted(uint8_t PortNum, USBPD_BISTMsg_TypeDef bistmode);
void                        PHY_TxCompleted(uint8_t portnum);

void                        USBPD_PHY_ResetCompleted(uint8_t PortNum, USBPD_SOPType_TypeDef Type);
USBPD_PHY_RX_Status_TypeDef PHY_Rx_Reset(uint8_t PortNum);
void PHY_Rx_HardReset(uint8_t PortNum);
void PHY_Rx_Completed(uint8_t PortNum, uint32_t MsgType, uint16_t RxPaySize);

/**
  * @}
  */

/* Exported functions ---------------------------------------------------------*/

/** @defgroup USBPD_DEVICE_PHY_Exported_Functions USBPD DEVICE PHY Exported functions
  * @{
  */
/**
  * @brief  Initialize the PHY of a specified port.
  * @param  PortNum       Number of the port.
  * @param  pCallbacks    PHY callbacks
  * @param  pRxBuffer     Buffer to storage received message.
  * @param  PowerRole     Power Role of the board.
  * @param  SupportedSOP  Supported SOP
  * @retval HAL status
  */
USBPD_StatusTypeDef USBPD_PHY_Init(uint8_t PortNum, USBPD_PHY_Callbacks *pCallbacks, uint8_t *pRxBuffer, USBPD_PortPowerRole_TypeDef PowerRole, uint32_t SupportedSOP)
{
  /* set all callbacks */
  USBPD_HW_IF_Callbacks hwif_cbs;
  hwif_cbs.USBPD_HW_IF_TxCompleted    = PHY_TxCompleted;
  hwif_cbs.USBPD_HW_IF_BistCompleted  = PHY_BistCompleted;
  hwif_cbs.USBPD_HW_IF_RX_Reset       = PHY_Rx_Reset;
  hwif_cbs.USBPD_HW_IF_RX_ResetIndication = PHY_Rx_HardReset;
  hwif_cbs.USBPD_HW_IF_RX_Completed   = PHY_Rx_Completed;
  hwif_cbs.USBPD_HW_IF_TX_HardResetCompleted = USBPD_PHY_ResetCompleted;

  Ports[PortNum].ptr_RxBuff = pRxBuffer;
  Ports[PortNum].cbs = hwif_cbs;
  /* Initialize the hardware for the port */
//  USBPD_HW_IF_PortHwInit(PortNum, hwif_cbs, PowerRole, pRxBuffer);

  /* Initialize port related functionalities inside this layer */
  PHY_Ports[PortNum].SupportedSOP = SupportedSOP;
  PHY_Ports[PortNum].cbs = pCallbacks;
//  if (PHY_PortInit(PortNum, pCallbacks, pRxBuffer, SupportedSOP))
//  {
//    return USBPD_ERROR;
//  }

  return USBPD_OK;
}
/**
  * @brief  return the retry counter value in us.
  * @param  PortNum    Number of the port.
  * @retval retry counter value in us.
  */
uint32_t USBPD_PHY_GetRetryTimerValue(uint8_t PortNum)
{
  return 945u;
}

/**
  * @brief  Reset the PHY of a specified port.
  * @param  PortNum    Number of the port.
  * @retval None
  */
void USBPD_PHY_Reset(uint8_t PortNum)
{
  /* reset PHY layer   */
  /* reset HW_IF layer */
}

/**
  * @brief  Request a Reset on a specified port.
  * @param  PortNum    Number of the port
  * @param  Type    The type of reset (hard or cable reset).
  * @retval HAL status
  */
USBPD_StatusTypeDef USBPD_PHY_ResetRequest(uint8_t PortNum, USBPD_SOPType_TypeDef Type)
{
  /* Send the requested reset */
  return USBPD_PHY_SendMessage(PortNum,Type,NULL,0);
}

/**
  * @brief  Send a Message.
  * @param  PortNum   port number value.
  * @param  Type    The type of the message.
  * @param  pBuffer    Pointer to the buffer to be transmitted.
  * @param  Size    The size of the buffer (bytes).
  * @retval HAL status
  */
USBPD_StatusTypeDef USBPD_PHY_SendMessage(uint8_t PortNum, USBPD_SOPType_TypeDef Type, uint8_t *pBuffer, uint16_t Size)
{
  /* Trace to track message */
  return USBPD_HW_IF_SendBuffer(PortNum, Type, pBuffer,  Size);
}

/**
  * @brief  send bit pattern.
  * @param  PortNum    Number of the port
  * @retval USBPD status
  */
USBPD_StatusTypeDef USBPD_PHY_Send_BIST_Pattern(uint8_t PortNum)
{
  /* Call the low-level function (HW_IF) to accomplish the BIST Carrier Mode Transmission */
  USBPD_HW_IF_Send_BIST_Pattern(PortNum);
  return USBPD_OK;
}

/**
  * @brief  Request PHY to exit of BIST mode 2
  * @param  PortNum   port number value
  * @param  mode      SOP BIST MODE 2
  * @retval USBPD status
  */
USBPD_StatusTypeDef USBPD_PHY_ExitTransmit(uint8_t PortNum, USBPD_SOPType_TypeDef mode)
{
  if(USBPD_SOPTYPE_BIST_MODE_2 == mode)
  {
    USBPD_HW_IF_StopBISTMode2(PortNum);
  }
  return USBPD_OK;
}

/**
  * @brief  Reset completed.
  * @param  PortNum port number
  * @param  Type  PD Type
  * @retval None
  */
void USBPD_PHY_ResetCompleted(uint8_t PortNum, USBPD_SOPType_TypeDef Type)
{
  /* reset PHY layer */
  USBPD_PHY_Reset(PortNum);
  if (PHY_Ports[PortNum].cbs->USBPD_PHY_ResetCompleted != NULL)
  {
    PHY_Ports[PortNum].cbs->USBPD_PHY_ResetCompleted(PortNum, Type);
  }
}

/**
 * @brief  function to set the SinkTxNg
 * @param  PortNum  Number of the port.
 * @retval none.
  */
void USBPD_PHY_SetResistor_SinkTxNG(uint8_t PortNum)
{
  USBPD_HW_IF_SetResistor_SinkTxNG(PortNum);
}

/**
  * @brief  function to set the SinkTxOK
  * @param  PortNum  Number of the port.
  * @retval none.
  */
void USBPD_PHY_SetResistor_SinkTxOK(uint8_t PortNum)
{
  USBPD_HW_IF_SetResistor_SinkTxOK(PortNum);
}

/**
  * @brief  function to set the supported SOP
  * @param  PortNum  Number of the port.
  * @param  List of the supported SOP
  * @retval None.
  */
void USBPD_PHY_SOPSupported(uint8_t PortNum,uint32_t SOPSupported)
{
  PHY_Ports[PortNum].SupportedSOP = SOPSupported;
}
/**
   * @}
   */

/**
  * @brief  Check if SinkTxOK is set or not
  * @param  PortNum  Number of the port.
  * @retval USBPD_TRUE or USBPD_FALSE
  */
uint8_t USBPD_PHY_IsResistor_SinkTxOk(uint8_t PortNum)
{
  return USBPD_HW_IF_IsResistor_SinkTxOk(PortNum);
}

/**
 * @brief  function to set the SinkTxOK
 * @param  PortNum  Number of the port.
 * @retval USBPD_TRUE USBPD_FALSE.
  */
void USBPD_PHY_FastRoleSwapSignalling(uint8_t PortNum)
{
  USBPD_HW_IF_FastRoleSwapSignalling(PortNum);
}

/**
  * @brief  Enable RX
  * @param  PortNum    Number of the port.
  * @retval None
  */
void USBPD_PHY_EnableRX(uint8_t PortNum)
{
  USBPD_HW_IF_EnableRX(PortNum);
}

/**
  * @brief  Disable RX
  * @param  PortNum    Number of the port.
  * @retval None
  */
void USBPD_PHY_DisableRX(uint8_t PortNum)
{
  USBPD_HW_IF_DisableRX(PortNum);
}

/**
  * @}
  */

/** @addtogroup USBPD_DEVICE_PHY_Private_functions
  * @{
  */
/******************************************************************/
/*********************** Private functions ************************/
/******************************************************************/

/**
  * @brief  Port initiatlization
  * @param  PortNum       Port Number Value
  * @param  pCallback     PHY callbacks
  * @param  pRxBuffer     Pointer on the reception buffer
  * @param  SupportedSOP  Supported SOP
  * @retval USBPD Status
  */
//USBPD_StatusTypeDef PHY_PortInit(uint8_t PortNum, USBPD_PHY_Callbacks *pCallback, uint8_t *pRxBuffer, uint32_t SupportedSOP)
//{
//  /* Associate the RXBUF */
//  PHY_Ports[PortNum].SupportedSOP = SupportedSOP;
//
//  /* Associate the callbacks */
//  PHY_Ports[PortNum].cbs = pCallback;
//
//  return USBPD_OK;
//}

/**
 * @brief  Callback to notify the bist is completed
 * @param  PortNum   Number of the port.
 * @param  bistmode  Modality of the bist.
 * @retval none.
 */
void PHY_BistCompleted(uint8_t PortNum, USBPD_BISTMsg_TypeDef bistmode)
{
  if (PHY_Ports[PortNum].cbs->USBPD_PHY_BistCompleted)
  {
    PHY_Ports[PortNum].cbs->USBPD_PHY_BistCompleted(PortNum, bistmode);
  }
}

/**
 * @brief  Callback to notify the a transmission is completed
 * @param  PortNum  Number of the port.
 * @retval none.
 */
void PHY_TxCompleted(uint8_t PortNum)
{
  if (PHY_Ports[PortNum].cbs->USBPD_PHY_TxCompleted)
  {
    PHY_Ports[PortNum].cbs->USBPD_PHY_TxCompleted(PortNum);
  }
}

/**
 * @brief  Callback to notify the start of reception
 * @param  PortNum  Number of the port.
 * @retval Status of current reception.
 */
USBPD_PHY_RX_Status_TypeDef PHY_Rx_Reset(uint8_t PortNum)
{
  return USBPD_PHY_RX_STATUS_OK;
}

/**
 * @brief  Callback to notify the start of reception
 * @param  PortNum  Number of the port.
 * @retval Status of current reception.
 */
void PHY_Rx_HardReset(uint8_t PortNum)
{
  /* nothing to do the message will be discarded and the port partner retry the send */
  if (PHY_Ports[PortNum].cbs->USBPD_PHY_ResetIndication != NULL)
  {
    PHY_Ports[PortNum].cbs->USBPD_PHY_ResetIndication(PortNum, USBPD_SOPTYPE_HARD_RESET);
  }
}

/**
 * @brief  Callback to notify the end of the current reception
 * @param  PortNum  Number of the port.
 * @retval None.
  */
void PHY_Rx_Completed(uint8_t PortNum, uint32_t MsgType, uint16_t RxPaySize)
{
  USBPD_SOPType_TypeDef _msgtpye;

  const USBPD_SOPType_TypeDef tab_sop_value[] = { USBPD_SOPTYPE_SOP,              USBPD_SOPTYPE_SOP1, USBPD_SOPTYPE_SOP2,
                                                  USBPD_SOPTYPE_SOP1_DEBUG, USBPD_SOPTYPE_SOP2_DEBUG, USBPD_SOPTYPE_CABLE_RESET };
  _msgtpye = tab_sop_value[MsgType];

  /* check if the message must be forwarded to usbpd stack */
  switch(_msgtpye)
  {
  case USBPD_SOPTYPE_CABLE_RESET :
    if(PHY_Ports[PortNum].SupportedSOP & 0x1E)
    {
      /* nothing to do the message will be discarded and the port partner retry the send */
      if (PHY_Ports[PortNum].cbs->USBPD_PHY_ResetIndication != NULL)
      {
        PHY_Ports[PortNum].cbs->USBPD_PHY_ResetIndication(PortNum, USBPD_SOPTYPE_CABLE_RESET);
      }
      return;
    }
    break;
  case USBPD_SOPTYPE_SOP :
  case USBPD_SOPTYPE_SOP1 :
  case USBPD_SOPTYPE_SOP2 :
  case USBPD_SOPTYPE_SOP1_DEBUG :
  case USBPD_SOPTYPE_SOP2_DEBUG :
    if(PHY_Ports[PortNum].SupportedSOP & (0x1 << _msgtpye))
    {
      goto exit;
    }
    break;
  default :
	  break;
  }
  return;

exit :
  if(PHY_Ports[PortNum].cbs->USBPD_PHY_MessageReceived != NULL)
  {
    PHY_Ports[PortNum].cbs->USBPD_PHY_MessageReceived(PortNum, _msgtpye);
  }
}

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

