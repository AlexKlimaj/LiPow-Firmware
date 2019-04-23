/**
  ******************************************************************************
  * @file    gui_api.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for GUI
  *          interface.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _GUI_API_H
#define _GUI_API_H

/* Includes ------------------------------------------------------------------*/

/** @addtogroup STM32_USBPD_LIBRARY
  * @{
  */

/** @addtogroup USBPD_GUI_API
  * @{
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup USBPD_GUI_API_Exported_Defines USBPD GUI API exported Defines
  * @{
  */
#define GUI_PE_PORT_NUM_Pos             (24u)
#define GUI_PE_PORT_NUM_Msk             (0x3u << GUI_PE_PORT_NUM_Pos)
#define GUI_PE_PORT_NUM_0               (0x0u << GUI_PE_PORT_NUM_Pos)
#define GUI_PE_PORT_NUM_1               (0x1u << GUI_PE_PORT_NUM_Pos)
#define GUI_PE_NOTIF_Pos                (4u)
#define GUI_PE_NOTIF_Msk                (0xFFu << GUI_PE_NOTIF_Pos)

#define GUI_NOTIF_MEASUREMENT           (USBPD_NOTIFY_ALL + 1)
#define GUI_NOTIF_MEASUREMENT_STEP      40u

/**
  * @brief  USB PD GUI STATE MACHINE STATE
  */
typedef enum
{
  GUI_STATE_INIT,     /*!< GUI init state */
  GUI_STATE_RUNNING,  /*!< GUI running state */
  GUI_STATE_RESET,    /*!< GUI reset state */
} USBPD_GUI_State;

/**
  * @brief  USB PD GUI TAG TYPE
  */
typedef enum
{
  DPM_RESET_REQ            = 0x01,
  DPM_INIT_REQ             = 0x02,
  DPM_INIT_CNF             = 0x03,
  DPM_CONFIG_GET_REQ       = 0x04,
  DPM_CONFIG_GET_CNF       = 0x05,
  DPM_CONFIG_SET_REQ       = 0x06,
  DPM_CONFIG_SET_CNF       = 0x07,
  DPM_CONFIG_REJ           = 0x08,
  DPM_MESSAGE_REQ          = 0x09,
  DPM_MESSAGE_CNF          = 0x0A,
  DPM_MESSAGE_REJ          = 0x0B,
  DPM_MESSAGE_IND          = 0x0C,
  DPM_MESSAGE_RSP          = 0x0D,
  DPM_REGISTER_READ_REQ    = 0x0E,
  DPM_REGISTER_READ_CNF    = 0x0F,
  DPM_REGISTER_WRITE_REQ   = 0x10,
  DPM_REGISTER_WRITE_CNF   = 0x11,
  DEBUG_STACK_MESSAGE      = 0x12,
} USBPD_GUI_Tag_TypeDef;

/** @defgroup USBPD_CORE_GUI_TYPE_NOTIFICATION USBPD CORE TRACE  TYPE NOTIFICATION
  * @{
  */
#define  GUI_NOTIF_NUMBEROFRCVSNKPDO                    (1 << 0)
#define  GUI_NOTIF_RDOPOSITION                          (1 << 1)
#define  GUI_NOTIF_LISTOFRCVSRCPDO                      (1 << 2)
#define  GUI_NOTIF_NUMBEROFRCVSRCPDO                    (1 << 3)
#define  GUI_NOTIF_LISTOFRCVSNKPDO                      (1 << 4)
#define  GUI_NOTIF_ISCONNECTED                          (1 << 5)
#define  GUI_NOTIF_DATAROLE                             (1 << 6)
#define  GUI_NOTIF_POWERROLE                            (1 << 7)
#define  GUI_NOTIF_CCDEFAULTCURRENTADVERTISED           (1 << 8)
#define  GUI_NOTIF_VCONNON                              (1 << 9)
#define  GUI_NOTIF_VCONNSWAPED                          (1 << 10)
#define  GUI_NOTIF_MEASUREREPORTING                     (1 << 11)
#define  GUI_NOTIF_CC                                   (1 << 12)
#define  GUI_NOTIF_PE_EVENT                             (1 << 13)
#define  GUI_NOTIF_TIMESTAMP                            (1 << 14)
#define  GUI_NOTIF_POWER_EVENT                          (1 << 15)

/**
  * @}
  */

/**
  * @}
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup USBPD_GUI_API_Exported_TypeDef USBPD GUI API exported TypeDef
  * @{
  */

typedef struct
{
  uint16_t VBUS_Level               :16;  /*!< VBUS Level */
  uint16_t IBUS_Level               :16;  /*!< IBUS Level */
  /* Measurement Reporting */
  union {
    uint8_t MeasurementReporting;
    struct {
      uint8_t MeasReportValue       :7;   /*!< Enable Measure reporting every tr x 40 ms  */
      uint8_t MeasReportActivation  :1;   /*!< Enable or Disable Measure reporting        */
    }d;
  }u;
  uint8_t Reserved                  :8;   /*!< Reserved bits */
} GUI_USER_ParamsTypeDef;

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/* Exported variable ---------------------------------------------------------*/
/** @defgroup USBPD_GUI_API_Exported_Variable USBPD GUI API exported Variable
  * @{
  */
extern GUI_USER_ParamsTypeDef GUI_USER_Params[USBPD_PORT_COUNT];
#if !defined(_RTOS)
extern volatile uint32_t      GUI_Flag;
#endif /* !_RTOS */
/**
  * @}
  */

/* Exported functions ------------------------------------------------------- */
/** @defgroup USBPD_GUI_API_Exported_Functions USBPD GUI API exported functions
  * @{
  */

void                  GUI_Init(const uint8_t* (*CB_HWBoardVersion)(void), const uint8_t* (*CB_HWPDType)(void));
void                  GUI_Start(void);
uint32_t              GUI_RXProcess(uint32_t Event);
uint32_t              GUI_FormatAndSendNotification(uint32_t PortNum, uint32_t TypeNotification, uint32_t Value);
uint32_t              GUI_GetMessage(uint8_t Character, uint8_t Error);
USBPD_GUI_State       GUI_SendAnswer(uint8_t **pMsgToSend, uint8_t *pSizeMsg);
USBPD_GUI_State       GUI_SendNotification(uint8_t PortNum, uint8_t **pMsgToSend, uint8_t *pSizeMsg, uint32_t TypeNotifcation, uint32_t Value);
void                  GUI_PostNotificationMessage(uint8_t PortNum, uint16_t EventVal);
USBPD_FunctionalState GUI_IsRunning(void);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif /* _GUI_API_H */

