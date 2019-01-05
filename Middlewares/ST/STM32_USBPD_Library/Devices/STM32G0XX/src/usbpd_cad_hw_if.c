/**
  ******************************************************************************
  * @file    usbpd_cad_hw_if.c
  * @author  MCD Application Team
  * @brief   This file contains power hardware interface cad functions.
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
#include "usbpd_devices_conf.h"
#include "usbpd_hw.h"
#include "usbpd_def.h"
#include "usbpd_cad_hw_if.h"
#include "usbpd_hw_if.h"
#include "usbpd_porthandle.h"
#include "usbpd_trace.h"

#ifdef _DISCO
#include "stm32g071b_discovery.h"
#include "stm32g071b_discovery_pwrmon.h"
#endif
/** @addtogroup STM32_USBPD_LIBRARY
  * @{
  */

/** @addtogroup USBPD_DEVICE
  * @{
  */

/** @addtogroup USBPD_DEVICE_CAD_HW_IF
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief CAD State value @ref USBPD_DEVICE_CAD_HW_IF
  * @{
  */
typedef enum
{
  USBPD_CAD_STATE_RESET               = 0,  /*!< USBPD CAD State Reset                              */
  USBPD_CAD_STATE_DETACHED            = 1,  /*!< USBPD CAD State No cable detected                  */
  USBPD_CAD_STATE_ATTACHED_WAIT       = 2,  /*!< USBPD CAD State Port partner detected              */
  USBPD_CAD_STATE_ATTACHED            = 3,  /*!< USBPD CAD State Port partner attached              */
  USBPD_CAD_STATE_EMC                 = 4,  /*!< USBPD CAD State Electronically Marked Cable detected */
  USBPD_CAD_STATE_ATTEMC              = 5,  /*!< USBPD CAD State Port Partner detected throug EMC   */
  USBPD_CAD_STATE_ACCESSORY           = 6,  /*!< USBPD CAD State Accessory detected                 */
  USBPD_CAD_STATE_DEBUG               = 7,  /*!< USBPD CAD State Debug detected                     */
  USBPD_CAD_STATE_SWITCH_TO_SRC       = 8,  /*!< USBPD CAD State switch to Source                   */
  USBPD_CAD_STATE_SWITCH_TO_SNK       = 9,  /*!< USBPD CAD State switch to Sink                     */
  USBPD_CAD_STATE_UNKNOW              = 10, /*!< USBPD CAD State unknow                             */
  //USBPD_CAD_STATE_ATTACHED_LEGACY     = 11, /*!< USBPD CAD State Port partner attached to legacy cable */
  USBPD_CAD_STATE_ERRORRECOVERY       = 12, /*!< USBPD CAD State error recovery                     */
  USBPD_CAD_STATE_ERRORRECOVERY_EXIT  = 13, /*!< USBPD CAD State to exit error recovery             */
} USBPD_CAD_STATE;
/**
  * @}
  */

/**
  * @brief USB PD CC lines HW condition
  */
typedef enum
{
  HW_Detachment                         = 0x00,    /*!< Nothing attached   */
  HW_Attachment                         = 0x01,    /*!< Sink attached   */
  HW_PwrCable_NoSink_Attachment         = 0x02,    /*!< Powered cable without Sink attached   */
  HW_PwrCable_Sink_Attachment           = 0x03,    /*!< Powered cable with Sink or VCONN-powered Accessory attached   */
  HW_Debug_Attachment                   = 0x04,    /*!< Debug Accessory Mode attached   */
  HW_AudioAdapter_Attachment            = 0x05     /*!< Audio Adapter Accessory Mode attached   */
} CAD_HW_Condition_TypeDef;


/**
  * @brief CAD State value @ref USBPD_DEVICE_CAD_HW_IF
  * @{
  */
typedef struct
{
  USBPD_CAD_STATE cstate                                    :4; /* current state  */
  USBPD_CAD_STATE pstate                                    :4; /* previous state */
  CCxPin_TypeDef  cc                                        :2;
  CAD_HW_Condition_TypeDef    CurrentHWcondition            :3;
  CAD_SNK_Source_Current_Adv_Typedef SNK_Source_Current_Adv :2;
  uint32_t CAD_tDebounce_flag                               :1;
  uint32_t CAD_ErrorRecoveryflag                            :1;
#if defined(_DRP) || defined(_SRC)
  uint32_t CAD_ResistorUpdateflag                           :1;
  uint32_t reserved                                         :12;
#else
  uint32_t reserved                                         :13;
#endif /* _SRC || _DRP */

#if defined(_DRP)
  uint32_t CAD_tToogle_start;
#endif /* _DRP */
  uint32_t CAD_tDebounce_start, CAD_tDebounce;      /* Variable used for attach or detach debounce timers */
} CAD_HW_HandleTypeDef;
/**
  * @}
  */

/* Private define ------------------------------------------------------------*/
#define CAD_TPDDEBOUCE_THRESHOLD         12u          /**< tPDDebounce threshold = 20ms                       */
#define CAD_TCCDEBOUCE_THRESHOLD         100u         /**< tCCDebounce threshold = 100ms                      */
#define CAD_TSRCDISCONNECT_THRESHOLD     1u           /**< tSRCDisconnect detach threshold between 0 to 20ms  */
#define CAD_INFINITE_TIME                0xFFFFFFFFu  /**< infinite time to wait a new interrupt event        */
#define CAD_TERROR_RECOVERY_TIME         26u          /**< tErrorRecovery min 25ms                            */

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
extern USBPD_PORT_HandleTypeDef Ports[USBPD_PORT_COUNT];

/* handle to manage the detection state machine */
static CAD_HW_HandleTypeDef CAD_HW_Handles[USBPD_PORT_COUNT];

/* Private function prototypes -----------------------------------------------*/
/** @defgroup USBPD_DEVICE_CAD_HW_IF_Private_Functions USBPD DEVICE_CAD HW IF Private Functions
  * @{
  */
static CCxPin_TypeDef CAD_Check_HW(uint8_t PortNum);
#if defined(_DRP) || defined(_SNK)
static uint8_t CAD_Check_VBus(uint8_t PortNum);
#endif /* _DRP || _SNK */

/**
  * @}
  */

/** @defgroup USBPD_DEVICE_CAD_HW_IF_Exported_Functions USBPD DEVICE_CAD HW IF Exported Functions
  * @{
  */

/**
  * @brief  CAD initialization function
  * @param  PortNum       port
  * @param  pSettings     Pointer on PD settings based on @ref USBPD_SettingsTypeDef
  * @param  pParams       Pointer on PD params based on @ref USBPD_ParamsTypeDef
  * @param  WakeUp        Wake-up callback function used for waking up CAD (not used by STM32F072 device)
  * @retval None
  */
void CAD_Init(uint8_t PortNum, USBPD_SettingsTypeDef *pSettings, USBPD_ParamsTypeDef *pParams,  void (*WakeUp)(void))
{
  CAD_HW_HandleTypeDef *_handle = &CAD_HW_Handles[PortNum];
  LL_UCPD_InitTypeDef settings;

  Ports[PortNum].params = pParams;
#ifndef _VALID_RP
#define _VALID_RP vRp_3_0A
#endif
  Ports[PortNum].params->RpResistor = _VALID_RP;
  Ports[PortNum].settings = pSettings;
  _handle->cstate = USBPD_CAD_STATE_RESET;
  _handle->cc = CCNONE;
  _handle->CurrentHWcondition = HW_Detachment;
  _handle->SNK_Source_Current_Adv = vRd_Undefined;

  Ports[PortNum].USBPD_CAD_WakeUp = WakeUp;

  /* Initialise the USBPD_IP */
  Ports[PortNum].husbpd = USBPD_HW_GetUSPDInstance(PortNum);

  /* initialise usbpd */
  LL_UCPD_StructInit(&settings);
  (void)LL_UCPD_Init(Ports[PortNum].husbpd, &settings);
  LL_UCPD_SetRxOrderSet(Ports[PortNum].husbpd, LL_UCPD_ORDERSET_SOP | LL_UCPD_ORDERSET_SOP1 | LL_UCPD_ORDERSET_SOP2 | LL_UCPD_ORDERSET_CABLERST | LL_UCPD_ORDERSET_HARDRST);
  LL_UCPD_SetccEnable(Ports[PortNum].husbpd, LL_UCPD_CCENABLE_CC1CC2);
#ifdef _LOW_POWER
  LL_UCPD_WakeUpEnable(Ports[PortNum].husbpd);
#endif

#ifdef _DISCO
  /* Remove the EN1 enable CC1 CC2 */
  BSP_MOSFET_Init(MOSFET_ENCC1);
  BSP_MOSFET_Init(MOSFET_ENCC2);
  BSP_MOSFET_On(MOSFET_ENCC1);
  BSP_MOSFET_Off(MOSFET_ENCC2);
#endif
  /* enable usbpd IP */

  LL_UCPD_Enable(Ports[PortNum].husbpd);

#if defined(_SRC) || defined(_DRP)
  /* Intialise usbpd interrupt */
  if (USBPD_PORTPOWERROLE_SRC == Ports[PortNum].params->PE_PowerRole)
  {
    USBPDM1_AssertRp(PortNum);
  }
  else
#endif /* _SRC || _DRP */
  {
    USBPDM1_AssertRd(PortNum);
  }

#ifdef _DISCO
  BSP_MOSFET_Init(MOSFET_RD_CC1);
  BSP_MOSFET_Off(MOSFET_RD_CC1);
#endif
}

/**
  * @brief  CAD enter error recovery state
  * @param  PortNum     Index of current used port
  * @retval None
  */
void CAD_Enter_ErrorRecovery(uint8_t PortNum)
{
  /* set the error recovery flag to allow the stack to switch into errorRecovery Flag */
  CAD_HW_Handles[PortNum].CAD_ErrorRecoveryflag = USBPD_TRUE;
  Ports[PortNum].USBPD_CAD_WakeUp();
}

/**
  * @brief  Set RPvalue for RP resistor
  * @param  PortNum     Index of current used port
  * @param  RpValue     RP value to set in devices based on @ref CAD_RP_Source_Current_Adv_Typedef
  * @retval 0 success else error
  */
uint32_t CAD_Set_ResistorRp(uint8_t PortNum, CAD_RP_Source_Current_Adv_Typedef RpValue)
{
#if defined(_DRP) || defined(_SRC)
  /* update the information about the default resitor value presented in detach mode */
  Ports[PortNum].params->RpResistor = RpValue;

  /* inform state machine about a resistor update */
  CAD_HW_Handles[PortNum].CAD_ResistorUpdateflag = USBPD_TRUE;
  Ports[PortNum].USBPD_CAD_WakeUp();
#endif /* _SRC || _DRP */
  return 0;
}

/**
  * @brief  CAD State machine
  * @param  PortNum Port
  * @param  Event   Pointer on CAD event based on @ref USBPD_CAD_EVENT
  * @param  CCXX    Pointer on CC Pin based on @ref CCxPin_TypeDef
  * @retval Timeout value
  */
uint32_t CAD_StateMachine(uint8_t PortNum, USBPD_CAD_EVENT *pEvent, CCxPin_TypeDef *pCCXX)
{
  CAD_HW_HandleTypeDef *_handle = &CAD_HW_Handles[PortNum];
  uint32_t _timing = 2;

  /* set by default event to none */
  *pEvent = USBPD_CAD_EVENT_NONE;

  if (Ports[PortNum].params->PE_SwapOngoing)
  {
    return _timing;
  }

  if(_handle->CAD_ErrorRecoveryflag == USBPD_TRUE)
  {
    /* Force the state error recovery */
    _handle->CAD_ErrorRecoveryflag = USBPD_FALSE;
    _handle->cstate = USBPD_CAD_STATE_ERRORRECOVERY;
#if defined(__DEBUG_CAD) && defined(_TRACE)
    USBPD_TRACE_Add(USBPD_TRACE_CAD_LOW, PortNum, _handle->cstate, NULL, 0);
#endif
  }

  /*Check CAD STATE*/
  switch (_handle->cstate)
  {
    case USBPD_CAD_STATE_SWITCH_TO_SRC:
    case USBPD_CAD_STATE_SWITCH_TO_SNK:
    {
#if defined(_DRP)
      if (Ports[PortNum].settings->CAD_RoleToggle == USBPD_TRUE)
      {
        if (USBPD_CAD_STATE_SWITCH_TO_SRC == _handle->cstate)
        {
          USBPDM1_AssertRp(PortNum);
          Ports[PortNum].params->PE_PowerRole = USBPD_PORTPOWERROLE_SRC;
          _timing = Ports[PortNum].settings->CAD_SRCToogleTime;
        }
        if (USBPD_CAD_STATE_SWITCH_TO_SNK == _handle->cstate)
        {
          USBPDM1_AssertRd(PortNum);
          Ports[PortNum].params->PE_PowerRole = USBPD_PORTPOWERROLE_SNK;
          _timing = Ports[PortNum].settings->CAD_SNKToogleTime;
        }
        _handle->CAD_tToogle_start = HAL_GetTick();
      }
#endif /* _DRP */
      /* execute detach to present the new resistor according the configuration selected */
      HW_SignalDetachment(PortNum);
      _handle->cstate = USBPD_CAD_STATE_DETACHED;
      break;
    }

    case USBPD_CAD_STATE_RESET:
    {
      /* on the first call of the usbpd state machine the interrupt and CC pin are enabled */
      LL_UCPD_EnableIT_TypeCEventCC2(Ports[PortNum].husbpd);
      LL_UCPD_EnableIT_TypeCEventCC1(Ports[PortNum].husbpd);
#if defined(_DRP)
      if (Ports[PortNum].settings->CAD_RoleToggle == USBPD_TRUE)
      {
        _handle->CAD_tToogle_start = HAL_GetTick();
      }
#endif /* _DRP */
      UCPD_INSTANCE0_ENABLEIRQ;
      UCPD_INSTANCE1_ENABLEIRQ;
      _handle->cstate = USBPD_CAD_STATE_DETACHED;
      break;
    }

    case USBPD_CAD_STATE_DETACHED:
    {
#if defined(_DRP) || defined(_SRC)
      if((_handle->CAD_ResistorUpdateflag == USBPD_TRUE) && (USBPD_PORTPOWERROLE_SRC == Ports[PortNum].params->PE_PowerRole))
      {
        /* update the reistor value */
        USBPDM1_AssertRp(PortNum);
        _handle->CAD_ResistorUpdateflag = USBPD_FALSE;

        /* let time to internal state machine update */
        HAL_Delay(1);
      }
#endif /* _DRP || _SRC */

      _handle->cc = CAD_Check_HW(PortNum);
      /* Change the status on the basis of the HW event given by CAD_Check_HW() */
      if (_handle->CurrentHWcondition == HW_Detachment)
      {
#if defined(_DRP)
        /* check if role switch must be perform and set the correct sleep time allowed */
        if (USBPD_TRUE == Ports[PortNum].settings->CAD_RoleToggle)
        {
          switch (Ports[PortNum].params->PE_PowerRole)
          {
            case USBPD_PORTPOWERROLE_SRC :
              if ((HAL_GetTick() - _handle->CAD_tToogle_start) > Ports[PortNum].settings->CAD_SRCToogleTime)
              {
                _handle->CAD_tToogle_start = HAL_GetTick();
                Ports[PortNum].params->PE_PowerRole = USBPD_PORTPOWERROLE_SNK;
                _timing = Ports[PortNum].settings->CAD_SNKToogleTime;
                USBPDM1_AssertRd(PortNum);
              }
              break;
            case USBPD_PORTPOWERROLE_SNK :
              if ((HAL_GetTick() - _handle->CAD_tToogle_start) > Ports[PortNum].settings->CAD_SNKToogleTime)
              {
                _handle->CAD_tToogle_start = HAL_GetTick();
                Ports[PortNum].params->PE_PowerRole = USBPD_PORTPOWERROLE_SRC;
                _timing = Ports[PortNum].settings->CAD_SRCToogleTime;
                USBPDM1_AssertRp(PortNum);
              }
              break;
          }
        }
        else
#endif /* _DRP */
        {
          _timing = CAD_INFINITE_TIME;
        }
      }
      else
      {
        if (_handle->CurrentHWcondition == HW_PwrCable_NoSink_Attachment)
        {
          _handle->cstate = USBPD_CAD_STATE_EMC;
        }
        else
        {
#if defined(_DRP) || defined(_SNK)
#if 0
          /* Check if legacy cable have detected (refer to Type-C specification
             'Legacy Cable Assemblies' */
          if ((USBPD_PORTPOWERROLE_SNK == Ports[PortNum].params->PE_PowerRole)
              && CAD_Check_VBus(PortNum)
              && (vRd_USB == _handle->SNK_Source_Current_Adv))
          {
            /* VBUS is already present. Port Partner does not support PD protocol (USB Type A/B)
               Only Default USB Type-C Current Rp resistor (56 k) has been detected */
            _handle->cstate = USBPD_CAD_STATE_ATTACHED_LEGACY;
            *pEvent         = USBPD_CAD_EVENT_LEGACY;
            _timing         = CAD_INFINITE_TIME;
          }
          else
#endif
#endif /* _DRP || _SNK */
          {
            _handle->cstate = USBPD_CAD_STATE_ATTACHED_WAIT;
            /* Get the time of this event */
            _handle->CAD_tDebounce_start = HAL_GetTick();
            _timing = CAD_TCCDEBOUCE_THRESHOLD + 1;
          }
        }
      }
      break;
    }

    /*CAD STATE ATTACHED WAIT*/
    case USBPD_CAD_STATE_ATTACHED_WAIT:
    {
      /* Evaluate elapsed time in Attach_Wait state */
      _handle->CAD_tDebounce = HAL_GetTick() - _handle->CAD_tDebounce_start;
      _handle->cc = CAD_Check_HW(PortNum);

      if ((_handle->CurrentHWcondition != HW_Detachment) && (_handle->CurrentHWcondition != HW_PwrCable_NoSink_Attachment))
      {
#if defined(_DRP) || defined(_SRC)
        if ((BSP_PWR_VBUSGetVoltage(PortNum) > BSP_PWR_LOW_VBUS_THRESHOLD)
            && (USBPD_PORTPOWERROLE_SRC == Ports[PortNum].params->PE_PowerRole))
        {
          _handle->cstate = USBPD_CAD_STATE_ATTACHED_WAIT;
          /* Get the time of this event */
          _handle->CAD_tDebounce_start = HAL_GetTick();
          return CAD_TCCDEBOUCE_THRESHOLD;
        }
#endif /* _DRP || _SRC */

        /* Check tCCDebounce */
        if (_handle->CAD_tDebounce > CAD_TCCDEBOUCE_THRESHOLD)
        {
#if defined(_DRP) || defined(_SRC)
          /* if tCCDebounce has expired state must be changed*/
          if (USBPD_PORTPOWERROLE_SRC == Ports[PortNum].params->PE_PowerRole)
          {
            switch (_handle->CurrentHWcondition)
            {
              case HW_Attachment:
                HW_SignalAttachement(PortNum, _handle->cc);
                _handle->cstate = USBPD_CAD_STATE_ATTACHED;
                *pEvent = USBPD_CAD_EVENT_ATTACHED;
                *pCCXX = _handle->cc;
                break;

              case HW_PwrCable_NoSink_Attachment:
                _handle->cstate = USBPD_CAD_STATE_EMC;
                *pEvent = USBPD_CAD_EVENT_EMC;
                *pCCXX = _handle->cc;
                break;

              case HW_PwrCable_Sink_Attachment:
                HW_SignalAttachement(PortNum, _handle->cc);
                _handle->cstate = USBPD_CAD_STATE_ATTEMC;
                *pEvent = USBPD_CAD_EVENT_ATTEMC;
                break;

              case HW_Debug_Attachment:
                _handle->cstate = USBPD_CAD_STATE_DEBUG;
                *pEvent = USBPD_CAD_EVENT_DEBUG;
                break;

              case HW_AudioAdapter_Attachment:
                _handle->cstate = USBPD_CAD_STATE_ACCESSORY;
                *pEvent = USBPD_CAD_EVENT_ACCESSORY;
                break;

              case HW_Detachment:
              default:
                break;
            } /* end of switch */
            *pCCXX = _handle->cc;
            _timing = 2;
          }
#if defined(_DRP)
          else /* Check state transition for SNK role */
#endif /* _DRP */
#endif /* _DRP || _SRC */
#if defined(_DRP) || defined(_SNK)
          {
            _timing = 2;
            if (CAD_Check_VBus(PortNum)) /* Check if Vbus is on */
            {
              _handle->cstate = USBPD_CAD_STATE_ATTACHED;
              *pEvent = USBPD_CAD_EVENT_ATTACHED;
              *pCCXX = _handle->cc;
              HW_SignalAttachement(PortNum, _handle->cc);
            }
          }
#endif /* _DRP || _SNK */
        }
        else
        {
          _timing = CAD_TCCDEBOUCE_THRESHOLD - _handle->CAD_tDebounce;
        }
        /* reset the flag for CAD_tDebounce */
        _handle->CAD_tDebounce_flag = USBPD_FALSE;
      }
      else /* CAD_HW_Condition[PortNum] = HW_Detachment */
      {
#if defined(_DRP) || defined(_SNK)
        if (USBPD_PORTPOWERROLE_SNK == Ports[PortNum].params->PE_PowerRole)
        {
          /* start counting of CAD_tDebounce */
          if (USBPD_FALSE == _handle->CAD_tDebounce_flag)
          {
            _handle->CAD_tDebounce_start = HAL_GetTick();
            _handle->CAD_tDebounce_flag = USBPD_TRUE;
            _timing = CAD_TPDDEBOUCE_THRESHOLD;
          }
          else /* CAD_tDebounce already running */
          {
            /* evaluate CAD_tDebounce */
            _handle->CAD_tDebounce = HAL_GetTick() - _handle->CAD_tDebounce_start;
            if((_handle->CAD_tDebounce > CAD_TPDDEBOUCE_THRESHOLD))
            {
              _handle->CAD_tDebounce_flag = USBPD_FALSE;
              _handle->cstate             = USBPD_CAD_STATE_SWITCH_TO_SRC;
            }
          }
        }
#if defined(_DRP)
        else /* (hcad->PortPowerRole != USBPD_PORTPOWERROLE_SNK)*/
#endif /* _DRP */
#endif /* _DRP || _SNK */
#if defined(_DRP) || defined(_SRC)
        {
          /* start counting of CAD_tDebounce */
          if (USBPD_FALSE == _handle->CAD_tDebounce_flag)
          {
            _handle->CAD_tDebounce_start  = HAL_GetTick();
            _handle->CAD_tDebounce_flag   = USBPD_TRUE;
            _timing                       = CAD_TSRCDISCONNECT_THRESHOLD;
          }
          else /* CAD_tDebounce already running */
          {
            /* evaluate CAD_tDebounce */
            _handle->CAD_tDebounce = HAL_GetTick() - _handle->CAD_tDebounce_start;
            if(_handle->CAD_tDebounce > CAD_TSRCDISCONNECT_THRESHOLD)
            {
              _handle->CAD_tDebounce_flag = USBPD_FALSE;
              _handle->cstate             = USBPD_CAD_STATE_SWITCH_TO_SNK;
            }
          }
        }
#endif /* _DRP || _SRC */
      }
    break;
    }

  /* CAD ELECTRONIC CABLE ATTACHED */
  case USBPD_CAD_STATE_EMC :
    {
      _timing = CAD_INFINITE_TIME;
      _handle->cc = CAD_Check_HW(PortNum);
      /* Change the status on the basis of the HW event given by CAD_Check_HW() */
      switch(_handle->CurrentHWcondition)
      {
      case HW_Detachment :
        _handle->cstate = USBPD_CAD_STATE_SWITCH_TO_SNK;
        *pEvent = USBPD_CAD_EVENT_DETACHED;
        *pCCXX = CCNONE;
        _timing = 1;
        break;
      case HW_PwrCable_Sink_Attachment:
      case HW_Attachment :
        _handle->cstate = USBPD_CAD_STATE_ATTACHED_WAIT;
        _handle->CAD_tDebounce_start = HAL_GetTick()-5;  /* this is only to check cable presence */
        _timing = 2;
        break;
      default :
      case HW_PwrCable_NoSink_Attachment:
        /* nothing to do still the same status */
#if defined(_DRP)
        if(USBPD_TRUE == Ports[PortNum].settings->CAD_RoleToggle)
        {
          if((HAL_GetTick() - _handle->CAD_tToogle_start) > Ports[PortNum].settings->CAD_SRCToogleTime)
          {
            _handle->cstate = USBPD_CAD_STATE_SWITCH_TO_SNK;
          }
          _timing = 0;
        }
#endif /* _DRP */
        break;
      }
      break;
    }

#if defined(_DRP) || defined(_SNK)
#if 0
  /*CAD STATE ATTACHED TO LEGACY CABLES */
  case USBPD_CAD_STATE_ATTACHED_LEGACY:
  {
    if (CAD_Check_VBus(PortNum) == 0) /* Check if Vbus is off */
    {
      HW_SignalDetachment(PortNum);
      _handle->cstate             = USBPD_CAD_STATE_SWITCH_TO_SRC;
      _handle->CurrentHWcondition = HW_Detachment;
      /* restart the toggle time */
#if defined(_DRP)
      _handle->CAD_tToogle_start  = HAL_GetTick();
#endif /* _DRP */
      *pEvent                     = USBPD_CAD_EVENT_DETACHED;
      *pCCXX                      = CCNONE;
    }
  }
  break;
#endif
#endif /* _DRP || _SNK */

  /*CAD electronic cable with Sink ATTACHED*/
  case USBPD_CAD_STATE_ATTEMC:
  case USBPD_CAD_STATE_ATTACHED:
    {
#if defined(_DRP) || defined(_SRC)
      if(USBPD_PORTPOWERROLE_SRC == Ports[PortNum].params->PE_PowerRole)
      {
        uint32_t ccx  = Ports[PortNum].CCx == CC1 ? Ports[PortNum].CC1: Ports[PortNum].CC2;
        uint32_t comp = Ports[PortNum].CCx == CC1 ? LL_UCPD_SRC_CC1_OPEN: LL_UCPD_SRC_CC2_OPEN;

        if (comp == ccx)
        {
          /* start counting of CAD_tDebounce */
          if (USBPD_FALSE == _handle->CAD_tDebounce_flag)
          {
            _handle->CAD_tDebounce_start  = HAL_GetTick();
            _handle->CAD_tDebounce_flag   = USBPD_TRUE;
            _timing                       = CAD_TSRCDISCONNECT_THRESHOLD;
          }
          else /* CAD_tDebounce already running */
          {
            /* evaluate CAD_tDebounce */
            _handle->CAD_tDebounce = HAL_GetTick() - _handle->CAD_tDebounce_start;
            if(_handle->CAD_tDebounce > CAD_TSRCDISCONNECT_THRESHOLD)
            {
              _handle->CAD_tDebounce_flag = USBPD_FALSE;
              // move inside state DETACH to avoid wrong VCONN level
              //HW_SignalDetachment(PortNum);
              _handle->cstate             = USBPD_CAD_STATE_SWITCH_TO_SNK;
              *pEvent                     = USBPD_CAD_EVENT_DETACHED;
              *pCCXX                      = CCNONE;
              _timing                     = 0;
            }
          }
        }
        else
        {
          /* Reset tPDDebounce flag*/
          _handle->CAD_tDebounce_flag   = USBPD_FALSE;
          _timing = CAD_INFINITE_TIME;
        }
      }
#if defined(_DRP)
      else /* USBPD_PORTPOWERROLE_SNK case */
#endif /* _DRP */
#endif /*_DRP || _SRC */
#if defined(_DRP) || defined(_SNK)
      {
        if (CAD_Check_VBus(PortNum) == 0) /* Check if Vbus is off */
        {
          _handle->CurrentHWcondition = HW_Detachment;
          /* restart the toggle time */
#if defined(_DRP)
          _handle->CAD_tToogle_start = HAL_GetTick();
#endif /* _DRP */
          _handle->cstate = USBPD_CAD_STATE_SWITCH_TO_SRC;
          *pEvent = USBPD_CAD_EVENT_DETACHED;
          *pCCXX = CCNONE;
          _timing = 0;
        }
        else
        {
          _timing = 10;
        }
      }
#endif /*_DRP || _SNK */
    break;
    }

  /*CAD STATE AUDIO ACCESSORY ATTACHED*/
  case USBPD_CAD_STATE_ACCESSORY:
    {
      _timing = CAD_INFINITE_TIME;
      _handle->cc = CAD_Check_HW(PortNum);
      if (_handle->CurrentHWcondition != HW_AudioAdapter_Attachment)
      {
        HW_SignalDetachment(PortNum);
        _handle->cstate = USBPD_CAD_STATE_SWITCH_TO_SNK;
        *pCCXX = _handle->cc = CCNONE;
        *pEvent = USBPD_CAD_EVENT_DETACHED;
      }
      break;
    }

  /*CAD STATE DEBUG ACCESSORY MODE ATTACHED*/
  case USBPD_CAD_STATE_DEBUG:
    {
      _timing = CAD_INFINITE_TIME;
      _handle->cc = CAD_Check_HW(PortNum);
      if (_handle->CurrentHWcondition != HW_Debug_Attachment)
      {
        HW_SignalDetachment(PortNum);
        _handle->cstate = USBPD_CAD_STATE_SWITCH_TO_SNK;
        *pCCXX = _handle->cc = CCNONE;
        *pEvent = USBPD_CAD_EVENT_DETACHED;
      }
      break;
    }

  case USBPD_CAD_STATE_ERRORRECOVERY :
    {
      /* Remove the resistor */
      /* Enter recovery = Switch to SRC with no resistor */
      USBPDM1_EnterErrorRecovery(1);

      /* forward detach event to DPM */
      *pCCXX = _handle->cc = CCNONE;
      *pEvent = USBPD_CAD_EVENT_DETACHED;

      /* start tErrorRecovery timeout */
      _handle->CAD_tDebounce_start = HAL_GetTick();
      _timing = CAD_TERROR_RECOVERY_TIME;
      _handle->cstate = USBPD_CAD_STATE_SWITCH_TO_SNK;
      break;
    }

  case USBPD_CAD_STATE_ERRORRECOVERY_EXIT :
    {
      if((HAL_GetTick() - _handle->CAD_tDebounce_start) >  CAD_TERROR_RECOVERY_TIME)
      {
        /* reconfigure the port
        port source  to src
        port snk     to snk
        port drp     to src   */
#if defined(_DRP) || defined(_SRC)
        if((USBPD_PORTPOWERROLE_SRC == Ports[PortNum].settings->PE_DefaultRole) || (Ports[PortNum].settings->CAD_RoleToggle == USBPD_TRUE))
        {
          USBPDM1_AssertRp(PortNum);
          Ports[PortNum].params->PE_PowerRole = USBPD_PORTPOWERROLE_SRC;
        }
#if defined(_DRP)
        else
#endif /* _DRP */
#endif /* _DRP || _SRC */
#if defined(_DRP) || defined(_SNK)
        {
          USBPDM1_AssertRd(PortNum);
          Ports[PortNum].params->PE_PowerRole = USBPD_PORTPOWERROLE_SNK;
        }
#endif /* _DRP || _SNK */
        /* switch to state detach */
        _handle->CAD_tDebounce_start = HAL_GetTick();
        _handle->cstate = USBPD_CAD_STATE_DETACHED;
      }
      break;
    }

  default :
    break;
  }

#if defined(__DEBUG_CAD) && defined(_TRACE)
  if (_handle->cstate != _handle->pstate)
  {
    _handle->pstate = _handle->cstate;
    USBPD_TRACE_Add(USBPD_TRACE_CAD_LOW, PortNum, _handle->cstate, NULL, 0);
  }
#endif /* defined(__DEBUG_CAD) && defined(_TRACE) */

#if !defined(_RTOS)
  /* Delay added for CC detection issue on NRTOS version */
  HAL_Delay(1);
#endif /*_RTOS*/
  return _timing;
}

/**
  * @}
  */

/** @addtogroup USBPD_DEVICE_CAD_HW_IF_Private_Functions
  * @{
  */

#if defined(_DRP) || defined(_SNK)
/**
  * @brief  Check if VBus is present or not
  * @param  PortNum  port
  * @retval Return 1 is VBUS is present (0 otherwise)
  */
uint8_t CAD_Check_VBus(uint8_t PortNum)
{
  return  HW_IF_PWR_GetVoltage(PortNum) > BSP_PWR_HIGH_VBUS_THRESHOLD ? USBPD_TRUE : USBPD_FALSE;
}
#endif /* _DRP || _SNK */

/**
  * @brief  Check CCx HW condition
  * @param  PortNum                     port
  * @retval CC pin line based on @ref CCxPin_TypeDef
  */
CCxPin_TypeDef CAD_Check_HW(uint8_t PortNum)
{
  uint32_t CC1_value, CC2_value;
  CAD_HW_HandleTypeDef *_handle = &CAD_HW_Handles[PortNum];
  CCxPin_TypeDef _CCx = CCNONE;
  /*
  ----------------------------------------------------------------------------
  | ANAMODE   |  ANASUBMODE[1:0]  |  Notes      |  TYPEC_VSTATE_CCx[1:0]      |
  |           |                   |             |  00  |  01  |  10  |  11    |
  ----------------------------------------------------------------------------
  | 0: Source | 00: Disabled      |Disabled N/A |         NA                  |
  |           | 01: Default USB Rp|             |vRaDef|vRdDef|vOPENDef|      |
  |           | 10: 1.5A Rp       |             |vRa1.5|vRd1.5|vOPEN1.5| NA   |
  |           | 11: 3.0A Rp       |             |vRa3.0|vRd3.0|vOPEN3.0| NA   |
  -----------------------------------------------------------------------------
  | 1: Sink   |                   |             |xx vRa|vRdUSB| vRd1.5 |vRd3.0|
  -----------------------------------------------------------------------------
  */
  CC1_value = Ports[PortNum].CC1;
  CC2_value = Ports[PortNum].CC2;

  _handle->CurrentHWcondition     = HW_Detachment;
  _handle->SNK_Source_Current_Adv = vRd_Undefined;

#if defined(_DRP) || defined(_SRC)
  if (USBPD_PORTPOWERROLE_SRC == Ports[PortNum].params->PE_PowerRole)
  {
    _handle->CurrentHWcondition = HW_Detachment;
    switch (CC1_value)
    {
    case LL_UCPD_SRC_CC1_VRA :
      {
        switch(CC2_value)
        {
          case LL_UCPD_SRC_CC2_VRA:
            _handle->CurrentHWcondition = HW_AudioAdapter_Attachment;
            _CCx = CC2;
            break;
          case LL_UCPD_SRC_CC2_VRD:
            _handle->CurrentHWcondition = HW_PwrCable_Sink_Attachment;
            _CCx = CC2;
            break;
          case LL_UCPD_SRC_CC2_OPEN:
            _handle->CurrentHWcondition = HW_PwrCable_NoSink_Attachment;
            _CCx = CC2;
            break;
        }
      }
      break;
    case LL_UCPD_SRC_CC1_VRD :
      {
        switch (CC2_value)
        {
          case LL_UCPD_SRC_CC2_VRA:
            _handle->CurrentHWcondition = HW_PwrCable_Sink_Attachment;
            _CCx = CC1;
            break;
          case LL_UCPD_SRC_CC2_VRD:
            _handle->CurrentHWcondition = HW_Debug_Attachment;
            _CCx = CC1;
            break;
          case LL_UCPD_SRC_CC2_OPEN:
            _handle->CurrentHWcondition = HW_Attachment;
            _CCx = CC1;
            break;
        }
      }
      break;
    case LL_UCPD_SRC_CC1_OPEN:
      {
        switch (CC2_value)
        {
          case LL_UCPD_SRC_CC2_VRA:
            _handle->CurrentHWcondition = HW_PwrCable_NoSink_Attachment;
            _CCx = CC2;
            break;
          case LL_UCPD_SRC_CC2_VRD:
            _handle->CurrentHWcondition = HW_Attachment;
            _CCx = CC2;
            break;
          case LL_UCPD_SRC_CC2_OPEN:
            _handle->CurrentHWcondition = HW_Detachment;
            _CCx = CCNONE;
            break;
        }
      }
      break;
    }
  }
#if defined(_DRP)
  else /* USBPD_PORTPOWERROLE_SNK */
#endif /* _DRP */
#endif /*_DRP || _SRC */
#if defined(_DRP) || defined(_SNK)
  {
    if ((CC1_value != LL_UCPD_SNK_CC1_VOPEN) && (CC2_value == LL_UCPD_SNK_CC2_VOPEN))
    {
      _handle->CurrentHWcondition = HW_Attachment;
      _CCx = CC1;
      switch (CC1_value)
      {
        case LL_UCPD_SNK_CC1_VRP:
          _handle->SNK_Source_Current_Adv = vRd_USB;
          break;
        case LL_UCPD_SNK_CC1_VRP15A:
          _handle->SNK_Source_Current_Adv = vRd_1_5A;
          break;
        case LL_UCPD_SNK_CC1_VRP30A:
          _handle->SNK_Source_Current_Adv = vRd_3_0A;
          break;
        default:
          break;
      }
    }
    if ((CC2_value != LL_UCPD_SNK_CC2_VOPEN) && (CC1_value == LL_UCPD_SNK_CC1_VOPEN))
    {
      _handle->CurrentHWcondition = HW_Attachment;
      _CCx = CC2;
      switch (CC2_value)
      {
        case LL_UCPD_SNK_CC2_VRP:
          _handle->SNK_Source_Current_Adv = vRd_USB;
          break;
        case LL_UCPD_SNK_CC2_VRP15A:
          _handle->SNK_Source_Current_Adv = vRd_1_5A;
          break;
        case LL_UCPD_SNK_CC2_VRP30A:
          _handle->SNK_Source_Current_Adv = vRd_3_0A;
          break;
        default:
          break;
      }
    }
  }
#endif /*_DRP || _SNK */

  return _CCx;
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

