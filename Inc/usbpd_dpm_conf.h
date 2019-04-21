/**
  ******************************************************************************
  * @file    usbpd_dpm_conf.h
  * @author  MCD Application Team
  * @brief   Header file for stack/application settings file
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

#ifndef __USBPD_DPM_CONF_H_
#define __USBPD_DPM_CONF_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbpd_pdo_defs.h"
#include "usbpd_vdm_user.h"
#include "usbpd_dpm_user.h"

/* Define   ------------------------------------------------------------------*/
/* Define VID, PID,... manufacturer parameters */
#define USBPD_VID (0x0483u)     /*!< Vendor ID (assigned by the USB-IF)                     */
#define USBPD_PID (0x0002u)     /*!< Product ID (assigned by the manufacturer)              */
#define USBPD_XID (0xF0000003u) /*!< Value provided by the USB-IF assigned to the product   */

/* Exported typedef ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#ifndef __USBPD_DPM_CORE_C
extern USBPD_SettingsTypeDef            DPM_Settings[USBPD_PORT_COUNT];
extern USBPD_USER_SettingsTypeDef       DPM_USER_Settings[USBPD_PORT_COUNT];
#else /* __USBPD_DPM_CORE_C */
USBPD_SettingsTypeDef       DPM_Settings[USBPD_PORT_COUNT] =
{
  {
    .PE_SupportedSOP = USBPD_SUPPORTED_SOP_SOP | USBPD_SUPPORTED_SOP_SOP1 | USBPD_SUPPORTED_SOP_SOP2, /* Supported SOP : SOP, SOP' SOP" SOP'Debug SOP"Debug      */
    .PE_SpecRevision = USBPD_SPECIFICATION_REV3,/* spec revision value                                     */
    .PE_DefaultRole = USBPD_PORTPOWERROLE_SNK,  /* Default port role                                       */
    .PE_RoleSwap = USBPD_TRUE,                  /* support port role swap                                  */
    .PE_VDMSupport = USBPD_TRUE,
    .PE_RespondsToDiscovSOP = USBPD_TRUE,      /*!< Can respond successfully to a Discover Identity */
    .PE_AttemptsDiscovSOP = USBPD_TRUE,        /*!< Can send a Discover Identity */
    .PE_PingSupport = USBPD_FALSE,             /* support Ping (only for PD3.0)                           */
    .PE_CapscounterSupport = USBPD_FALSE,       /* support caps counter                                    */
    .CAD_RoleToggle = USBPD_TRUE,               /* cad role toggle                                         */
    .CAD_TryFeature = USBPD_FALSE,              /* cad try feature                                         */
    .CAD_AccesorySupport = USBPD_FALSE,         /* cas accessory support                                   */
    .PE_PD3_Support.d =                           /*!< PD3 SUPPORT FEATURE                                              */
    {
      .PE_UnchunkSupport                = USBPD_FALSE,  /*!< Unchunked mode Support                */
      .PE_FastRoleSwapSupport           = USBPD_FALSE,  /* support fast role swap only spec revsion 3.0            */
      .Is_GetPPSStatus_Supported        = USBPD_TRUE,         /*!< PPS message supported or not by DPM */
      .Is_SrcCapaExt_Supported          = USBPD_TRUE,   /*!< Source_Capabilities_Extended message supported or not by DPM */
      .Is_Alert_Supported               = USBPD_FALSE,  /*!< Alert message supported or not by DPM */
      .Is_GetStatus_Supported           = USBPD_FALSE,  /*!< Status message supported or not by DPM (Is_Alert_Supported should be enabled) */
      .Is_GetManufacturerInfo_Supported = USBPD_FALSE,  /*!< Manufacturer_Info message supported or not by DPM */
      .Is_GetCountryCodes_Supported     = USBPD_FALSE,  /*!< Country_Codes message supported or not by DPM */
      .Is_GetCountryInfo_Supported      = USBPD_FALSE,  /*!< Country_Info message supported or not by DPM */
      .Is_SecurityRequest_Supported     = USBPD_FALSE,  /*!< Security_Response message supported or not by DPM */
      .Is_FirmUpdateRequest_Supported   = USBPD_FALSE,  /*!< Firmware update response message supported by PE */
      .Is_SnkCapaExt_Supported          = USBPD_FALSE,  /*!< Sink_Capabilities_Extended message supported or not by DPM */
    },
    .CAD_DefaultResistor = vRp_3_0A,
    .CAD_SRCToggleTime = 40,                    /* uint8_t CAD_SRCToggleTime; */
    .CAD_SNKToggleTime = 40,                    /* uint8_t CAD_SNKToggleTime; */
  }
};

USBPD_USER_SettingsTypeDef       DPM_USER_Settings[USBPD_PORT_COUNT] =
{
  {
    .DPM_SNKRequestedPower =                        /*!< Requested Power by the sink board                                    */
    {
      .MaxOperatingCurrentInmAunits = USBPD_CORE_PDO_SNK_FIXED_MAX_CURRENT,
      .OperatingVoltageInmVunits    = USBPD_BOARD_REQUESTED_VOLTAGE_MV,
      .MaxOperatingVoltageInmVunits = USBPD_BOARD_MAX_VOLTAGE_MV,
      .MinOperatingVoltageInmVunits = USBPD_BOARD_MIN_VOLTAGE_MV_PORT0,
      .OperatingPowerInmWunits      = (USBPD_CORE_PDO_SNK_FIXED_MAX_CURRENT * USBPD_BOARD_REQUESTED_VOLTAGE_MV)/1000,
      .MaxOperatingPowerInmWunits   = (USBPD_CORE_PDO_SNK_FIXED_MAX_CURRENT * USBPD_BOARD_MAX_VOLTAGE_MV)/1000
    },
    .PE_DataSwap = USBPD_TRUE,                  /* support data swap                                       */
    .PE_VconnSwap = USBPD_TRUE,                 /* support VCONN swap                                  */
    .DPM_SRCExtendedCapa =                        /*!< SRC Extended Capability           */
      {
        .VID = USBPD_VID,      /*!< Vendor ID (assigned by the USB-IF)                   */
        .PID = USBPD_PID,      /*!< Product ID (assigned by the manufacturer)            */
        .XID = USBPD_XID,      /*!< Value provided by the USB-IF assigned to the product */
        .FW_revision = 1,      /*!< Firmware version number                              */
        .HW_revision = 2,      /*!< Hardware version number                              */
        .Voltage_regulation= 0,/*!< Voltage Regulation                                   */
        .Holdup_time = 0,      /*!< Holdup Time                                          */
        .Compliance = 0,       /*!< Compliance                                           */
        .TouchCurrent = 0,     /*!< Touch Current                                        */
        .PeakCurrent1 = 0,     /*!< Peak Current1                                        */
        .PeakCurrent2 = 0,     /*!< Peak Current2                                        */
        .PeakCurrent3 = 0,     /*!< Peak Current3                                        */
        .Touchtemp = 0,        /*!< Touch Temp                                           */
        .Source_inputs = 0,    /*!< Source Inputs                                        */
        .NbBatteries = 0,      /*!< Number of Batteries/Battery Slots                    */
        .SourcePDP = (uint8_t)USBPD_PDP_SRC_IN_WATTS,       /*!< Source PDP  5V*3A                                    */
      },

    .DPM_ManuInfoPort =                      /*!< Manufacturer information used for the port            */
    {
      .VID = USBPD_VID,                      /*!< Vendor ID (assigned by the USB-IF)        */
      .PID = USBPD_PID,                      /*!< Product ID (assigned by the manufacturer) */
      .ManuString = "STMicroelectronics",    /*!< Vendor defined byte array                 */
    },
  },
};
#endif /* !__USBPD_DPM_CORE_C */

/* Exported define -----------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __USBPD_DPM_CONF_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
