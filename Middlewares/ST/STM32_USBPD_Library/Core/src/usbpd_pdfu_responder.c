/**
  ******************************************************************************
  * @file    usbpd_pdfu_responder.c
  * @brief   USBPD FW Update API for PDFU responder
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
#if defined(_FWUPDATE_RESPONDER)

#define USBPD_PDFU_RESPONDER_C

#include "usbpd_pdfu_responder.h"
#include "usbpd_dpm_conf.h"
#include "usbpd_dpm_user.h"
#if defined(_TRACE)
#include "usbpd_trace.h"
#include <stdio.h>
#endif /* _TRACE */
/** @addtogroup STM32_USBPD_PDFU
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/** @defgroup USBPD_PDFU_Private_Defines USBPD PDFU Private Structures
  * @{
  */

typedef enum 
{
  PDFU_FLOW_ENUMERATION,          /*!< PDFU Flow Enumeration: PDFU Initiator queries the PDFU Responder                                     */
  PDFU_FLOW_RECONFIGURATION,      /*!< PDFU Flow Reconfiguration: FU Initiator and the PDFU Responder perform any necessary
                                       reconfiguration in order to prepare for the transfer of the new firmware image                       */
  PDFU_FLOW_RECONFIGURATION_WAIT, /*!< PDFU Flow Reconfiguration: Wait for PDFU responder answer                                            */
  PDFU_FLOW_RECONFIGURATION_WAIT_DATA, /*!< PDFU Flow Reconfiguration: Wait for PDFU_DATA request                                           */
  PDFU_FLOW_TRANSFER,             /*!< PDFU Flow Transfer: New firmware image is transferred from the PDFU Initiator to the PDFU Responder. */
  PDFU_FLOW_VALIDATION,           /*!< PDFU Flow Validation: PDFU Responder validates the new firmware image                                */
  PDFU_FLOW_MANIFESTATION,        /*!< PDFU Flow Manifestation: PDFU Responder switches to using the new firmware image                     */
  PDFU_FLOW_DISABLED,             /*!< PDFU Flow Disabled: no PDFU Responder                                                                */
} PDFU_FlowTypeDef;

/** @defgroup USBPD_PDFU_Private_Defines USBPD PDFU Responder Transfer states
  * @{
  */

typedef enum 
{
  PDFU_RSP_TRANSFER_STATE_REQ_RCVD,   /*!< Req Received           */
  PDFU_RSP_TRANSFER_STATE_SEND_RESP,  /*!< Send Response          */
  PDFU_RSP_TRANSFER_STATE_EXIT_FLOW,  /*!< Exit Flow              */
  PDFU_RSP_TRANSFER_STATE_TIMEOUT,    /*!< Timeout                */
  PDFU_RSP_TRANSFER_STATE_GOTO_VALID, /*!< Go to Validation Phase */
  PDFU_RSP_TRANSFER_STATE_PAUSE,      /*!< Pause state            */
} PDFU_RspTransferStateTypeDef;

/**
  * @}
  */

/** @defgroup USBPD_PDFU_Private_Defines USBPD PDFU Responder Reception states
  * @{
  */

typedef enum 
{
  PDFU_RSP_STATE_EXPECTED,                    /*!< Expected requests                                          */
  PDFU_RSP_STATE_EXPECTED_ABORT,              /*!< Expected requests - abort Msg                              */
  PDFU_RSP_STATE_EXPECTED_VENDOR,             /*!< Expected requests - Vendor Msg                             */
  PDFU_RSP_STATE_UNEXPECTED_NOTIFY,           /*!< Unexpected requests - Notify as unexpected                 */
  PDFU_RSP_STATE_UNEXPECTED_NOTIFY_RESERVED,  /*!< Unexpected requests - Reserved Msg - Notify as unexpected  */
  PDFU_RSP_STATE_UNEXPECTED_NOTIFY_VENDOR,    /*!< Unexpected requests - Vendor Msg - Notify as unexpected    */
  PDFU_RSP_STATE_UNEXPECTED_IGNORE,           /*!< Unexpected requests - Ignore                               */
} PDFU_RspStateTypeDef;

/**
  * @}
  */

/** @defgroup PDFU_HandleTypeDef USB PD handle Structure definition
  * @brief  PDFU handle Structure definition
  * @{
  */
typedef struct
{
  USBPD_ParamsTypeDef         *PE_Params;         /*!< PE Params                            */
  PDFU_FlowTypeDef            CurrentState;       /*!< PDFU Current state                   */
  PDFU_RspTransferStateTypeDef Rsp_SubStateTransfer; /*!< Responder Transfer SubState       */
  USBPD_PDFU_Callbacks        *PDFUCallbacks;     /*!< PDFU PDFU callbacks                  */
  uint8_t                     NbResend;           /*!< PDFU Number of resend                */
  USBPD_FWUPD_MsgType_TypeDef ReceivedMsgID;      /*!< PDFU Received Msg ID                 */
  uint16_t                    VIDReceived;        /*!< VID received in VENDOR_SPECIFIC req  */
  volatile uint16_t           PDFU_ResponseRcvd;  /*!< tPDFUResponseRcvd timer              */
} PDFU_HandleTypeDef;

/**
  * @}
  */

/**
  * @}
  */

/* Private define ------------------------------------------------------------*/
/** @defgroup USBPD_PDFU_Private_Defines USBPD PDFU Private Defines
  * @{
  */
#define USBPD_MAX_EXTENDED_MSGLEN_CHUNKLEN  26u  /*!< Maximum length of an Extended Message as expressed in the Data Size field.*/

#define PDFU_TIMER_ENABLE_MSK         ((uint16_t)0x8000U)  /*!< Enable Timer Mask    */
#define PDFU_TIMER_READ_MSK           ((uint16_t)0x7FFFU)  /*!< Read Timer Mask      */

/**
  * @brief  PDFU Timeout Values for a PD Firmware Update PDFU Initiator and Responder
  */
/* tPDFUResponseRcvd: Timeout for any PDFU Request requiring a response */
#define PDFU_TRESPONSERCVD_UNCHUNKED  (56u)  /*!< Min = 54 ms / Max = 60 ms   */

/* tPDFUNextRequestSent: Maximum time between receiving a PDFU Response and waiting a specified 
   WaitTime and sending the next PDFU Request */
#define PDFU_TNEXTREQUESTSENT         (27u)  /*!< 27 ms */

/**
  * @brief  PDFU Constants required by the PDFU Firmware Update Protocol
  */
#define PDFU_ENUMERATE_RESEND         (10u)
#define PDFU_RECONFIGURE_RESEND       (3u)
#define PDFU_DATA_RESEND              (3u)
#define PDFU_VALIDATE_RESEND          (3u)
#define PDFU_PAUSE_RESEND             (3u)

/**
  * @brief  PDFU Offset
  */
#define PDFU_OFFSET_PROT_VER          (0u)
#define PDFU_OFFSET_MESSAGE_TYPE      (1u)
#define PDFU_OFFSET_PAYLOAD           (2u)

#if !defined(_RTOS)
#define osWaitForever     0xFFFFFFFF     ///< wait forever timeout value
#endif /* !_RTOS */

/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/** @defgroup USBPD_PDFU_Private_Macros USBPD PDFU Private Macros
  * @{
  */
/**
  * @brief  PDFU Timeout macros
  */
#define PDFU_START_TIMER(_PDFU_HANDLE_, _TIMER_,_TIMEOUT_) _PDFU_HANDLE_->_TIMER_ = (_TIMEOUT_) |  PDFU_TIMER_ENABLE_MSK;\
                                                      _timing = (_TIMEOUT_);

#define PDFU_START_TIMER_PE(_PDFU_HANDLE_, _TIMER_,_TIMEOUT_) _PDFU_HANDLE_->_TIMER_ = (_TIMEOUT_) |  PDFU_TIMER_ENABLE_MSK;

#define PDFU_STOP_TIMER(_PDFU_HANDLE_, _TIMER_) _PDFU_HANDLE_->_TIMER_ = 0;

#define PDFU_TIMER_REMAINING(_PDFU_HANDLE_, _TIMER_) (_PDFU_HANDLE_->_TIMER_ & PDFU_TIMER_READ_MSK)

#define IS_PDFU_TIMER_RUNNING(_PDFU_HANDLE_, _TIMER_) ((_PDFU_HANDLE_->_TIMER_ & PDFU_TIMER_READ_MSK) > 0)


#define IS_PDFU_TIMER_STARTED(_PDFU_HANDLE_, _TIMER_) (_PDFU_HANDLE_->_TIMER_ > 0)

#define IS_PDFU_TIMER_EXPIRED(_PDFU_HANDLE_, _TIMER_) (PDFU_TIMER_ENABLE_MSK == _PDFU_HANDLE_->_TIMER_)

/**
  * @brief  PDFU useful macros
  */
#define PDFU_SWITCH_TO_STATE(_PDFU_HANDLE_, _STATE_)  _PDFU_HANDLE_->CurrentState = (_STATE_)

#define PDFU_SWITCH_TO_RESP_SUBSTATE(_PDFU_HANDLE_, _SUB_STATE_)  _PDFU_HANDLE_->Rsp_SubStateTransfer = (_SUB_STATE_)

#if USBPD_PORT_COUNT==2
#define OSTHREAD_PDFU(__VAL__) (__VAL__)==USBPD_PORT_0?osThread(PDFU_0):osThread(PDFU_1)
#else
#define OSTHREAD_PDFU(__VAL__) osThread(PDFU_0)
#endif /* USBPD_PORT_COUNT==2 */

/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
/** @defgroup USBPD_PDFU_Private_Functions USBPD PDFU Private Functions
  * @{
  */
#ifdef _RTOS
void            PDFU_Task_P0(void const *argument);
#if USBPD_PORT_COUNT==2
void            PDFU_Task_P1(void const *argument);
#endif /* USBPD_PORT_COUNT==2 */
uint32_t        PDFU_Task(uint8_t PortNum, uint8_t Event);
#endif /* _RTOS */
static void     PDFU_Reset(uint8_t PortNum);
static uint32_t PDFU_Get_ResponseRcvdTimeout(uint8_t PortNum, uint16_t SizeOutgoing, uint16_t SizeIncoming);
uint32_t        PDFU_Responder_StateMachine(uint8_t PortNum);
static uint32_t PDFU_Responder_StateMachine_Transfer(uint8_t PortNum);
static uint32_t PDFU_CheckUnexpectedRequest(uint8_t PortNum, USBPD_FWUPD_MsgType_TypeDef MsgType);

/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/
/** @defgroup USBPD_PDFU_Private_variables USBPD PDFU Private Variables
  * @{
  */
/* Global handle for PDFU */
static PDFU_HandleTypeDef PDFU_Handles[USBPD_PORT_COUNT];
#ifdef _RTOS
osThreadDef(PDFU_0, PDFU_Task_P0, osPriorityLow, 0, 120);
#if USBPD_PORT_COUNT==2
osThreadDef(PDFU_1, PDFU_Task_P1, osPriorityLow, 0, 120);
#endif /* USBPD_PORT_COUNT==2 */
osMessageQDef(queuePDFU, 2, uint16_t);
static osThreadId PDFU_Thread_Table[USBPD_PORT_COUNT];
#else
uint8_t PDFU_TaskCreated      = 0;
uint8_t PDFU_MessageReceived  = 0;
uint8_t PDFU_TimerExpired     = 0;
#endif /* _RTOS */

USBPD_FRQDB_TypeDef PDFU_FRQDB_Data[USBPD_PORT_COUNT] =
{
  {
    .ProtocolVersion = USBPD_FWUPD_PROT_VER_V1P0
  },
#if USBPD_PORT_COUNT==2
  {
    .ProtocolVersion = USBPD_FWUPD_PROT_VER_V1P0
  }
#endif /* USBPD_PORT_COUNT==2 */
};

#if defined(_TRACE)
const char* Tab_State[] =
{
  "ENUMERATION     ",
  "RECONF          ",
  "RECONF_WAIT     ",
  "RECONF_WAIT_DATA",
  "TRANSFER        ",
  "VALIDATION      ",
  "MANIFESTATION   ",
  "DISABLED        ",
};

const char* Tab_RspTransferState[] = 
{
  "REQ_RCVD    ",
  "SEND_RESP   ",
  "EXIT_FLOW   ",
  "TIMEOUT     ",
  "GOTO_VALID  ",
  "PAUSE       ",
};
#endif /* _TRACE */
/**
  * @}
  */

/** @addtogroup USBPD_PDFU_Exported_Functions
  * @{
  */

/** @addtogroup USBPD_PDFU_Exported_Functions_Group1
  * @{
  */

/**
  * @brief  Initialization function
  * @param  PortNum         Port number value
  * @param  pParams         Pointer on USBPD param (based on @ref USBPD_ParamsTypeDef)
  * @param  pPDFUCallbacks  Pointer on PDFU callbacks (based on @ref USBPD_PDFU_Callbacks)
  * @retval None
  */
void USBPD_PDFU_init(uint8_t PortNum, USBPD_ParamsTypeDef *pParams, const USBPD_PDFU_Callbacks *pPDFUCallbacks)
{
  PDFU_HandleTypeDef *_pdhandle     = &PDFU_Handles[PortNum];
  PDFU_Reset(PortNum);
  _pdhandle->PE_Params              = pParams;
  _pdhandle->PDFUCallbacks          = (USBPD_PDFU_Callbacks *)pPDFUCallbacks;

#ifdef _RTOS
  /* Create the queue corresponding to PE task */
  PDFUQueueId[PortNum] = osMessageCreate(osMessageQ(queuePDFU), NULL);
#endif /* _RTOS */
}

/**
  * @brief  Create PDFU Task
  * @param  PortNum Port number value
  * @retval None
  */
void USBPD_PDFU_CreateTask(uint8_t PortNum)
{
#ifdef _RTOS
  /* Create PDFU task */
  if (PDFU_Thread_Table[PortNum] == NULL)
  {
    PDFU_Thread_Table[PortNum] = osThreadCreate(OSTHREAD_PDFU(PortNum), &PDFUQueueId[PortNum]);
    if (PDFU_Thread_Table[PortNum] == NULL)
    {
      /* should not occurr. May be an issue with FreeRTOS heap size too small */
      while(1);
    }
  }
#else
  PDFU_TaskCreated = 1;
#endif /* _RTOS */
}

/**
  * @brief  Terminate PDFU Task
  * @param  PortNum Port number value
  * @retval None
  */
void USBPD_PDFU_TerminateTask(uint8_t PortNum)
{
  PDFU_Reset(PortNum);
#ifdef _RTOS
  if (PDFU_Thread_Table[PortNum] != NULL)
  {
    osThreadTerminate(PDFU_Thread_Table[PortNum]);
    PDFU_Thread_Table[PortNum] = NULL;
  }
#else
  PDFU_TaskCreated = 0;
#endif /* _RTOS */
}

/**
  * @brief  Manage the PDFU timer
  * @param  PortNum Port number value
  * @retval None
  */
void USBPD_PDFU_TimerCounter(uint8_t PortNum)
{
  PDFU_HandleTypeDef *_pdhandle = &PDFU_Handles[PortNum];

  if (IS_PDFU_TIMER_RUNNING(_pdhandle, PDFU_ResponseRcvd))
  {
    _pdhandle->PDFU_ResponseRcvd--;
#ifndef _RTOS
    if (IS_PDFU_TIMER_EXPIRED(_pdhandle, PDFU_ResponseRcvd))
    {
      PDFU_TimerExpired = 1;
    }
#endif /* _RTOS */
  }
}

void USBPD_PDFU_FWUpdateRequestReceived(uint8_t PortNum, uint8_t *ptrData, uint16_t DataSize)
{
  PDFU_HandleTypeDef *_pdhandle = &PDFU_Handles[PortNum];
  _pdhandle->ReceivedMsgID = (USBPD_FWUPD_MsgType_TypeDef)ptrData[PDFU_OFFSET_MESSAGE_TYPE];
  if (USBPD_FWUPD_MSGTYPE_REQ_VENDOR_SPECIFIC == _pdhandle->ReceivedMsgID)
  {
    /* Save Silent update */
    USBPD_FWUPD_VendorSpecificReqPayload_TypeDef *payload = (USBPD_FWUPD_VendorSpecificReqPayload_TypeDef*)&ptrData[PDFU_OFFSET_PAYLOAD];
    _pdhandle->VIDReceived = payload->VID;
  }
  /* Copy PDFU message in user code */
  if (PDFU_OFFSET_PAYLOAD >= DataSize)
  {
    /* No payload in the response */
    _pdhandle->PDFUCallbacks->USBPD_PDFU_SetDataInfo(PortNum, _pdhandle->ReceivedMsgID, NULL, 0);
  }
  else
  {
    /* Payload in the response */
    _pdhandle->PDFUCallbacks->USBPD_PDFU_SetDataInfo(PortNum, _pdhandle->ReceivedMsgID, &ptrData[PDFU_OFFSET_PAYLOAD], (DataSize - PDFU_OFFSET_PAYLOAD));
  }
#ifdef _RTOS
  osMessagePut(PDFUQueueId[PortNum], PDFU_EVENT_RESPONDER, osWaitForever);
#else
  PDFU_MessageReceived  = 1;
#endif
}

/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup USBPD_PDFU_Private_Functions
  * @{
  */

#ifdef _RTOS
/**
  * @brief  Main task for PDFU layer on port0
  * @param  queue_id Pointer on the queue id
  * @retval None
  */
void PDFU_Task_P0(void const *queue_id)
{
  osMessageQId  queue = *(osMessageQId *)queue_id;
  uint32_t _timing = osWaitForever;

  for(;;)
  {
    osEvent event = osMessageGet(queue, _timing);
    _timing = PDFU_Task(USBPD_PORT_0, ((DPM_USER_EVENT)event.value.v & 0xFF));
  }
}

#if USBPD_PORT_COUNT==2
/**
  * @brief  Main task for PDFU layer on port1
  * @param  queue_id Pointer on the queue id
  * @retval None
  */
void PDFU_Task_P1(void const *queue_id)
{
  osMessageQId  queue = *(osMessageQId *)queue_id;
  uint32_t _timing = osWaitForever;

  for(;;)
  {
    osEvent event = osMessageGet(queue, _timing);
    _timing = PDFU_Task(USBPD_PORT_1, ((DPM_USER_EVENT)event.value.v & 0xFF));
  }
}
#endif /* USBPD_PORT_COUNT==2 */
#endif /* _RTOS */

uint32_t PDFU_Task(uint8_t PortNum, uint8_t Event)
{
  uint32_t _timing = osWaitForever;
#ifdef _RTOS
  switch ((DPM_USER_EVENT)Event)
  {
    case PDFU_EVENT_TIMER:
    case PDFU_EVENT_RESPONDER:
      _timing = PDFU_Responder_StateMachine(PortNum);
      break;
    case PDFU_EVENT_NONE:
    default:
      break;
  }
#else
  if (1 == PDFU_TaskCreated)
  {
    if ((1 == PDFU_TimerExpired) || (1 == PDFU_MessageReceived))
    {
      Event = PDFU_EVENT_TIMER;
      PDFU_TimerExpired     = 0;
      PDFU_MessageReceived  = 0;
    }
    switch ((DPM_USER_EVENT)Event)
    {
      case PDFU_EVENT_TIMER:
      case PDFU_EVENT_RESPONDER:
        _timing = PDFU_Responder_StateMachine(PortNum);
        if (0 == _timing)
        {
          PDFU_TimerExpired = 1;
        }
        break;
      case PDFU_EVENT_NONE:
      default:
        break;
    }
  }
#endif
  return _timing;
}

static void PDFU_Reset(uint8_t PortNum)
{
  PDFU_HandleTypeDef *_pdhandle     = &PDFU_Handles[PortNum];
  _pdhandle->NbResend               = 0;
  _pdhandle->CurrentState           = PDFU_FLOW_ENUMERATION;
  _pdhandle->ReceivedMsgID          = USBPD_FWUPD_MSGTYPE_NONE;
  _pdhandle->Rsp_SubStateTransfer   = PDFU_RSP_TRANSFER_STATE_REQ_RCVD;
#ifndef _RTOS
  PDFU_TimerExpired     = 0;
  PDFU_MessageReceived  = 0;
#endif
}

uint32_t PDFU_Responder_StateMachine(uint8_t PortNum)
{
  uint32_t _timing = 0, size = 0;
  PDFU_HandleTypeDef *_pdhandle = &PDFU_Handles[PortNum];

  /* Manage generic messages or unexpected message*/
  if (PDFU_FLOW_DISABLED != _pdhandle->CurrentState)
  {
    _timing = PDFU_CheckUnexpectedRequest(PortNum, _pdhandle->ReceivedMsgID);
  }

  /* Request message is expected, ignore or switch to disabled state */
  switch (_pdhandle->CurrentState)
  {
    /**************************************************************************
      ENUMERATION PHASE
      *************************************************************************/
    case PDFU_FLOW_ENUMERATION:
    {
      size = 0;
      switch (_pdhandle->ReceivedMsgID)
      {
        /*
          Expected messages
        */
        case USBPD_FWUPD_MSGTYPE_REQ_GET_FW_ID:
          PDFU_FRQDB_Data[PortNum].MessageType      = (_pdhandle->ReceivedMsgID & 0x7F);
          _pdhandle->PDFUCallbacks->USBPD_PDFU_GetDataInfo(PortNum, (USBPD_FWUPD_MsgType_TypeDef)PDFU_FRQDB_Data[PortNum].MessageType, PDFU_FRQDB_Data[PortNum].Payload, &size);
          size += 2;
          _timing = osWaitForever;
          /* Stay in the same PDFU flow state */
          break;
        case USBPD_FWUPD_MSGTYPE_REQ_PDFU_INITIATE:
          {
            PDFU_SWITCH_TO_STATE(_pdhandle, PDFU_FLOW_RECONFIGURATION);
          }
          break;
        default:
          break;
      }
      /* Send FIRM_UPDATE_RESPONSE if requested */
      if (0 != size)
      {
        USBPD_DPM_RequestFirwmwareUpdate(PortNum, USBPD_EXT_FIRM_UPDATE_RESPONSE, (uint8_t*)&PDFU_FRQDB_Data[PortNum], size);
      }
    }
    break;
    /**************************************************************************
      RECONFIGURATION PHASE
      *************************************************************************/
    case PDFU_FLOW_RECONFIGURATION:
      {
        USBPD_FWUPD_PdfuInitRspPayload_TypeDef *payload;
        /* Should move to Reconfiguration phase after receiving a PDFU INITIATE request */
        PDFU_FRQDB_Data[PortNum].MessageType      = USBPD_FWUPD_MSGTYPE_RSP_PDFU_INITIATE;
        _pdhandle->PDFUCallbacks->USBPD_PDFU_GetDataInfo(PortNum, (USBPD_FWUPD_MsgType_TypeDef)PDFU_FRQDB_Data[PortNum].MessageType, PDFU_FRQDB_Data[PortNum].Payload, &size);
        size += 2;
        payload = (USBPD_FWUPD_PdfuInitRspPayload_TypeDef*)PDFU_FRQDB_Data[PortNum].Payload;
        if (0 == payload->WaitTime)
        {
          /* PDFU responder send a PDFU_INITIATE resp and should wait for a PDFU_DATA request */
          uint16_t size_outgoing = 2 + USBPD_FWUPD_SIZE_PAYLOAD_REQ_PDFU_DATA_MAX;
          uint16_t size_incoming = 2 + USBPD_FWUPD_SIZE_PAYLOAD_RSP_PDFU_INITIATE;
          PDFU_START_TIMER(_pdhandle, PDFU_ResponseRcvd, PDFU_Get_ResponseRcvdTimeout(PortNum, size_outgoing, size_incoming));
          PDFU_SWITCH_TO_STATE(_pdhandle, PDFU_FLOW_RECONFIGURATION_WAIT_DATA);
        }
        else if (0xFF == payload->WaitTime)
        {
          /* PDFU responder is unable to initiate a FW Update*/
          PDFU_SWITCH_TO_STATE(_pdhandle, PDFU_FLOW_DISABLED);
        }
        else
        {
          /* PDFU responder should wait for a new PDFU initiate request */
          uint8_t size_outgoing = 2 + USBPD_FWUPD_SIZE_PAYLOAD_REQ_PDFU_INITIATE;
          uint8_t size_incoming = 2 + USBPD_FWUPD_SIZE_PAYLOAD_RSP_PDFU_INITIATE;
          PDFU_START_TIMER(_pdhandle, PDFU_ResponseRcvd, payload->WaitTime + PDFU_Get_ResponseRcvdTimeout(PortNum, size_outgoing, size_incoming));
          PDFU_SWITCH_TO_STATE(_pdhandle, PDFU_FLOW_RECONFIGURATION_WAIT);
        }
        USBPD_DPM_RequestFirwmwareUpdate(PortNum, USBPD_EXT_FIRM_UPDATE_RESPONSE, (uint8_t*)&PDFU_FRQDB_Data[PortNum], size);
      }
      break;
    case PDFU_FLOW_RECONFIGURATION_WAIT:
      /* Check if PDFU_INITIATE REQ has been received */
      if (USBPD_FWUPD_MSGTYPE_REQ_PDFU_INITIATE == _pdhandle->ReceivedMsgID)
      {
        _pdhandle->ReceivedMsgID  = USBPD_FWUPD_MSGTYPE_NONE;
        _pdhandle->NbResend       = 0;
        PDFU_STOP_TIMER(_pdhandle, PDFU_ResponseRcvd);
        PDFU_SWITCH_TO_STATE(_pdhandle, PDFU_FLOW_RECONFIGURATION);
      }

      if (IS_PDFU_TIMER_EXPIRED(_pdhandle, PDFU_ResponseRcvd))
      {
        PDFU_STOP_TIMER(_pdhandle, PDFU_ResponseRcvd);
        if (_pdhandle->NbResend < PDFU_RECONFIGURE_RESEND)
        {
          _pdhandle->NbResend++;
          PDFU_SWITCH_TO_STATE(_pdhandle, PDFU_FLOW_RECONFIGURATION);
        }
        else
        {
          /* There is no PDFU Responder */
          PDFU_SWITCH_TO_STATE(_pdhandle, PDFU_FLOW_DISABLED);
        }
      }
      break;
    case PDFU_FLOW_RECONFIGURATION_WAIT_DATA:
      /* Wait for PDFU_DATA request */
      if (USBPD_FWUPD_MSGTYPE_REQ_PDFU_DATA == _pdhandle->ReceivedMsgID)
      {
        _pdhandle->NbResend       = 0;
        PDFU_STOP_TIMER(_pdhandle, PDFU_ResponseRcvd);
        PDFU_SWITCH_TO_STATE(_pdhandle, PDFU_FLOW_TRANSFER);
      }

      if (IS_PDFU_TIMER_EXPIRED(_pdhandle, PDFU_ResponseRcvd))
      {
        PDFU_STOP_TIMER(_pdhandle, PDFU_ResponseRcvd);
        if (_pdhandle->NbResend < PDFU_DATA_RESEND)
        {
          _pdhandle->NbResend++;
          PDFU_SWITCH_TO_STATE(_pdhandle, PDFU_FLOW_RECONFIGURATION);
        }
        else
        {
          /* There is PDFU_DATA received by responder */
          PDFU_SWITCH_TO_STATE(_pdhandle, PDFU_FLOW_DISABLED);
        }
      }
      break;
    /**************************************************************************
      TRANSFER PHASE
      *************************************************************************/
    case PDFU_FLOW_TRANSFER:
      _timing = PDFU_Responder_StateMachine_Transfer(PortNum);
      break;
    /**************************************************************************
      VALIDATION PHASE
      *************************************************************************/
    case PDFU_FLOW_VALIDATION:
      if (USBPD_FWUPD_MSGTYPE_REQ_PDFU_VALIDATE == _pdhandle->ReceivedMsgID)
      {
        /* PDFU_VALIDATION req has been received. */
        _pdhandle->ReceivedMsgID = USBPD_FWUPD_MSGTYPE_NONE;
        PDFU_STOP_TIMER(_pdhandle, PDFU_ResponseRcvd);

        PDFU_FRQDB_Data[PortNum].MessageType      = USBPD_FWUPD_MSGTYPE_RSP_PDFU_VALIDATE;
        _pdhandle->PDFUCallbacks->USBPD_PDFU_GetDataInfo(PortNum, (USBPD_FWUPD_MsgType_TypeDef)PDFU_FRQDB_Data[PortNum].MessageType, PDFU_FRQDB_Data[PortNum].Payload, &size);
        size += 2;
        USBPD_DPM_RequestFirwmwareUpdate(PortNum, USBPD_EXT_FIRM_UPDATE_RESPONSE, (uint8_t*)&PDFU_FRQDB_Data[PortNum], size);

        /* Switch to MANIFESTATION PHASE */
        PDFU_SWITCH_TO_STATE(_pdhandle, PDFU_FLOW_MANIFESTATION);
      }
      else
      {
        /* PDFUNextRequestTime has been expired. Switch to DISABLED state*/
        PDFU_SWITCH_TO_STATE(_pdhandle, PDFU_FLOW_DISABLED);
      }
      break;
    /**************************************************************************
      MANIFESTATION PHASE
      *************************************************************************/
    case PDFU_FLOW_MANIFESTATION:
      _pdhandle->PDFUCallbacks->USBPD_PDFU_InformManifestation(PortNum);
    case PDFU_FLOW_DISABLED:
    default:
      /* Wait for new enumeration */
      PDFU_SWITCH_TO_STATE(_pdhandle, PDFU_FLOW_ENUMERATION);
      PDFU_SWITCH_TO_RESP_SUBSTATE(_pdhandle, PDFU_RSP_TRANSFER_STATE_REQ_RCVD);
      _pdhandle->ReceivedMsgID = USBPD_FWUPD_MSGTYPE_NONE;
      PDFU_STOP_TIMER(_pdhandle, PDFU_ResponseRcvd);
      _timing = osWaitForever;
      break;
  }

#if defined(_TRACE)
  uint8_t tab[32];
  size = sprintf((char*)tab, "PDFU resp State: %s", Tab_State[_pdhandle->CurrentState]);
  USBPD_TRACE_Add(USBPD_TRACE_DEBUG, PortNum, 0, tab, size);
#endif /* _TRACE */
  return _timing;
}

static uint32_t PDFU_Responder_StateMachine_Transfer(uint8_t PortNum)
{
  uint32_t size, _timing = 0;
  PDFU_HandleTypeDef *_pdhandle = &PDFU_Handles[PortNum];

  switch (_pdhandle->Rsp_SubStateTransfer)
  {
    case PDFU_RSP_TRANSFER_STATE_REQ_RCVD:   /*!< PDFU_DATA req received or PDFU_DATA_NR */
      if (USBPD_FWUPD_MSGTYPE_REQ_PDFU_DATA == _pdhandle->ReceivedMsgID)
      {
        /* PDFU_DATA req has been received. Switch to SEND_RESP state */
        /* Reset the Retries and stop timer */
        _pdhandle->ReceivedMsgID  = USBPD_FWUPD_MSGTYPE_NONE;
        _pdhandle->NbResend       = 0;
        PDFU_STOP_TIMER(_pdhandle, PDFU_ResponseRcvd);
        PDFU_SWITCH_TO_RESP_SUBSTATE(_pdhandle, PDFU_RSP_TRANSFER_STATE_SEND_RESP);
      }
      else if (USBPD_FWUPD_MSGTYPE_REQ_PDFU_DATA_NR == _pdhandle->ReceivedMsgID)
      {
        /* PDFU_DATA_NR req has been received.*/
        _pdhandle->ReceivedMsgID  = USBPD_FWUPD_MSGTYPE_NONE;
        /* Restart PDFUNextRequestTime and stay in REQ_RCVD state */
        uint16_t size_outgoing = 2 + USBPD_FWUPD_SIZE_PAYLOAD_REQ_PDFU_DATA_NR_MAX;
        uint16_t size_incoming = 2 + USBPD_FWUPD_SIZE_PAYLOAD_REQ_PDFU_DATA_NR_MAX;
        PDFU_START_TIMER(_pdhandle, PDFU_ResponseRcvd, PDFU_Get_ResponseRcvdTimeout(PortNum, size_outgoing, size_incoming));
      }
      else if (USBPD_FWUPD_MSGTYPE_REQ_PDFU_DATA_PAUSE == _pdhandle->ReceivedMsgID)
      {
        /* PDFU_DATA_PAUSE req has been received.*/
        _pdhandle->ReceivedMsgID  = USBPD_FWUPD_MSGTYPE_NONE;
        _pdhandle->NbResend       = 0;
        PDFU_STOP_TIMER(_pdhandle, PDFU_ResponseRcvd);
        PDFU_SWITCH_TO_RESP_SUBSTATE(_pdhandle, PDFU_RSP_TRANSFER_STATE_PAUSE);
      }
      else
      {
        /* PDFUNextRequestTime has been expired. Switch to TIMEOUT state*/
        PDFU_SWITCH_TO_RESP_SUBSTATE(_pdhandle, PDFU_RSP_TRANSFER_STATE_TIMEOUT);
      }
      break;
    case PDFU_RSP_TRANSFER_STATE_SEND_RESP:  /*!< Send Response          */
      PDFU_FRQDB_Data[PortNum].MessageType      = USBPD_FWUPD_MSGTYPE_RSP_PDFU_DATA;
      _pdhandle->PDFUCallbacks->USBPD_PDFU_GetDataInfo(PortNum, (USBPD_FWUPD_MsgType_TypeDef)PDFU_FRQDB_Data[PortNum].MessageType, PDFU_FRQDB_Data[PortNum].Payload, &size);
      if (0 != size)
      {
        /* Start PDFUNextRequestTime and switch to REQ_RCVD */
        uint16_t size_outgoing = 2 + USBPD_FWUPD_SIZE_PAYLOAD_REQ_PDFU_DATA_MAX;
        uint16_t size_incoming = 2 + USBPD_FWUPD_SIZE_PAYLOAD_REQ_PDFU_DATA_MAX;
        PDFU_START_TIMER(_pdhandle, PDFU_ResponseRcvd, PDFU_Get_ResponseRcvdTimeout(PortNum, size_outgoing, size_incoming));
        PDFU_SWITCH_TO_RESP_SUBSTATE(_pdhandle, PDFU_RSP_TRANSFER_STATE_REQ_RCVD);
      }
      else
      {
        /* No data remaining - go to Validation Phase*/
        /* Reset the Retries and stop timer */
        _pdhandle->NbResend = 0;
        PDFU_STOP_TIMER(_pdhandle, PDFU_ResponseRcvd);
        PDFU_SWITCH_TO_RESP_SUBSTATE(_pdhandle, PDFU_RSP_TRANSFER_STATE_GOTO_VALID);
      }
      size += 2;
      USBPD_DPM_RequestFirwmwareUpdate(PortNum, USBPD_EXT_FIRM_UPDATE_RESPONSE, (uint8_t*)&PDFU_FRQDB_Data[PortNum], size);
      _pdhandle->NbResend++;
      break;
    case PDFU_RSP_TRANSFER_STATE_GOTO_VALID: /*!< Go to Validation Phase    */
      PDFU_SWITCH_TO_STATE(_pdhandle, PDFU_FLOW_VALIDATION);
      PDFU_SWITCH_TO_RESP_SUBSTATE(_pdhandle, PDFU_RSP_TRANSFER_STATE_REQ_RCVD);
      /* Start PDFUNextRequestTime to expect to receive PDFU_VALIDATE req   */
      PDFU_START_TIMER(_pdhandle, PDFU_ResponseRcvd, PDFU_TNEXTREQUESTSENT);
      break;
    case PDFU_RSP_TRANSFER_STATE_TIMEOUT:    /*!< Timeout                   */
      if (_pdhandle->NbResend < PDFU_DATA_RESEND)
      {
        PDFU_SWITCH_TO_RESP_SUBSTATE(_pdhandle, PDFU_RSP_TRANSFER_STATE_SEND_RESP);
      }
      else
      {
        /* There is no PDFU Responder */
        PDFU_SWITCH_TO_RESP_SUBSTATE(_pdhandle, PDFU_RSP_TRANSFER_STATE_EXIT_FLOW);
      }
      break;
    case PDFU_RSP_TRANSFER_STATE_EXIT_FLOW:  /*!< Exit Flow              */
      /* PDFU Responder should exit PDFU flow and then shall reenter in ENUMERATION Phase*/
      /* Reset the Retries and stop timer */
      _pdhandle->NbResend = 0;
      PDFU_STOP_TIMER(_pdhandle, PDFU_ResponseRcvd);
      PDFU_SWITCH_TO_STATE(_pdhandle, PDFU_FLOW_DISABLED);
      break;
    case PDFU_RSP_TRANSFER_STATE_PAUSE:      /*!< Pause state              */
      {
        USBPD_FWUPD_PdfuDataPauseRspPayload_TypeDef *payload;
        /* Should move to Reconfiguration phase after receiving a PDFU INITIATE request */
        PDFU_FRQDB_Data[PortNum].MessageType      = USBPD_FWUPD_MSGTYPE_RSP_PDFU_DATA_PAUSE;
        _pdhandle->PDFUCallbacks->USBPD_PDFU_GetDataInfo(PortNum, (USBPD_FWUPD_MsgType_TypeDef)PDFU_FRQDB_Data[PortNum].MessageType, PDFU_FRQDB_Data[PortNum].Payload, &size);
        size += 2;
        payload = (USBPD_FWUPD_PdfuDataPauseRspPayload_TypeDef*)PDFU_FRQDB_Data[PortNum].Payload;
        if (USBPD_FWUPD_STATUS_ERR_REJECT_PAUSE != payload->Status)
        {
          PDFU_SWITCH_TO_RESP_SUBSTATE(_pdhandle, PDFU_RSP_TRANSFER_STATE_REQ_RCVD);
          _timing = osWaitForever;
        }
        else
        {
          /* Discard the partially received FW and exit PDFU flow */
          PDFU_SWITCH_TO_STATE(_pdhandle, PDFU_FLOW_DISABLED);
        }
        USBPD_DPM_RequestFirwmwareUpdate(PortNum, USBPD_EXT_FIRM_UPDATE_RESPONSE, (uint8_t*)&PDFU_FRQDB_Data[PortNum], size);
      }
      break;
    default:
      _timing = osWaitForever;
      break;
  }

#if defined(_TRACE)
    uint8_t tab[40];
    size = sprintf((char*)tab, "PDFU resp Transfer state: %s", Tab_RspTransferState[_pdhandle->Rsp_SubStateTransfer]);
    USBPD_TRACE_Add(USBPD_TRACE_DEBUG, PortNum, 0, tab, size);
#endif /* _TRACE */
    return _timing;
}

static uint32_t PDFU_Get_ResponseRcvdTimeout(uint8_t PortNum, uint16_t SizeOutgoing, uint16_t SizeIncoming)
{
  uint32_t _timing = 0;
  PDFU_HandleTypeDef *_pdhandle = &PDFU_Handles[PortNum];

  /* Check if Chunked is enabled or not */
  if (USBPD_TRUE == _pdhandle->PE_Params->PE_UnchunkSupport)
  {
    /* Chunked = 0 */
    _timing = PDFU_TRESPONSERCVD_UNCHUNKED;
  }
  else
  {
    /* Chunked = 1 */
    /* 30 x NumOutgoingChunks + 30 x NumIncomingChunks ms */
    _timing = 30 * ((SizeOutgoing + USBPD_MAX_EXTENDED_MSGLEN_CHUNKLEN) / USBPD_MAX_EXTENDED_MSGLEN_CHUNKLEN)
            + 30 * ((SizeIncoming + USBPD_MAX_EXTENDED_MSGLEN_CHUNKLEN) / USBPD_MAX_EXTENDED_MSGLEN_CHUNKLEN);
  }

  return _timing;
}

static uint32_t PDFU_CheckUnexpectedRequest(uint8_t PortNum, USBPD_FWUPD_MsgType_TypeDef MsgType)
{
  uint32_t _timing = 0;
  uint32_t _size   = 0;

  /* Expected */
  /* Unexpected - Notify as unexpected */
  /* Unexpected - Ignore */
  PDFU_RspStateTypeDef _state   = PDFU_RSP_STATE_EXPECTED;
  PDFU_HandleTypeDef *_pdhandle = &PDFU_Handles[PortNum];
  switch(MsgType)
  {
      case USBPD_FWUPD_MSGTYPE_NONE:
        /* Process linked to timer expiration */
        _state = PDFU_RSP_STATE_UNEXPECTED_IGNORE;
        break;
      case USBPD_FWUPD_MSGTYPE_REQ_GET_FW_ID:
      switch(_pdhandle->CurrentState)
      {
        /* Expected */
        case PDFU_FLOW_ENUMERATION:
          break;
        /* Unexpected - Notify as unexpected */
        case PDFU_FLOW_RECONFIGURATION:
        case PDFU_FLOW_RECONFIGURATION_WAIT:
        case PDFU_FLOW_RECONFIGURATION_WAIT_DATA:
        case PDFU_FLOW_TRANSFER:
        case PDFU_FLOW_VALIDATION:
        case PDFU_FLOW_MANIFESTATION:
        case PDFU_FLOW_DISABLED:
        default:
          _state = PDFU_RSP_STATE_UNEXPECTED_NOTIFY;
          break;
      }
      break;
    case USBPD_FWUPD_MSGTYPE_REQ_PDFU_INITIATE:
      switch(_pdhandle->CurrentState)
      {
        /* Expected */
        case PDFU_FLOW_ENUMERATION:
        case PDFU_FLOW_RECONFIGURATION:
        case PDFU_FLOW_RECONFIGURATION_WAIT:
        case PDFU_FLOW_RECONFIGURATION_WAIT_DATA:
          break;
        /* Unexpected - Notify as unexpected */
        case PDFU_FLOW_TRANSFER:
        case PDFU_FLOW_VALIDATION:
        case PDFU_FLOW_MANIFESTATION:
        case PDFU_FLOW_DISABLED:
        default:
          _state = PDFU_RSP_STATE_UNEXPECTED_NOTIFY;
          break;
      }
      break;
    case USBPD_FWUPD_MSGTYPE_REQ_PDFU_DATA:
      switch(_pdhandle->CurrentState)
      {
        /* Expected */
        case PDFU_FLOW_RECONFIGURATION_WAIT_DATA:
        case PDFU_FLOW_TRANSFER:
          break;
        /* Unexpected - Notify as unexpected */
        case PDFU_FLOW_ENUMERATION:
        case PDFU_FLOW_RECONFIGURATION:
        case PDFU_FLOW_RECONFIGURATION_WAIT:
        case PDFU_FLOW_VALIDATION:
        case PDFU_FLOW_MANIFESTATION:
        case PDFU_FLOW_DISABLED:
        default:
          _state = PDFU_RSP_STATE_UNEXPECTED_NOTIFY;
          break;
      }
      break;
    case USBPD_FWUPD_MSGTYPE_REQ_PDFU_DATA_NR:
      switch(_pdhandle->CurrentState)
      {
        /* Expected */
        case PDFU_FLOW_TRANSFER:
          break;
        /* Unexpected - Ignore */
        case PDFU_FLOW_ENUMERATION:
        case PDFU_FLOW_RECONFIGURATION:
        case PDFU_FLOW_RECONFIGURATION_WAIT:
        case PDFU_FLOW_RECONFIGURATION_WAIT_DATA:
        case PDFU_FLOW_VALIDATION:
        case PDFU_FLOW_MANIFESTATION:
        case PDFU_FLOW_DISABLED:
        default:
          _state = PDFU_RSP_STATE_UNEXPECTED_IGNORE;
          break;
      }
      break;
    case USBPD_FWUPD_MSGTYPE_REQ_PDFU_VALIDATE:
      switch(_pdhandle->CurrentState)
      {
        /* Expected */
        case PDFU_FLOW_VALIDATION:
          break;
        /* Unexpected - Notify as unexpected */
        case PDFU_FLOW_ENUMERATION:
        case PDFU_FLOW_RECONFIGURATION:
        case PDFU_FLOW_RECONFIGURATION_WAIT:
        case PDFU_FLOW_RECONFIGURATION_WAIT_DATA:
        case PDFU_FLOW_TRANSFER:
        case PDFU_FLOW_MANIFESTATION:
        case PDFU_FLOW_DISABLED:
        default:
          _state = PDFU_RSP_STATE_UNEXPECTED_NOTIFY;
          break;
      }
      break;
    case USBPD_FWUPD_MSGTYPE_REQ_PDFU_ABORT:
      switch(_pdhandle->CurrentState)
      {
        /* Expected */
        case PDFU_FLOW_ENUMERATION:
        case PDFU_FLOW_RECONFIGURATION:
        case PDFU_FLOW_RECONFIGURATION_WAIT:
        case PDFU_FLOW_RECONFIGURATION_WAIT_DATA:
        case PDFU_FLOW_TRANSFER:
        case PDFU_FLOW_VALIDATION:
        case PDFU_FLOW_MANIFESTATION:
        case PDFU_FLOW_DISABLED:
        default:
          _state = PDFU_RSP_STATE_EXPECTED_ABORT;
          break;
      }
      break;
    case USBPD_FWUPD_MSGTYPE_REQ_PDFU_DATA_PAUSE:
      switch(_pdhandle->CurrentState)
      {
        /* Expected */
        case PDFU_FLOW_TRANSFER:
          break;
        /* Unexpected - Ignore */
        case PDFU_FLOW_ENUMERATION:
        case PDFU_FLOW_RECONFIGURATION:
        case PDFU_FLOW_RECONFIGURATION_WAIT:
        case PDFU_FLOW_RECONFIGURATION_WAIT_DATA:
        case PDFU_FLOW_VALIDATION:
        case PDFU_FLOW_MANIFESTATION:
        case PDFU_FLOW_DISABLED:
        default:
          _state = PDFU_RSP_STATE_UNEXPECTED_IGNORE;
          break;
      }
      break;
    case USBPD_FWUPD_MSGTYPE_REQ_VENDOR_SPECIFIC:
      if (USBPD_VID != _pdhandle->VIDReceived)
      {
        _state = PDFU_RSP_STATE_UNEXPECTED_NOTIFY_VENDOR;
      }
      else
      {
        _state = PDFU_RSP_STATE_EXPECTED_VENDOR;
      }
      break;
    default:
      _state = PDFU_RSP_STATE_UNEXPECTED_NOTIFY_RESERVED;
      break;
  }

  /* Manage the received message */
  switch (_state)
  {
    case PDFU_RSP_STATE_EXPECTED:
      break;
    case PDFU_RSP_STATE_UNEXPECTED_IGNORE:
      /*
        Ignore
      */
      /* Message to be ignored. Restart timer if running */
      if (IS_PDFU_TIMER_RUNNING(_pdhandle, PDFU_ResponseRcvd))
      {
        _timing = PDFU_TIMER_REMAINING(_pdhandle, PDFU_ResponseRcvd);
      }
      else
      {
        _timing = osWaitForever;
      }
      break;
    case PDFU_RSP_STATE_EXPECTED_VENDOR:
      /* VENDOR Message have been received.*/
      PDFU_FRQDB_Data[PortNum].MessageType      = (_pdhandle->ReceivedMsgID & 0x7F);
      _pdhandle->PDFUCallbacks->USBPD_PDFU_GetDataInfo(PortNum, (USBPD_FWUPD_MsgType_TypeDef)PDFU_FRQDB_Data[PortNum].MessageType, PDFU_FRQDB_Data[PortNum].Payload, &_size);
      _size += 2;
      /* Vendor message to be sent. Restart timer if running */
      if (IS_PDFU_TIMER_RUNNING(_pdhandle, PDFU_ResponseRcvd))
      {
        _timing = PDFU_TIMER_REMAINING(_pdhandle, PDFU_ResponseRcvd);
      }
      else
      {
        _timing = osWaitForever;
      }
      break;
    case PDFU_RSP_STATE_EXPECTED_ABORT:
      /* ABORT Message have been received.*/
      /* Stop the PDFU flow */
      PDFU_SWITCH_TO_STATE(_pdhandle, PDFU_FLOW_DISABLED);
      break;
    case PDFU_RSP_STATE_UNEXPECTED_NOTIFY_VENDOR:
      /*
        Notify as unexpected with status byte set to errUNEXPECTED_REQUEST
        Stay in enumeration phase
      */
      PDFU_FRQDB_Data[PortNum].MessageType      = (_pdhandle->ReceivedMsgID & 0x7F);
      _pdhandle->PDFUCallbacks->USBPD_PDFU_GetDataInfo(PortNum, (USBPD_FWUPD_MsgType_TypeDef)PDFU_FRQDB_Data[PortNum].MessageType, PDFU_FRQDB_Data[PortNum].Payload, &_size);
      PDFU_FRQDB_Data[PortNum].Payload[0]       = USBPD_FWUPD_STATUS_ERR_UNEXPECTED_REQUEST;
      PDFU_FRQDB_Data[PortNum].Payload[1]       = LE16(&_pdhandle->VIDReceived);
      _size = 5;
      /* Stop the PDFU flow */
      PDFU_SWITCH_TO_STATE(_pdhandle, PDFU_FLOW_DISABLED);
      break;
    case PDFU_RSP_STATE_UNEXPECTED_NOTIFY_RESERVED:
      /*
        Reserved requests. Set MsgType to 0x7F
      */
      PDFU_FRQDB_Data[PortNum].MessageType      = 0x7F;
      _size += 2;
      /* Stop the PDFU flow */
      PDFU_SWITCH_TO_STATE(_pdhandle, PDFU_FLOW_DISABLED);
      break;
    case PDFU_RSP_STATE_UNEXPECTED_NOTIFY:
      /* VENDOR Message have been received.*/
      PDFU_FRQDB_Data[PortNum].MessageType      = (_pdhandle->ReceivedMsgID & 0x7F);
      PDFU_FRQDB_Data[PortNum].Payload[0]       = USBPD_FWUPD_STATUS_ERR_UNEXPECTED_REQUEST;
      _size = 3;
      /* Stop the PDFU flow */
      PDFU_SWITCH_TO_STATE(_pdhandle, PDFU_FLOW_DISABLED);
      break;
    default:
#if defined(_TRACE)
      USBPD_TRACE_Add(USBPD_TRACE_DEBUG, PortNum, 0, "Default TBC", 11);
#endif /* _TRACE */
      break;
  }

  /* Response message to be sent ?*/
  if (0 != _size)
  {
    USBPD_DPM_RequestFirwmwareUpdate(PortNum, USBPD_EXT_FIRM_UPDATE_RESPONSE, (uint8_t*)&PDFU_FRQDB_Data[PortNum], _size);
  }
  return _timing;
}

/**
  * @}
  */

/**
  * @}
  */

#endif /* _FWUPDATE_RESPONDER */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

