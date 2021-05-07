/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usbpd_pwr_user.c
  * @author  MCD Application Team
  * @brief   USBPD PWR user code
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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

/* Includes ------------------------------------------------------------------*/
#include "usbpd_pwr_user.h"
#include "stm32g0xx_hal.h"
#if defined(_TRACE)
#include "usbpd_core.h"
#include "usbpd_trace.h"
#endif /* _TRACE */

/* USER CODE BEGIN include */
/* USER CODE END include */

/** @addtogroup BSP
  * @{
  */

/** @addtogroup POWER
  * @{
  */
/** @defgroup POWER_Private_Typedef Private Typedef
  * @{
  */
/* USER CODE BEGIN POWER_Private_Typedef */

/* USER CODE END POWER_Private_Typedef */
/**
  * @}
  */

/** @defgroup POWER_Private_Constants Private Constants
* @{
*/
/* USER CODE BEGIN POWER_Private_Constants */

/* USER CODE END POWER_Private_Constants */
/**
  * @}
  */

/** @defgroup POWER_Private_Macros Private Macros
  * @{
  */
#if defined(_TRACE)
#define PWR_DEBUG_TRACE(_PORT_, __MESSAGE__)  USBPD_TRACE_Add(USBPD_TRACE_DEBUG,    (_PORT_), 0u, (uint8_t*)(__MESSAGE__), sizeof(__MESSAGE__) - 1u)
#else
#define PWR_DEBUG_TRACE(_PORT_, __MESSAGE__)
#endif /* _TRACE */
/* USER CODE BEGIN POWER_Private_Macros */

/* USER CODE END POWER_Private_Macros */
/**
  * @}
  */

/** @defgroup POWER_Private_Variables Private Variables
  * @{
  */
/* USER CODE BEGIN POWER_Private_Variables */

/* USER CODE END POWER_Private_Variables */
/**
  * @}
  */

/** @defgroup POWER_Private_Functions Private Functions
  * @{
  */
/* USER CODE BEGIN POWER_Private_Prototypes */

/* USER CODE END POWER_Private_Prototypes */
/**
  * @}
  */

/** @defgroup POWER_Exported_Variables Exported Variables
  * @{
  */
/* USER CODE BEGIN POWER_Exported_Variables */

/* USER CODE END POWER_Exported_Variables */
/**
  * @}
  */

/** @addtogroup POWER_Exported_Functions
  * @{
  */
/**
  * @brief  Initialize the hardware resources used by the Type-C power delivery (PD)
  *         controller.
  * @note   Obsolete interface, new interface should be called.
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
  *         @arg TYPE_C_PORT_2
  * @retval PD controller status
  */
__weak PWR_StatusTypeDef BSP_PWR_VBUSInit(uint32_t               PortId)
{
  PWR_DEBUG_TRACE(PortId, "ADVICE: Obsolete BSP_PWR_VBUSInit");
/* USER CODE BEGIN BSP_PWR_VBUSInit */
  return ((BSP_ERROR_NONE == BSP_USBPD_PWR_VBUSInit(PortId)) ? PWR_OK : PWR_ERROR);
/* USER CODE END BSP_PWR_VBUSInit */
}

/**
  * @brief  Release the hardware resources used by the Type-C power delivery (PD)
  *         controller.
  * @note   Obsolete interface, new interface should be called.
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
  *         @arg TYPE_C_PORT_2
  * @retval PD controller status
  */
__weak PWR_StatusTypeDef BSP_PWR_VBUSDeInit(uint32_t PortId)
{
  PWR_DEBUG_TRACE(PortId, "ADVICE: Obsolete BSP_PWR_VBUSDeInit");
/* USER CODE BEGIN BSP_PWR_VBUSDeInit */
  return ((BSP_ERROR_NONE == BSP_USBPD_PWR_VBUSDeInit(PortId)) ? PWR_OK : PWR_ERROR);
/* USER CODE END BSP_PWR_VBUSDeInit */
}

/**
  * @brief  Enable power supply over VBUS.
  * @note   Obsolete interface, new interface should be called.
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
  *         @arg TYPE_C_PORT_2
  * @retval PD controller status
  */
__weak PWR_StatusTypeDef BSP_PWR_VBUSOn(uint32_t PortId)
{
  PWR_DEBUG_TRACE(PortId, "ADVICE: Obsolete BSP_PWR_VBUSOn");
/* USER CODE BEGIN BSP_PWR_VBUSOn */
  return ((BSP_ERROR_NONE == BSP_USBPD_PWR_VBUSOn(PortId)) ? PWR_OK : PWR_ERROR);
/* USER CODE END BSP_PWR_VBUSOn */
}

/**
  * @brief  Disable power supply over VBUS.
  * @note   Obsolete interface, new interface should be called.
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
  *         @arg TYPE_C_PORT_2
  * @retval PD controller status
  */
__weak PWR_StatusTypeDef BSP_PWR_VBUSOff(uint32_t PortId)
{
  PWR_DEBUG_TRACE(PortId, "ADVICE: Obsolete BSP_PWR_VBUSOff");
/* USER CODE BEGIN BSP_PWR_VBUSOff */
  return ((BSP_ERROR_NONE == BSP_USBPD_PWR_VBUSOff(PortId)) ? PWR_OK : PWR_ERROR);
/* USER CODE END BSP_PWR_VBUSOff */
}

/**
  * @brief  Set a fixed/variable PDO and manage the power control.
  * @note   Obsolete interface, new interface should be called.
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
  *         @arg TYPE_C_PORT_2
  * @param  VbusTargetInmv the vbus Target (in mV)
  * @param  OperatingCurrent the Operating Current (in mA)
  * @param  MaxOperatingCurrent the Max Operating Current (in mA)
  * @retval PD controller status
  */
__weak PWR_StatusTypeDef BSP_PWR_VBUSSetVoltage_Fixed(uint32_t PortId,
                                               uint32_t VbusTargetInmv,
                                               uint32_t OperatingCurrent,
                                               uint32_t MaxOperatingCurrent)
{
  PWR_DEBUG_TRACE(PortId, "ADVICE: Obsolete BSP_PWR_VBUSSetVoltage_Fixed");
/* USER CODE BEGIN BSP_PWR_VBUSSetVoltage_Fixed */
  return ((BSP_ERROR_NONE == BSP_USBPD_PWR_VBUSSetVoltage_Fixed(PortId, VbusTargetInmv, OperatingCurrent, MaxOperatingCurrent)) ? PWR_OK : PWR_ERROR);
/* USER CODE END BSP_PWR_VBUSSetVoltage_Fixed */
}

/**
  * @brief  Set a fixed/variable PDO and manage the power control.
  * @note   Obsolete interface, new interface should be called.
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
  *         @arg TYPE_C_PORT_2
  * @param  VbusTargetMinInmv the vbus Target min (in mV)
  * @param  VbusTargetMaxInmv the vbus Target max (in mV)
  * @param  OperatingCurrent the Operating Current (in mA)
  * @param  MaxOperatingCurrent the Max Operating Current (in mA)
  * @retval PD controller status
  */
__weak PWR_StatusTypeDef BSP_PWR_VBUSSetVoltage_Variable(uint32_t PortId,
                                                  uint32_t VbusTargetMinInmv,
                                                  uint32_t VbusTargetMaxInmv,
                                                  uint32_t OperatingCurrent,
                                                  uint32_t MaxOperatingCurrent)
{
  PWR_DEBUG_TRACE(PortId, "ADVICE: Obsolete BSP_PWR_VBUSSetVoltage_Variable");
/* USER CODE BEGIN BSP_PWR_VBUSSetVoltage_Variable */
  return ((BSP_ERROR_NONE == BSP_USBPD_PWR_VBUSSetVoltage_Variable(PortId, VbusTargetMinInmv, VbusTargetMaxInmv, OperatingCurrent, MaxOperatingCurrent)) ? PWR_OK : PWR_ERROR);
/* USER CODE BEGIN BSP_PWR_VBUSSetVoltage_Variable */
}

/**
  * @brief  Set a Battery PDO and manage the power control.
  * @note   Obsolete interface, new interface should be called.
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
  *         @arg TYPE_C_PORT_2
  * @param  VbusTargetMin the vbus Target min (in mV)
  * @param  VbusTargetMax the vbus Target max (in mV)
  * @param  OperatingPower the Operating Power (in mW)
  * @param  MaxOperatingPower the Max Operating Power (in mW)
  * @retval PD controller status
  */
__weak PWR_StatusTypeDef BSP_PWR_VBUSSetVoltage_Battery(uint32_t PortId,
                                                 uint32_t VbusTargetMin,
                                                 uint32_t VbusTargetMax,
                                                 uint32_t OperatingPower,
                                                 uint32_t MaxOperatingPower)
{
  PWR_DEBUG_TRACE(PortId, "ADVICE: Obsolete BSP_PWR_VBUSSetVoltage_Battery");
/* USER CODE BEGIN BSP_PWR_VBUSSetVoltage_Battery */
  return ((BSP_ERROR_NONE == BSP_USBPD_PWR_VBUSSetVoltage_Battery(PortId, VbusTargetMin, VbusTargetMax, OperatingPower, MaxOperatingPower)) ? PWR_OK : PWR_ERROR);
/* USER CODE BEGIN BSP_PWR_VBUSSetVoltage_Battery */
}

/**
  * @brief  Set a APDO and manage the power control.
  * @note   Obsolete interface, new interface should be called.
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
  *         @arg TYPE_C_PORT_2
  * @param  VbusTargetInmv the vbus Target (in mV)
  * @param  OperatingCurrent the Operating current (in mA)
  * @param  Delta Delta between with previous APDO (in mV), 0 means APDO start
  * @retval PD controller status
  */
__weak PWR_StatusTypeDef BSP_PWR_VBUSSetVoltage_APDO(uint32_t PortId,
                                              uint32_t VbusTargetInmv,
                                              uint32_t OperatingCurrent,
                                              int32_t Delta)
{
  PWR_DEBUG_TRACE(PortId, "ADVICE: Obsolete BSP_PWR_VBUSSetVoltage_APDO");
/* USER CODE BEGIN BSP_PWR_VBUSSetVoltage_APDO */
  return ((BSP_ERROR_NONE == BSP_USBPD_PWR_VBUSSetVoltage_APDO(PortId, VbusTargetInmv, OperatingCurrent, Delta)) ? PWR_OK : PWR_ERROR);
/* USER CODE BEGIN BSP_PWR_VBUSSetVoltage_APDO */
}

/**
  * @brief  Get actual voltage level measured on the VBUS line.
  * @note   Obsolete interface, new interface should be called.
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
  *         @arg TYPE_C_PORT_2
  * @retval Voltage measured voltage level (in mV)
  */
__weak uint32_t BSP_PWR_VBUSGetVoltage(uint32_t PortId)
{
  PWR_DEBUG_TRACE(PortId, "ADVICE: Obsolete BSP_PWR_VBUSGetVoltage");
/* USER CODE BEGIN BSP_PWR_VBUSGetVoltage */
  uint32_t voltage = 0;

  (void)BSP_USBPD_PWR_VBUSGetVoltage(PortId, &voltage);

  return voltage;
/* USER CODE END BSP_PWR_VBUSGetVoltage */
}

/**
  * @brief  Get actual current level measured on the VBUS line.
  * @note   Obsolete interface, new interface should be called.
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
  *         @arg TYPE_C_PORT_2
  * @retval Current measured current level (in mA)
  */
__weak int32_t BSP_PWR_VBUSGetCurrent(uint32_t PortId)
{
  PWR_DEBUG_TRACE(PortId, "ADVICE: Obsolete BSP_PWR_VBUSGetCurrent");
/* USER CODE BEGIN BSP_PWR_VBUSGetCurrent */
  int32_t current = 0;

  (void)BSP_USBPD_PWR_VBUSGetCurrent(PortId, &current);

  return current;
/* USER CODE END BSP_PWR_VBUSGetCurrent */
}

/**
  * @brief  Initialize VCONN sourcing.
  * @note   Obsolete interface, new interface should be called.
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
  *         @arg TYPE_C_PORT_2
  * @param  CCPinId Type-C CC pin identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_CC1
  *         @arg TYPE_C_CC2
  * @retval PD controller status
  */
__weak PWR_StatusTypeDef BSP_PWR_VCONNInit(uint32_t PortId,
                                    uint32_t CCPinId)
{
  PWR_DEBUG_TRACE(PortId, "ADVICE: Obsolete BSP_PWR_VCONNInit");
/* USER CODE BEGIN BSP_PWR_VCONNInit */
  return ((BSP_ERROR_NONE == BSP_USBPD_PWR_VCONNInit(PortId, CCPinId)) ? PWR_OK : PWR_ERROR);
/* USER CODE END BSP_PWR_VCONNInit */
}

/**
  * @brief  Un-Initialize VCONN sourcing.
  * @note   Obsolete interface, new interface should be called.
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
  *         @arg TYPE_C_PORT_2
  * @param  CCPinId Type-C CC pin identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_CC1
  *         @arg TYPE_C_CC2
  * @retval PD controller status
  */
__weak PWR_StatusTypeDef BSP_PWR_VCONNDeInit(uint32_t PortId,
                                      uint32_t CCPinId)
{
  PWR_DEBUG_TRACE(PortId, "ADVICE: Obsolete BSP_PWR_VCONNDeInit");
/* USER CODE BEGIN BSP_PWR_VCONNDeInit */
  return ((BSP_ERROR_NONE == BSP_USBPD_PWR_VCONNDeInit(PortId, CCPinId)) ? PWR_OK : PWR_ERROR);
/* USER CODE END BSP_PWR_VCONNDeInit */
}

/**
  * @brief  Enable VCONN sourcing.
  * @note   Obsolete interface, new interface should be called.
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
  *         @arg TYPE_C_PORT_2
  * @param  CCPinId Type-C CC pin identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_CC1
  *         @arg TYPE_C_CC2
  * @retval PD controller status
  */
__weak PWR_StatusTypeDef BSP_PWR_VCONNOn(uint32_t PortId,
                                  uint32_t CCPinId)
{
  PWR_DEBUG_TRACE(PortId, "ADVICE: Obsolete BSP_PWR_VCONNOn");
/* USER CODE BEGIN BSP_PWR_VCONNOn */
  return ((BSP_ERROR_NONE == BSP_USBPD_PWR_VCONNOn(PortId, CCPinId)) ? PWR_OK : PWR_ERROR);
/* USER CODE END BSP_PWR_VCONNOn */
}

/**
  * @brief  Disable VCONN sourcing.
  * @note   Obsolete interface, new interface should be called.
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
  *         @arg TYPE_C_PORT_2
  * @param  CCPinId CC pin identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_CC1
  *         @arg TYPE_C_CC2
  * @retval PD controller status
  */
__weak PWR_StatusTypeDef BSP_PWR_VCONNOff(uint32_t PortId,
                                   uint32_t CCPinId)
{
  PWR_DEBUG_TRACE(PortId, "ADVICE: Obsolete BSP_PWR_VCONNOff");
/* USER CODE BEGIN BSP_PWR_VCONNOff */
  return ((BSP_ERROR_NONE == BSP_USBPD_PWR_VCONNOff(PortId, CCPinId)) ? PWR_OK : PWR_ERROR);
/* USER CODE END BSP_PWR_VCONNOff */
}

/**
  * @brief  Set the VBUS disconnection voltage threshold.
  * @note   Obsolete interface, new interface should be called.
  * @note   Callback function registered through BSP_PWR_RegisterVBUSDetectCallback
  *         function call is invoked when VBUS falls below programmed threshold.
  * @note   By default VBUS disconnection threshold is set to 3.3V
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
  *         @arg TYPE_C_PORT_2
  * @param  VoltageThreshold: VBUS disconnection voltage threshold (in mV)
  * @retval PD controller status
  */
__weak void BSP_PWR_SetVBUSDisconnectionThreshold(uint32_t PortId,
                                           uint32_t VoltageThreshold)
{
  PWR_DEBUG_TRACE(PortId, "ADVICE: Obsolete BSP_PWR_SetVBUSDisconnectionThreshold");
/* USER CODE BEGIN BSP_PWR_SetVBUSDisconnectionThreshold */
  (void)BSP_USBPD_PWR_SetVBUSDisconnectionThreshold(PortId, VoltageThreshold);
/* USER CODE END BSP_PWR_SetVBUSDisconnectionThreshold */
}

/**
  * @brief  Register USB Type-C Current callback function.
  * @note   Obsolete interface, new interface should be called.
  * @note   Callback function invoked when VBUS rises above 4V (VBUS present) or
  *         when VBUS falls below programmed threshold (VBUS absent).
  * @note   Callback function is un-registered when callback function pointer
  *         argument is NULL.
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
  *         @arg TYPE_C_PORT_2
  * @param  pfnVBUSDetectCallback callback function pointer
  * @retval 0 success else fail
  */
__weak PWR_StatusTypeDef BSP_PWR_RegisterVBUSDetectCallback(uint32_t                       PortId,
                                                     PWR_VBUSDetectCallbackFunc *   pfnVBUSDetectCallback)
{
  PWR_DEBUG_TRACE(PortId, "ADVICE: Obsolete BSP_PWR_RegisterVBUSDetectCallback");
/* USER CODE BEGIN BSP_PWR_RegisterVBUSDetectCallback */
  return ((BSP_ERROR_NONE == BSP_USBPD_PWR_RegisterVBUSDetectCallback(PortId, (USBPD_PWR_VBUSDetectCallbackFunc*)pfnVBUSDetectCallback)) ? PWR_OK : PWR_ERROR);
/* USER CODE END BSP_PWR_RegisterVBUSDetectCallback */
}

/**
  * @brief  Get actual VBUS status.
  * @note   Obsolete interface, new interface should be called.
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
  *         @arg TYPE_C_PORT_2
  * @retval VBUS status (1: On, 0: Off)
  */
__weak uint8_t BSP_PWR_VBUSIsOn(uint32_t PortId)
{
  PWR_DEBUG_TRACE(PortId, "ADVICE: Obsolete BSP_PWR_VBUSIsOn");
/* USER CODE BEGIN BSP_PWR_VBUSIsOn */
  uint8_t state = 0;

  (void)BSP_USBPD_PWR_VBUSIsOn(PortId, &state);

  return state;
/* USER CODE END BSP_PWR_VBUSIsOn */
}

/**
  * @brief  Get actual VCONN status.
  * @note   Obsolete interface, new interface should be called.
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
  *         @arg TYPE_C_PORT_2
  * @param  CCPinId Type-C CC pin identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_CC1
  *         @arg TYPE_C_CC2
  * @retval VCONN status (1: On, 0: Off)
  */
__weak uint8_t BSP_PWR_VCONNIsOn(uint32_t PortId,
                          uint32_t CCPinId)
{
  PWR_DEBUG_TRACE(PortId, "ADVICE: Obsolete BSP_PWR_VCONNIsOn");
/* USER CODE BEGIN BSP_PWR_VCONNIsOn */
  uint8_t state = 0;

  (void)BSP_USBPD_PWR_VCONNIsOn(PortId, CCPinId, &state);

  return state;
/* USER CODE END BSP_PWR_VCONNIsOn */
}

/**
  * @}
  */

/** @addtogroup POWER_Exported_Functions2
  * @{
  */

/**
  * @brief  Global initialization of PWR resource used by USB-PD
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_2
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_Init(uint32_t Instance)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_Init */
  /* Check if instance is valid       */
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  return ret;
  /* USER CODE END BSP_USBPD_PWR_Init */
}

/**
  * @brief  Global de-initialization of PWR resource used by USB-PD
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_2
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_Deinit(uint32_t Instance)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_Deinit */
  /* Check if instance is valid       */
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  return ret;
  /* USER CODE END BSP_USBPD_PWR_Deinit */
}

/**
  * @brief  Initialize the hardware resources used by the Type-C power delivery (PD)
  *         controller.
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_2
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_VBUSInit(uint32_t Instance)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_VBUSInit */
  /* Check if instance is valid       */
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* !!!
      BSP_PWR_VBUSInit is obsolete. You may need to move your user code
      inside this function
    !!! */
    PWR_DEBUG_TRACE(Instance, "ADVICE: Update BSP_USBPD_PWR_VBUSInit");
  }

  return ret;
  /* USER CODE END BSP_USBPD_PWR_VBUSInit */
}

/**
  * @brief  Release the hardware resources used by the Type-C power delivery (PD)
  *         controller.
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_2
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_VBUSDeInit(uint32_t Instance)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_VBUSDeInit */
  /* Check if instance is valid       */
  int32_t ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  /* !!!
      BSP_PWR_VBUSDeInit is obsolete. You may need to move your user code
      inside this function
   !!! */
  PWR_DEBUG_TRACE(Instance, "ADVICE: Update BSP_USBPD_PWR_VBUSDeInit");
  return ret;
  /* USER CODE END BSP_USBPD_PWR_VBUSDeInit */
}

/**
  * @brief  Enable power supply over VBUS.
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_2
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_VBUSOn(uint32_t Instance)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_VBUSOn */
  /* Check if instance is valid       */
  int32_t ret;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
    /* !!!
        BSP_PWR_VBUSOn is obsolete. You may need to move your user code
        inside this function
     !!! */
    PWR_DEBUG_TRACE(Instance, "ADVICE: Update BSP_USBPD_PWR_VBUSOn");
  }
  return ret;
  /* USER CODE END BSP_USBPD_PWR_VBUSOn */
}

/**
  * @brief  Disable power supply over VBUS.
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_2
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_VBUSOff(uint32_t Instance)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_VBUSOff */
  /* Check if instance is valid       */
  int32_t ret;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
    /* !!!
        BSP_PWR_VBUSOff is obsolete. You may need to move your user code
        inside this function
     !!! */
    PWR_DEBUG_TRACE(Instance, "ADVICE: Update BSP_USBPD_PWR_VBUSOff");
  }
  return ret;
  /* USER CODE END BSP_USBPD_PWR_VBUSOff */
}

/**
  * @brief  Set a fixed/variable PDO and manage the power control.
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_2
  * @param  VbusTargetInmv the vbus Target (in mV)
  * @param  OperatingCurrent the Operating Current (in mA)
  * @param  MaxOperatingCurrent the Max Operating Current (in mA)
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_VBUSSetVoltage_Fixed(uint32_t Instance,
                                           uint32_t VbusTargetInmv,
                                           uint32_t OperatingCurrent,
                                           uint32_t MaxOperatingCurrent)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_VBUSSetVoltage_Fixed */
  /* Check if instance is valid       */
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  /* !!!
      BSP_PWR_VBUSSetVoltage_Fixed is obsolete. You may need to move your user code
      inside this function
   !!! */
  PWR_DEBUG_TRACE(Instance, "ADVICE: Update BSP_USBPD_PWR_VBUSSetVoltage_Fixed");
  return ret;
  /* USER CODE END BSP_USBPD_PWR_VBUSSetVoltage_Fixed */
}

/**
  * @brief  Set a fixed/variable PDO and manage the power control.
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_2
  * @param  VbusTargetMinInmv the vbus Target min (in mV)
  * @param  VbusTargetMaxInmv the vbus Target max (in mV)
  * @param  OperatingCurrent the Operating Current (in mA)
  * @param  MaxOperatingCurrent the Max Operating Current (in mA)
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_VBUSSetVoltage_Variable(uint32_t Instance,
                                              uint32_t VbusTargetMinInmv,
                                              uint32_t VbusTargetMaxInmv,
                                              uint32_t OperatingCurrent,
                                              uint32_t MaxOperatingCurrent)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_VBUSSetVoltage_Variable */
  /* Check if instance is valid       */
  int32_t ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  /* !!!
      BSP_PWR_VBUSSetVoltage_Variable is obsolete. You may need to move your user code
      inside this function
   !!! */
  PWR_DEBUG_TRACE(Instance, "ADVICE: Update BSP_USBPD_PWR_VBUSSetVoltage_Variable");
  return ret;
  /* USER CODE END BSP_USBPD_PWR_VBUSSetVoltage_Variable */
}

/**
  * @brief  Set a Battery PDO and manage the power control.
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_2
  * @param  VbusTargetMin the vbus Target min (in mV)
  * @param  VbusTargetMax the vbus Target max (in mV)
  * @param  OperatingPower the Operating Power (in mW)
  * @param  MaxOperatingPower the Max Operating Power (in mW)
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_VBUSSetVoltage_Battery(uint32_t Instance,
                                             uint32_t VbusTargetMin,
                                             uint32_t VbusTargetMax,
                                             uint32_t OperatingPower,
                                             uint32_t MaxOperatingPower)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_VBUSSetVoltage_Battery */
  /* Check if instance is valid       */
  int32_t ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  /* !!!
      BSP_PWR_VBUSSetVoltage_Battery is obsolete. You may need to move your user code
      inside this function
   !!! */
  PWR_DEBUG_TRACE(Instance, "ADVICE: Update BSP_USBPD_PWR_VBUSSetVoltage_Battery");
  return ret;
  /* USER CODE END BSP_USBPD_PWR_VBUSSetVoltage_Battery */
}

/**
  * @brief  Set a APDO and manage the power control.
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_2
  * @param  VbusTargetInmv the vbus Target (in mV)
  * @param  OperatingCurrent the Operating current (in mA)
  * @param  Delta Delta between with previous APDO (in mV), 0 means APDO start
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_VBUSSetVoltage_APDO(uint32_t Instance,
                                          uint32_t VbusTargetInmv,
                                          uint32_t OperatingCurrent,
                                          int32_t Delta)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_VBUSSetVoltage_APDO */
  /* Check if instance is valid       */
  int32_t ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  /* !!!
      BSP_PWR_VBUSSetVoltage_APDO is obsolete. You may need to move your user code
      inside this function
   !!! */
  PWR_DEBUG_TRACE(Instance, "ADVICE: Update BSP_USBPD_PWR_VBUSSetVoltage_APDO");
  return ret;
  /* USER CODE END BSP_USBPD_PWR_VBUSSetVoltage_APDO */
}

/**
  * @brief  Get actual voltage level measured on the VBUS line.
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_2
  * @param  pVoltage Pointer on measured voltage level (in mV)
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_VBUSGetVoltage(uint32_t Instance, uint32_t *pVoltage)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_VBUSGetVoltage */

  /* Check if instance is valid       */
  int32_t ret = BSP_ERROR_NONE;

  if ((Instance >= USBPD_PWR_INSTANCES_NBR) || (NULL == pVoltage))
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  *pVoltage = 0u;
  /* !!!
      BSP_PWR_VBUSGetVoltage is obsolete. You may need to move your user code
      inside this function
   !!! */
  PWR_DEBUG_TRACE(Instance, "ADVICE: Update BSP_USBPD_PWR_VBUSGetVoltage");
  return ret;
  /* USER CODE END BSP_USBPD_PWR_VBUSGetVoltage */
}

/**
  * @brief  Get actual current level measured on the VBUS line.
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_2
  * @param  pCurrent Pointer on measured current level (in mA)
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_VBUSGetCurrent(uint32_t Instance, int32_t *pCurrent)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_VBUSGetCurrent */
  /* Check if instance is valid       */
  int32_t ret;

  if ((Instance >= USBPD_PWR_INSTANCES_NBR) || (NULL == pCurrent))
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    *pCurrent = 0;
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  /* !!!
      BSP_PWR_VBUSGetCurrent is obsolete. You may need to move your user code
      inside this function
   !!! */
  PWR_DEBUG_TRACE(Instance, "ADVICE: Update BSP_USBPD_PWR_VBUSGetCurrent");
  return ret;
  /* USER CODE END BSP_USBPD_PWR_VBUSGetCurrent */
}

/**
  * @brief  Initialize VCONN sourcing.
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_2
  * @param  CCPinId Type-C CC pin identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_CC1
  *         @arg @ref USBPD_PWR_TYPE_C_CC2
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_VCONNInit(uint32_t Instance,
                                uint32_t CCPinId)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_VCONNInit */
  /* Check if instance is valid       */
  int32_t ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  /* !!!
      BSP_PWR_VCONNInit is obsolete. You may need to move your user code
      inside this function
   !!! */
  PWR_DEBUG_TRACE(Instance, "ADVICE: Update BSP_USBPD_PWR_VCONNInit");
  return ret;
  /* USER CODE END BSP_USBPD_PWR_VCONNInit */
}

/**
  * @brief  Un-Initialize VCONN sourcing.
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_2
  * @param  CCPinId Type-C CC pin identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_CC1
  *         @arg @ref USBPD_PWR_TYPE_C_CC2
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_VCONNDeInit(uint32_t Instance,
                                  uint32_t CCPinId)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_VCONNDeInit */
  /* Check if instance is valid       */
  int32_t ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  /* !!!
      BSP_PWR_VCONNDeInit is obsolete. You may need to move your user code
      inside this function
   !!! */
  PWR_DEBUG_TRACE(Instance, "ADVICE: Update BSP_USBPD_PWR_VCONNDeInit");
  return ret;
  /* USER CODE END BSP_USBPD_PWR_VCONNDeInit */
}

/**
  * @brief  Enable VCONN sourcing.
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_2
  * @param  CCPinId Type-C CC pin identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_CC1
  *         @arg @ref USBPD_PWR_TYPE_C_CC2
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_VCONNOn(uint32_t Instance,
                              uint32_t CCPinId)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_VCONNOn */
  /* Check if instance is valid       */
  int32_t ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  /* !!!
      BSP_PWR_VCONNOn is obsolete. You may need to move your user code
      inside this function
   !!! */
  PWR_DEBUG_TRACE(Instance, "ADVICE: Update BSP_USBPD_PWR_VCONNOn");
  return ret;
  /* USER CODE END BSP_USBPD_PWR_VCONNOn */
}

/**
  * @brief  Disable VCONN sourcing.
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_2
  * @param  CCPinId CC pin identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_CC1
  *         @arg @ref USBPD_PWR_TYPE_C_CC2
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_VCONNOff(uint32_t Instance,
                               uint32_t CCPinId)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_VCONNOff */
  /* Check if instance is valid       */
  int32_t ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  /* !!!
      BSP_PWR_VCONNOff is obsolete. You may need to move your user code
      inside this function
   !!! */
  PWR_DEBUG_TRACE(Instance, "ADVICE: Update BSP_USBPD_PWR_VCONNOff");
  return ret;
  /* USER CODE END BSP_USBPD_PWR_VCONNOff */
}

/**
  * @brief  Get actual VCONN status.
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_2
  * @param  CCPinId Type-C CC pin identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_CC1
  *         @arg @ref USBPD_PWR_TYPE_C_CC2
  * @param  pState VCONN status (1: On, 0: Off)
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_VCONNIsOn(uint32_t Instance,
                                uint32_t CCPinId, uint8_t *pState)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_VCONNIsOn */
  /* Check if instance is valid       */
  int32_t ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  *pState = 0u;
  /* !!!
      BSP_PWR_VCONNIsOn is obsolete. You may need to move your user code
      inside this function
   !!! */
  PWR_DEBUG_TRACE(Instance, "ADVICE: Update BSP_USBPD_PWR_VCONNIsOn");
  return ret;
  /* USER CODE END BSP_USBPD_PWR_VCONNIsOn */
}

/**
  * @brief  Set the VBUS disconnection voltage threshold.
  * @note   Callback function registered through BSP_USBPD_PWR_RegisterVBUSDetectCallback
  *         function call is invoked when VBUS falls below programmed threshold.
  * @note   By default VBUS disconnection threshold is set to 3.3V
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_2
  * @param  VoltageThreshold VBUS disconnection voltage threshold (in mV)
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_SetVBUSDisconnectionThreshold(uint32_t Instance,
                                                    uint32_t VoltageThreshold)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_SetVBUSDisconnectionThreshold */
  /* Check if instance is valid       */
  int32_t ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  /* !!!
      BSP_PWR_SetVBUSDisconnectionThreshold is obsolete. You may need to move your user code
      inside this function
   !!! */
  PWR_DEBUG_TRACE(Instance, "ADVICE: Update BSP_USBPD_PWR_SetVBUSDisconnectionThreshold");
  return ret;
  /* USER CODE END BSP_USBPD_PWR_SetVBUSDisconnectionThreshold */
}

/**
  * @brief  Register USB Type-C Current callback function.
  * @note   Callback function invoked when VBUS rises above 4V (VBUS present) or
  *         when VBUS falls below programmed threshold (VBUS absent).
  * @note   Callback function is un-registered when callback function pointer
  *         argument is NULL.
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_2
  * @param  pfnVBUSDetectCallback callback function pointer
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_RegisterVBUSDetectCallback(uint32_t  Instance,
                                                 USBPD_PWR_VBUSDetectCallbackFunc *pfnVBUSDetectCallback)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_RegisterVBUSDetectCallback */
  /* Check if instance is valid       */
  int32_t ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  /* !!!
      BSP_PWR_RegisterVBUSDetectCallback is obsolete. You may need to move your user code
      inside this function
   !!! */
  PWR_DEBUG_TRACE(Instance, "ADVICE: Update BSP_USBPD_PWR_RegisterVBUSDetectCallback");
  return ret;
  /* USER CODE END BSP_USBPD_PWR_RegisterVBUSDetectCallback */
}

/**
  * @brief  Get actual VBUS status.
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_2
  * @param  pState VBUS status (1: On, 0: Off)
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_VBUSIsOn(uint32_t Instance, uint8_t *pState)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_VBUSIsOn */
  /* Check if instance is valid       */
  int32_t ret;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
    /* !!!
        BSP_PWR_VBUSIsOn is obsolete. You may need to move your user code
        inside this function
     !!! */
    PWR_DEBUG_TRACE(Instance, "ADVICE: Update BSP_USBPD_PWR_VBUSIsOn");
  }
  *pState = 0u;
  return ret;
  /* USER CODE END BSP_USBPD_PWR_VBUSIsOn */
}

/**
  * @brief  Set state of VCC (specific functions)
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_2
  * @param  State VCC state
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_VCCSetState(uint32_t Instance, uint32_t State)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_VCCSetState */
  /* Check if instance is valid       */
  int32_t ret = BSP_ERROR_NONE;

  if ((Instance != USBPD_PWR_TYPE_C_PORT_1) && (Instance != USBPD_PWR_TYPE_C_PORT_2))
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
    PWR_DEBUG_TRACE(Instance, "ADVICE: Update BSP_USBPD_PWR_VCCSetState");
  }
  return ret;
  /* USER CODE END BSP_USBPD_PWR_VCCSetState */
}
/**
  * @}
  */

/** @addtogroup POWER_Private_Functions
  * @{
  */

/* USER CODE BEGIN POWER_Private_Functions */

/* USER CODE END POWER_Private_Functions */

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

