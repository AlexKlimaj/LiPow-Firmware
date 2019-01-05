/**
  ******************************************************************************
  * @file    usbpd_dpm_core.c
  * @author  MCD Application Team
  * @brief   USBPD dpm core file
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

#define __USBPD_DPM_CORE_C

/* Includes ------------------------------------------------------------------*/
#include "usbpd_core.h"
#if defined(USBPD_TCPM_MODULE_ENABLED)
#if defined(USBPD_STUSB1605)
#include "p-nucleo-usb002.h"
#else
#include "usbpd_tcpci.h"
#endif /*USBPD_STUSB1605*/
#endif /* USBPD_TCPM_MODULE_ENABLED */
#include "usbpd_trace.h"
#include "usbpd_dpm_core.h"
#include "usbpd_dpm_user.h"
#if defined(_VDM)||defined(_VCONN_SUPPORT)
#include "usbpd_vdm_user.h"
#endif /*_VDM || _VCONN_SUPPORT */
#include "usbpd_dpm_conf.h"
#ifdef _RTOS
#include "cmsis_os.h"
#else
#if defined(USBPD_TCPM_MODULE_ENABLED)
#include "usbpd_timersserver.h"
#endif /* USBPD_TCPM_MODULE_ENABLED */
#endif /* _RTOS */
#ifdef _GUI_INTERFACE
#include "string.h"
#include "gui_api.h"
#endif /* _GUI_INTERFACE */
#if defined(_FWUPDATE_RESPONDER)
#include "usbpd_pdfu_responder.h"
#endif /* _FWUPDATE_RESPONDER */

#ifndef _RTOS
#if USBPD_PORT_COUNT >= 2
#error "the non RTOS mode is supported only for one port configuration"
#endif
#endif

/* Private enum */
enum {
  USBPD_THREAD_PORT_0 = USBPD_PORT_0,
#if USBPD_PORT_COUNT == 2
  USBPD_THREAD_PORT_1 = USBPD_PORT_1,
#endif
  USBPD_THREAD_CAD
};

/* Private function prototypes -----------------------------------------------*/
void USBPD_DPM_CADCallback(uint8_t PortNum, USBPD_CAD_EVENT State, CCxPin_TypeDef Cc);
void USBPD_PE_Task(void const *argument);
#if defined(USBPD_TCPM_MODULE_ENABLED)
void USBPD_ALERT_Task(void const *argument);
#else
void USBPD_CAD_Task(void const *argument);
#endif /* USBPD_TCPM_MODULE_ENABLED */
#if defined(_TRACE) || defined(_GUI_INTERFACE)
void USBPD_TRACE_TX_Task(void const *argument);
#endif /* _TRACE || _GUI_INTERFACE */

static void USBPD_PE_TaskWakeUp(uint8_t PortNum);
static void USBPD_DPM_CADTaskWakeUp(void);

#ifdef _RTOS
/* Private typedef -----------------------------------------------------------*/
#if defined(_VDM)
osThreadDef(PE_0, USBPD_PE_Task, osPriorityHigh, 0, 350);
osThreadDef(PE_1, USBPD_PE_Task, osPriorityHigh, 0, 350);
#elif defined(__AUTHENTICATION__)
osThreadDef(PE_0, USBPD_PE_Task, osPriorityHigh, 0, 150);
osThreadDef(PE_1, USBPD_PE_Task, osPriorityHigh, 0, 150);
#else
#if defined(USBPD_TCPM_MODULE_ENABLED)
osThreadDef(PE_0, USBPD_PE_Task, osPriorityHigh, 0, 150);
osThreadDef(PE_1, USBPD_PE_Task, osPriorityHigh, 0, 150);
#else
osThreadDef(PE_0, USBPD_PE_Task, osPriorityHigh, 0, 200);
osThreadDef(PE_1, USBPD_PE_Task, osPriorityHigh, 0, 200);
#endif /*USBPD_TCPM_MODULE_ENABLED*/
#endif /*_VDM*/

/* Private define ------------------------------------------------------------*/
#define MAX_TREAD_POWER   (USBPD_PORT_COUNT + 1 + USBPD_USER_THREAD_COUNT)

#define OSTHREAD_PE(__VAL__) __VAL__==USBPD_PORT_0?osThread(PE_0):osThread(PE_1)

#endif

/* Private macro -------------------------------------------------------------*/
#define CHECK_PE_FUNCTION_CALL(_function_)  if(USBPD_OK != _function_) return USBPD_ERROR;
#define CHECK_CAD_FUNCTION_CALL(_function_) if(USBPD_CAD_OK != _function_) return USBPD_ERROR;

/* Private variables ---------------------------------------------------------*/
#ifdef _GUI_INTERFACE
#ifdef _RTOS
SemaphoreHandle_t GUI_Requests_To_Process;
#endif /* _RTOS */
#endif /* _GUI_INTERFACE */
static const USBPD_CAD_Callbacks CAD_cbs = { USBPD_DPM_CADCallback, USBPD_DPM_CADTaskWakeUp };

#ifdef _RTOS
static osThreadId DPM_Thread_Table[MAX_TREAD_POWER];
static uint32_t DPM_Sleep_time[MAX_TREAD_POWER];
#if defined(USBPD_TCPM_MODULE_ENABLED)
osMessageQId  AlarmMsgBox;
#endif /* USBPD_TCPM_MODULE_ENABLED */
osMessageQDef(queuePE, 2, uint16_t);
osMessageQDef(queueCAD, 1, uint16_t);
static osMessageQId PEQueueId[USBPD_PORT_COUNT], CADQueueId;
#endif /* _RTOS */

/* variable to get dynamique memory allocated by usbpd stack */
static const USBPD_PE_Callbacks dpmCallbacks =
{
#if defined(_SRC) || defined(_DRP)
  USBPD_DPM_SetupNewPower,
#else
  NULL,
#endif
  USBPD_DPM_HardReset,
  USBPD_DPM_EvaluatePowerRoleSwap,
  USBPD_DPM_Notification,
#ifdef USBPD_REV30_SUPPORT
  USBPD_DPM_ExtendedMessageReceived,
#else
  NULL,
#endif /* USBPD_REV30_SUPPORT */
  USBPD_DPM_GetDataInfo,
  USBPD_DPM_SetDataInfo,
#if defined(_SRC) || defined(_DRP)
  USBPD_DPM_EvaluateRequest,
#else
  NULL,
#endif /*_SRC || DRP */
#if defined(_SNK) || defined(_DRP)
  USBPD_DPM_SNK_EvaluateCapabilities,
#else
    NULL,
#endif /*_SNK || DRP */
#if defined(_DRP)
  USBPD_DPM_PowerRoleSwap,
#else
    NULL,
#endif /*  _DRP  */
  USBPD_PE_TaskWakeUp,
#if defined(_VCONN_SUPPORT)
  USBPD_DPM_EvaluateVconnSwap,
  USBPD_DPM_PE_VconnPwr,
#else
  NULL,
  NULL,
#endif /* _VCONN_SUPPORT */
#ifdef _ERROR_RECOVERY
  USBPD_DPM_EnterErrorRecovery,
#else
  NULL,
#endif
  USBPD_DPM_EvaluateDataRoleSwap,
  USBPD_DPM_IsPowerReady
};

USBPD_ParamsTypeDef   DPM_Params[USBPD_PORT_COUNT];

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initialize DPM (port power role, PWR_IF, CAD and PE Init procedures)
  * @retval USBPD status
  */
USBPD_StatusTypeDef USBPD_DPM_Init(void)
{
  uint32_t stack_dynamemsize;

  /* Check the lib selected */
  if( USBPD_TRUE != USBPD_PE_CheckLIB(_LIB_ID))
  {
    return USBPD_ERROR;
  }

  /* to get how much memory are dynamically allocated by the stack
     the memory return is corresponding to 2 ports so if the application
     managed only one port divide the value return by 2                   */
  stack_dynamemsize = USBPD_PE_GetMemoryConsumption();

  /* done to avoid warning */
  stack_dynamemsize--;

#if defined(_GUI_INTERFACE)
  /* Perform copy from FLASH to RAM */
  memcpy(DPM_Settings, DPM_FLASH_Settings, (sizeof(USBPD_SettingsTypeDef) * USBPD_PORT_COUNT));
  memcpy(DPM_USER_Settings, DPM_USER_FLASH_Settings, (sizeof(USBPD_USER_SettingsTypeDef) * USBPD_PORT_COUNT));
#ifdef _VDM
  memcpy(DPM_VDM_Settings, DPM_VDM_FLASH_Settings, (sizeof(USBPD_VDM_SettingsTypeDef) * USBPD_PORT_COUNT));
#endif /* _VDM */
#endif /* _GUI_INTERFACE */

  /* Variable to be sure that DPM is correctly initialized */
  DPM_Params[USBPD_PORT_0].DPM_Initialized = USBPD_FALSE;
#if USBPD_PORT_COUNT == 2
  DPM_Params[USBPD_PORT_1].DPM_Initialized = USBPD_FALSE;
#endif /* USBPD_PORT_COUNT == 2 */

  /* check the stack settings */
  DPM_Params[USBPD_PORT_0].PE_SpecRevision  = DPM_Settings[USBPD_PORT_0].PE_SpecRevision;
  DPM_Params[USBPD_PORT_0].PE_PowerRole     = DPM_Settings[USBPD_PORT_0].PE_DefaultRole;
  DPM_Params[USBPD_PORT_0].PE_SwapOngoing   = 0;
  DPM_Params[USBPD_PORT_0].ActiveCCIs       = CCNONE;
  DPM_Params[USBPD_PORT_0].VconnCCIs        = CCNONE;
  DPM_Params[USBPD_PORT_0].VconnStatus      = USBPD_FALSE;
#if USBPD_PORT_COUNT == 2
  DPM_Params[USBPD_PORT_1].PE_SpecRevision  = DPM_Settings[USBPD_PORT_1].PE_SpecRevision;
  DPM_Params[USBPD_PORT_1].PE_PowerRole     = DPM_Settings[USBPD_PORT_1].PE_DefaultRole;
  DPM_Params[USBPD_PORT_1].PE_SwapOngoing   = 0;
  DPM_Params[USBPD_PORT_1].ActiveCCIs       = CCNONE;
  DPM_Params[USBPD_PORT_1].VconnCCIs        = CCNONE;
  DPM_Params[USBPD_PORT_1].VconnStatus      = USBPD_FALSE;
#endif /* USBPD_PORT_COUNT == 2 */

#if defined(_TRACE) || defined(_GUI_INTERFACE)
  /* Initialise the TRACE */
  USBPD_TRACE_Init();
#endif /* _TRACE || _GUI_INTERFACE */

#if defined(USBPD_TCPM_MODULE_ENABLED)
#ifndef _RTOS
  USBPD_TIM_Init();
#endif /* !_RTOS*/

#if !defined(USBPD_STUSB1605)
  USBPD_TCPCI_Init();
#endif /* !USBPD_STUSB1605 */
  TCPC_DrvTypeDef* tcpc_driver;
  USBPD_TCPCI_GetDevicesDrivers(USBPD_PORT_0, &tcpc_driver);
  USBPD_TCPM_HWInit(USBPD_PORT_0, DPM_Settings[USBPD_PORT_0].CAD_RoleToggle, &DPM_Params[USBPD_PORT_0], (USBPD_CAD_Callbacks *)&CAD_cbs, tcpc_driver);
#if USBPD_PORT_COUNT == 2
  USBPD_TCPCI_GetDevicesDrivers(USBPD_PORT_1, &tcpc_driver);
  USBPD_TCPM_HWInit(USBPD_PORT_1, DPM_Settings[USBPD_PORT_1].CAD_RoleToggle, &DPM_Params[USBPD_PORT_1], (USBPD_CAD_Callbacks *)&CAD_cbs, tcpc_driver);
#endif /* USBPD_PORT_COUNT == 2 */
#else
  /* CAD SET UP : Port 0 */
  CHECK_CAD_FUNCTION_CALL(USBPD_CAD_Init(USBPD_PORT_0, (USBPD_CAD_Callbacks*)&CAD_cbs, (USBPD_SettingsTypeDef *)&DPM_Settings[USBPD_PORT_0], &DPM_Params[USBPD_PORT_0]));
#if USBPD_PORT_COUNT == 2
  CHECK_CAD_FUNCTION_CALL(USBPD_CAD_Init(USBPD_PORT_1, (USBPD_CAD_Callbacks*)&CAD_cbs, (USBPD_SettingsTypeDef *)&DPM_Settings[USBPD_PORT_1], &DPM_Params[USBPD_PORT_1]));
#endif /* USBPD_PORT_COUNT == 2 */
#endif /* USBPD_TCPM_MODULE_ENABLED */

  /* PE SET UP : Port 0 */
  CHECK_PE_FUNCTION_CALL(USBPD_PE_Init(USBPD_PORT_0, (USBPD_SettingsTypeDef *)&DPM_Settings[USBPD_PORT_0], &DPM_Params[USBPD_PORT_0], &dpmCallbacks));
#if USBPD_PORT_COUNT == 2
  CHECK_PE_FUNCTION_CALL(USBPD_PE_Init(USBPD_PORT_1, (USBPD_SettingsTypeDef *)&DPM_Settings[USBPD_PORT_1], &DPM_Params[USBPD_PORT_1], &dpmCallbacks));
#endif /* USBPD_PORT_COUNT == 2 */

#ifdef _VDM
  if(USBPD_TRUE == DPM_Settings[USBPD_PORT_0].PE_VDMSupport)
  {
    if (USBPD_OK != USBPD_VDM_UserInit(USBPD_PORT_0, (USBPD_VDM_SettingsTypeDef *)&DPM_VDM_Settings[USBPD_PORT_0], &DPM_Params[USBPD_PORT_0]))
    {
      return USBPD_ERROR;
    }
  }
#if USBPD_PORT_COUNT == 2
  if(USBPD_TRUE == DPM_Settings[USBPD_PORT_1].PE_VDMSupport)
  {
    if (USBPD_OK != USBPD_VDM_UserInit(USBPD_PORT_1, (USBPD_VDM_SettingsTypeDef *)&DPM_VDM_Settings[USBPD_PORT_1], &DPM_Params[USBPD_PORT_1]))
    {
      return USBPD_ERROR;
    }
  }
#endif /* USBPD_PORT_COUNT == 2 */
#elif defined(_VCONN_SUPPORT)
  if (USBPD_OK != USBPD_VDM_UserInit(USBPD_PORT_0, NULL, NULL))
  {
    return USBPD_ERROR;
  }
#if USBPD_PORT_COUNT == 2
  if (USBPD_OK != USBPD_VDM_UserInit(USBPD_PORT_1, NULL, NULL))
  {
    return USBPD_ERROR;
  }
#endif /* USBPD_PORT_COUNT == 2 */
#endif /* _VDM */

  /* DPM is correctly initialized */
  DPM_Params[USBPD_PORT_0].DPM_Initialized = USBPD_TRUE;
#if USBPD_PORT_COUNT == 2
  DPM_Params[USBPD_PORT_1].DPM_Initialized = USBPD_TRUE;
#endif /* USBPD_PORT_COUNT == 2 */

  /* Initialize */
  if (USBPD_OK != USBPD_DPM_UserInit())
  {
    return USBPD_ERROR;
  }

#ifdef _RTOS
#if defined(USBPD_TCPM_MODULE_ENABLED)
  osMessageQDef(MsgBox, TCPM_ALARMBOX_MESSAGES_MAX, uint16_t);

#if defined(_TRACE)
  osThreadDef(ALERTTask, USBPD_ALERT_Task, osPriorityRealtime, 0, 240);
#else
  osThreadDef(ALERTTask, USBPD_ALERT_Task, osPriorityRealtime, 0, 120);
#endif /* _TRACE */
  AlarmMsgBox = osMessageCreate(osMessageQ(MsgBox), NULL);
  if (NULL == osThreadCreate(osThread(ALERTTask), &AlarmMsgBox))
  {
    return USBPD_ERROR;
  }
#else
  /* Enable CAD on Port 0 */
  USBPD_CAD_PortEnable(USBPD_PORT_0, USBPD_CAD_ENABLE);
#if USBPD_PORT_COUNT == 2
  USBPD_CAD_PortEnable(USBPD_PORT_1, USBPD_CAD_ENABLE);
#endif /* USBPD_PORT_COUNT == 2 */

  osThreadDef(CAD, USBPD_CAD_Task, osPriorityRealtime, 0, 300);
  if((DPM_Thread_Table[USBPD_THREAD_CAD] = osThreadCreate(osThread(CAD), NULL)) == NULL)
  {
    return USBPD_ERROR;
  }
  CADQueueId = osMessageCreate(osMessageQ(queueCAD), NULL);

#endif /* USBPD_TCPM_MODULE_ENABLED */

#if defined(_TRACE) || defined(_GUI_INTERFACE)
  osThreadDef(TRA_TX, USBPD_TRACE_TX_Task, osPriorityLow, 0, configMINIMAL_STACK_SIZE * 2);
  if(NULL == osThreadCreate(osThread(TRA_TX), NULL))
  {
    return USBPD_ERROR;
  }
#endif /* _TRACE || _GUI_INTERFACE */

  /* Create the queue corresponding to PE task */
  PEQueueId[0] = osMessageCreate(osMessageQ(queuePE), NULL);
#if USBPD_PORT_COUNT == 2
  PEQueueId[1] = osMessageCreate(osMessageQ(queuePE), NULL);
#endif /* USBPD_PORT_COUNT == 2 */

  /* PE task to be created on attachment */
  DPM_Thread_Table[USBPD_THREAD_PORT_0] = NULL;
#if USBPD_PORT_COUNT == 2
  DPM_Thread_Table[USBPD_THREAD_PORT_1] = NULL;
#endif /* USBPD_PORT_COUNT == 2 */

#if defined(_OPTIM_CONSO)
  extern void   SystemClock_Config_8Mhz(void);

  SystemClock_Config_8Mhz();
#endif /*_OPTIM_CONSO_*/

#if defined(USBPD_TCPM_MODULE_ENABLED)
  USBPD_TCPI_AlertInit();
#endif /* USBPD_TCPM_MODULE_ENABLED */

  /* Start the scheduler */
  osKernelStart();

  return USBPD_OK;
#else /* !_RTOS*/

#if !defined(USBPD_TCPM_MODULE_ENABLED)
  /* Enable CAD on Port 0 */
  USBPD_CAD_PortEnable(USBPD_PORT_0, USBPD_CAD_ENABLE);

#ifdef _SIMULATOR
  return USBPD_OK;
}

void USBPD_DPM_Run(void)
{
#endif
  extern uint32_t HAL_GetTick(void);
  extern void  HAL_Delay(volatile uint32_t Delay);

#endif /* USBPD_TCPM_MODULE_ENABLED */
#ifndef _SIMULATOR
  do
  {
#endif
#if defined(USBPD_TCPM_MODULE_ENABLED)
#else /* !USBPD_TCPM_MODULE_ENABLED */
      USBPD_CAD_Process();
#ifndef _SIMULATOR
      //HAL_Delay(1);
#endif
#endif /* USBPD_TCPM_MODULE_ENABLED */
#ifdef _DRP
    USBPD_PE_StateMachine_DRP(USBPD_PORT_0);
#elif _SRC
    USBPD_PE_StateMachine_SRC(USBPD_PORT_0);
#elif _SNK
    USBPD_PE_StateMachine_SNK(USBPD_PORT_0);
#endif /* _DRP */

    USBPD_DPM_UserExecute(NULL);
#if defined(_TRACE) || defined(_GUI_INTERFACE)
    USBPD_TRACE_TX_Process();
#endif /* _TRACE || _GUI_INTERFACE */
#ifndef _SIMULATOR
    }
  while(1);
#endif
#endif /* _RTOS */
}

/**
  * @brief  Initialize DPM (port power role, PWR_IF, CAD and PE Init procedures)
  * @retval USBPD status
  */
void USBPD_DPM_TimerCounter(void)
{
  /* Call PE/PRL timers functions only if DPM is initialized */
  if (USBPD_TRUE == DPM_Params[USBPD_PORT_0].DPM_Initialized)
  {
    USBPD_DPM_UserTimerCounter(USBPD_PORT_0);
    USBPD_PE_TimerCounter(USBPD_PORT_0);
    USBPD_PRL_TimerCounter(USBPD_PORT_0);
#if defined(_FWUPDATE_RESPONDER)
    USBPD_PDFU_TimerCounter(USBPD_PORT_0);
#endif /* _FWUPDATE_RESPONDER */
  }
#if USBPD_PORT_COUNT==2
  if (USBPD_TRUE == DPM_Params[USBPD_PORT_1].DPM_Initialized)
  {
    USBPD_DPM_UserTimerCounter(USBPD_PORT_1);
    USBPD_PE_TimerCounter(USBPD_PORT_1);
    USBPD_PRL_TimerCounter(USBPD_PORT_1);
#if defined(_FWUPDATE_RESPONDER)
    USBPD_PDFU_TimerCounter(USBPD_PORT_1);
#endif /* _FWUPDATE_RESPONDER */
  }
#endif /* USBPD_PORT_COUNT == 2 */

#ifdef _RTOS
  /* check to avoid count before OSKernel Start */
  if (uxTaskGetNumberOfTasks() != 0)
  {
    osSystickHandler();
  }
#endif /* _RTOS */
}

/**
  * @brief  WakeUp PE task
  * @param  PortNum port number
  * @retval None
  */
static void USBPD_PE_TaskWakeUp(uint8_t PortNum)
{
#ifdef _RTOS
  osMessagePut(PEQueueId[PortNum], 0xFFFF, 0);
#endif
}

/**
  * @brief  WakeUp CAD task
  * @retval None
  */
static void USBPD_DPM_CADTaskWakeUp(void)
{
#ifdef _RTOS
  osMessagePut(CADQueueId, 0xFFFF, 0);
#endif
}

#ifdef _RTOS
/**
  * @brief  Main task for PE layer
  * @param  argument Not used
  * @retval None
  */
void USBPD_PE_Task(void const *argument)
{
  uint8_t _port = (uint32_t)argument;

  for(;;)
  {
    DPM_Sleep_time[_port] =
#ifdef _DRP
     USBPD_PE_StateMachine_DRP(_port);
#elif defined(_SRC)
    USBPD_PE_StateMachine_SRC(_port);
#elif defined(_SNK)
    USBPD_PE_StateMachine_SNK(_port);
#endif
    osMessageGet(PEQueueId[_port], DPM_Sleep_time[_port]);

#if defined(USBPD_TCPM_MODULE_ENABLED)&&!defined(TCPC_FL7101)
    /* During SRC tests, VBUS is disabled by the FUSB but the detection is not
       well done */
    if ((DPM_Params[_port].PE_SwapOngoing == 0) && (USBPD_ERROR == USBPD_TCPM_VBUS_IsVsafe5V(_port)))
    {
      osMessagePut(AlarmMsgBox, (_port << 8 | 2), osWaitForever);
    }
#endif /* USBPD_TCPM_MODULE_ENABLED */
  }
}

#if defined(USBPD_TCPM_MODULE_ENABLED)
/**
  * @brief  Main task for ALERT layer
  * @param  argument: Not used
  * @retval None
  */
void USBPD_ALERT_Task(void const *queue_id)
{
  osMessageQId  queue = *(osMessageQId *)queue_id;
  uint8_t port;
  for(;;)
  {
    osEvent event = osMessageGet(queue, osWaitForever);
    port = (event.value.v >> 8);
#if defined(_TRACE)
    USBPD_TRACE_Add(USBPD_TRACE_DEBUG, port, ((event.value.v) & 0x00FF), "ALERT_TASK", sizeof("ALERT_TASK")-1);
#endif /* _TRACE */
    USBPD_TCPM_alert(event.value.v);
    HAL_NVIC_EnableIRQ(ALERT_GPIO_IRQHANDLER(port));
  }
}
#else /* !USBPD_TCPM_MODULE_ENABLED */
/**
  * @brief  Main task for CAD layer
  * @param  argument Not used
  * @retval None
  */
void USBPD_CAD_Task(void const *argument)
{
  for(;;)
  {
    DPM_Sleep_time[USBPD_THREAD_CAD] = USBPD_CAD_Process();
    osMessageGet(CADQueueId, DPM_Sleep_time[USBPD_THREAD_CAD]);
  }
}
#endif /* USBPD_TCPM_MODULE_ENABLED */

#if defined(_TRACE) || defined(_GUI_INTERFACE)
/**
  * @brief  Main task for TRACE TX layer
  * @param  argument Not used
  * @retval None
  */
void USBPD_TRACE_TX_Task(void const *argument)
{
  for(;;)
  {
    USBPD_TRACE_TX_Process();
    osDelay(5);
  }
}
#endif /* _TRACE || _GUI_INTERFACE */
#endif /* _RTOS */


/**
  * @brief  CallBack reporting events on a specified port from CAD layer.
  * @param  PortNum   The handle of the port
  * @param  State     CAD state
  * @param  Cc        The Communication Channel for the USBPD communication
  * @retval None
  */
void USBPD_DPM_CADCallback(uint8_t PortNum, USBPD_CAD_EVENT State, CCxPin_TypeDef Cc)
{
#ifdef _TRACE
  USBPD_TRACE_Add(USBPD_TRACE_CADEVENT, PortNum, State, NULL, 0);
#endif /* _TRACE */

  switch(State)
  {
  case USBPD_CAD_EVENT_ATTEMC :
#ifdef _VCONN_SUPPORT
    DPM_Params[PortNum].VconnStatus = USBPD_TRUE;
#endif /* _VCONN_SUPPORT */
  case USBPD_CAD_EVENT_ATTACHED :
#ifdef _VCONN_SUPPORT
    if(CC1 == Cc) DPM_Params[PortNum].VconnCCIs = CC2;
    if(CC2 == Cc) DPM_Params[PortNum].VconnCCIs = CC1;
#endif /* _VCONN_SUPPORT */
    DPM_Params[PortNum].ActiveCCIs = Cc;
    USBPD_PE_IsCableConnected(PortNum, 1);

    USBPD_DPM_UserCableDetection(PortNum, State);
#if defined(USBPD_TCPM_MODULE_ENABLED)
    /* Add a delay to postpone the 1st send of SRC capa
       FUS305 seems not react correctly if it sent too quickly */
    USBPD_DPM_WaitForTime(6);
#endif /* USBPD_TCPM_MODULE_ENABLED */

#ifdef _RTOS
    /* Create PE task */
    if (DPM_Thread_Table[PortNum] == NULL)
    {
      DPM_Thread_Table[PortNum] = osThreadCreate(OSTHREAD_PE(PortNum), (void *)((uint32_t)PortNum));
      if (DPM_Thread_Table[PortNum] == NULL)
      {
        /* should not occurr. May be an issue with FreeRTOS heap size too small */
        while(1);
      }
    }
#endif /* _RTOS */
    break;

  case USBPD_CAD_EVENT_DETACHED :
  case USBPD_CAD_EVENT_EMC :
    /* The ufp is detached */
    USBPD_PE_IsCableConnected(PortNum, 0);
    /* Terminate PE task */
#ifdef _RTOS
    if (DPM_Thread_Table[PortNum] != NULL)
    {
      osThreadTerminate(DPM_Thread_Table[PortNum]);
      DPM_Thread_Table[PortNum] = NULL;
    }
#endif /* _RTOS */
    USBPD_DPM_UserCableDetection(PortNum, State);
    DPM_Params[PortNum].ActiveCCIs = CCNONE;
    DPM_Params[PortNum].PE_Power   = USBPD_POWER_NO;
#ifdef _VCONN_SUPPORT
    DPM_Params[PortNum].VconnCCIs = CCNONE;
    DPM_Params[PortNum].VconnStatus = USBPD_FALSE;
#endif /* _VCONN_SUPPORT */
    break;
#if 0
  case USBPD_CAD_EVENT_LEGACY:
    DPM_Params[PortNum].ActiveCCIs  = Cc;
    DPM_Params[PortNum].PE_Power    = USBPD_POWER_DEFAULT5V;
    USBPD_DPM_UserCableDetection(PortNum, State);
    break;
#endif
  default :
    /* nothing to do */
    break;
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
