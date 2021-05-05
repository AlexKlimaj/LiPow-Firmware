/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usbpd_pwr_user.h
  * @author  MCD Application Team
  * @brief   Header file for usbpd_pwr_user.c file
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBPD_PWR_USER_H_
#define __USBPD_PWR_USER_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup POWER
  * @{
  */

/** @defgroup POWER_Exported_Types Exported Types (BSP V1)
  * @{
  */

/**
  * @brief  POWER Status
  */
typedef enum
{
    PWR_OK = 0,
    PWR_ERROR
} PWR_StatusTypeDef;

/**
  * @brief  Power role
  */
#define PWR_PowerRoleTypeDef USBPD_PWR_PowerRoleTypeDef

/**
  * @brief  Voltage control mode
  */
#define PWR_DCDCCtrlModeTypeDef USBPD_PWR_DCDCCtrlModeTypeDef

/**
  * @brief  VBUS connection status
  */
#define PWR_VBUSConnectionStatusTypeDef USBPD_PWR_VBUSConnectionStatusTypeDef

/**
  * @}
  */

/** @defgroup POWER_Exported_Types2 Exported Types based on BSP V2 specification
  *@{
  */

/**
  * @brief  Power role
  */
typedef enum
{
  POWER_ROLE_SOURCE = 0,
  POWER_ROLE_SINK,
  POWER_ROLE_DUAL
} USBPD_PWR_PowerRoleTypeDef;

/**
  * @brief  Voltage control mode
  */
typedef enum
{
  DCDC_CTRL_MODE_UNKNOWN = 0,
  DCDC_CTRL_MODE_GPIO,
  DCDC_CTRL_MODE_PWM,
} USBPD_PWR_DCDCCtrlModeTypeDef;

/**
  * @brief  VBUS connection status
  */
typedef enum
{
  VBUS_CONNECTED = 0,
  VBUS_NOT_CONNECTED
} USBPD_PWR_VBUSConnectionStatusTypeDef;

/* Keep for Legacy BSP V1 */
/**
  * @brief VBUS Detection Callback
  */
typedef void PWR_VBUSDetectCallbackFunc(uint32_t Instance,
                                        PWR_VBUSConnectionStatusTypeDef VBUSConnectionStatus);

/**
  * @brief VBUS Detection Callback
  */
typedef void USBPD_PWR_VBUSDetectCallbackFunc(uint32_t Instance,
                                              USBPD_PWR_VBUSConnectionStatusTypeDef VBUSConnectionStatus);

/**
  * @}
  */

/** @defgroup POWER_Exported_Constants Exported Constants (BSP V1)
  * @{
  */
/**
  * @brief  Number of TypeC ports
  */
#define PWR_TYPEC_PORT_NB USBPD_PWR_INSTANCES_NBR

/**
  * @brief  Type-C port identifier
  */
#define TYPE_C_PORT_1  USBPD_PWR_TYPE_C_PORT_1
#define TYPE_C_PORT_2  USBPD_PWR_TYPE_C_PORT_2

/**
  * @brief  CC pin identifier
  */
#define TYPE_C_CC1     USBPD_PWR_TYPE_C_CC1
#define TYPE_C_CC2     USBPD_PWR_TYPE_C_CC2

/**
  * @brief  VBUS disconnection threshold values (in mV)
  */
#define BSP_PWR_HIGH_VBUS_THRESHOLD     USBPD_PWR_HIGH_VBUS_THRESHOLD
#define BSP_PWR_LOW_VBUS_THRESHOLD      USBPD_PWR_LOW_VBUS_THRESHOLD

/**
  * @brief  VBUS discharge parameters
  */
#define BSP_PWR_DISCHARGE_MARGIN        USBPD_PWR_DISCHARGE_MARGIN
#define BSP_PWR_DISCHARGE_TIME          USBPD_PWR_DISCHARGE_TIME

/**
  * @brief  Calibration settings
  */
#define BSP_PWR_DCDC_PRECISION          USBPD_PWR_DCDC_PRECISION
#define BSP_PWR_CALIBRATION_ENABLED     USBPD_PWR_CALIBRATION_ENABLED
#define BSP_PWR_CALIBRATION_DISABLED    USBPD_PWR_CALIBRATION_DISABLED

/**
  * @brief  Standard VBUS voltage levels
  */
#define BSP_PWR_VBUS_5V                 5000
#define BSP_PWR_VBUS_9V                 9000
#define BSP_PWR_VBUS_15V                15000

/**
  * @brief  power timeout
  */
#define BSP_PWR_TIMEOUT_PDO             USBPD_PWR_TIMEOUT_PDO
#define BSP_PWR_TIMEOUT_APDO            USBPD_PWR_TIMEOUT_APDO

/**
  * @brief  Invalid value set during issue with voltage setting
  */
#define BSP_PWR_INVALID_VALUE           USBPD_PWR_INVALID_VALUE

/**
  * @}
  */

/** @defgroup POWER_Exported_Constants2 Exported Constants (BSP V2)
  * @{
  */
/* Common Error codes */
#define BSP_ERROR_NONE                         0
#define BSP_ERROR_NO_INIT                     -1
#define BSP_ERROR_WRONG_PARAM                 -2
#define BSP_ERROR_BUSY                        -3
#define BSP_ERROR_PERIPH_FAILURE              -4
#define BSP_ERROR_COMPONENT_FAILURE           -5
#define BSP_ERROR_UNKNOWN_FAILURE             -6
#define BSP_ERROR_UNKNOWN_COMPONENT           -7
#define BSP_ERROR_BUS_FAILURE                 -8
#define BSP_ERROR_CLOCK_FAILURE               -9
#define BSP_ERROR_MSP_FAILURE                 -10
#define BSP_ERROR_FEATURE_NOT_SUPPORTED       -11

/**
  * @brief  Number of TypeC ports
  */
#define USBPD_PWR_INSTANCES_NBR                 2u

/**
  * @brief  Type-C port identifier
  */
#define USBPD_PWR_TYPE_C_PORT_1                 0u
#define USBPD_PWR_TYPE_C_PORT_2                 1u

/**
  * @brief  CC pin identifier
  */
#define USBPD_PWR_TYPE_C_CC1                    1U
#define USBPD_PWR_TYPE_C_CC2                    2U

/**
  * @brief  VBUS disconnection threshold values (in mV)
  */
#define USBPD_PWR_HIGH_VBUS_THRESHOLD           (2800U)
#define USBPD_PWR_LOW_VBUS_THRESHOLD            (750U)
#define USBPD_PWR_VBUS_THRESHOLD_5V             (3900U)
#define USBPD_PWR_VBUS_THRESHOLD_9V             (7000U)
#define USBPD_PWR_VBUS_THRESHOLD_15V            (12500U)
#define USBPD_PWR_VBUS_THRESHOLD_20V            (17000U)
#define USBPD_PWR_VBUS_THRESHOLD_APDO           (2150U)

/**
  * @brief  VBUS discharge parameters
  */
#define USBPD_PWR_DISCHARGE_MARGIN              (500U)
#define USBPD_PWR_DISCHARGE_TIME                (6U)

/**
  * @brief  Calibration settings
  */
#define USBPD_PWR_DCDC_PRECISION                (20U)     /* DCDC output precision set to 20mV (Noise)*/
#define USBPD_PWR_CALIBRATION_ENABLED           (1U)
#define USBPD_PWR_CALIBRATION_DISABLED          (0U)

/**
  * @brief  Standard VBUS voltage levels
  */
#define USBPD_PWR_VBUS_5V                       5000U
#define USBPD_PWR_VBUS_9V                       9000U
#define USBPD_PWR_VBUS_15V                      15000U

/**
  * @brief  power timeout
  */
#define USBPD_PWR_TIMEOUT_PDO                   250U         /* Timeout for PDO to PDO or PDO to APDO at 250ms*/
#define USBPD_PWR_TIMEOUT_APDO                  25U          /* Timeout for APDO to APDO at 25ms*/

/**
  * @brief  Invalid value set during issue with voltage setting
  */
#define USBPD_PWR_INVALID_VALUE           0xFFFFFFFFU

/**
  * @}
  */

/** @defgroup POWER_Exported_Functions Exported Functions (BSP V1)
  * @{
  */
PWR_StatusTypeDef BSP_PWR_VBUSInit(uint32_t PortId);

PWR_StatusTypeDef BSP_PWR_VBUSDeInit(uint32_t PortId);

PWR_StatusTypeDef BSP_PWR_VBUSOn(uint32_t PortId);

PWR_StatusTypeDef BSP_PWR_VBUSOff(uint32_t PortId);

PWR_StatusTypeDef BSP_PWR_VBUSSetVoltage_Fixed(uint32_t PortNum,
                                               uint32_t VbusTargetInmv,
                                               uint32_t OperatingCurrent,
                                               uint32_t MaxOperatingCurrent);

PWR_StatusTypeDef BSP_PWR_VBUSSetVoltage_Variable(uint32_t PortNum,
                                                  uint32_t VbusTargetMaxInmv,
                                                  uint32_t VbusTargetMinInmv,
                                                  uint32_t OperatingCurrent,
                                                  uint32_t MaxOperatingCurrent);

PWR_StatusTypeDef BSP_PWR_VBUSSetVoltage_Battery(uint32_t PortId,
                                                 uint32_t VbusTargetMin,
                                                 uint32_t VbusTargetMax,
                                                 uint32_t OperatingPower,
                                                 uint32_t MaxOperatingPower);
PWR_StatusTypeDef BSP_PWR_VBUSSetVoltage_APDO(uint32_t PortId,
                                              uint32_t VbusTargetInmv,
                                              uint32_t OperatingCurrent,
                                              int32_t Delta);
uint32_t BSP_PWR_VBUSGetVoltage(uint32_t PortId);

int32_t BSP_PWR_VBUSGetCurrent(uint32_t PortId);

PWR_StatusTypeDef BSP_PWR_VCONNInit(uint32_t PortId,
                                    uint32_t CCPinId);

PWR_StatusTypeDef BSP_PWR_VCONNDeInit(uint32_t PortId,
                                      uint32_t CCPinId);

PWR_StatusTypeDef BSP_PWR_VCONNOn(uint32_t PortId,
                                  uint32_t CCPinId);

PWR_StatusTypeDef BSP_PWR_VCONNOff(uint32_t PortId,
                                   uint32_t CCPinId);

void BSP_PWR_SetVBUSDisconnectionThreshold(uint32_t PortId,
                                           uint32_t VoltageThreshold);

PWR_StatusTypeDef BSP_PWR_RegisterVBUSDetectCallback(uint32_t                      PortId,
                                                     PWR_VBUSDetectCallbackFunc *  pfnVBUSDetectCallback);

uint8_t BSP_PWR_VBUSIsOn(uint32_t PortId);

uint8_t BSP_PWR_VCONNIsOn(uint32_t PortId,
                          uint32_t CCPinId);

/**
  * @}
  */

/** @defgroup POWER_Exported_Functions2 Exported Functions (BSP V2)
  * @{
  */
/* Common functions */
int32_t BSP_USBPD_PWR_Init(uint32_t Instance);

int32_t BSP_USBPD_PWR_Deinit(uint32_t Instance);

int32_t BSP_USBPD_PWR_VBUSInit(uint32_t Instance);

int32_t BSP_USBPD_PWR_VBUSDeInit(uint32_t Instance);

int32_t BSP_USBPD_PWR_VBUSOn(uint32_t Instance);

int32_t BSP_USBPD_PWR_VBUSOff(uint32_t Instance);

int32_t BSP_USBPD_PWR_VBUSSetVoltage_Fixed(uint32_t Instance,
                                           uint32_t VbusTargetInmv,
                                           uint32_t OperatingCurrent,
                                           uint32_t MaxOperatingCurrent);

int32_t BSP_USBPD_PWR_VBUSSetVoltage_Variable(uint32_t Instance,
                                              uint32_t VbusTargetMaxInmv,
                                              uint32_t VbusTargetMinInmv,
                                              uint32_t OperatingCurrent,
                                              uint32_t MaxOperatingCurrent);

int32_t BSP_USBPD_PWR_VBUSSetVoltage_Battery(uint32_t Instance,
                                             uint32_t VbusTargetMin,
                                             uint32_t VbusTargetMax,
                                             uint32_t OperatingPower,
                                             uint32_t MaxOperatingPower);

int32_t BSP_USBPD_PWR_VBUSSetVoltage_APDO(uint32_t Instance,
                                          uint32_t VbusTargetInmv,
                                          uint32_t OperatingCurrent,
                                          int32_t Delta);

int32_t BSP_USBPD_PWR_VBUSGetVoltage(uint32_t Instance, uint32_t *pVoltage);

int32_t BSP_USBPD_PWR_VBUSGetCurrent(uint32_t Instance, int32_t *pCurrent);

int32_t BSP_USBPD_PWR_VCONNInit(uint32_t Instance,
                                uint32_t CCPinId);

int32_t BSP_USBPD_PWR_VCONNDeInit(uint32_t Instance,
                                  uint32_t CCPinId);

int32_t BSP_USBPD_PWR_VCONNOn(uint32_t Instance,
                              uint32_t CCPinId);

int32_t BSP_USBPD_PWR_VCONNOff(uint32_t Instance,
                               uint32_t CCPinId);

int32_t BSP_USBPD_PWR_SetVBUSDisconnectionThreshold(uint32_t Instance,
                                                    uint32_t VoltageThreshold);

int32_t BSP_USBPD_PWR_RegisterVBUSDetectCallback(uint32_t Instance,
                                                 USBPD_PWR_VBUSDetectCallbackFunc *pfnVBUSDetectCallback);

int32_t BSP_USBPD_PWR_VBUSIsOn(uint32_t Instance, uint8_t *pState);

int32_t BSP_USBPD_PWR_VCONNIsOn(uint32_t Instance,
                                uint32_t CCPinId, uint8_t *pState);

int32_t BSP_USBPD_PWR_VCCSetState(uint32_t Instance, uint32_t State);

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

#endif /* __USBPD_PWR_USER_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

