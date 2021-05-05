/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usbpd_dpm_core.c
  * @author  MCD Application Team
  * @brief   USBPD dpm core file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#define __USBPD_DPM_CORE_C

/* Includes ------------------------------------------------------------------*/
#include "usbpd_core.h"
#include "usbpd_trace.h"
#include "usbpd_dpm_core.h"
#include "usbpd_dpm_user.h"
#include "usbpd_dpm_conf.h"

#include "cmsis_os.h"
#if (osCMSIS >= 0x20000U)
#include "task.h"
#endif /* osCMSIS >= 0x20000U */

/* Generic STM32 prototypes */
extern uint32_t HAL_GetTick(void);

/* Private function prototypes -----------------------------------------------*/
#if (osCMSIS < 0x20000U)
void USBPD_PE_Task(void const *argument);
void USBPD_CAD_Task(void const *argument);

#else /* osCMSIS >= 0x20000U */

void USBPD_PE_Task_P0(void *argument);
void USBPD_PE_Task_P1(void *argument);
static void PE_Task(uint32_t PortNum);
void USBPD_CAD_Task(void *argument);
#endif /* osCMSIS < 0x20000U */

/* Private typedef -----------------------------------------------------------*/
#if (osCMSIS < 0x20000U)
#define DPM_STACK_SIZE_ADDON_FOR_CMSIS              1
#else
#define DPM_STACK_SIZE_ADDON_FOR_CMSIS              4
#endif /* osCMSIS < 0x20000U */
#define FREERTOS_PE_PRIORITY                    osPriorityAboveNormal
#define FREERTOS_PE_STACK_SIZE                  (200 * DPM_STACK_SIZE_ADDON_FOR_CMSIS)
#define FREERTOS_CAD_PRIORITY                   osPriorityRealtime
#define FREERTOS_CAD_STACK_SIZE                 (300 * DPM_STACK_SIZE_ADDON_FOR_CMSIS)

#if (osCMSIS < 0x20000U)
osThreadDef(PE_0, USBPD_PE_Task, FREERTOS_PE_PRIORITY, 0, FREERTOS_PE_STACK_SIZE);
osThreadDef(PE_1, USBPD_PE_Task, FREERTOS_PE_PRIORITY, 0, FREERTOS_PE_STACK_SIZE);
osMessageQDef(queuePE, 1, uint16_t);
osThreadDef(CAD, USBPD_CAD_Task, FREERTOS_CAD_PRIORITY, 0, FREERTOS_CAD_STACK_SIZE);
osMessageQDef(queueCAD, 2, uint16_t);

#else /* osCMSIS >= 0x20000U */

osThreadAttr_t PE0_Thread_Atrr =
 {
  .name       = "PE_0",
  .priority   = FREERTOS_PE_PRIORITY, /*osPriorityAboveNormal,*/
  .stack_size = FREERTOS_PE_STACK_SIZE
};
osThreadAttr_t PE1_Thread_Atrr =
 {
  .name       = "PE_1",
  .priority   = FREERTOS_PE_PRIORITY,
  .stack_size = FREERTOS_PE_STACK_SIZE
};

osThreadAttr_t CAD_Thread_Atrr = {
  .name       = "CAD",
  .priority   = FREERTOS_CAD_PRIORITY, /*osPriorityRealtime,*/
  .stack_size = FREERTOS_CAD_STACK_SIZE
};

#endif /* osCMSIS < 0x20000U */

/* Private define ------------------------------------------------------------*/
#if (osCMSIS < 0x20000U)
#define OSTHREAD_PE(__PORT__)       (((__PORT__) == USBPD_PORT_0) ? osThread(PE_0) : osThread(PE_1))
#else
#define OSTHREAD_PE(__PORT__)       (((__PORT__) == USBPD_PORT_0) ? USBPD_PE_Task_P0 : USBPD_PE_Task_P1)
#define OSTHREAD_PE_ATTR(__PORT__)  (((__PORT__) == USBPD_PORT_0) ? &PE0_Thread_Atrr : &PE1_Thread_Atrr)
#endif /* osCMSIS < 0x20000U */

/* Private macro -------------------------------------------------------------*/
#define CHECK_PE_FUNCTION_CALL(_function_)  _retr = _function_;                  \
                                            if(USBPD_OK != _retr) {return _retr;}
#define CHECK_CAD_FUNCTION_CALL(_function_) if(USBPD_CAD_OK != _function_) {return USBPD_ERROR;}

#if defined(_DEBUG_TRACE)
#define DPM_CORE_DEBUG_TRACE(_PORTNUM_, __MESSAGE__)  USBPD_TRACE_Add(USBPD_TRACE_DEBUG, _PORTNUM_, 0u, (uint8_t *)(__MESSAGE__), sizeof(__MESSAGE__) - 1u);
#else
#define DPM_CORE_DEBUG_TRACE(_PORTNUM_, __MESSAGE__)
#endif /* _DEBUG_TRACE */

/* Private variables ---------------------------------------------------------*/
static osThreadId DPM_PEThreadId_Table[USBPD_PORT_COUNT];
static osMessageQId CADQueueId;
static osMessageQId PEQueueId[USBPD_PORT_COUNT];

USBPD_ParamsTypeDef   DPM_Params[USBPD_PORT_COUNT];

/* Private function prototypes -----------------------------------------------*/
static void USBPD_PE_TaskWakeUp(uint8_t PortNum);
static void DPM_ManageAttachedState(uint8_t PortNum, USBPD_CAD_EVENT State, CCxPin_TypeDef Cc);
void USBPD_DPM_CADCallback(uint8_t PortNum, USBPD_CAD_EVENT State, CCxPin_TypeDef Cc);
static void USBPD_DPM_CADTaskWakeUp(void);

/**
  * @brief  Initialize the core stack (port power role, PWR_IF, CAD and PE Init procedures)
  * @retval USBPD status
  */
USBPD_StatusTypeDef USBPD_DPM_InitCore(void)
{
  /* variable to get dynamique memory allocated by usbpd stack */
  uint32_t stack_dynamemsize;
  USBPD_StatusTypeDef _retr = USBPD_OK;

  static const USBPD_PE_Callbacks dpmCallbacks =
  {
    NULL,
    USBPD_DPM_HardReset,
    NULL,
    USBPD_DPM_Notification,
    USBPD_DPM_ExtendedMessageReceived,
    USBPD_DPM_GetDataInfo,
    USBPD_DPM_SetDataInfo,
    NULL,
    USBPD_DPM_SNK_EvaluateCapabilities,
    NULL,
    USBPD_PE_TaskWakeUp,
#if defined(_VCONN_SUPPORT)
    USBPD_DPM_EvaluateVconnSwap,
    USBPD_DPM_PE_VconnPwr,
#else
    NULL,
    NULL,
#endif /* _VCONN_SUPPORT */
    USBPD_DPM_EnterErrorRecovery,
    USBPD_DPM_EvaluateDataRoleSwap,
    USBPD_DPM_IsPowerReady
  };

  static const USBPD_CAD_Callbacks CAD_cbs =
  {
    USBPD_DPM_CADCallback,
    USBPD_DPM_CADTaskWakeUp
  };

  /* Check the lib selected */
  if (USBPD_TRUE != USBPD_PE_CheckLIB(_LIB_ID))
  {
    return USBPD_ERROR;
  }

  /* to get how much memory are dynamically allocated by the stack
     the memory return is corresponding to 2 ports so if the application
     managed only one port divide the value return by 2                   */
  stack_dynamemsize = USBPD_PE_GetMemoryConsumption();

  /* done to avoid warning */
  (void)stack_dynamemsize;

  for (uint8_t _port_index = 0; _port_index < USBPD_PORT_COUNT; ++_port_index)
  {
    /* Variable to be sure that DPM is correctly initialized */
    DPM_Params[_port_index].DPM_Initialized = USBPD_FALSE;

    /* check the stack settings */
    DPM_Params[_port_index].PE_SpecRevision  = DPM_Settings[_port_index].PE_SpecRevision;
    DPM_Params[_port_index].PE_PowerRole     = DPM_Settings[_port_index].PE_DefaultRole;
    DPM_Params[_port_index].PE_SwapOngoing   = USBPD_FALSE;
    DPM_Params[_port_index].ActiveCCIs       = CCNONE;
    DPM_Params[_port_index].VconnCCIs        = CCNONE;
    DPM_Params[_port_index].VconnStatus      = USBPD_FALSE;

    /* CAD SET UP */
    CHECK_CAD_FUNCTION_CALL(USBPD_CAD_Init(_port_index, (USBPD_CAD_Callbacks *)&CAD_cbs,
                                           (USBPD_SettingsTypeDef *)&DPM_Settings[_port_index], &DPM_Params[_port_index]));

    /* PE SET UP */
    CHECK_PE_FUNCTION_CALL(USBPD_PE_Init(_port_index, (USBPD_SettingsTypeDef *)&DPM_Settings[_port_index],
                                         &DPM_Params[_port_index], &dpmCallbacks));

    /* DPM is correctly initialized */
    DPM_Params[_port_index].DPM_Initialized = USBPD_TRUE;

    /* Enable CAD */
    USBPD_CAD_PortEnable(_port_index, USBPD_CAD_ENABLE);
  }

  return _retr;
}

/**
  * @brief  Initialize the OS parts (task, queue,... )
  * @retval USBPD status
  */
USBPD_StatusTypeDef USBPD_DPM_InitOS(void)
{
#if (osCMSIS < 0x20000U)
  CADQueueId = osMessageCreate(osMessageQ(queueCAD), NULL);
  if (osThreadCreate(osThread(CAD), NULL) == NULL)
#else
  CADQueueId = osMessageQueueNew (2, sizeof(uint16_t), NULL);
  if (NULL == osThreadNew(USBPD_CAD_Task, &CADQueueId, &CAD_Thread_Atrr))
#endif /* osCMSIS < 0x20000U */
  {
    return USBPD_ERROR;
  }

  /* Create the queue corresponding to PE task */
#if (osCMSIS < 0x20000U)
  PEQueueId[0] = osMessageCreate(osMessageQ(queuePE), NULL);
#if USBPD_PORT_COUNT == 2
  PEQueueId[1] = osMessageCreate(osMessageQ(queuePE), NULL);
#endif /* USBPD_PORT_COUNT == 2 */
#else
  PEQueueId[0] = osMessageQueueNew (1, sizeof(uint16_t), NULL);
#if USBPD_PORT_COUNT == 2
  PEQueueId[1] = osMessageQueueNew (1, sizeof(uint16_t), NULL);
#endif /* USBPD_PORT_COUNT == 2 */
#endif /* osCMSIS < 0x20000U */

  /* PE task to be created on attachment */
  DPM_PEThreadId_Table[USBPD_PORT_0] = NULL;
#if USBPD_PORT_COUNT == 2
  DPM_PEThreadId_Table[USBPD_PORT_1] = NULL;
#endif /* USBPD_PORT_COUNT == 2 */

  return USBPD_OK;
}

/**
  * @brief  Initialize the OS parts (port power role, PWR_IF, CAD and PE Init procedures)
  * @retval None
  */
void USBPD_DPM_Run(void)
{
#if (osCMSIS >= 0x20000U)
  osKernelInitialize();
#endif /* osCMSIS >= 0x20000U */
  osKernelStart();
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
  }
#if USBPD_PORT_COUNT==2
  if (USBPD_TRUE == DPM_Params[USBPD_PORT_1].DPM_Initialized)
  {
    USBPD_DPM_UserTimerCounter(USBPD_PORT_1);
    USBPD_PE_TimerCounter(USBPD_PORT_1);
    USBPD_PRL_TimerCounter(USBPD_PORT_1);
  }
#endif /* USBPD_PORT_COUNT == 2 */

}

/**
  * @brief  WakeUp PE task
  * @param  PortNum port number
  * @retval None
  */
static void USBPD_PE_TaskWakeUp(uint8_t PortNum)
{
#if (osCMSIS < 0x20000U)
  (void)osMessagePut(PEQueueId[PortNum], 0xFFFF, 0);
#else
  uint32_t event = 0xFFFFU;
  (void)osMessageQueuePut(PEQueueId[PortNum], &event, 0U, 0U);
#endif /* osCMSIS < 0x20000U */
}

/**
  * @brief  WakeUp CAD task
  * @retval None
  */
static void USBPD_DPM_CADTaskWakeUp(void)
{
#if (osCMSIS < 0x20000U)
  (void)osMessagePut(CADQueueId, 0xFFFF, 0);
#else
  uint32_t event = 0xFFFFU;
  (void)osMessageQueuePut(CADQueueId, &event, 0U, 0U);
#endif /* osCMSIS < 0x20000U */
}

#if (osCMSIS < 0x20000U)
/**
  * @brief  Main task for PE layer
  * @param  argument Not used
  * @retval None
  */
void USBPD_PE_Task(void const *argument)
{
  uint8_t _port = (uint32_t)argument;
  uint32_t _timing;

  for(;;)
  {
    _timing = USBPD_PE_StateMachine_SNK(_port);
    osMessageGet(PEQueueId[_port],_timing);
  }
}

#else /* osCMSIS > 0x20000U */

/**
  * @brief  Main task for PE layer on Port0
  * @param  argument Not used
  * @retval None
  */
void USBPD_PE_Task_P0(void *argument)
{
  PE_Task(USBPD_PORT_0);
}

/**
  * @brief  Main task for PE layer on Port1
  * @param  argument Not used
  * @retval None
  */
void USBPD_PE_Task_P1(void *argument)
{
  PE_Task(USBPD_PORT_1);
}

/**
  * @brief  Main task for PE layer
  * @param  argument Not used
  * @retval None
  */
static void PE_Task(uint32_t PortNum)
{
  for (;;)
  {
    uint32_t event;
    (void)osMessageQueueGet(PEQueueId[PortNum], &event, NULL,
    USBPD_PE_StateMachine_SNK(PortNum));
  }
}
#endif /* osCMSIS < 0x20000U */

/**
  * @brief  Main task for CAD layer
  * @param  argument Not used
  * @retval None
  */
#if (osCMSIS < 0x20000U)
void USBPD_CAD_Task(void const *argument)
#else
void USBPD_CAD_Task(void *argument)
#endif /* osCMSIS < 0x20000U */
{
  for (;;)
  {
#if (osCMSIS < 0x20000U)
    osMessageGet(CADQueueId, USBPD_CAD_Process());
#else
    uint32_t event;
    (void)osMessageQueueGet(CADQueueId, &event, NULL, USBPD_CAD_Process());
#endif /* osCMSIS < 0x20000U */
  }
}

/**
  * @brief  CallBack reporting events on a specified port from CAD layer.
  * @param  PortNum   The handle of the port
  * @param  State     CAD state
  * @param  Cc        The Communication Channel for the USBPD communication
  * @retval None
  */
void USBPD_DPM_CADCallback(uint8_t PortNum, USBPD_CAD_EVENT State, CCxPin_TypeDef Cc)
{

  switch (State)
  {
    case USBPD_CAD_EVENT_ATTEMC :
    {
#ifdef _VCONN_SUPPORT
      DPM_Params[PortNum].VconnStatus = USBPD_TRUE;
#endif /* _VCONN_SUPPORT */
      DPM_ManageAttachedState(PortNum, State, Cc);
#ifdef _VCONN_SUPPORT
      DPM_CORE_DEBUG_TRACE(PortNum, "Note: VconnStatus=TRUE");
#endif /* _VCONN_SUPPORT */
      break;
    }
    case USBPD_CAD_EVENT_ATTACHED :
      DPM_ManageAttachedState(PortNum, State, Cc);
      break;
    case USBPD_CAD_EVENT_DETACHED :
    case USBPD_CAD_EVENT_EMC :
    {
      /* The ufp is detached */
      (void)USBPD_PE_IsCableConnected(PortNum, 0);
      /* Terminate PE task */
      if (DPM_PEThreadId_Table[PortNum] != NULL)
      {
        osThreadTerminate(DPM_PEThreadId_Table[PortNum]);
        DPM_PEThreadId_Table[PortNum] = NULL;
      }
      DPM_Params[PortNum].PE_SwapOngoing = USBPD_FALSE;
      DPM_Params[PortNum].ActiveCCIs = CCNONE;
      DPM_Params[PortNum].PE_Power   = USBPD_POWER_NO;
      USBPD_DPM_UserCableDetection(PortNum, State);
#ifdef _VCONN_SUPPORT
      DPM_Params[PortNum].VconnCCIs = CCNONE;
      DPM_Params[PortNum].VconnStatus = USBPD_FALSE;
      DPM_CORE_DEBUG_TRACE(PortNum, "Note: VconnStatus=FALSE");
#endif /* _VCONN_SUPPORT */
      break;
    }
    default :
      /* nothing to do */
      break;
  }
}

static void DPM_ManageAttachedState(uint8_t PortNum, USBPD_CAD_EVENT State, CCxPin_TypeDef Cc)
{
#ifdef _VCONN_SUPPORT
  if (CC1 == Cc)
  {
    DPM_Params[PortNum].VconnCCIs = CC2;
  }
  if (CC2 == Cc)
  {
    DPM_Params[PortNum].VconnCCIs = CC1;
  }
#endif /* _VCONN_SUPPORT */
  DPM_Params[PortNum].ActiveCCIs = Cc;
  (void)USBPD_PE_IsCableConnected(PortNum, 1);

  USBPD_DPM_UserCableDetection(PortNum, State);

  /* Create PE task */
  if (DPM_PEThreadId_Table[PortNum] == NULL)
  {
#if (osCMSIS < 0x20000U)
    DPM_PEThreadId_Table[PortNum] = osThreadCreate(OSTHREAD_PE(PortNum), (void *)((uint32_t)PortNum));
#else
    DPM_PEThreadId_Table[PortNum] = osThreadNew(OSTHREAD_PE(PortNum), NULL, OSTHREAD_PE_ATTR(PortNum));
#endif /* osCMSIS < 0x20000U */
    if (DPM_PEThreadId_Table[PortNum] == NULL)
    {
      /* should not occur. May be an issue with FreeRTOS heap size too small */
      while (1);
    }
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
