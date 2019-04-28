/**
  ******************************************************************************
  * @file    usbpd_pwr_if.c
  * @author  MCD Application Team
  * @brief   This file contains power interface control functions.
  ******************************************************************************
   * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#define __USBPD_PWR_IF_C

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx.h"
#include "usbpd_core.h"
#include "usbpd_pwr_if.h"
#include "usbpd_hw_if.h"
#include "usbpd_dpm_core.h"
#include "usbpd_dpm_conf.h"
#include "usbpd_pdo_defs.h"
#if defined (_TRACE)
#include "usbpd_trace.h"
#endif /* _TRACE */
#include "string.h"
#include <stdio.h>
#include "cmsis_os.h"

#include "main.h"

/* Private typedef -----------------------------------------------------------*/
#ifdef _TRACE
#define POWER_IF_TRACE(_PORT_,_MSG_,_SIZE_) USBPD_TRACE_Add(USBPD_TRACE_DEBUG,_PORT_,0,(uint8_t *)_MSG_, _SIZE_);
#else
#define POWER_IF_TRACE(_PORT_,_MSG_,_SIZE_)
#endif
/* Private define ------------------------------------------------------------*/

#define ABS(__VAL__) ((__VAL__) < 0 ? - (__VAL__) : (__VAL__))

/* Private macros ------------------------------------------------------------*/
#define _PWR_UPDATE_VOLTAGE_MIN(_PDO_VOLT_, _SNK_VOLT_) \
            if ((_PDO_VOLT_) < (_SNK_VOLT_)) \
            { \
              /* Update min voltage */ \
              (_SNK_VOLT_) = (_PDO_VOLT_); \
            }
#define _PWR_UPDATE_VOLTAGE_MAX(_PDO_VOLT_, _SNK_VOLT_) \
            if ((_PDO_VOLT_) > (_SNK_VOLT_)) \
            { \
              /* Update min voltage */ \
              (_SNK_VOLT_) = (_PDO_VOLT_); \
            }
#define _PWR_UPDATE_CURRENT_MAX(_PDO_CURRENT_, _SNK_CURRENT_) \
            if ((_PDO_CURRENT_) > (_SNK_CURRENT_)) \
            { \
              /* Update min current */ \
              (_SNK_CURRENT_) = (_PDO_CURRENT_); \
            }
#define _PWR_UPDATE_POWER_MAX(_PDO_POWER_, _SNK_POWER_) \
            if ((_PDO_POWER_) > (_SNK_POWER_)) \
            { \
              /* Update min POWER */ \
              (_SNK_POWER_) = (_PDO_POWER_); \
            }
#define _PWR_CHECK_VOLTAGE_MIN(_PDO_VOLT_, _SNK_VOLT_) \
          if ((_PDO_VOLT_) != (_SNK_VOLT_)) \
          { \
            /* Disalignment between PDO and DPM_SNKRequestedPower structure */ \
            _status = USBPD_ERROR; \
          }
#define _PWR_CHECK_VOLTAGE_MAX(_PDO_VOLT_, _SNK_VOLT_) \
          if ((_PDO_VOLT_) != (_SNK_VOLT_)) \
          { \
            /* Disalignment between PDO and DPM_SNKRequestedPower structure */ \
            _status = USBPD_ERROR; \
          }
#define _PWR_CHECK_CURRENT_MAX(_PDO_CURRENT_, _SNK_CURRENT_) \
          if ((_PDO_CURRENT_) != (_SNK_CURRENT_)) \
          { \
            /* Disalignment between PDO and DPM_SNKRequestedPower structure */ \
            _status = USBPD_ERROR; \
          }
#define _PWR_CHECK_POWER_MAX(_PDO_POWER_, _SNK_POWER_) \
          if ((_PDO_POWER_) != (_SNK_POWER_)) \
          { \
            /* Disalignment between PDO and DPM_SNKRequestedPower structure */ \
            _status = USBPD_ERROR; \
          }

/* Private variables ---------------------------------------------------------*/
/**
  * @brief  USBPD Port safty data
  */
uint8_t                  safety_contract = 0;
USBPD_PDO_TypeDef        safety_pdo;
USBPD_SNKRDO_TypeDef     safety_rdo;

/**
  * @brief  USBPD Port PDO Storage array declaration
  */

/**** PDO ****/
USBPD_PWR_Port_PDO_Storage_TypeDef PWR_Port_PDO_Storage[USBPD_PORT_COUNT];

/* Private function prototypes -----------------------------------------------*/
/* Functions to initialize Source PDOs */
uint32_t _PWR_SRCFixedPDO(float  _C_, float _V_,
                          USBPD_CORE_PDO_PeakCurr_TypeDef _PK_,
                          USBPD_CORE_PDO_DRDataSupport_TypeDef DRDSupport,
                          USBPD_CORE_PDO_USBCommCapable_TypeDef UsbCommCapable,
                          USBPD_CORE_PDO_ExtPowered_TypeDef ExtPower,
                          USBPD_CORE_PDO_USBSuspendSupport_TypeDef UsbSuspendSupport,
                          USBPD_CORE_PDO_DRPowerSupport_TypeDef DRPSupport);

uint32_t _PWR_SRCVariablePDO(float _MAXV_, float _MINV_, float _C_);

uint32_t _PWR_SRCBatteryPDO(float _MAXV_,float _MINV_,float _PWR_);
/* Functions to initialize Sink PDOs */

uint32_t _PWR_SNKFixedPDO(float  _C_, float _V_,
                          USBPD_CORE_PDO_DRDataSupport_TypeDef DRDSupport,
                          USBPD_CORE_PDO_USBCommCapable_TypeDef UsbCommCapable,
                          USBPD_CORE_PDO_ExtPowered_TypeDef ExtPower,
                          USBPD_CORE_PDO_HigherCapability_TypeDef HigherCapab,
                          USBPD_CORE_PDO_DRPowerSupport_TypeDef DRPSupport);

uint32_t _PWR_SNKVariablePDO(float  _MAXV_,float _MINV_,float _C_);

uint32_t _PWR_SNKBatteryPDO(float _MAXV_,float _MINV_,float _PWR_);
uint32_t _PWR_ProgrammablePowerSupplyAPDO(float _MAXC_,float _MINV_,float _MAXV_);

/* Private functions ---------------------------------------------------------*/
void USBPD_PWR_IF_MonitorSafety(void const *argument);

/**
  * @brief  Initialize structures and variables related to power board profiles
  *         used by Sink and Source, for all available ports.
  * @retval USBPD status
*/
USBPD_StatusTypeDef USBPD_PWR_IF_Init(void)
{
  USBPD_StatusTypeDef _status = USBPD_OK;
  uint32_t index;

  /* Reset PDO values */
  memset(PWR_Port_PDO_Storage, 0, sizeof(PWR_Port_PDO_Storage));

  for (index = 0; index < USBPD_MAX_NB_PDO; index++)
  {
    /* SRC PDO for Port 0 */
    PWR_Port_PDO_Storage[USBPD_PORT_0].SourcePDO.ListOfPDO[index] = PORT0_PDO_ListSRC[index];


    /* SNK PDO for Port 0 */
    PWR_Port_PDO_Storage[USBPD_PORT_0].SinkPDO.ListOfPDO[index] = PORT0_PDO_ListSNK[index];
  }
  PWR_Port_PDO_Storage[USBPD_PORT_0].SinkPDO.NumberOfPDO   = USBPD_NbPDO[0];
  PWR_Port_PDO_Storage[USBPD_PORT_0].SourcePDO.NumberOfPDO = USBPD_NbPDO[1];

  _status |= USBPD_PWR_IF_CheckUpdateSNKPower(USBPD_PORT_0);

  return _status;
}


USBPD_StatusTypeDef USBPD_PWR_IF_StartMonitoring(void)
{
  osThreadDef(SAFE, USBPD_PWR_IF_MonitorSafety, osPriorityLow, 0, 100);
  if(NULL == osThreadCreate(osThread(SAFE), NULL))
  {
    return USBPD_ERROR;
  }
  return USBPD_OK;
}

/**
  * @brief  Sets the required power profile, now it works only with Fixed ones
  * @param  PortNum Port number
  * @retval USBPD status
*/
USBPD_StatusTypeDef USBPD_PWR_IF_SetProfile(uint8_t PortNum)
{
  USBPD_StatusTypeDef      _ret = USBPD_ERROR;
  USBPD_PDO_TypeDef        _pdo;
  USBPD_SNKRDO_TypeDef     _rdo;
  static USBPD_SNKRDO_TypeDef _rdo_previous = {
                                                .d32 = 0
                                              };

  uint32_t _vbusTargetInmv = 0;
  uint32_t _PowerProfileSelected =  DPM_Ports[PortNum].DPM_RDOPosition - 1;
  uint8_t counter = 0;

  /* Check if valid port */
  /* Check if profile nb is valid for this port */
  if (( !USBPD_PORT_IsValid(PortNum) ) || (_PowerProfileSelected >= PWR_Port_PDO_Storage[PortNum].SourcePDO.NumberOfPDO)
     || (0 == PWR_Port_PDO_Storage[PortNum].SourcePDO.NumberOfPDO))
  {
    return _ret;
  }

  safety_pdo.d32 = _pdo.d32 = PWR_Port_PDO_Storage[PortNum].SourcePDO.ListOfPDO[_PowerProfileSelected];
  safety_rdo.d32 = _rdo.d32 = DPM_Ports[PortNum].DPM_RcvRequestDOMsg;

  switch(_pdo.GenericPDO.PowerObject)
  {
  case USBPD_CORE_PDO_TYPE_FIXED :
    {
      _vbusTargetInmv               = _pdo.SRCFixedPDO.VoltageIn50mVunits * 50;
      uint32_t _OperatingCurrent    = _rdo.FixedVariableRDO.OperatingCurrentIn10mAunits * 10;
      uint32_t _MaxOperatingCurrent = _rdo.FixedVariableRDO.MaxOperatingCurrent10mAunits * 10;

      _ret = BSP_PWR_VBUSSetVoltage_Fixed(PortNum, _vbusTargetInmv, _OperatingCurrent, _MaxOperatingCurrent) == PWR_OK ? USBPD_OK: USBPD_ERROR;
      break;
    }
  case USBPD_CORE_PDO_TYPE_VARIABLE :
    {
      _vbusTargetInmv               = _pdo.SRCVariablePDO.MinVoltageIn50mVunits * 50;
      uint32_t _vbusTargetMaxInmv   = _pdo.SRCVariablePDO.MaxVoltageIn50mVunits * 50;
      uint32_t _OperatingCurrent    = _rdo.FixedVariableRDO.OperatingCurrentIn10mAunits * 10;
      uint32_t _MaxOperatingCurrent = _rdo.FixedVariableRDO.MaxOperatingCurrent10mAunits * 10;

      _ret = BSP_PWR_VBUSSetVoltage_Variable(PortNum, _vbusTargetInmv, _vbusTargetMaxInmv, _OperatingCurrent, _MaxOperatingCurrent) == PWR_OK ? USBPD_OK: USBPD_ERROR;
      break;
    }
  case USBPD_CORE_PDO_TYPE_APDO :
    {
      _vbusTargetInmv      = _rdo.ProgRDO.OutputVoltageIn20mV * 20;
      uint32_t _OperatingCurrent    = _rdo.ProgRDO.OperatingCurrentIn50mAunits * 50;
      int32_t _delta = 0;
      if(_rdo.ProgRDO.ObjectPosition == _rdo_previous.ProgRDO.ObjectPosition)
      {
        _delta = (_rdo_previous.ProgRDO.OutputVoltageIn20mV - _rdo.ProgRDO.OutputVoltageIn20mV) * 20;
      }
      _ret = BSP_PWR_VBUSSetVoltage_APDO(PortNum, _vbusTargetInmv, _OperatingCurrent, _delta) == PWR_OK ? USBPD_OK: USBPD_ERROR;
      break;
    }
  case USBPD_CORE_PDO_TYPE_BATTERY :
  default :
    {
      /* not yet implemented */
      return _ret;
    }
  }

  /* save the previous rd, mainly for APDO management */
  _rdo_previous.d32 = _rdo.d32;

  if (_ret != USBPD_OK)
  {
    _rdo_previous.d32 = 0;
    return _ret;
  }
  else
  {
    /* before sending PS_READY if the power is at the expected level */
    int32_t _delta;
    uint32_t _vbusOriginInmv;
    uint8_t _exit = 0;

    uint32_t _precision;
    if (USBPD_CORE_PDO_TYPE_APDO == _pdo.GenericPDO.PowerObject)
      /* Set the power, precision should be 20mV */
      _precision = 20;
    else
      /* Set the power, the precision must be at 4% for a fixed PDO */
      _precision = (_vbusTargetInmv * 4) / 100;
    do {
      counter++;
      if (USBPD_CORE_PDO_TYPE_APDO == _pdo.GenericPDO.PowerObject)
        USBPD_DPM_WaitForTime(1);
      else
        USBPD_DPM_WaitForTime(5);
      _vbusOriginInmv = BSP_PWR_VBUSGetVoltage(PortNum);
      _delta = _vbusOriginInmv - _vbusTargetInmv;
      if((ABS(_delta)) < _precision)
      {
        _exit = 1;
      }
    }while((_exit == 0) && (counter < 22));
  }

  if (22 == counter)
    return USBPD_FAIL;
  else
  {
    safety_contract = 1;
    return USBPD_OK;
  }
}


/**
  * @brief  Resets the Power Board
  * @retval USBPD status
*/
USBPD_StatusTypeDef USBPD_PWR_IF_PowerResetGlobal(void)
{
  int i = 0;

  /* Resets all the ports */
  for(i = 0; i < USBPD_PORT_COUNT; i++)
  {
    USBPD_PWR_IF_PowerReset(i);
  }
  return USBPD_OK;
}

/**
  * @brief  Resets the Power on a specified port
  * @param  PortNum Port number
  * @retval USBPD status
*/
USBPD_StatusTypeDef USBPD_PWR_IF_PowerReset(uint8_t PortNum)
{
  /* check for valid port */
  if (!USBPD_PORT_IsValid(PortNum))
  {
    return USBPD_ERROR;
  }
  /* Put the usbpd port into ready to start the application */
  return USBPD_PWR_IF_InitPower(PortNum);
}

/**
  * @brief  Checks if the power on a specified port is ready
  * @param  PortNum Port number
  * @param  Vsafe   Vsafe status based on @ref USBPD_VSAFE_StatusTypeDef
  * @retval USBPD status
  */
USBPD_StatusTypeDef USBPD_PWR_IF_SupplyReady(uint8_t PortNum, USBPD_VSAFE_StatusTypeDef Vsafe)
{
  USBPD_StatusTypeDef status = USBPD_ERROR;

  /* check for valid port */
  if (!USBPD_PORT_IsValid(PortNum))
  {
    return USBPD_ERROR;
  }

  if (USBPD_VSAFE_0V == Vsafe)
  {
    /* Vsafe0V */
    status = ((BSP_PWR_VBUSGetVoltage(PortNum) < BSP_PWR_HIGH_VBUS_THRESHOLD)? USBPD_OK: USBPD_ERROR);
  }
  else
  {
    /* Vsafe5V */
    status = ((BSP_PWR_VBUSGetVoltage(PortNum) > BSP_PWR_HIGH_VBUS_THRESHOLD)? USBPD_OK: USBPD_ERROR);
  }

  return status;
}

/**
  * @brief  Enables VBUS power on a specified port
  * @param  PortNum Port number
  * @retval USBPD status
*/
USBPD_StatusTypeDef USBPD_PWR_IF_VBUSEnable(uint8_t PortNum)
{
  USBPD_StatusTypeDef _status = USBPD_ERROR;

  /* check for valid port */
  if (USBPD_PORT_IsValid(PortNum))
  {
    POWER_IF_TRACE(PortNum, "EN_VBUS", 7);
    /* Set the new state */
#ifdef _TRACE
    char str[20];
    sprintf(str, "CC:%d VCONN:%d", DPM_Params[PortNum].VconnCCIs, DPM_Params[PortNum].VconnStatus);
    POWER_IF_TRACE(PortNum, (uint8_t*)str, strlen(str));
#endif /* _TRACE */
    //_status = (USBPD_StatusTypeDef)HW_IF_PWR_Enable(PortNum, USBPD_ENABLE, DPM_Params[PortNum].VconnCCIs, DPM_Params[PortNum].VconnStatus, USBPD_PORTPOWERROLE_SRC);
  }
  return _status;
}

/**
  * @brief  Disbale VBUS/VCONN the power on a specified port
  * @param  PortNum Port number
  * @retval USBPD status
*/
USBPD_StatusTypeDef USBPD_PWR_IF_VBUSDisable(uint8_t PortNum)
{
  USBPD_StatusTypeDef _status = USBPD_ERROR;

  /* check for valid port */
  if (USBPD_PORT_IsValid(PortNum))
  {
    POWER_IF_TRACE(PortNum, "DIS VBUS", 8);
    /* Set the new state */
    /* Set the new state */
    //_status = (USBPD_StatusTypeDef)HW_IF_PWR_Enable(PortNum, USBPD_DISABLE, DPM_Params[PortNum].VconnCCIs, DPM_Params[PortNum].VconnStatus, USBPD_PORTPOWERROLE_SRC);
    DPM_Params[PortNum].VconnStatus = USBPD_FALSE;
  }

  /* Safety add On */
  if(0 == PortNum)
  {
    safety_contract = 0;
  }

  return  _status;
}

/**
  * @brief  Disable the SNK to stop current consumption
  * @param  PortNum Port number
  * @retval USBPD status
  */
USBPD_StatusTypeDef USBPD_PWR_IF_SNKDisable(uint8_t PortNum)
{
  USBPD_StatusTypeDef _status = USBPD_ERROR;

  /* check for valid port */
  if (USBPD_PORT_IsValid(PortNum))
  {
    /* Set the new state */
    _status = (USBPD_StatusTypeDef)HW_IF_PWR_Enable(PortNum, USBPD_DISABLE, DPM_Params[PortNum].VconnCCIs, DPM_Params[PortNum].VconnStatus, USBPD_PORTPOWERROLE_SNK);
  }
  return _status;
}

/**
  * @brief  Initialize power on a specified port
  * @param  PortNum Port number
  * @retval USBPD status
  */
USBPD_StatusTypeDef USBPD_PWR_IF_InitPower(uint8_t PortNum)
{
  return USBPD_OK;
}

/**
  * @brief  Checks if the power on a specified port is enabled
  * @param  PortNum Port number
  * @retval USBPD_ENABLE or USBPD_DISABLE
*/
USBPD_FunctionalState USBPD_PWR_IF_VBUSIsEnabled(uint8_t PortNum)
{
  /* Get the Status of the port */
  return USBPD_PORT_IsValid(PortNum) ? (USBPD_FunctionalState)HW_IF_PWR_VBUSIsEnabled(PortNum) : USBPD_DISABLE;
}

/**
  * @brief  Reads the voltage and the current on a specified port
  * @param  PortNum Port number
  * @param  pVoltage: The Voltage in mV
  * @param  pCurrent: The Current in mA
  * @retval USBPD_ERROR or USBPD_OK
*/
USBPD_StatusTypeDef USBPD_PWR_IF_ReadVA(uint8_t PortNum, uint16_t *pVoltage, uint16_t *pCurrent)
{
  /* check for valid port */
  if (!USBPD_PORT_IsValid(PortNum))
  {
    return USBPD_ERROR;
  }

  /* USBPD_OK if at least one pointer is not null, otherwise USBPD_ERROR */
  USBPD_StatusTypeDef ret = USBPD_ERROR;

  /* Get values from HW_IF */
  if (pVoltage != NULL)
  {
    *pVoltage = HW_IF_PWR_GetVoltage(PortNum);
    ret = USBPD_OK;
  }
  if (pCurrent != NULL)
  {
    *pCurrent = HW_IF_PWR_GetCurrent(PortNum);
    ret = USBPD_OK;
  }

  return ret;
}

/**
  * @brief  Create SRC Fixed PDO object
  * @param  _C_: Current in A
  * @param  _V_: voltage in V
  * @param  _PK_: The peak of current
  * @param  DRDSupport: Data Role Swap support indication
  * @param  UsbCommCapable: USB communications capability indication
  * @param  ExtPower: Port externally powered indication
  * @param  UsbSuspendSupport: USB Suspend support indication
  * @param  DRPSupport: Dual Role Power support indication
  * @retval PDO object value (expressed on u32)
*/
uint32_t _PWR_SRCFixedPDO(float  _C_, float _V_,
                          USBPD_CORE_PDO_PeakCurr_TypeDef _PK_,
                          USBPD_CORE_PDO_DRDataSupport_TypeDef DRDSupport,
                          USBPD_CORE_PDO_USBCommCapable_TypeDef UsbCommCapable,
                          USBPD_CORE_PDO_ExtPowered_TypeDef ExtPower,
                          USBPD_CORE_PDO_USBSuspendSupport_TypeDef UsbSuspendSupport,
                          USBPD_CORE_PDO_DRPowerSupport_TypeDef DRPSupport)
{
  USBPD_PDO_TypeDef pdo;

  pdo.d32 = 0;
  pdo.SRCFixedPDO.MaxCurrentIn10mAunits       = PWR_A_10MA(_C_);
  pdo.SRCFixedPDO.VoltageIn50mVunits          = PWR_V_50MV(_V_);
  pdo.SRCFixedPDO.PeakCurrent                 = _PK_;
  pdo.SRCFixedPDO.DataRoleSwap                = DRDSupport;
  pdo.SRCFixedPDO.USBCommunicationsCapable    = UsbCommCapable;
  pdo.SRCFixedPDO.ExternallyPowered           = ExtPower;
  pdo.SRCFixedPDO.USBSuspendSupported         = UsbSuspendSupport;
  pdo.SRCFixedPDO.DualRolePower               = DRPSupport;
  pdo.SRCFixedPDO.FixedSupply                 = USBPD_CORE_PDO_TYPE_FIXED;
  return pdo.d32;
}

/**
  * @brief  Create SRC Variable PDO object
  * @param  _MAXV_ Max voltage in V
  * @param  _MINV_ Min voltage in V
  * @param  _C_: Max current in A
  * @retval PDO object value (expressed on u32)
*/
uint32_t _PWR_SRCVariablePDO(float _MAXV_, float _MINV_, float _C_)
{
  USBPD_PDO_TypeDef pdo;

  pdo.d32 = 0;
  pdo.SRCVariablePDO.MaxCurrentIn10mAunits = PWR_A_10MA(_C_);
  pdo.SRCVariablePDO.MaxVoltageIn50mVunits = PWR_V_50MV(_MAXV_);
  pdo.SRCVariablePDO.MinVoltageIn50mVunits = PWR_V_50MV(_MINV_);
  pdo.SRCVariablePDO.VariableSupply        = USBPD_CORE_PDO_TYPE_VARIABLE;
  return pdo.d32;
}

/**
  * @brief  Create SRC Battery PDO object
  * @param  _MAXV_ Max voltage in V
  * @param  _MINV_ Min voltage in V
  * @param  _PWR_ Max allowable power in W
  * @retval PDO object value (expressed on u32)
*/
uint32_t _PWR_SRCBatteryPDO(float _MAXV_,float _MINV_,float _PWR_)
{
  USBPD_PDO_TypeDef pdo;

  pdo.d32 = 0;
  pdo.SRCBatteryPDO.MaxAllowablePowerIn250mWunits = PWR_W(_PWR_);
  pdo.SRCBatteryPDO.MinVoltageIn50mVunits         = PWR_V_50MV(_MINV_);
  pdo.SRCBatteryPDO.MaxVoltageIn50mVunits         = PWR_V_50MV(_MAXV_);
  pdo.SRCBatteryPDO.Battery                       = USBPD_CORE_PDO_TYPE_BATTERY;
  return pdo.d32;
}

/**
  * @brief  Create SNK Fixed PDO object
  * @param  _C_ Current in A
  * @param  _V_ voltage in V
  * @param  DRDSupport: Data Role Swap support indication
  * @param  UsbCommCapable: USB communications capability indication
  * @param  ExtPower: Port externally powered indication
  * @param  HigherCapab: Sink needs more than vSafe5V to provide full functionality indication
  * @param  DRPSupport: Dual Role Power support indication
  * @retval PDO object value (expressed on u32)
*/
uint32_t _PWR_SNKFixedPDO(float  _C_, float _V_,
                          USBPD_CORE_PDO_DRDataSupport_TypeDef DRDSupport,
                          USBPD_CORE_PDO_USBCommCapable_TypeDef UsbCommCapable,
                          USBPD_CORE_PDO_ExtPowered_TypeDef ExtPower,
                          USBPD_CORE_PDO_HigherCapability_TypeDef HigherCapab,
                          USBPD_CORE_PDO_DRPowerSupport_TypeDef DRPSupport)
{
  USBPD_PDO_TypeDef pdo;

  pdo.d32 = 0;
  pdo.SNKFixedPDO.OperationalCurrentIn10mAunits = PWR_A_10MA(_C_);
  pdo.SNKFixedPDO.VoltageIn50mVunits            = PWR_V_50MV(_V_);
  pdo.SNKFixedPDO.DataRoleSwap                  = DRDSupport;
  pdo.SNKFixedPDO.USBCommunicationsCapable      = UsbCommCapable;
  pdo.SNKFixedPDO.ExternallyPowered             = ExtPower;
  pdo.SNKFixedPDO.HigherCapability              = HigherCapab;
  pdo.SNKFixedPDO.DualRolePower                 = DRPSupport;
  pdo.SNKFixedPDO.FixedSupply                   = USBPD_CORE_PDO_TYPE_FIXED;

  return pdo.d32;
}

/**
  * @brief  Create SNK Variable PDO object
  * @param  _MAXV_ Max voltage in V
  * @param  _MINV_ Min voltage in V
  * @param  _C_: Max current in A
  * @retval PDO object value (expressed on u32)
*/
uint32_t _PWR_SNKVariablePDO(float  _MAXV_,float _MINV_,float _C_)
{
  USBPD_PDO_TypeDef pdo;

  pdo.d32 = 0;
  pdo.SRCVariablePDO.MaxCurrentIn10mAunits = PWR_A_10MA(_C_);
  pdo.SRCVariablePDO.MaxVoltageIn50mVunits = PWR_V_50MV(_MAXV_);
  pdo.SRCVariablePDO.MinVoltageIn50mVunits = PWR_V_50MV(_MINV_);
  pdo.SRCVariablePDO.VariableSupply        = USBPD_CORE_PDO_TYPE_VARIABLE;
  return pdo.d32;
}

/**
  * @brief  Create SNK Battery PDO object
  * @param  _MAXV_ Max voltage in V
  * @param  _MINV_ Min voltage in V
  * @param  _PWR_ Max allowable power in W
  * @retval PDO object value (expressed on u32)
*/
uint32_t _PWR_SNKBatteryPDO(float _MAXV_,float _MINV_,float _PWR_)
{
  USBPD_PDO_TypeDef pdo;

  pdo.d32 = 0;
  pdo.SRCBatteryPDO.MaxAllowablePowerIn250mWunits = PWR_W(_PWR_);
  pdo.SRCBatteryPDO.MinVoltageIn50mVunits         = PWR_V_50MV(_MINV_);
  pdo.SRCBatteryPDO.MaxVoltageIn50mVunits         = PWR_V_50MV(_MAXV_);
  pdo.SRCBatteryPDO.Battery                       = USBPD_CORE_PDO_TYPE_BATTERY;
  return pdo.d32;
}

/**
  * @brief  Create Programmable Power Supply APDO object
  * @param  _MAXC_ Max Current in A
  * @param  _MINV_ Min voltage in V
  * @param  _MAXV_ Max voltage in V
  * @retval PDO object value (expressed on u32)
*/
uint32_t _PWR_ProgrammablePowerSupplyAPDO(float _MAXC_,float _MINV_,float _MAXV_)
{
  USBPD_PDO_TypeDef apdo;

  apdo.d32                    = 0;
  apdo.SRCSNKAPDO.MaxCurrentIn50mAunits  = PWR_A_50MA(_MAXC_);   /*!< Maximum Current in 50mA increments       */
  apdo.SRCSNKAPDO.MinVoltageIn100mV      = PWR_V_100MV(_MINV_);  /*!< Minimum Voltage in 100mV increments      */
  apdo.SRCSNKAPDO.MaxVoltageIn100mV      = PWR_V_100MV(_MAXV_);  /*!< Maximum Voltage in 100mV increments      */
  apdo.SRCSNKAPDO.PPS_APDO               = USBPD_CORE_PDO_TYPE_APDO;
  return apdo.d32;
}

#if defined(_VCONN_SUPPORT)
/**
  * @brief  Enables the VConn on the port.
  * @param  PortNum Port number
  * @param  CC      Specifies the CCx to be selected based on @ref CCxPin_TypeDef structure
  * @retval USBPD status
  */
USBPD_StatusTypeDef USBPD_PWR_IF_Enable_VConn(uint8_t PortNum, CCxPin_TypeDef CC)
{
  PWR_StatusTypeDef _status = PWR_OK;
  POWER_IF_TRACE(PortNum, "VCONN ON", 8);
  _status = BSP_PWR_VCONNOn(PortNum, CC);
  if(PWR_OK == _status)
  {
    DPM_Params[PortNum].VconnStatus = USBPD_TRUE;
  }
  return _status == PWR_OK? USBPD_OK: USBPD_ERROR;
}

/**
  * @brief  Disable the VConn on the port.
  * @param  PortNum Port number
  * @param  CC      Specifies the CCx to be selected based on @ref CCxPin_TypeDef structure
  * @retval USBPD status
  */
USBPD_StatusTypeDef USBPD_PWR_IF_Disable_VConn(uint8_t PortNum, CCxPin_TypeDef CC)
{
  PWR_StatusTypeDef _status = PWR_OK;
  POWER_IF_TRACE(PortNum, "VCONN OFF", 9);
  _status = BSP_PWR_VCONNOff(PortNum, CC);
  if(PWR_OK == _status)
  {
    DPM_Params[PortNum].VconnStatus = USBPD_FALSE;
  }
  return _status == PWR_OK? USBPD_OK:USBPD_ERROR;
}
#endif /* _VCONN_SUPPORT */

/**
  * @brief  Allow PDO data reading from PWR_IF storage.
  * @param  PortNum Port number
  * @param  DataId Type of data to be read from PWR_IF
  *         This parameter can be one of the following values:
  *           @arg @ref USBPD_CORE_DATATYPE_SRC_PDO Source PDO reading requested
  *           @arg @ref USBPD_CORE_DATATYPE_SNK_PDO Sink PDO reading requested
  * @param  Ptr Pointer on address where PDO values should be written (u8 pointer)
  * @param  Size Pointer on nb of u32 written by PWR_IF (nb of PDOs)
  * @retval None
  */
void USBPD_PWR_IF_GetPortPDOs(uint8_t PortNum, USBPD_CORE_DataInfoType_TypeDef DataId, uint8_t *Ptr, uint32_t *Size)
{
  uint32_t   nbpdo, index, nb_valid_pdo = 0;
  uint32_t   *ptpdoarray = NULL;
  USBPD_PDO_TypeDef pdo_first;
  USBPD_PDO_TypeDef pdo;

  /* Check if valid port */
  if (USBPD_PORT_IsValid(PortNum))
  {
    /* According to type of PDO to be read, set pointer on values and nb of elements */
    switch (DataId)
    {
    case USBPD_CORE_DATATYPE_SRC_PDO:
      nbpdo = PWR_Port_PDO_Storage[PortNum].SourcePDO.NumberOfPDO;
      ptpdoarray = PWR_Port_PDO_Storage[PortNum].SourcePDO.ListOfPDO;
      /* Save the 1st PDO */
      pdo_first.d32 = *ptpdoarray;
      /* Reset unchunked bit if current revision is PD2.0*/
      if (USBPD_SPECIFICATION_REV2 == DPM_Params[PortNum].PE_SpecRevision)
      {
        pdo_first.SRCFixedPDO.UnchunkedExtendedMessage  = USBPD_PDO_SRC_FIXED_UNCHUNK_NOT_SUPPORTED;
      }
      break;
    case USBPD_CORE_DATATYPE_SNK_PDO:
      nbpdo = PWR_Port_PDO_Storage[PortNum].SinkPDO.NumberOfPDO;
      ptpdoarray = PWR_Port_PDO_Storage[PortNum].SinkPDO.ListOfPDO;
      /* Save the 1st PDO */
      pdo_first.d32 = *ptpdoarray;
      /* Reset FRS bit if current revision is PD2.0*/
      if (USBPD_SPECIFICATION_REV2 == DPM_Params[PortNum].PE_SpecRevision)
      {
        pdo_first.SNKFixedPDO.FastRoleSwapRequiredCurrent = USBPD_PDO_SNK_FIXED_FRS_NOT_SUPPORTED;
      }
      break;
    default:
      nbpdo = 0;
      break;
    }

    /* Copy PDO data in output buffer */
    for (index = 0; index < nbpdo; index++)
    {
      pdo.d32 = *ptpdoarray;
      /* Copy only PDO (and not APDO in case of current revision is PD2.0) */
      if ((USBPD_SPECIFICATION_REV2 == DPM_Params[PortNum].PE_SpecRevision)
       && (pdo.GenericPDO.PowerObject == USBPD_CORE_PDO_TYPE_APDO))
      {
      }
      else
      {
        /* Copy 1st PDO as potentially FRS or UNCHUNKED bits have been reset */
        if (0 == index)
        {
          (void)memcpy(Ptr, (uint8_t*)&pdo_first.d32, 4u);
        }
        else
        {
          (void)memcpy((Ptr + (nb_valid_pdo * 4u)), (uint8_t*)ptpdoarray, 4u);
        }
        nb_valid_pdo++;
      }
      ptpdoarray++;
    }
    /* Set nb of read PDO (nb of u32 elements); */
    *Size = nb_valid_pdo;
  }
}

/**
  * @brief  Find out SRC PDO pointed out by a position provided in a Request DO (from Sink).
  * @param  PortNum Port number
  * @param  RdoPosition RDO Position in list of provided PDO
  * @param  Pdo Pointer on PDO value pointed out by RDO position (u32 pointer)
  * @retval Status of search
  *         USBPD_OK : Src PDO found for requested DO position (output Pdo parameter is set)
  *         USBPD_FAIL : Position is not compliant with current Src PDO for this port (no corresponding PDO value)
  */
USBPD_StatusTypeDef USBPD_PWR_IF_SearchRequestedPDO(uint8_t PortNum, uint32_t RdoPosition, uint32_t *Pdo)
{
  if((RdoPosition == 0) || (RdoPosition > PWR_Port_PDO_Storage[PortNum].SourcePDO.NumberOfPDO))
  {
    /* Invalid PDO index */
    return USBPD_FAIL;
  }

  *Pdo = PWR_Port_PDO_Storage[PortNum].SourcePDO.ListOfPDO[RdoPosition - 1];
  return USBPD_OK;
}


/**
  * @brief  the function is called in case of critical issue is detected to switch in safety mode.
  * @param  None
  * @retval None
  */
void USBPD_PWR_IF_Alarm()
{
  /* Disable the power on SRC */
  BSP_PWR_VBUSOff(0);

  /* Remove the resitor */
  USBPD_CAD_EnterErrorRecovery(0);
#if USBPD_PORT_COUNT == 2
  USBPD_CAD_EnterErrorRecovery(1);
#endif /* USBPD_PORT_COUNT == 2 */

  /* Stop RTOS scheduling */
  vTaskSuspendAll ();

  /* Display the error */
  //DEMO_Display_Error(0);

  /* disable all interrupt to lock the system */
  __disable_irq();

  while(1);
}

/**
  * @brief  Function to check validity between SNK PDO and power user settings
  * @param  PortNum Port number
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_PWR_IF_CheckUpdateSNKPower(uint8_t PortNum)
{
  USBPD_StatusTypeDef _status = USBPD_OK;
  USBPD_PDO_TypeDef pdo;
  uint32_t _max_power = 0;
  uint16_t _voltage = 0, _current = 0, _power = 0;
  uint16_t _min_voltage = 0xFFFF, _max_voltage = 0, _max_current = 0;

  for (uint32_t _index = 0; _index < PWR_Port_PDO_Storage[PortNum].SinkPDO.NumberOfPDO; _index++)
  {
    pdo.d32 = PWR_Port_PDO_Storage[PortNum].SinkPDO.ListOfPDO[_index];
    switch (pdo.GenericPDO.PowerObject)
    {
      case USBPD_CORE_PDO_TYPE_FIXED:    /*!< Fixed Supply PDO                             */
        _voltage = PWR_DECODE_50MV(pdo.SNKFixedPDO.VoltageIn50mVunits);
        _PWR_UPDATE_VOLTAGE_MIN(_voltage, _min_voltage);
        _PWR_UPDATE_VOLTAGE_MAX(_voltage, _max_voltage);
        _current = PWR_DECODE_10MA(pdo.SNKFixedPDO.OperationalCurrentIn10mAunits);
        _PWR_UPDATE_CURRENT_MAX(_current, _max_current);
        break;
      case USBPD_CORE_PDO_TYPE_BATTERY:  /*!< Battery Supply PDO                           */
        _voltage = PWR_DECODE_50MV(pdo.SNKBatteryPDO.MinVoltageIn50mVunits);
        _PWR_UPDATE_VOLTAGE_MIN(_voltage, _min_voltage);
        _voltage = PWR_DECODE_50MV(pdo.SNKBatteryPDO.MaxVoltageIn50mVunits);
        _PWR_UPDATE_VOLTAGE_MAX(_voltage, _max_voltage);
        _power = PWR_DECODE_MW(pdo.SNKBatteryPDO.OperationalPowerIn250mWunits);
        _PWR_UPDATE_POWER_MAX(_power, _max_power);
        break;
      case USBPD_CORE_PDO_TYPE_VARIABLE: /*!< Variable Supply (non-battery) PDO            */
        _voltage = PWR_DECODE_50MV(pdo.SNKVariablePDO.MinVoltageIn50mVunits);
        _PWR_UPDATE_VOLTAGE_MIN(_voltage, _min_voltage);
        _voltage = PWR_DECODE_50MV(pdo.SNKVariablePDO.MaxVoltageIn50mVunits);
        _PWR_UPDATE_VOLTAGE_MAX(_voltage, _max_voltage);
        _current = PWR_DECODE_10MA(pdo.SNKVariablePDO.OperationalCurrentIn10mAunits);
        _PWR_UPDATE_CURRENT_MAX(_current, _max_current);
        break;
      case USBPD_CORE_PDO_TYPE_APDO:     /*!< Augmented Power Data Object (APDO)           */
        _voltage = PWR_DECODE_100MV(pdo.SRCSNKAPDO.MinVoltageIn100mV);
        _PWR_UPDATE_VOLTAGE_MIN(_voltage, _min_voltage);
        _voltage = PWR_DECODE_100MV(pdo.SRCSNKAPDO.MaxVoltageIn100mV);
        _PWR_UPDATE_VOLTAGE_MAX(_voltage, _max_voltage);
        _current = PWR_DECODE_50MA(pdo.SRCSNKAPDO.MaxCurrentIn50mAunits);
        _PWR_UPDATE_CURRENT_MAX(_current, _max_current);
        break;
      default:
        break;
    }
  }

  _PWR_CHECK_VOLTAGE_MIN(_min_voltage, DPM_USER_Settings[PortNum].DPM_SNKRequestedPower.MinOperatingVoltageInmVunits);
  _PWR_CHECK_VOLTAGE_MAX(_max_voltage, DPM_USER_Settings[PortNum].DPM_SNKRequestedPower.MaxOperatingVoltageInmVunits);
  _PWR_CHECK_CURRENT_MAX(_max_current, DPM_USER_Settings[PortNum].DPM_SNKRequestedPower.MaxOperatingCurrentInmAunits);
  _max_power = (_max_voltage * _max_current) / 1000;
  _PWR_CHECK_POWER_MAX(_max_power, DPM_USER_Settings[PortNum].DPM_SNKRequestedPower.MaxOperatingPowerInmWunits);

  return _status;
}

/**
  * @brief  Manage the Safety to avoid material issue, it the current execed the limitation an alarm is set
  * @param  None
  * @retval None
  */
void USBPD_PWR_IF_MonitorSafety(void const *argument)
{
  uint32_t _MaxOperatingCurrent = 1000;
  int32_t isense_safety[2];

  for(;;) /* infinite loop to monitore the current from port 0 */
  {
    /* Monitor the current for the SRC power PORT0 */
    isense_safety[USBPD_PORT_0] = BSP_PWR_VBUSGetCurrent(USBPD_PORT_0);

    if ((DPM_Params[USBPD_PORT_0].PE_IsConnected == USBPD_TRUE) && (USBPD_PORTPOWERROLE_SRC == DPM_Params[USBPD_PORT_0].PE_PowerRole)
        && (1 == safety_contract))
    {
      /* check if we are aligned with the limitation done by the SINK */
      switch(safety_pdo.GenericPDO.PowerObject)
      {
      case USBPD_CORE_PDO_TYPE_FIXED :
      case USBPD_CORE_PDO_TYPE_VARIABLE :
        {
          _MaxOperatingCurrent = safety_rdo.FixedVariableRDO.MaxOperatingCurrent10mAunits * 10;
          break;
        }
#if defined(USBPD_REV30_SUPPORT)
      case USBPD_CORE_PDO_TYPE_APDO :
        {
          _MaxOperatingCurrent    = safety_rdo.ProgRDO.OperatingCurrentIn50mAunits * 50;
          break;
        }
#endif
      default :
        {
        }
      }
    }

    if(ABS(isense_safety[USBPD_PORT_0]) > (_MaxOperatingCurrent*1.1 +100))
    {
      USBPD_PWR_IF_Alarm();
    }

#if USBPD_PORT_COUNT == 2
    /* Monitor the current for the SNK power PORT1 */
    isense_safety[USBPD_PORT_1] = BSP_PWR_VBUSGetCurrent(USBPD_PORT_1);

    /* check if we are aligned with the limitation done by the SINK */
    if (ABS(isense_safety[USBPD_PORT_1]) > 1000)
    {
      USBPD_PWR_IF_Alarm();
    }
#endif

    osDelay(1);
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
