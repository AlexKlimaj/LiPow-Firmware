/**
  ******************************************************************************
  * @file    gui_api.c
  * @author  MCD Application Team
  * @brief   GUI_API provides functions associated with answers the MCU can send
             to the computer via UART.
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

#if defined(_GUI_INTERFACE)

#include <stdlib.h>
#include "usbpd_def.h"
#include "gui_api.h"
#include "bsp_gui.h"
#include "data_struct_tlv.h"
#include "usbpd_core.h"
#include "usbpd_dpm_core.h"
#include "usbpd_dpm_user.h"
#include "usbpd_dpm_conf.h"
#include "tracer_emb.h"
#ifdef _VDM
#include "usbpd_vdm_user.h"
#endif /* _VDM */
#include "usbpd_pdo_defs.h"
#ifndef USBPD_TCPM_MODULE_ENABLED
#include "usbpd_hw_if.h"
#else
#include "usbpd_tcpci.h"
#endif
#include "cmsis_os.h"
#include "string.h"
#if defined(_SNK) || defined(_DRP)
#include "usbpd_pwr_if.h"
#endif /* _SNK) || _DRP */

/* generic hal function valid for all stm32 */
extern uint32_t HAL_GetTick(void);
extern void HAL_NVIC_SystemReset(void);

/** @addtogroup STM32_USBPD_LIBRARY
  * @{
  */

/** @addtogroup USBPD_GUI_API
  * @{
  */

/* Private enums -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @defgroup USBPD_GUI_API_Private_Defines USBPD GUI API Private Defines
  * @{
  */
#define GUI_PORT_BIT_POSITION     5u /* Bit position of port number in TAG id */

/**
  * @brief  USB PD GUI REJECT REASON
  */
typedef enum
{
  GUI_REJ_DPM_REJECT                         = 0x00, /*<! Use in DPM_CONFIG_REJ */
  GUI_REJ_DPM_BUSY                           = 0x01, /*<! Use in DPM_CONFIG_REJ */
  GUI_REJ_DPM_TIMEOUT                        = 0x02, /*<! Use in DPM_CONFIG_REJ */
  GUI_REJ_DPM_INVALID_MESSAGE                = 0x03, /*<! Use in DPM_CONFIG_REJ (incorrect param in DPM_CONFIG_GET) */
  GUI_REJ_DPM_OUT_OF_RANGE                   = 0x04, /*<! Use in DPM_CONFIG_REJ */
  GUI_REJ_DPM_INVALID_PORT_NUMBER            = 0x05, /*<! Use in DPM_CONFIG_REJ */
  GUI_REJ_DPM_NOT_READY                      = 0x06, /*<! Use in DPM_MESSAGE_REJ */
} USBPD_GUI_Reject_Reason;

/**
  * @brief  USB PD GUI REGISTER TCPC
  */
#define GUI_REG_NUMBER_OF_REGISTERS          39
typedef enum
{
  GUI_REG_VENDOR_ID                          = 0x00, /*<! VENDOR_ID */
  GUI_REG_PRODUCT_ID                         = 0x02, /*<! PRODUCT_ID */
  GUI_REG_DEVICE_ID                          = 0x04, /*<! DEVICE_ID */
  GUI_REG_USBTYPEC_REV                       = 0x06, /*<! USBTYPEC_REV */
  GUI_REG_USBPD_REV_VER                      = 0x08, /*<! USBPD_REV_VER */
  GUI_REG_USBPD_INTERFACE_REV                = 0x0A, /*<! USBPD_INTERFACE_REV */
  GUI_REG_ALERT                              = 0x10, /*<! ALERT */
  GUI_REG_ALERT_MASK                         = 0x12, /*<! ALERT_MASK */
  GUI_REG_POWER_STATUS_MASK                  = 0x14, /*<! POWER_STATUS_MASK */
  GUI_REG_FAULT_STATUS_MASK                  = 0x15, /*<! FAULT_STATUS_MASK */
  GUI_REG_CONFIG_STANDARD_OUTPUT             = 0x18, /*<! CONFIG_STANDARD_OUTPUT */
  GUI_REG_TCPC_CONTROL                       = 0x19, /*<! TCPC_CONTROL */
  GUI_REG_ROLE_CONTROL                       = 0x1A, /*<! ROLE_CONTROL */
  GUI_REG_FAULT_CONTROL                      = 0x1B, /*<! FAULT_CONTROL */
  GUI_REG_POWER_CONTROL                      = 0x1C, /*<! POWER_CONTROL */
  GUI_REG_CC_STATUS                          = 0x1D, /*<! CC_STATUS */
  GUI_REG_POWER_STATUS                       = 0x1E, /*<! POWER_STATUS */
  GUI_REG_FAULT_STATUS                       = 0x1F, /*<! FAULT_STATUS */
  GUI_REG_COMMAND                            = 0x23, /*<! COMMAND */
  GUI_REG_DECIVE_CAP1                        = 0x24, /*<! DECIVE_CAP1 */
  GUI_REG_DECIVE_CAP2                        = 0x26, /*<! DECIVE_CAP2 */
  GUI_REG_STD_INPUT_CAP                      = 0x28, /*<! STD_INPUT_CAP */
  GUI_REG_STD_OUTPUT_CAP                     = 0x29, /*<! STD_OUTPUT_CAP */
  GUI_REG_MSG_HEADER_INFO                    = 0x2E, /*<! MSG_HEADER_INFO */
  GUI_REG_RX_DETECT                          = 0x2F, /*<! RX_DETECT */
  GUI_REG_RX_BYTE_COUNT                      = 0x30, /*<! RX_BYTE_COUNT */
  GUI_REG_RX_BUFFER_FRAME_TYPE               = 0x31, /*<! RX_BUFFER_FRAME_TYPE */
  GUI_REG_RX_HEADER                          = 0x32, /*<! RX_HEADER */
  GUI_REG_RX_DATA                            = 0x34, /*<! RX_DATA */
  GUI_REG_TRANSMIT                           = 0x50, /*<! TRANSMIT */
  GUI_REG_TX_BYTE_COUNT                      = 0x51, /*<! TX_BYTE_COUNT */
  GUI_REG_TX_HEADER                          = 0x52, /*<! TX_HEADER */
  GUI_REG_TX_DATA                            = 0x54, /*<! TX_DATA */
  GUI_REG_VBUS_VOLTAGE                       = 0x70, /*<! VBUS_VOLTAGE */
  GUI_REG_VBUS_SINK_DISCONNECT_THRESHOLD     = 0x72, /*<! VBUS_SINK_DISCONNECT_THRESHOLD */
  GUI_REG_VBUS_STOP_DISCHARGE_THRESHOLD      = 0x74, /*<! VBUS_STOP_DISCHARGE_THRESHOLD */
  GUI_REG_VBUS_VOLTAGE_ALARM_HI_CFG          = 0x76, /*<! VBUS_VOLTAGE_ALARM_HI_CFG */
  GUI_REG_VBUS_VOLTAGE_ALARM_LO_CFG          = 0x78, /*<! VBUS_VOLTAGE_ALARM_LO_CFG */
  GUI_REG_VENDOR_DATA                        = 0x80, /*<! VENDOR_DATA */
} USBPD_GUI_RegisterTCPC;

/**
  * @brief  USB PD GUI TAG PARAM
  */
typedef enum
{
  GUI_PARAM_SOP                              = 0x00, /*<! SOP support */
  GUI_PARAM_SOP1                             = 0x00, /*<! SOP prime support */
  GUI_PARAM_SOP2                             = 0x00, /*<! SOP second support */
  GUI_PARAM_FASTROLESWAP                     = 0x02, /*<! It enables the FRS */
  GUI_PARAM_DATAROLESWAP                     = 0x03, /*<! Data role swap */
  GUI_PARAM_DEFAULTPOWERROLE                 = 0x04, /*<! DRP on PE side but no toggle on CAD (dependancy with CADRoleToggle) */
  GUI_PARAM_DRP_SUPPORT                      = 0x05, /*<! DRP support (Power Role swap support) */
  GUI_PARAM_CADROLETOGGLE                    = 0x06, /*<! CAD Role Toggle */
  GUI_PARAM_PE_SCAP_HR                       = 0x07, /*<! Hard reset after N_SOURCE_CAPS message sending tentatives */
  GUI_PARAM_VCONNSWAP                        = 0x08, /*<! It enables or disables the VCONN swap capability */
  GUI_PARAM_VDM_SUPPORT                      = 0x09, /*<! Indicate if VDM messages support is enabled or not */
  GUI_PARAM_PING_SUPPORT                     = 0x0A, /*<! Indicate if Ping message support is enabled or not */
  GUI_PARAM_PPS_SUPPORT                      = 0x0B, /*<! PPS message supported by PE  */
  GUI_PARAM_SRCCAPAEXTSUPPORT                = 0x0B, /*<! Source_Capabilities_Extended message supported by PE  */
  GUI_PARAM_ALERTSUPPORT                     = 0x0B, /*<! Alert message supported by PE  */
  GUI_PARAM_STATUSSUPPORT                    = 0x0B, /*<! Status message supported by PE (Is_Alert_Supported should be enabled)  */
  GUI_PARAM_BATTERYSTATUSSUPPORT             = 0x0B, /*<! Battery_Status message supported by PE  */
  GUI_PARAM_BATTERYCAPASUPPORT               = 0x0B, /*<! Battery_Capabilities message supported by PE  */
  GUI_PARAM_MANUINFOSUPPORT                  = 0x0B, /*<! Manufacturer_Info message supported by PE  */
  GUI_PARAM_COUNTRYCODESSUPPORT              = 0x0B, /*<! Country_Codes message supported by PE  */
  GUI_PARAM_COUNTRYINFOSUPPORT               = 0x0B, /*<! Country_Info message supported by PE  */
  GUI_PARAM_SECURESPONSESUPPORT              = 0x0B, /*<! Security_Response message supported by PE  */
  GUI_PARAM_SNK_PDO                          = 0x0C, /*<! maximun 7 * 32bit */
  GUI_PARAM_SRC_PDO                          = 0x0D, /*<! maximun 7 * 32bit */
  GUI_PARAM_TDRP                             = 0x0E, /*<! The period a DRP shall complete a Source to Sink and back advertisement */
  GUI_PARAM_DCSRC_DRP                        = 0x0F, /*<! The percent of time that a DRP shall advertise Source during tDRP (in %) */
  GUI_PARAM_RESPONDS_TO_DISCOV_SOP           = 0x10, /*<! UUT can respond successfully to a Discover Identity command from its port partner using SOP. */
  GUI_PARAM_ATTEMPTS_DISCOV_SOP              = 0x11, /*<! UUT can send a Discover Identity command to its port partner using SOP */
  GUI_PARAM_XID_SOP                          = 0x12, /*<! A decimal number assigned by USB-IF before certification */
  GUI_PARAM_DATA_CAPABLE_AS_USB_HOST_SOP     = 0x13, /*<! Indicates whether the UUT is capable of enumerating USB Host */
  GUI_PARAM_DATA_CAPABLE_AS_USB_DEVICE_SOP   = 0x14, /*<! Indicates whether the UUT is capable of enumerating as a USB Device */
  GUI_PARAM_PRODUCT_TYPE_SOP                 = 0x15, /*<! What is the product type of the UUT?  */
  GUI_PARAM_MODAL_OPERATION_SUPPORTED_SOP    = 0x16, /*<! Does the product support Modes?  */
  GUI_PARAM_USB_VID_SOP                      = 0x17, /*<! A unique 16-bit number, assigned to the Vendor by USB-IF. */
  GUI_PARAM_PID_SOP                          = 0x18, /*<! A unique number assigned by the Vendor ID holder identifying the product */
  GUI_PARAM_BCDDEVICE_SOP                    = 0x19, /*<! A unique number assigned by the Vendor ID holder containing identity information relevant to the release version of the product.  */
  GUI_PARAM_MEASUREREPORTING                 = 0x1A, /*<! 0x00: Disable Measure reporting
                                                          (1 << 8) | (tr between 0 & 127): Enable Measure reporting every tr x 40 ms (if xx = 0 means only 1 indication to report to GUI) */
  GUI_PARAM_MANUINFOPORT_VID                 = 0x1B, /*<! Vendor ID (assigned by the USB-IF)) used by Manufacturer info  set for port */
  GUI_PARAM_MANUINFOPORT_PID                 = 0x1B, /*<! Product ID (assigned by the manufacturer) used by Manufacturer info  set for port */
  GUI_PARAM_MANUINFOPORT_STRING              = 0x1B, /*<! Vendor string used by Manufacturer info  set for port */
  GUI_PARAM_NB_TAG                           = (GUI_PARAM_MANUINFOPORT_STRING + 1), /*<! Number max of param */
  GUI_PARAM_ALL                              = 0xFFu,
} USBPD_GUI_Tag_Param;

/**
  * @brief  USB PD GUI TAG INDICATION
  */
typedef enum
{
  GUI_IND_NUMBEROFRCVSNKPDO                  = 0x00, /*<! The number of received Sink Power Data Objects from port Partner (when Port partner is a Sink or a DRP port) */
  GUI_IND_RDOPOSITION                        = 0x01, /*<!  RDO Position of requested DO in Source list of capabilities */
  GUI_IND_LISTOFRCVSRCPDO                    = 0x02, /*<! The list of received Source Power Data Objects from Port partner (when Port partner is a Source or a DRP port) */
  GUI_IND_NUMBEROFRCVSRCPDO                  = 0x03, /*<! The number of received Source Power Data Objects from port Partner (when Port partner is a Source or a DRP port) */
  GUI_IND_LISTOFRCVSNKPDO                    = 0x04, /*<! The list of received Sink Power Data Objects from Port partner (when Port partner is a Sink or a DRP port) */
  GUI_IND_ISCONNECTED                        = 0x05, /*<! USB PD connection state */
  GUI_IND_CC                                 = 0x06, /*<! CC side */
  GUI_IND_DATAROLE                           = 0x07, /*<! It defines the initial data role. */
  GUI_IND_POWERROLE                          = 0x08, /*<! It defines the power role. */
  GUI_IND_CCDEFAULTCURRENTADVERTISED         = 0x09, /*<! advertising the current capability */
  GUI_IND_VCONNON                            = 0x0A, /*<! Vconn Status */
  GUI_IND_VBUS_LEVEL                         = 0x0C, /*<! VBUS level (in mV) */
  GUI_IND_IBUS_LEVEL                         = 0x0C, /*<! IBUS level (in mA) */
  GUI_IND_PD_SPECREVISION                    = 0x0D, /*<! Selected Specification revision */
  GUI_IND_PD_MESSAGENOTIF                    = 0x0E, /*<! Send notifications of PD messages */
  GUI_IND_NBBATTERIES                        = 0x0F, /*<! Number of batteries supported by the devices. */
  GUI_IND_COUNTRYCODES                       = 0x10, /*<! List of the country codes received in the COUNTRY_CODES message */
  GUI_IND_SVDM_SVIDS                         = 0x11, /*<! List of the SVDM SVID received in the SVDM Discovery SVID message */
  GUI_IND_SVDM_MODES                         = 0x12, /*<! List of the country codes received in the SVDM Discovery SVID message */
  GUI_IND_TIMESTAMP                          = 0x13, /*<! Timestamp used for VBUS and IBUS values */
  GUI_IND_PPS                                = 0x14, /*<! PPS value based on @ref USBPD_PPSSDB_TypeDef */
  GUI_IND_STATUS                             = 0x15, /*<! Status value based on @ref USBPD_SDB_TypeDef */
  GUI_IND_VDM_IDENTITY                       = 0x16, /*<! VDM Identity based on @ref USBPD_IDHeaderVDO_TypeDef */
  GUI_IND_CABLE_VDO                          = 0x19, /*<! VDM Cable Object based on @ref USBPD_CableVdo_TypeDef */
  GUI_IND_ALL                                = (GUI_IND_CABLE_VDO + 1), /*<! Number max of indication */
} USBPD_GUI_Tag_Indication;

/**
  * @brief  USB PD GUI TAG PARAM_MSG
  */
typedef enum
{
  GUI_PARAM_MSG_SOPTYPE                      = 0x00, /*<! SOP type used for DPM_MESSAGE_REQ */
  GUI_PARAM_MSG_RDOPOSITION                  = 0x01, /*<! RDO position of Received SRC PDO (used for RESET_REQ) */
  GUI_PARAM_MSG_REQUESTEDVOLTAGE             = 0x02, /*<! Requested voltage in REQUEST message (in mV) */
  GUI_PARAM_MSG_ALERTMSG                     = 0x04, /*<! Alert message used in ALERT message */
  GUI_PARAM_MSG_COUNTRYCODE                  = 0x05, /*<! Coutry code used in GET_COUNTRY_INFO message */
  GUI_PARAM_MSG_SVDM_SVID                    = 0x06, /*<! SVID used in SVDM disco mode, enter mode & exit mode */
  GUI_PARAM_MSG_SVDM_MODEINDEX               = 0x07, /*<! Mode index used in SVDM enter and exit mode messages */
  GUI_PARAM_MSG_UVDM_DATA                    = 0x08, /*<! Data used in UVDM message */
  GUI_PARAM_MSG_DP_STATUS                    = 0x09, /*<! Data used in DP status message */
  GUI_PARAM_MSG_DP_CONFIGURE                 = 0x0A, /*<! Data used in DP config message */
  GUI_PARAM_MSG_DP_ATTENTION                 = 0x0B, /*<! Data used in DP attention message */
  GUI_PARAM_MSG_BATTERYREF                   = 0x0C, /*<! Battery reference used in Get battery capa & status message */
  GUI_PARAM_MSG_MANUINFODATA                 = 0x0D, /*<! Manufacturer_Info data used in Get Manufacturer info message */
  GUI_PARAM_MSG_ALL                          = (GUI_PARAM_MSG_MANUINFODATA + 1), /*<! Number max of msg param */
} USBPD_GUI_Tag_ParamMsg;

/**
  * @brief  USB PD GUI MESSAGES
  */
typedef enum
{
  GUI_MSG_GOTOMIN                            = 0x00, /*<! Note: Refer to Control message in USB_PD specv3.0 */
  GUI_MSG_PING                               = 0x01, /*<! Note: Refer to Control message in USB_PD specv3.0 */
  GUI_MSG_GET_SRC_CAPA                       = 0x03, /*<! Note: Refer to Control message in USB_PD specv3.0 */
  GUI_MSG_GET_SNK_CAPA                       = 0x04, /*<! Message sent only if port partner is DRP. Answer to this message is a SNK_CAPA
                                                          Note: Refer to Control message in USB_PD specv3.0
                                                           */
  GUI_MSG_DR_SWAP                            = 0x05, /*<! Note: Refer to Control message in USB_PD specv3.0 */
  GUI_MSG_PR_SWAP                            = 0x06, /*<! Note: Refer to Control message in USB_PD specv3.0 */
  GUI_MSG_VCONN_SWAP                         = 0x07, /*<! Note: Refer to Control message in USB_PD specv3.0 */
  GUI_MSG_SOFT_RESET                         = 0x08, /*<! Note: Refer to Control message in USB_PD specv3.0 */
  GUI_MSG_GET_SOURCE_CAPA_EXTENDED           = 0x09, /*<! Specific to PD3.0
                                                          Note: Refer to Control message in USB_PD specv3.0 */
  GUI_MSG_GET_STATUS                         = 0x0A, /*<! Specific to PD3.0. Return a STATUS message
                                                          Note: Refer to Control message in USB_PD specv3.0 */
  GUI_MSG_FR_SWAP                            = 0x0B, /*<! Specific to PD3.0
                                                          Note: Refer to Control message in USB_PD specv3.0 */
  GUI_MSG_GET_PPS_STATUS                     = 0x0C, /*<! Specific to PD3.0. Wait for PPS_STATUS message
                                                          Note: Refer to Control message in USB_PD specv3.0 */
  GUI_MSG_GET_COUNTRY_CODES                  = 0x0D, /*<! Specific to PD3.0. Return a COUNTRY_CODES message
                                                          Note: Refer to Control message in USB_PD specv3.0 */
  GUI_MSG_SOURCE_CAPA                        = 0x0E, /*<! DPM_CONFIG_SET_REQ to be sent before sending this message
                                                          Note: Refer to data message in USB_PD specv3.0 */
  GUI_MSG_REQUEST                            = 0x0F, /*<! Select one of the capability available.
                                                          Note: Refer to data message in USB_PD specv3.0  */
  GUI_MSG_ALERT                              = 0x11, /*<! Specific to PD3.0
                                                          Note: Refer to data message in USB_PD specv3.0 */
  GUI_MSG_GET_COUNTRY_INFO                   = 0x12, /*<! Return COUNTRY_INFO message.
                                                          Note: Refer to data message in USB_PD specv3.0 */
  GUI_MSG_VDM_DISCO_IDENT                    = 0x13, /*<! Note: Refer to VDM message in USB_PD specv3.0 */
  GUI_MSG_VDM_DISCO_SVID                     = 0x14, /*<! Note: Refer to VDM message in USB_PD specv3.0 */
  GUI_MSG_VDM_DISCO_MODE                     = 0x15, /*<! Note: Refer to VDM message in USB_PD specv3.0 */
  GUI_MSG_VDM_ENTER_MODE                     = 0x16, /*<! Note: Refer to VDM message in USB_PD specv3.0 */
  GUI_MSG_VDM_EXIT_MODE                      = 0x17, /*<! Note: Refer to VDM message in USB_PD specv3.0 */
  GUI_MSG_VDM_ATTENTION                      = 0x18, /*<! Note: Refer to VDM message in USB_PD specv3.0 */
  GUI_MSG_VDM_UNSTRUCTURED                   = 0x19, /*<! 1..7 x 32bit
                                                          Note: Refer to VDM message in USB_PD specv3.0 */
  GUI_MSG_DISPLAY_PORT_STATUS                = 0x20, /*<! 1..7 x 32bit
                                                          Note: Refer to VDM message in USB_PD specv3.0 */
  GUI_MSG_DISPLAY_PORT_CONFIG                = 0x21, /*<! Note: Refer to VDM message in USB_PD specv3.0 */
  GUI_MSG_DISPLAY_PORT_ATTENTION             = 0x22, /*<! Note: Refer to VDM message in USB_PD specv3.0 */
  GUI_MSG_HARD_RESET                         = 0x24, /*<! Note: refer to Reset messages in USB_PD specv3.0 */
  GUI_MSG_CABLE_RESET                        = 0x25, /*<! Note: refer to Reset messages in USB_PD specv3.0 */
  GUI_MSG_GET_BAT_CAPA                       = 0x26, /*<! Return BAT_CAPA message
                                                          Note: Refer to extended message in USB_PD specv3.0 */
  GUI_MSG_GET_BAT_STATUS                     = 0x27, /*<! Return BAT_STATUS message
                                                          Note: Refer to extended message in USB_PD specv3.0 */
  GUI_MSG_GET_MANU_INFO                      = 0x28, /*<! Return MANU_INFO message
                                                          Note: Refer to extended message in USB_PD specv3.0 */
  GUI_MSG_SECU_REQUEST                       = 0x29, /*<! Note: Refer to extended message in USB_PD specv3.0 */
  GUI_MSG_FIRM_UPDATE_REQUEST                = 0x2A, /*<! Note: Refer to extended message in USB_PD specv3.0 */
  GUI_MSG_GET_SINK_CAPA_EXTENDED             = 0x2B, /*<! Specific to PD3.0
                                                            Note: Refer to Control message in USB_PD specv3.0 */
} USBPD_GUI_Messages;

/**
  * @brief  USB PD GUI INIT
  */
typedef enum
{
  GUI_INIT_HWBOARDVERSION                    = 0x00, /*<! ASCII stream to indicate STM32 version like STM32F032xB */
  GUI_INIT_HWPDTYPE                          = 0x01, /*<! ASCII stream to indicate PD type used by the devices like:
                                                          - AFE solution (MB1257B)
                                                          - 1602 solution (MB1303 rev A)
                                                          - TCPM solution (TCPC FUSB305)
                                                          - () */
  GUI_INIT_NBPORTMAX                         = 0x02, /*<! Indicate maximum number of ports which can be configured in the device (value 1, 2 , 3) */
  GUI_INIT_FWVERSION                         = 0x03, /*<! 4 bytes for FW version + 4 bytes for Stack version */
  GUI_INIT_TYPECSPECREVISION                 = 0x04, /*<! Type-C revision supported by the port */
  GUI_INIT_PDSPECREVISION                    = 0x04, /*<! PD spec revision supported by the port */
  GUI_INIT_EXTENDEDMESSAGESUNCKUNKED         = 0x06, /*<! Supports extended messages */
  GUI_INIT_ACCESSORYSUPP                     = 0x07, /*<! Accessory supported ot not */
  GUI_INIT_POWERACCESSORYDETECTION           = 0x08, /*<! It enables or disables powered accessory detection */
  GUI_INIT_POWERACCESSORYTRANSITION          = 0x09, /*<! It enables or disables transition from Powered.accessory to Try.SNK */
  GUI_INIT_ISCABLE                           = 0x0B, /*<! Cable (asserts Ra on VConn, communicates using SOP' and SOP''), when this mode is selected the stack will run in cable mode so all parameters like PowerRole are not used */
  GUI_INIT_TRYFEATURE                        = 0x10, /*<! Try Feature  */
  GUI_INIT_RPRESISTORVALUE                   = 0x12, /*<! RP resistor value */
  GUI_INIT_USBSUPPORT                        = 0x13, /*<! USB_Comms_Capable, is the UUT capable of enumerating as a USB host or device? */
  GUI_INIT_USBDEVICE                         = 0x14, /*<! Type_C_Can_Act_As_Device, Indicates whether the UUT can communicate with USB 2.0 or USB 3.1 as a device or as the Upstream Facing Port of a hub.  */
  GUI_INIT_USBHOST                           = 0x15, /*<!  Type_C_Can_Act_As_Host, Indicates whether the UUT can communicate with USB 2.0 or USB 3.1 as a host or as the Downstream Facing Port of a hub */
  GUI_INIT_UNCONSTRAINED_POWERED             = 0x16, /*<! UUT has an external power source available that is sufficient to adequately power the system while charging external devices or the UUTs primary function is to charge external devices. */
  GUI_INIT_USBSUSPENDSUPPORT                 = 0x17, /*<! USB Suspend support */
  GUI_INIT_VCONNDISCHARGE                    = 0x18, /*<! It enables or disables the VCONN discharge on CC pin */
  GUI_INIT_VCONNILIM                         = 0x19, /*<! enables or not VconnLim */
  GUI_INIT_VCONNILIMVALUE                    = 0x1A, /*<! It allows changing the default current limit supplying VCONN on the CC pins */
  GUI_INIT_VCONNMONITORING                   = 0x1B, /*<! It enables or disables UVLO threshold detection on VCONN pin */
  GUI_INIT_VCONNTHRESHOLDUVLO                = 0x1C, /*<! High UVLO threshold of 4.65 V; 1b: Low UVLO threshold of 2.65 V (case of VCONN-powered accessories operating down to 2.7 V) */
  GUI_INIT_VCONNSUPPLY                       = 0x1D, /*<! It enables or disables the VCONN supply capability on CC pin  */
  GUI_INIT_NB_PORT_START                     = 0x1E, /*<! Start Port number                                             */
} USBPD_GUI_Init;

/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/** @defgroup USBPD_GUI_API_Private_Macros USBPD GUI API Private Macros
  * @{
  */
#define __GUI_SET_TAG_ID(_PORT_, _TAG_)  ((_PORT_) << GUI_PORT_BIT_POSITION | (_TAG_))
/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** @defgroup USBPD_GUI_API_Private_Variables USBPD GUI API Private Variables
  * @{
  */

extern USBPD_HandleTypeDef                DPM_Ports[USBPD_PORT_COUNT];
extern USBPD_SettingsTypeDef              DPM_Settings[USBPD_PORT_COUNT];
#ifdef _VDM
extern USBPD_VDM_SettingsTypeDef          DPM_VDM_Settings[USBPD_PORT_COUNT];
extern USBPD_VDM_SettingsTypeDef          DPM_VDM_FLASH_Settings[USBPD_PORT_COUNT];
extern USBPD_ModeInfo_TypeDef             sModesInfo[USBPD_PORT_COUNT];
#endif /* _VDM */
extern USBPD_USER_SettingsTypeDef         DPM_USER_Settings[USBPD_PORT_COUNT];
extern USBPD_USER_SettingsTypeDef         DPM_USER_FLASH_Settings[USBPD_PORT_COUNT];
extern USBPD_PWR_Port_PDO_Storage_TypeDef PWR_Port_PDO_Storage[USBPD_PORT_COUNT];
GUI_USER_ParamsTypeDef           GUI_USER_Params[USBPD_PORT_COUNT];

uint8_t PtrDataRx[TLV_SIZE_MAX];
uint8_t Processed[TLV_SIZE_MAX];

USBPD_GUI_State GUI_State = GUI_STATE_INIT;

#if !defined(_RTOS)
volatile uint32_t GUI_Flag = DPM_USER_EVENT_NONE;
#endif /* _RTOS */

const uint8_t* (*pCB_HWBoardVersion)(void)  = NULL;
const uint8_t* (*pCB_HWPDType)(void)        = NULL;
/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/
/** @defgroup USBPD_GUI_API_Private_Functions GUI API Private Functions
  * @{
  */
static void      GUI_CALLBACK_RX(uint8_t Character, uint8_t Error);
static void      Send_DpmInitCnf(uint8_t PortNum, uint8_t *string);
static void      Request_MessageReq(uint8_t PortNum, uint8_t* instruction, uint8_t *pEncodedMsg);
static void      Send_DpmConfigSetCnf(uint8_t PortNum, uint8_t* instruction, uint8_t *pEncodedMsg);
static void      Send_DpmConfigGetCnf(uint8_t PortNum, uint8_t* instruction, uint8_t *pEncodedMsg);
static void      Send_DpmConfigGetRej(uint8_t PortNum, uint8_t *pEncodedMsg, USBPD_GUI_Reject_Reason RejecReason);
static void      Send_DpmMessageRej(uint8_t PortNum, uint8_t *pEncodedMsg, USBPD_GUI_Reject_Reason RejecReason);
static void      Send_DpmRegisterReadCnf(uint8_t PortNum, uint8_t *string, uint8_t param);
static void      Send_DpmRegisterWriteCnf(uint8_t PortNum, uint8_t *string, uint8_t *toProcess);
/**
  * @}
  */

/* Exported functions ------------------------------------------------------- */
/** @addtogroup USBPD_GUI_API_Exported_Functions
  * @{
  */

/**
  * @brief  GUI initialization function
  * @param  CB_HWBoardVersion BSP callback to retrieve HW Board version
  * @param  CB_HWPDType       BSP callback to retrieve HW PD Type
  * @retval None
  */
void GUI_Init(const uint8_t* (*CB_HWBoardVersion)(void), const uint8_t* (*CB_HWPDType)(void))
{
  /* Retrieve data from FLASH if needed */
  BSP_GUI_LoadDataFromFlash();

  pCB_HWBoardVersion  = CB_HWBoardVersion;
  pCB_HWPDType        = CB_HWPDType;
}


void GUI_Start(void)
{
  /* register the GUI callback to manage UART reception */
  TRACER_EMB_StartRX(GUI_CALLBACK_RX);
}

/**
  * @brief  callback called to end a transfert.
  * @param  Character Byte received by the device
  * @param  Error     Error detected in the reception
  * @retval 1 if message to send to DPM application
  */
void GUI_CALLBACK_RX(uint8_t Character, uint8_t Error)
{
  uint32_t event;
  __disable_irq();
  event = GUI_GetMessage(Character, Error);
  __enable_irq();
  if (event == 1)
  {
#if defined(_RTOS)
    osMessagePut(DPMMsgBox, DPM_USER_EVENT_GUI, 1);
#else
    GUI_Flag = DPM_USER_EVENT_GUI;
#endif /* _RTOS */
  }
}

/**
  * @brief  Main Trace RX process to push data on the media.
  * @param  Event     GUI trace event
  * @retval Timing
  */
uint32_t GUI_RXProcess(uint32_t Event)
{
  uint8_t *msg;
  USBPD_GUI_State state;
  uint8_t size;

  if (0 == (Event & GUI_PE_NOTIF_Msk))
  {
    /* Message have been received by GUI */
    state = GUI_SendAnswer(&msg, &size);

    switch (state)
    {
    case GUI_STATE_RESET:
      /* DPM_RESET_REQ received by the device */
      /* Save parameters in FLASH */
      BSP_GUI_SaveDataInFlash();
      HAL_NVIC_SystemReset();
      break;
    case GUI_STATE_INIT:
      /* DPM_INIT_REQ received by the device */
      break;
    default:
      /* Another GUI messages received by the device */
      TRACER_EMB_Add(msg, size);
      break;
    }
  }
  else
  {
    uint16_t type_event = (Event & GUI_PE_NOTIF_Msk) >> GUI_PE_NOTIF_Pos;
    if (GUI_NOTIF_MEASUREMENT == type_event)
    {
      /* Notification related to timeout for measure reporting */
      /* Send a notification to associated port */
      GUI_FormatAndSendNotification(((Event & GUI_PE_PORT_NUM_Msk) >> GUI_PE_PORT_NUM_Pos), GUI_NOTIF_MEASUREREPORTING, HAL_GetTick());
    }
    else
    {
      /* Message have been received by PE */
      /* Send a notification to associated port */
      GUI_FormatAndSendNotification(((Event & GUI_PE_PORT_NUM_Msk) >> GUI_PE_PORT_NUM_Pos), GUI_NOTIF_PE_EVENT, type_event);
    }
  }

  return 0;
}

/**
  * @brief  TRACE function to send a notification
  * @param  PortNum           Index of current used port
  * @param  TypeNotification  Type of requested notification is a combination of @ref USBPD_TRACE_TRACE_TYPE_NOTIFICATION
  * @param  Value             Value depending of TypeNotification
  * @note   If TypeNotification == GUI_NOTIF_ISCONNECTED, Value should be equal to 0 (Not connected) or 1 (connected)
  * @retval USBPD Status
  */
uint32_t GUI_FormatAndSendNotification(uint32_t PortNum, uint32_t TypeNotification, uint32_t Value)
{
  uint8_t *msg;
  uint8_t size;
  /* Only send notification if GUI is connected */
  if (GUI_STATE_INIT != GUI_SendNotification(PortNum, &msg, &size, TypeNotification, Value))
  {
    TRACER_EMB_Add(msg, size);
  }
  return 0;
}


/**
  * @brief  Function called through UART IT RX to fill the GUI RX buffer
  * @param  Character Received byte
  * @param  Error     Error if issue during reception
  * @retval 1 indicates that a complete GUI message have been received
  */
uint32_t GUI_GetMessage(uint8_t Character, uint8_t Error)
{
  uint32_t status = 0;
  static uint16_t counter = 0;    /* Allows for example to count the number of SOF or EOF we received consecutively */
  static uint16_t cursor = 0;     /* Our position in the buffer */
  static uint16_t currentSize;    /* How many bytes we should expect */
  static enum receptionStateMachine
  {
    Rstart,
    RonGoing,
    Rsize,
    Rend,
    Rerror
  } Rstate = Rstart;

  if(cursor < TLV_SIZE_MAX)
  {
    PtrDataRx[cursor] = Character;
  }
  else /* buffer too small, overflow */
  {
    cursor = 0;
    counter = 0;
    Rstate = Rstart;
    return 0;
  }

  if(Error) Rstate = Rerror;

  switch(Rstate)
  {
  case Rstart :
    if(PtrDataRx[cursor] == TLV_SOF)
    {
      counter++;
    }
    else
    {
      counter = 0;
      cursor = 0xFFFF; /* cursor is unsigned, but it will inevitably increments at the end of function, and we need a 0 at the next pass here */
    }

    if(counter == 4) /* The whole SOF is received */
    {
      counter = 0;
      Rstate = Rsize;
    }
    break;
  case Rsize :
    counter++;

    if(counter == 3) /* We have received the size */
    {
      currentSize = (PtrDataRx[cursor-1] << 8) + PtrDataRx[cursor];
      if (0 == currentSize)
      {
        Rstate = Rend;
      }
      else
      {
        Rstate = RonGoing;
      }
      counter = 0;
    }
    break;
  case RonGoing :
    counter++;

    if(counter == currentSize)
    {
      /* When there is no value, the first EOF is handled by Rongoing, before Rend takes control */
      counter = 0;
      Rstate = Rend;
    }
    break;
  case Rend :
    counter++;
    if(PtrDataRx[cursor] == TLV_EOF
       && PtrDataRx[cursor-1] == TLV_EOF
         && PtrDataRx[cursor-2] == TLV_EOF
           && PtrDataRx[cursor-3] == TLV_EOF)
    {
      /* The semaphore must be given only in this case, because otherwise it means we didn't receive the correct size of bytes */
      if(counter == 4)
      {
        status = 1;
      }
      counter = 0;
      cursor = 0xFFFF;
      Rstate = Rstart;
    }

    if(cursor == currentSize + 11)
    {
      /* No complete EOF arrived. We reset the buffer for safety even if the instruction might be complete. */
      counter = 0;
      cursor = 0xFFFF;
      Rstate = Rstart;
    }
    break;
  case Rerror :
    counter = 0;
    cursor = 0xFFFF;
    Rstate = Rstart;
    break;
  }

  cursor++;
  return status;
}

/**
  * @brief  Answer to a message received by the GUI
  * @param  pMsgToSend Pointer on the message to send
  * @param  pSizeMsg   Pointer on the size of the message to send
  * @retval GUI state
  */
USBPD_GUI_State GUI_SendAnswer(uint8_t **pMsgToSend, uint8_t *pSizeMsg)
{
  /* Extract the port from the tag*/
  uint8_t port = PtrDataRx[TLV_TAG_POSITION] >> GUI_PORT_BIT_POSITION;

  /* Do the appropriate treatment in response to what we have received */
  switch(PtrDataRx[TLV_TAG_POSITION] & 0x1F)
  {
  case DPM_RESET_REQ:
    /* Reset*/
    return GUI_STATE_RESET;

  case DPM_INIT_REQ:
    {
      Send_DpmInitCnf(port, Processed);
      *pMsgToSend = Processed;
      *pSizeMsg = TLV_get_string_length(Processed) + 8;
      TRACER_EMB_Add(*pMsgToSend, *pSizeMsg);
      GUI_State = GUI_STATE_RUNNING;
      if (0 == port)
      {
        /* Send a notification all the port */
        GUI_FormatAndSendNotification(USBPD_PORT_0, GUI_NOTIF_ISCONNECTED | GUI_NOTIF_PE_EVENT | GUI_NOTIF_TIMESTAMP, USBPD_NOTIFY_ALL);
#if USBPD_PORT_COUNT==2
        GUI_FormatAndSendNotification(USBPD_PORT_1, GUI_NOTIF_ISCONNECTED | GUI_NOTIF_PE_EVENT | GUI_NOTIF_TIMESTAMP, USBPD_NOTIFY_ALL);
#endif /* USBPD_PORT_COUNT == 2 */
      }
      return GUI_STATE_INIT;
    }

  case DPM_CONFIG_GET_REQ:
    if (0 != port)
    {
      Send_DpmConfigGetCnf((port - 1), PtrDataRx, Processed);
    }
    else
    {
      Send_DpmConfigGetRej(port, Processed, GUI_REJ_DPM_INVALID_PORT_NUMBER);
    }
    break;
  case DPM_CONFIG_SET_REQ:
    if (0 != port)
    {
      Send_DpmConfigSetCnf((port - 1), PtrDataRx, Processed);
    }
    else
    {
      Send_DpmConfigGetRej(port, Processed, GUI_REJ_DPM_INVALID_PORT_NUMBER);
    }
    break;
  case DPM_MESSAGE_REQ:
    if (0 != port)
    {
      Request_MessageReq((port - 1), PtrDataRx, Processed);
    }
    else
    {
      Send_DpmMessageRej(port, Processed, GUI_REJ_DPM_INVALID_PORT_NUMBER);
    }
    break;
  case DPM_REGISTER_READ_REQ:
    if (0 != port)
    {
      /* If size is 0*/
      if(!PtrDataRx[TLV_LENGTH_HIGH_POSITION] && !PtrDataRx[TLV_LENGTH_LOW_POSITION])
      {
        Send_DpmRegisterReadCnf((port - 1), Processed, 0xFF);
      }
      else
      {
        /* PtrDataRx[TLV_VALUE_POSITION] is the tag in parameter,
           when only one parameter is needed (cf USBPD GUI specification)*/
        Send_DpmRegisterReadCnf((port - 1), Processed, PtrDataRx[TLV_VALUE_POSITION]);
      }
    }
    else
    {
      Send_DpmConfigGetRej(port, Processed, GUI_REJ_DPM_INVALID_PORT_NUMBER);
    }
    break;
  case DPM_REGISTER_WRITE_REQ:
    if (0 != port)
    {
      Send_DpmRegisterWriteCnf((port - 1), Processed, PtrDataRx);
    }
    else
    {
      Send_DpmConfigGetRej(port, Processed, GUI_REJ_DPM_INVALID_PORT_NUMBER);
    }
    break;
  default :
    break;
  }

  *pMsgToSend = Processed;
  *pSizeMsg = TLV_get_string_length(Processed) + 8;
  return GUI_State;
}

/**
  * @brief  Answer to a message received by the GUI
  * @param  PortNum           Port number
  * @param  pMsgToSend        Pointer on the message to send
  * @param  pSizeMsg          Pointer on the size of the message to send
  * @param  TypeNotification  Type of the notification
  * @param  Value             Value of the notification
  * @retval GUI state
  */
USBPD_GUI_State GUI_SendNotification(uint8_t PortNum, uint8_t **pMsgToSend, uint8_t *pSizeMsg, uint32_t TypeNotification, uint32_t Value)
{
  TLV_ToSend_Data send_tlv;
  USBPD_GUI_State gui_state = GUI_STATE_INIT;

  /* Send a notitification only if GUI is running */
  if (GUI_State == GUI_STATE_RUNNING)
  {
    TLV_init_encode(&send_tlv, __GUI_SET_TAG_ID((PortNum + 1), DPM_MESSAGE_IND), TLV_SIZE_MAX, Processed);

    /* Check PD connection */
    if ((TypeNotification & GUI_NOTIF_ISCONNECTED) == GUI_NOTIF_ISCONNECTED)
    {
      /* Is Connected*/
      TLV_add(&send_tlv, GUI_IND_ISCONNECTED, 1, (uint8_t[]){DPM_Params[PortNum].PE_Power});
      if (USBPD_TRUE == DPM_Params[PortNum].PE_IsConnected)
      {
        uint8_t rp_value = 0;
        /* CC line */
        TLV_add(&send_tlv, GUI_IND_CC, 1, (uint8_t[]){DPM_Params[PortNum].ActiveCCIs});
        /* Power Role*/
        TLV_add(&send_tlv, GUI_IND_POWERROLE, 1, (uint8_t[]){DPM_Params[PortNum].PE_PowerRole});
        /* CC Default Current Advertised */
        rp_value = 3; /* (uint8_t)CAD_GetRPValue(PortNum); */
        TLV_add(&send_tlv, GUI_IND_CCDEFAULTCURRENTADVERTISED, 1, &rp_value);
        if (USBPD_POWER_EXPLICITCONTRACT == DPM_Params[PortNum].PE_Power)
        {
          /* Data Role*/
          TLV_add(&send_tlv, GUI_IND_DATAROLE, 1, (uint8_t[]){DPM_Params[PortNum].PE_DataRole});
          /* Vconn ON*/
          TLV_add(&send_tlv, GUI_IND_VCONNON, 1, (uint8_t[]){DPM_Params[PortNum].VconnStatus});
          /* PD_Spec Revision */
          TLV_add(&send_tlv, GUI_IND_PD_SPECREVISION, 1, (uint8_t[]){DPM_Params[PortNum].PE_SpecRevision});
        }
      }
    }

    /* Check PD message event */
    if ((TypeNotification & GUI_NOTIF_PE_EVENT) == GUI_NOTIF_PE_EVENT)
    {
      if (USBPD_NOTIFY_ALL != Value)
      {
        /* PD_MessageNotif */
        TLV_add(&send_tlv, GUI_IND_PD_MESSAGENOTIF, 1, (uint8_t*)&Value);

        switch (Value)
        {
          case USBPD_NOTIFY_POWER_STATE_CHANGE :
            /* Is Connected*/
            TLV_add(&send_tlv, GUI_IND_ISCONNECTED, 1, (uint8_t[]){DPM_Params[PortNum].PE_Power});
            if (USBPD_POWER_NO == DPM_Params[PortNum].PE_Power)
            {
              /* CC line */
              TLV_add(&send_tlv, GUI_IND_CC, 1, (uint8_t[]){DPM_Params[PortNum].ActiveCCIs});
              /* PowerRole*/
              TLV_add(&send_tlv, GUI_IND_POWERROLE, 1, (uint8_t[]){DPM_Params[PortNum].PE_PowerRole});
            }
            break;
          case USBPD_NOTIFY_GETSNKCAP_ACCEPTED :
            /* NumberOfRcvSNKPDO */
            TLV_add(&send_tlv, GUI_IND_NUMBEROFRCVSNKPDO, 1, (uint8_t[]){DPM_Ports[PortNum].DPM_NumberOfRcvSNKPDO});
            /* ListOfRcvSNKPDO*/
            TLV_add(&send_tlv, GUI_IND_LISTOFRCVSNKPDO, DPM_Ports[PortNum].DPM_NumberOfRcvSNKPDO * 4, (uint8_t *) DPM_Ports[PortNum].DPM_ListOfRcvSNKPDO);
            break;
          case USBPD_NOTIFY_GETSRCCAP_ACCEPTED :
            /* NumberOfRcvSRCPDO*/
            TLV_add(&send_tlv, GUI_IND_NUMBEROFRCVSRCPDO, 1, (uint8_t[]){DPM_Ports[PortNum].DPM_NumberOfRcvSRCPDO});
            /* ListOfRcvSRCPDO*/
            TLV_add(&send_tlv, GUI_IND_LISTOFRCVSRCPDO, DPM_Ports[PortNum].DPM_NumberOfRcvSRCPDO * 4, (uint8_t *) DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO);
            break;
          case USBPD_NOTIFY_POWER_EXPLICIT_CONTRACT :
            if (USBPD_PORTPOWERROLE_SNK == DPM_Params[PortNum].PE_PowerRole)
            {
              /* NumberOfRcvSRCPDO*/
              TLV_add(&send_tlv, GUI_IND_NUMBEROFRCVSRCPDO, 1, (uint8_t[]){DPM_Ports[PortNum].DPM_NumberOfRcvSRCPDO});
              /* ListOfRcvSRCPDO*/
              TLV_add(&send_tlv, GUI_IND_LISTOFRCVSRCPDO, DPM_Ports[PortNum].DPM_NumberOfRcvSRCPDO * 4, (uint8_t *) DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO);
            }
            /* RDOPosition */
            TLV_add(&send_tlv, GUI_IND_RDOPOSITION, 1, (uint8_t[]){DPM_Ports[PortNum].DPM_RDOPosition});
            /* DataRole*/
            TLV_add(&send_tlv, GUI_IND_DATAROLE, 1, (uint8_t[]){DPM_Params[PortNum].PE_DataRole});
            /* PowerRole*/
            TLV_add(&send_tlv, GUI_IND_POWERROLE, 1, (uint8_t[]){DPM_Params[PortNum].PE_PowerRole});
            /* VconnON*/
            TLV_add(&send_tlv, GUI_IND_VCONNON, 1, (uint8_t[]){DPM_Params[PortNum].VconnStatus});
            /* PD_SpecRevision */
            TLV_add(&send_tlv, GUI_IND_PD_SPECREVISION, 1, (uint8_t[]){DPM_Params[PortNum].PE_SpecRevision});
            break;
          case USBPD_NOTIFY_POWER_SWAP_TO_SNK_DONE :
          case USBPD_NOTIFY_POWER_SWAP_TO_SRC_DONE :
            /* PowerRole*/
            TLV_add(&send_tlv, GUI_IND_POWERROLE, 1, (uint8_t[]){DPM_Params[PortNum].PE_PowerRole});
            break;
          case USBPD_NOTIFY_DATAROLESWAP_UFP :
          case USBPD_NOTIFY_DATAROLESWAP_DFP :
            /* DataRole*/
            TLV_add(&send_tlv, GUI_IND_DATAROLE, 1, (uint8_t[]){DPM_Params[PortNum].PE_DataRole});
            break;
          case USBPD_NOTIFY_PD_SPECIFICATION_CHANGE :
            /* PD_SpecRevision */
            TLV_add(&send_tlv, GUI_IND_PD_SPECREVISION, 1, (uint8_t[]){DPM_Params[PortNum].PE_SpecRevision});
            break;
          case USBPD_NOTIFY_VCONN_SWAP_COMPLETE :
            /* VconnON */
            TLV_add(&send_tlv, GUI_IND_VCONNON, 1, (uint8_t[]){DPM_Params[PortNum].VconnStatus});
            break;
#if _PPS
          case USBPD_NOTIFY_PPS_STATUS_RECEIVED :
            /* PPS value */
            TLV_add(&send_tlv, GUI_IND_PPS, 4, (uint8_t*)&DPM_Ports[PortNum].DPM_RcvPPSStatus.d32);
            break;
#endif /* _PPS */
#if _STATUS
          case USBPD_NOTIFY_STATUS_RECEIVED :
            {
              uint8_t tab[5] = {
                DPM_Ports[PortNum].DPM_RcvStatus.InternalTemp,
                DPM_Ports[PortNum].DPM_RcvStatus.PresentInput,
                DPM_Ports[PortNum].DPM_RcvStatus.PresentBatteryInput,
                DPM_Ports[PortNum].DPM_RcvStatus.EventFlags,
                DPM_Ports[PortNum].DPM_RcvStatus.TemperatureStatus
              };
              /* Status value */
              TLV_add(&send_tlv, GUI_IND_STATUS, 5, tab);
            }
            break;
#endif /* _STATUS */
#ifdef _VDM
          case USBPD_NOTIFY_VDM_IDENTIFY_RECEIVED :
            TLV_add(&send_tlv, GUI_IND_VDM_IDENTITY, 4, (uint8_t*)&DPM_Ports[PortNum].VDM_DiscoIdentify.IDHeader.d32);
            break;
          case USBPD_NOTIFY_VDM_SVID_RECEIVED :
            {
              uint32_t index;
              uint16_t size;
              /* List of received SVDM SVID */
              if (0 != DPM_Ports[PortNum].VDM_SVIDPortPartner.NumSVIDs)
              {
                TLV_add(&send_tlv, GUI_IND_SVDM_SVIDS, 2, (uint8_t*)&DPM_Ports[PortNum].VDM_SVIDPortPartner.SVIDs[0]);
                size = 2;
                for (index = 1; index < DPM_Ports[PortNum].VDM_SVIDPortPartner.NumSVIDs; index++)
                {
                  TLV_addValue(&send_tlv, (uint8_t*)&DPM_Ports[PortNum].VDM_SVIDPortPartner.SVIDs[index], 2);
                  size += 2;
                }
                TLV_UpdateSizeTag(&send_tlv, size);
              }
            }
            break;
          case USBPD_NOTIFY_VDM_MODE_RECEIVED :
            {
              uint32_t index;
              uint16_t size;
              /* List of received SVDM MODES */
              if (0 != DPM_Ports[PortNum].VDM_ModesPortPartner.NumModes)
              {
                TLV_add(&send_tlv, GUI_IND_SVDM_MODES, 4, (uint8_t*)&DPM_Ports[PortNum].VDM_ModesPortPartner.Modes[0]);
                size = 4;
                for (index = 1; index < DPM_Ports[PortNum].VDM_ModesPortPartner.NumModes; index++)
                {
                  TLV_addValue(&send_tlv, (uint8_t*)&DPM_Ports[PortNum].VDM_ModesPortPartner.Modes[index], 4);
                  size += 4;
                }
                TLV_UpdateSizeTag(&send_tlv, size);
              }
            }
            break;
#endif /* _VDM */
#if defined(_VCONN_SUPPORT)
          case USBPD_NOTIFY_VDM_CABLE_IDENT_RECEIVED :
            TLV_add(&send_tlv, GUI_IND_CABLE_VDO, 4, (uint8_t*)&DPM_Ports[PortNum].VDM_DiscoCableIdentify.CableVDO.d32);
            break;
#endif /* _VCONN_SUPPORT */
        }
      }
      else
      {
        /* Send all the indication parameters if connected */
        if (USBPD_TRUE == DPM_Params[PortNum].PE_IsConnected)
        {
#if defined(_VDM)
          uint32_t index;
#endif /* _VDM */

          if (0 != DPM_Ports[PortNum].DPM_NumberOfRcvSNKPDO)
          {
            /* NumberOfRcvSNKPDO */
            TLV_add(&send_tlv, GUI_IND_NUMBEROFRCVSNKPDO, 1, (uint8_t[]){DPM_Ports[PortNum].DPM_NumberOfRcvSNKPDO});
            /* ListOfRcvSNKPDO*/
            TLV_add(&send_tlv, GUI_IND_LISTOFRCVSNKPDO, DPM_Ports[PortNum].DPM_NumberOfRcvSNKPDO * 4, (uint8_t *) DPM_Ports[PortNum].DPM_ListOfRcvSNKPDO);
          }
          /* RDOPosition */
          TLV_add(&send_tlv, GUI_IND_RDOPOSITION, 1, (uint8_t[]){DPM_Ports[PortNum].DPM_RDOPosition});
          if (USBPD_PORTPOWERROLE_SNK == DPM_Params[PortNum].PE_PowerRole)
          {
            /* NumberOfRcvSRCPDO*/
            TLV_add(&send_tlv, GUI_IND_NUMBEROFRCVSRCPDO, 1, (uint8_t[]){DPM_Ports[PortNum].DPM_NumberOfRcvSRCPDO});
            /* ListOfRcvSRCPDO*/
            TLV_add(&send_tlv, GUI_IND_LISTOFRCVSRCPDO, DPM_Ports[PortNum].DPM_NumberOfRcvSRCPDO * 4, (uint8_t *) DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO);
          }
          /* PD_SpecRevision */
          TLV_add(&send_tlv, GUI_IND_PD_SPECREVISION, 1, (uint8_t[]){DPM_Params[PortNum].PE_SpecRevision});

#if defined(USBPD_REV30_SUPPORT)
#if _SRC_CAPA_EXT
          /* Number of batteries supported by the port partner */
          TLV_add(&send_tlv, GUI_IND_NBBATTERIES, 1, (uint8_t[]){DPM_Ports[PortNum].DPM_RcvSRCExtendedCapa.NbBatteries});
#endif /* _SRC_CAPA_EXT */
#endif /* USBPD_REV30_SUPPORT */

#if defined(_VDM)
          if (0 != DPM_Ports[PortNum].VDM_DiscoIdentify.IDHeader.d32)
          {
            TLV_add(&send_tlv, GUI_IND_VDM_IDENTITY, 4, (uint8_t*)&DPM_Ports[PortNum].VDM_DiscoIdentify.IDHeader.d32);
          }
#if defined(_VCONN_SUPPORT)
          if (1 == DPM_Ports[PortNum].VDM_DiscoCableIdentify.CableVDO_Presence)
          {
            TLV_add(&send_tlv, GUI_IND_CABLE_VDO, 4, (uint8_t*)&DPM_Ports[PortNum].VDM_DiscoCableIdentify.CableVDO.d32);
          }
#endif /* _VCONN_SUPPORT */
          /* List of received SVDM SVID */
          if (0 != DPM_Ports[PortNum].VDM_SVIDPortPartner.NumSVIDs)
          {
            uint16_t size;
            TLV_add(&send_tlv, GUI_IND_SVDM_SVIDS, 2, (uint8_t*)&DPM_Ports[PortNum].VDM_SVIDPortPartner.SVIDs[0]);
            size = 2;
            for (index = 1; index < DPM_Ports[PortNum].VDM_SVIDPortPartner.NumSVIDs; index++)
            {
              TLV_addValue(&send_tlv, (uint8_t*)&DPM_Ports[PortNum].VDM_SVIDPortPartner.SVIDs[index], 2);
              size += 2;
            }
            TLV_UpdateSizeTag(&send_tlv, size);
          }

          /* List of received SVDM MODES */
          if (0 != DPM_Ports[PortNum].VDM_ModesPortPartner.NumModes)
          {
            uint16_t size;
            TLV_add(&send_tlv, GUI_IND_SVDM_MODES, 4, (uint8_t*)&DPM_Ports[PortNum].VDM_ModesPortPartner.Modes[0]);
            size = 4;
            for (index = 1; index < DPM_Ports[PortNum].VDM_ModesPortPartner.NumModes; index++)
            {
              TLV_addValue(&send_tlv, (uint8_t*)&DPM_Ports[PortNum].VDM_ModesPortPartner.Modes[index], 4);
              size += 4;
            }
            TLV_UpdateSizeTag(&send_tlv, size);
          }
#endif /* _VDM */
        }
      }
    }

    /* Check if notification linked to a measure report */
    if ((TypeNotification & GUI_NOTIF_MEASUREREPORTING) == GUI_NOTIF_MEASUREREPORTING)
    {
      uint32_t vsense = 0;
      int32_t isense = 0;

      TLV_add(&send_tlv, GUI_IND_TIMESTAMP, 4, (uint8_t*)&Value);

      vsense = HW_IF_PWR_GetVoltage(PortNum);
      isense = HW_IF_PWR_GetCurrent(PortNum);
      if(isense < 0)
      {
        isense = -isense;
      }

      /* VBUS & IBUS level */
      vsense = (isense << 16) | vsense;
      TLV_add(&send_tlv, GUI_IND_VBUS_LEVEL, 4, (uint8_t*)&vsense);
    }

    /* Check if timestamp */
    if ((TypeNotification & GUI_NOTIF_TIMESTAMP) == GUI_NOTIF_TIMESTAMP)
    {
      TLV_add(&send_tlv, GUI_IND_TIMESTAMP, 4, (uint8_t*)&Value);
    }

    /* Check if power change */
    if ((TypeNotification & GUI_NOTIF_POWER_EVENT) == GUI_NOTIF_POWER_EVENT)
    {
      /* Is Connected*/
      TLV_add(&send_tlv, GUI_IND_ISCONNECTED, 1, (uint8_t[]){DPM_Params[PortNum].PE_Power});
      if (USBPD_POWER_NO == DPM_Params[PortNum].PE_Power)
      {
        /* CC line */
        TLV_add(&send_tlv, GUI_IND_CC, 1, (uint8_t[]){DPM_Params[PortNum].ActiveCCIs});
        /* PowerRole*/
        TLV_add(&send_tlv, GUI_IND_POWERROLE, 1, (uint8_t[]){DPM_Params[PortNum].PE_PowerRole});
      }
    }

    TLV_deinit_encode(&send_tlv);

    if (0 != TLV_get_string_length(Processed))
    {
      *pMsgToSend = Processed;
      *pSizeMsg = TLV_get_string_length(Processed) + 8;
      gui_state = GUI_STATE_RUNNING;

    }
  }

  return gui_state;
}

/**
  * @brief  Callback function called by PE to inform GUI about PE event.
  * @param  PortNum   Port number
  * @param  EventVal Event vale @ref USBPD_NotifyEventValue_TypeDef
  * @retval None
  */
void GUI_PostNotificationMessage(uint8_t PortNum, uint16_t EventVal)
{
  /* Send a notitification only if GUI is running */
  if (GUI_State == GUI_STATE_RUNNING)
  {
    uint32_t event = DPM_USER_EVENT_GUI | (PortNum << GUI_PE_PORT_NUM_Pos) | (EventVal << GUI_PE_NOTIF_Pos);
#if defined(_RTOS)
    osMessagePut(DPMMsgBox, event, 0);
#else
    GUI_Flag = event;
#endif /* _RTOS */
  }
}

/**
  * @brief  Check is GUI is connected to the board or not
  * @retval USBPD state
  */
USBPD_FunctionalState GUI_IsRunning(void)
{
  return ((GUI_State == GUI_STATE_RUNNING) ? USBPD_ENABLE : USBPD_DISABLE);
}

/**
  * @}
  */

/** @defgroup USBPD_GUI_API_Private_Functions GUI API Private Functions
  * @{
  */
/**
  * @brief  Send DPM_INIT_CNF message to GUI
  * @param  PortNum   Port number
  * @param  pEncodedMsg Pointer on the message to be encoded
  * @retval None
  */
static void Send_DpmInitCnf(uint8_t PortNum, uint8_t *pEncodedMsg)
{
  TLV_ToSend_Data ToSendTLV;
  TLV_init_encode(&ToSendTLV, __GUI_SET_TAG_ID(PortNum, DPM_INIT_CNF), TLV_SIZE_MAX, pEncodedMsg);

  /* Information by board */
  if (0 == PortNum)
  {
    /* HWBoardVersion */
    const uint8_t* hwversion;
    if (NULL != pCB_HWBoardVersion)
    {
      hwversion = pCB_HWBoardVersion();
    }
    else
    {
      hwversion = (uint8_t*) "UNKNOWN";
    }
    TLV_add(&ToSendTLV, GUI_INIT_HWBOARDVERSION, strlen((const char*)hwversion), hwversion);

    /* HWPDType */
    const uint8_t* hwpdtype;
    if (NULL != pCB_HWBoardVersion)
    {
      hwpdtype  = pCB_HWPDType();
    }
    else
    {
      hwpdtype = (uint8_t*) "UNKNOWN";
    }
    TLV_add(&ToSendTLV, GUI_INIT_HWPDTYPE, strlen((const char*)hwpdtype), hwpdtype);

    /* NbPortMax */
    TLV_add(&ToSendTLV, GUI_INIT_NBPORTMAX, 1, (uint8_t[]){USBPD_PORT_COUNT});
    /* FW Version */
    TLV_add(&ToSendTLV, GUI_INIT_FWVERSION, 8, (uint8_t[]){
      (USBPD_FW_VERSION & 0x000000FF),
        (USBPD_FW_VERSION & 0x0000FF00) >> 8,
          (USBPD_FW_VERSION & 0x00FF0000) >> 16,
            (USBPD_FW_VERSION & 0xFF000000) >> 24,
      (_LIB_ID & 0x000000FF),
        (_LIB_ID & 0x0000FF00) >> 8,
          (_LIB_ID & 0x00FF0000) >> 16,
            (_LIB_ID & 0xFF000000) >> 24}
    );
    /* Start Port number */
    TLV_add(&ToSendTLV, GUI_INIT_NB_PORT_START, 1, (uint8_t[]){USBPD_START_PORT_NUMBER});
    /* Start Port number */
    TLV_add(&ToSendTLV, GUI_INIT_NB_PORT_START, 1, (uint8_t[]){USBPD_START_PORT_NUMBER});

    TLV_deinit_encode(&ToSendTLV);
  }
  else
  {
    uint16_t settings;
    uint8_t port = PortNum - 1;
#if defined(MB1303)
    uint16_t fake = 0xADDE;
#endif /*MB1303*/
    /* TypeCSpecVersion & PDSpecRevision */
    settings = DPM_Settings[port].PE_SpecRevision;
    TLV_add(&ToSendTLV, GUI_INIT_TYPECSPECREVISION, 2, (uint8_t[]){0x12, settings});
#if defined(USBPD_REV30_SUPPORT)
    /* Extended Messages UnCkunked */
    settings = DPM_Settings[port].PE_PD3_Support.d.PE_UnchunkSupport;
    TLV_add(&ToSendTLV, GUI_INIT_EXTENDEDMESSAGESUNCKUNKED, 1, (uint8_t*)&settings);
#endif /* USBPD_REV30_SUPPORT */
    /* AccessorySupp */
    settings = DPM_Settings[port].CAD_AccesorySupport;
    TLV_add(&ToSendTLV, GUI_INIT_ACCESSORYSUPP, 1, (uint8_t*)&settings);
    /*PowerAccessoryDetection*/
    settings = DPM_USER_Settings[port].PWR_AccessoryDetection;
    TLV_add(&ToSendTLV, GUI_INIT_POWERACCESSORYDETECTION, 1, (uint8_t*)&settings);
    /*PowerAccessoryTransition*/
    settings = DPM_USER_Settings[port].PWR_AccessoryTransition;
    TLV_add(&ToSendTLV, GUI_INIT_POWERACCESSORYTRANSITION, 1, (uint8_t*)&settings);
    /*IsCable*/
    TLV_add(&ToSendTLV, GUI_INIT_ISCABLE, 1, (uint8_t[]){0x00});
    /*TryFeature*/
    settings = DPM_Settings[port].CAD_TryFeature;
    TLV_add(&ToSendTLV, GUI_INIT_TRYFEATURE, 1, (uint8_t*)&settings);
    /*RpResistorValue*/
    settings = DPM_USER_Settings[port].PWR_RpResistorValue;
    TLV_add(&ToSendTLV, GUI_INIT_RPRESISTORVALUE, 1, (uint8_t*)&settings);
    /*USBSupport*/
    settings = DPM_USER_Settings[port].USB_Support;
    TLV_add(&ToSendTLV, GUI_INIT_USBSUPPORT, 1, (uint8_t*)&settings);
    /*USBDevice*/
    settings = DPM_USER_Settings[port].USB_Device;
    TLV_add(&ToSendTLV, GUI_INIT_USBDEVICE, 1, (uint8_t*)&settings);
    /*USBHost*/
    settings = DPM_USER_Settings[port].USB_Host;
    TLV_add(&ToSendTLV, GUI_INIT_USBHOST, 1, (uint8_t*)&settings);
    /*Externally_Powered*/
    settings = DPM_USER_Settings[port].PWR_UnconstrainedPower;
    TLV_add(&ToSendTLV, GUI_INIT_UNCONSTRAINED_POWERED, 1, (uint8_t*)&settings);
    /*USBSuspendSupport*/
    settings = DPM_USER_Settings[port].USB_SuspendSupport;
    TLV_add(&ToSendTLV, GUI_INIT_USBSUSPENDSUPPORT, 1, (uint8_t*)&settings);
#if defined(MB1303)
    /*VConnDischarge*/
    TLV_add(&ToSendTLV, GUI_INIT_VCONNDISCHARGE, 2, (uint8_t*)&fake);
    /*VConnLim*/
    TLV_add(&ToSendTLV, GUI_INIT_VCONNILIM, 2, (uint8_t*)&fake);
    /*VConnLimValue*/
    TLV_add(&ToSendTLV, GUI_INIT_VCONNILIMVALUE, 2, (uint8_t*)&fake);
    /*VConnMonitoring*/
    TLV_add(&ToSendTLV, GUI_INIT_VCONNMONITORING, 2, (uint8_t*)&fake);
    /*VConnThresholdUVLO*/
    TLV_add(&ToSendTLV, GUI_INIT_VCONNTHRESHOLDUVLO, 2, (uint8_t*)&fake);
    /*VConnSupply*/
    TLV_add(&ToSendTLV, GUI_INIT_VCONNSUPPLY, 2, (uint8_t*)&fake);
#endif /*MB1303*/

    TLV_deinit_encode(&ToSendTLV);
  }
}

/**
  * @brief      Execution of the message received
  * @note       Generation of a TLV instruction to send in the context of DPM_MESSAGE
  */
static void Request_MessageReq(uint8_t PortNum, uint8_t* instruction, uint8_t *pEncodedMsg)
{
  USBPD_StatusTypeDef status = USBPD_ERROR;
  TLV_Received_Data process_tlv;
  TLV_ToSend_Data send_tlv;

  TLV_init_decode(&process_tlv, instruction);
  TLV_init_encode(&send_tlv, __GUI_SET_TAG_ID((PortNum + 1), DPM_MESSAGE_CNF), TLV_SIZE_MAX, pEncodedMsg);

  /*
    In case there is an error, we use this variable to know when to send a DPM_MESSAGE_REJ
    instead of a DPM_MESSAGE_CNF. This variable contains the cause if there is one.
  */
  USBPD_GUI_Reject_Reason error = GUI_REJ_DPM_REJECT;

  uint8_t tag;
  uint16_t length;
  uint8_t *value;
  TLV_get(&process_tlv, &tag, &length, &value);
  switch(tag)
    /*
      Each case should:
      - Apply his specific request in the stack. If parameters are needed, they can be found in the value pointer
      - If there is an error, set the error variable according to it (cf USBPD_GUI_Specification)
      - Otherwise in case of success, do nothing else
    */
  {
  case GUI_MSG_GOTOMIN :
    status = USBPD_DPM_RequestGotoMin(PortNum);
    break;
  case GUI_MSG_PING :
    status = USBPD_DPM_RequestPing(PortNum);
    break;
  case GUI_MSG_GET_SRC_CAPA :
    status = USBPD_DPM_RequestGetSourceCapability(PortNum);
    break;
  case GUI_MSG_GET_SNK_CAPA :
    status = USBPD_DPM_RequestGetSinkCapability(PortNum);
    break;
  case GUI_MSG_DR_SWAP :
    status = USBPD_DPM_RequestDataRoleSwap(PortNum);
    break;
  case GUI_MSG_PR_SWAP :
    status = USBPD_DPM_RequestPowerRoleSwap(PortNum);
    break;
  case GUI_MSG_VCONN_SWAP :
    status = USBPD_DPM_RequestVconnSwap(PortNum);
    break;
  case GUI_MSG_SOURCE_CAPA :
    status = USBPD_DPM_RequestSourceCapability(PortNum);
    break;
  case GUI_MSG_REQUEST :
    {
      uint16_t voltage = 0;
      uint8_t nb_expected_tag = 0;
      uint8_t index_pdo = 0;
      if (length > TLV_SIZE_MAX) break;
      TLV_get(&process_tlv, &tag, &length, &value);
      while((0 != length) && (TLV_SIZE_MAX > length) && (nb_expected_tag < 2))
      {
        nb_expected_tag++;
        if (GUI_PARAM_MSG_RDOPOSITION == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          index_pdo = value[0];
        }
        else if (GUI_PARAM_MSG_REQUESTEDVOLTAGE == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          voltage = LE16(&value[0]);
        }
        TLV_get(&process_tlv, &tag, &length, &value);
      }
      if (2 == nb_expected_tag)
      {
        status = USBPD_DPM_RequestMessageRequest(PortNum, index_pdo, voltage);
      }
    }
    break;
  case GUI_MSG_SOFT_RESET :
    TLV_get(&process_tlv, &tag, &length, &value);
    if (GUI_PARAM_MSG_SOPTYPE == (USBPD_GUI_Tag_ParamMsg)tag)
    {
      status = USBPD_DPM_RequestSoftReset(PortNum, (USBPD_SOPType_TypeDef)value[0]);
    }
    break;
  case GUI_MSG_HARD_RESET :
    status = USBPD_DPM_RequestHardReset(PortNum);
    break;
  case GUI_MSG_CABLE_RESET :
    status = USBPD_DPM_RequestCableReset(PortNum);
    break;
#if defined(_VDM)
  case GUI_MSG_VDM_DISCO_IDENT :
    if (length > TLV_SIZE_MAX) break;
    TLV_get(&process_tlv, &tag, &length, &value);
    if (GUI_PARAM_MSG_SOPTYPE == (USBPD_GUI_Tag_ParamMsg)tag)
    {
      status = USBPD_DPM_RequestVDM_DiscoveryIdentify(PortNum, (USBPD_SOPType_TypeDef)value[0]);
    }
    break;
  case GUI_MSG_VDM_DISCO_SVID :
    if (length > TLV_SIZE_MAX) break;
    TLV_get(&process_tlv, &tag, &length, &value);
    if (GUI_PARAM_MSG_SOPTYPE == (USBPD_GUI_Tag_ParamMsg)tag)
    {
      status = USBPD_DPM_RequestVDM_DiscoverySVID(PortNum, (USBPD_SOPType_TypeDef)value[0]);
    }
    break;
  case GUI_MSG_VDM_DISCO_MODE :
    {
      uint16_t svid = 0;
      uint8_t nb_expected_tag = 0;
      uint8_t sop = USBPD_SOPTYPE_SOP;
      if (length > TLV_SIZE_MAX) break;
      TLV_get(&process_tlv, &tag, &length, &value);
      while((0 != length) && (TLV_SIZE_MAX > length) && (nb_expected_tag < 2))
      {
        nb_expected_tag++;
        if (GUI_PARAM_MSG_SOPTYPE == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          sop = value[0];
        }
        else if (GUI_PARAM_MSG_SVDM_SVID == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          svid = LE16(&value[0]);
        }
        TLV_get(&process_tlv, &tag, &length, &value);
      }
      if (2 == nb_expected_tag)
      {
        status = USBPD_DPM_RequestVDM_DiscoveryMode(PortNum, (USBPD_SOPType_TypeDef)sop, svid);
      }
    }
    break;
  case GUI_MSG_VDM_ENTER_MODE :
    {
      uint16_t mode = 0, svid = 0;
      uint8_t nb_expected_tag = 0;
      uint8_t sop = USBPD_SOPTYPE_SOP;
      if (length > TLV_SIZE_MAX) break;
      TLV_get(&process_tlv, &tag, &length, &value);
      while((0 != length) && (TLV_SIZE_MAX > length) && (nb_expected_tag < 3))
      {
        nb_expected_tag++;
        if (GUI_PARAM_MSG_SOPTYPE == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          sop = value[0];
        }
        else if (GUI_PARAM_MSG_SVDM_SVID == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          svid = LE16(&value[0]);
        }
        else if (GUI_PARAM_MSG_SVDM_MODEINDEX == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          mode = LE16(&value[0]);
        }
        TLV_get(&process_tlv, &tag, &length, &value);
      }
      if (3 == nb_expected_tag)
      {
        status = USBPD_DPM_RequestVDM_EnterMode(PortNum, (USBPD_SOPType_TypeDef)sop, svid, mode);
      }
    }
    break;
  case GUI_MSG_VDM_EXIT_MODE :
    {
      uint16_t svid = 0, mode = 0;
      uint8_t nb_expected_tag = 0;
      uint8_t sop = USBPD_SOPTYPE_SOP;
      if (length > TLV_SIZE_MAX) break;
      TLV_get(&process_tlv, &tag, &length, &value);
      while((0 != length) && (TLV_SIZE_MAX > length) && (nb_expected_tag < 3))
      {
        nb_expected_tag++;
        if (GUI_PARAM_MSG_SOPTYPE == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          sop = value[0];
        }
        else if (GUI_PARAM_MSG_SVDM_SVID == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          svid = LE16(&value[0]);
        }
        else if (GUI_PARAM_MSG_SVDM_MODEINDEX == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          mode = value[0];
        }
        TLV_get(&process_tlv, &tag, &length, &value);
      }
      if (3 == nb_expected_tag)
      {
        status = USBPD_DPM_RequestVDM_ExitMode(PortNum, (USBPD_SOPType_TypeDef)sop, svid, mode);
      }
    }
    break;
  case GUI_MSG_VDM_ATTENTION :
    {
      uint16_t svid = 0;
      uint8_t nb_expected_tag = 0;
      uint8_t sop = USBPD_SOPTYPE_SOP;
      if (length > TLV_SIZE_MAX) break;
      TLV_get(&process_tlv, &tag, &length, &value);
      while((0 != length) && (TLV_SIZE_MAX > length) && (nb_expected_tag < 2))
      {
        nb_expected_tag++;
        if (GUI_PARAM_MSG_SOPTYPE == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          sop = value[0];
        }
        else if (GUI_PARAM_MSG_SVDM_SVID == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          svid = LE16(&value[0]);
        }
        TLV_get(&process_tlv, &tag, &length, &value);
      }
      if (2 == nb_expected_tag)
      {
        status = USBPD_DPM_RequestAttention(PortNum, (USBPD_SOPType_TypeDef)sop, svid);
      }
    }
    break;
  case GUI_MSG_VDM_UNSTRUCTURED :
    break;
  case GUI_MSG_DISPLAY_PORT_STATUS :
    {
      uint32_t dp_status;
      uint16_t svid = 0;
      uint8_t nb_expected_tag = 0;
      uint8_t sop = USBPD_SOPTYPE_SOP;
      if (length > TLV_SIZE_MAX) break;
      TLV_get(&process_tlv, &tag, &length, &value);
      while((0 != length) && (TLV_SIZE_MAX > length) && (nb_expected_tag < 3))
      {
        nb_expected_tag++;
        if (GUI_PARAM_MSG_SOPTYPE == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          sop = value[0];
        }
        else if (GUI_PARAM_MSG_SVDM_SVID == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          svid = LE16(&value[0]);
        }
        else if (GUI_PARAM_MSG_DP_STATUS == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          dp_status = LE32(&value[0]);
        }
        TLV_get(&process_tlv, &tag, &length, &value);
      }
      if (3 == nb_expected_tag)
      {
        status = USBPD_DPM_RequestDisplayPortStatus(PortNum, (USBPD_SOPType_TypeDef)sop, svid, &dp_status);
      }
    }
    break;
  case GUI_MSG_DISPLAY_PORT_CONFIG :
    {
      uint32_t dp_config;
      uint16_t svid = 0;
      uint8_t nb_expected_tag = 0;
      uint8_t sop = USBPD_SOPTYPE_SOP;
      if (length > TLV_SIZE_MAX) break;
      TLV_get(&process_tlv, &tag, &length, &value);
      while((0 != length) && (TLV_SIZE_MAX > length) && (nb_expected_tag < 3))
      {
        nb_expected_tag++;
        if (GUI_PARAM_MSG_SOPTYPE == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          sop = value[0];
        }
        else if (GUI_PARAM_MSG_SVDM_SVID == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          svid = LE16(&value[0]);
        }
        else if (GUI_PARAM_MSG_DP_CONFIGURE== (USBPD_GUI_Tag_ParamMsg)tag)
        {
          dp_config = LE32(&value[0]);
        }
        TLV_get(&process_tlv, &tag, &length, &value);
      }
      if (3 == nb_expected_tag)
      {
        status = USBPD_DPM_RequestDisplayPortConfig(PortNum, (USBPD_SOPType_TypeDef)sop, svid, &dp_config);
      }
    }
    break;
  case GUI_MSG_DISPLAY_PORT_ATTENTION :
    break;
#endif /* _VDM */
#ifdef USBPD_REV30_SUPPORT
  case GUI_MSG_FR_SWAP :
    status = USBPD_DPM_RequestFastRoleSwap(PortNum);
    break;
  case GUI_MSG_GET_PPS_STATUS :
    status = USBPD_DPM_RequestGetPPS_Status(PortNum);
    break;
  case GUI_MSG_GET_COUNTRY_CODES :
    status = USBPD_DPM_RequestGetCountryCodes(PortNum);
    break;
  case GUI_MSG_GET_STATUS :
    status = USBPD_DPM_RequestGetStatus(PortNum);
    break;
  case GUI_MSG_GET_SOURCE_CAPA_EXTENDED :
    status = USBPD_DPM_RequestGetSourceCapabilityExt(PortNum);
    break;
  case GUI_MSG_GET_SINK_CAPA_EXTENDED :
    status = USBPD_DPM_RequestGetSinkCapabilityExt(PortNum);
    break;
  case GUI_MSG_ALERT :
    if (length > TLV_SIZE_MAX) break;
    TLV_get(&process_tlv, &tag, &length, &value);
    if (GUI_PARAM_MSG_ALERTMSG == (USBPD_GUI_Tag_ParamMsg)tag)
    {
      USBPD_ADO_TypeDef alert;
      alert.d32 = LE32(&value[0]);
      status = USBPD_DPM_RequestAlert(PortNum, alert);
    }
    break;
  case GUI_MSG_GET_COUNTRY_INFO :
    if (length > TLV_SIZE_MAX) break;
    TLV_get(&process_tlv, &tag, &length, &value);
    if (GUI_PARAM_MSG_COUNTRYCODE == (USBPD_GUI_Tag_ParamMsg)tag)
    {
      uint16_t country_code;
      country_code = LE16(&value[0]);
      status = USBPD_DPM_RequestGetCountryInfo(PortNum, country_code);
    }
    break;
  case GUI_MSG_GET_BAT_CAPA :
    if (length > TLV_SIZE_MAX) break;
    TLV_get(&process_tlv, &tag, &length, &value);
    if (GUI_PARAM_MSG_BATTERYREF == (USBPD_GUI_Tag_ParamMsg)tag)
    {
      status = USBPD_DPM_RequestGetBatteryCapability(PortNum, (uint8_t*)&value[0]);
    }
    break;
  case GUI_MSG_GET_BAT_STATUS :
    if (length > TLV_SIZE_MAX) break;
    TLV_get(&process_tlv, &tag, &length, &value);
    if (GUI_PARAM_MSG_BATTERYREF == (USBPD_GUI_Tag_ParamMsg)tag)
    {
      status = USBPD_DPM_RequestGetBatteryStatus(PortNum, (uint8_t*)&value[0]);
    }
    break;
  case GUI_MSG_GET_MANU_INFO :
    {
      uint16_t manu_info;
      uint8_t nb_expected_tag = 0;
      uint8_t sop = USBPD_SOPTYPE_SOP;
      if (length > TLV_SIZE_MAX) break;
      TLV_get(&process_tlv, &tag, &length, &value);
      while((0 != length) && (TLV_SIZE_MAX > length) && (nb_expected_tag < 2))
      {
        nb_expected_tag++;
        if (GUI_PARAM_MSG_SOPTYPE == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          sop = value[0];
        }
        else if (GUI_PARAM_MSG_MANUINFODATA == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          manu_info = LE16(&value[0]);
        }
        TLV_get(&process_tlv, &tag, &length, &value);
      }
      if (2 == nb_expected_tag)
      {
        status = USBPD_DPM_RequestGetManufacturerInfo(PortNum, (USBPD_SOPType_TypeDef)sop, (uint8_t*)&manu_info);
      }
    }
    break;
  case GUI_MSG_SECU_REQUEST :
    status = USBPD_DPM_RequestSecurityRequest(PortNum);
    break;
  case GUI_MSG_FIRM_UPDATE_REQUEST :
    break;
#endif /* USBPD_REV30_SUPPORT */
  default :
    break;
  }

  /* Only applies if an error was specified (= if the case couldn't success)*/
  if (USBPD_OK != status)
  {
    switch (status)
    {
    case USBPD_BUSY:
      error = GUI_REJ_DPM_NOT_READY;
      break;
    case USBPD_TIMEOUT:
      error = GUI_REJ_DPM_TIMEOUT;
      break;
    default:
    case USBPD_NOTSUPPORTED:
    case USBPD_ERROR:
      error = GUI_REJ_DPM_REJECT;
      break;
    }
    TLV_deinit_encode(&send_tlv);
    TLV_init_encode(&send_tlv, __GUI_SET_TAG_ID((PortNum + 1), DPM_MESSAGE_REJ), TLV_SIZE_MAX, pEncodedMsg); /* Turn the tag 0x0A into 0x0B to signal an error*/
    TLV_addValue(&send_tlv, (uint8_t*)&error, 1);
  }

  TLV_deinit_encode(&send_tlv);
  TLV_deinit_decode(&process_tlv);
}

/**
  * @brief      Application of the received configuration
  * @note       Generation of a TLV instruction to send in the context of DPM_CONFIG_SET
  */
static void Send_DpmConfigSetCnf(uint8_t PortNum, uint8_t* instruction, uint8_t *pEncodedMsg)
{
  TLV_ToSend_Data send_tlv;
  TLV_Received_Data process_tlv;
  /*
    In case there is an error, we use this variable to know when to send a DPM_CONFIG_SET instead
    of a DPM_MESSAGE_CNF. This variable contains the cause if there is one.
  */
  uint8_t *value;
  uint16_t size;
  uint16_t dcdrp = 0;
  uint8_t error = 0xFF;
  uint8_t param_not_applicated[GUI_PARAM_ALL];         /* List of parameters who could not be applied. */
  uint8_t counter_param_not_applicated = 0;  /* Counter of not applicated parameters */
  uint8_t tag, flag_drp = 0, tdrp = 0;

  TLV_init_decode(&process_tlv, instruction);
  /* This is the pEncodedMsg can be sent as it is at this point, if there isn't any error during application of settings*/
  TLV_init_encode(&send_tlv, __GUI_SET_TAG_ID((PortNum + 1), DPM_CONFIG_SET_CNF), TLV_SIZE_MAX, pEncodedMsg);

  /* Do while loop, used to parse and apply each parameter */
  while(0xFF != TLV_get(&process_tlv, &tag, &size, &value))
    /*
      As long as we haven't read all the parameters. If error is set, we continue to loop
      to apply the other parameters if it's because of a DPM_REJECT
      While loop, used to parse and apply each parameter
    */
  {
    switch((USBPD_GUI_Tag_Param)tag)
      /*
        Each case should:
        - Apply his specific parameter. If data is needed, it can be found in the value pointer
        - In case of success, do nothing else
        - Otherwise if there is an error, set the error variable according to it (cf USBPD_GUI_Specification)
        - If the error is DPM_REJECT (0x00), add the parameter tag to param_not_applicated, using the
          counter counter_param_not_applicated. It allows to list the rejected parameters in the DPM_CONFIG_REJ
      */
    {
    case GUI_PARAM_SOP :
      /* SOP & SOP1 & SOP2 */
      /* SOP1_Debug & SOP2_Debug not implemented */
      DPM_Settings[PortNum].PE_SupportedSOP = value[0];
      break;
#if defined(USBPD_REV30_SUPPORT)
    case GUI_PARAM_FASTROLESWAP :
      /*FastRoleSwap*/
      DPM_Settings[PortNum].PE_PD3_Support.d.PE_FastRoleSwapSupport = value[0];
      break;
#endif /*USBPD_REV30_SUPPORT*/
    case GUI_PARAM_DATAROLESWAP :
      /*DataRoleSwap*/
      DPM_USER_Settings[PortNum].PE_DataSwap = value[0];
      break;
    case GUI_PARAM_DEFAULTPOWERROLE :
      /*DefaultPowerRole*/
      DPM_Settings[PortNum].PE_DefaultRole = (USBPD_PortPowerRole_TypeDef)value[0];
      break;
    case GUI_PARAM_DRP_SUPPORT :
      /*DRP_Support*/
      DPM_Settings[PortNum].PE_RoleSwap = value[0];
      break;
    case GUI_PARAM_CADROLETOGGLE :
      /*CADRoleToggle*/
      DPM_Settings[PortNum].CAD_RoleToggle = value[0];
      break;
    case GUI_PARAM_PE_SCAP_HR :
      /*PE_SCAP_HR*/
      DPM_Settings[PortNum].PE_CapscounterSupport = value[0];
      break;
    case GUI_PARAM_VCONNSWAP :
      /*VConnSwap*/
      DPM_USER_Settings[PortNum].PE_VconnSwap = value[0];
      break;
#if defined(_VDM)
    case GUI_PARAM_VDM_SUPPORT :
      /* DRP Support */
      DPM_Settings[PortNum].PE_VDMSupport = value[0];
      break;
#endif /*_VDM*/
    case GUI_PARAM_PING_SUPPORT :
      /* Ping Support */
      DPM_Settings[PortNum].PE_PingSupport = value[0];
      break;
#if defined(USBPD_REV30_SUPPORT)
    case GUI_PARAM_PPS_SUPPORT :
      DPM_Settings[PortNum].PE_PD3_Support.PD3_Support = LE16(&value[0]);
      break;
#endif /*USBPD_REV30_SUPPORT*/
#if defined(_SNK) || defined(_DRP)
    case GUI_PARAM_SNK_PDO :
      {
        uint32_t index_pdo, index_gui;
        uint32_t pdo;
        memset(PWR_Port_PDO_Storage[PortNum].SinkPDO.ListOfPDO, 0, sizeof(PWR_Port_PDO_Storage[PortNum].SinkPDO.ListOfPDO));
        for (index_pdo = 0, index_gui = 0; index_gui < size; index_pdo++)
        {
          pdo = LE32(&value[index_gui]);
          PWR_Port_PDO_Storage[PortNum].SinkPDO.ListOfPDO[index_pdo] = pdo;
          index_gui = index_gui + 4;
        }
        PWR_Port_PDO_Storage[PortNum].SinkPDO.NumberOfPDO = (size / 4);
        USBPD_PWR_IF_CheckUpdateSNKPower(PortNum);
      }
      break;
#endif /* (_SNK) || (_DRP) */
#if defined(_SRC) || defined(_DRP)
    case GUI_PARAM_SRC_PDO :
      {
        uint32_t index_pdo, index_gui;
        uint32_t pdo;
        memset(PWR_Port_PDO_Storage[PortNum].SourcePDO.ListOfPDO, 0, sizeof(PWR_Port_PDO_Storage[PortNum].SourcePDO.ListOfPDO));
        for (index_pdo = 0, index_gui = 0; index_gui < size; index_pdo++)
        {
          pdo = LE32(&value[index_gui]);
          PWR_Port_PDO_Storage[PortNum].SourcePDO.ListOfPDO[index_pdo] = pdo;
          index_gui = index_gui + 4;
        }
         PWR_Port_PDO_Storage[PortNum].SourcePDO.NumberOfPDO = (size / 4);
      }
      break;
#endif /* (_SRC) || (_DRP) */
    case GUI_PARAM_TDRP :
      tdrp = value[0];
      DPM_USER_Settings[PortNum].CAD_tDRP = tdrp;
      flag_drp = 1;
      break;
    case GUI_PARAM_DCSRC_DRP :
      dcdrp = value[0];
      DPM_USER_Settings[PortNum].CAD_dcSRC_DRP = dcdrp;
      flag_drp = 1;
      break;
    case GUI_PARAM_RESPONDS_TO_DISCOV_SOP :
    case GUI_PARAM_ATTEMPTS_DISCOV_SOP :
      /* Parameter is a CONST. Cannot be changed */
      error = GUI_REJ_DPM_REJECT;
      param_not_applicated[counter_param_not_applicated] = tag;
      counter_param_not_applicated++;
      break;
#if defined(_VDM)
    case GUI_PARAM_XID_SOP :
      DPM_VDM_Settings[PortNum].VDM_XID_SOP           = LE32(&value[0]);
      break;
    case GUI_PARAM_DATA_CAPABLE_AS_USB_HOST_SOP :
      DPM_VDM_Settings[PortNum].VDM_USBHostSupport    = (USBPD_USBCapa_TypeDef)value[0];
      break;
    case GUI_PARAM_DATA_CAPABLE_AS_USB_DEVICE_SOP :
      DPM_VDM_Settings[PortNum].VDM_USBDeviceSupport  = (USBPD_USBCapa_TypeDef)value[0];
      break;
    case GUI_PARAM_PRODUCT_TYPE_SOP :
      DPM_VDM_Settings[PortNum].VDM_ProductTypeUFPorCP= (USBPD_ProductType_TypeDef)value[0];
      break;
    case GUI_PARAM_MODAL_OPERATION_SUPPORTED_SOP :
      DPM_VDM_Settings[PortNum].VDM_ModalOperation    = (USBPD_ModalOp_TypeDef)value[0];
      break;
    case GUI_PARAM_USB_VID_SOP :
      DPM_VDM_Settings[PortNum].VDM_USB_VID_SOP       = LE16(&value[0]);
      break;
    case GUI_PARAM_PID_SOP :
      DPM_VDM_Settings[PortNum].VDM_PID_SOP           = LE16(&value[0]);
      break;
    case GUI_PARAM_BCDDEVICE_SOP :
      DPM_VDM_Settings[PortNum].VDM_bcdDevice_SOP     = LE16(&value[0]);
      break;
#endif /* _VDM */
    case GUI_PARAM_MEASUREREPORTING :
      /* MeasurementReporting */
      GUI_USER_Params[PortNum].u.MeasurementReporting = value[0];
      break;
#if defined(USBPD_REV30_SUPPORT)
#if _MANU_INFO
    case GUI_PARAM_MANUINFOPORT_VID :
      DPM_USER_Settings[PortNum].DPM_ManuInfoPort.VID = LE16(&value[0]);
      DPM_USER_Settings[PortNum].DPM_ManuInfoPort.PID = LE16(&value[2]);
      size -= 4;
      memcpy(DPM_USER_Settings[PortNum].DPM_ManuInfoPort.ManuString, &value[4], size);
      break;
#endif /* _MANU_INFO */
#endif /*USBPD_REV30_SUPPORT*/
    default :
      error = GUI_REJ_DPM_REJECT;
      param_not_applicated[counter_param_not_applicated] = tag;
      counter_param_not_applicated++;
      break;
    }
  }

  /* Updtate CAD_SNKToggleTime & CAD_SRCToggleTime only if TDRP and/or DC_SRC_DRP have been received */
  if (1 == flag_drp)
  {
    uint16_t calcul;
    if (0 == tdrp)
    {
      tdrp = DPM_Settings[PortNum].CAD_SNKToggleTime + DPM_Settings[PortNum].CAD_SRCToggleTime;
    }
    if (0 == dcdrp)
    {
      dcdrp = (DPM_Settings[PortNum].CAD_SRCToggleTime * 100) / (DPM_Settings[PortNum].CAD_SNKToggleTime + DPM_Settings[PortNum].CAD_SRCToggleTime);
    }
    calcul = (tdrp * dcdrp) / 100;
    DPM_Settings[PortNum].CAD_SRCToggleTime = (uint8_t)calcul;
    calcul = tdrp * (100 - dcdrp) / 100;
    DPM_Settings[PortNum].CAD_SNKToggleTime = (uint8_t)calcul;
  }

  /* Only applies if an error was specified. Will send a DPM_CONFIG_REJ instead of DPM_CONFIG_SET_CNF*/
  if(error != 0xFF)
  {
    TLV_deinit_encode(&send_tlv);
    TLV_init_encode(&send_tlv, __GUI_SET_TAG_ID((PortNum + 1), DPM_CONFIG_REJ), TLV_SIZE_MAX, pEncodedMsg);
    TLV_addValue(&send_tlv, &error, 1);

    /* If the cause of error is DPM_REJECT, we have to list the tag of parameters which didn't pass */
    if(!error)
    {
      uint32_t index;
      for(index = 0; index < counter_param_not_applicated; index++)
      {
        TLV_addValue(&send_tlv, &param_not_applicated[index], 1);
      }
    }
    TLV_deinit_encode(&send_tlv);
  }
}

static void Send_DpmConfigGetCnf(uint8_t PortNum, uint8_t* instruction, uint8_t *pEncodedMsg)
{
  TLV_ToSend_Data send_tlv;
  TLV_init_encode(&send_tlv, __GUI_SET_TAG_ID((PortNum + 1), DPM_CONFIG_GET_CNF), TLV_SIZE_MAX, pEncodedMsg);
  uint16_t length = TLV_get_string_length(instruction) - TLV_HEADER_SIZE;
  uint8_t index = 0;
  uint8_t param;

  /* This is a state machine. */
  do
  {
    /* If there is no parameters, we go through each case of the state machine in one pass. (conditionnal breaks) */
    if(0 == length)
    {
      param = GUI_PARAM_ALL;
    }
    else
    {
      /* If there are, we loop the state machine and go through one case each time. */
      param = instruction[TLV_VALUE_POSITION + index];
    }

    /*
    Each case should simply use the TLV_add function to insert his corresponding data,
    as suggested in comment. Manual cast to (uint8_t *) may be required.
    */
    switch((USBPD_GUI_Tag_Param)param)
    {
    case GUI_PARAM_ALL :
    case GUI_PARAM_SOP :
      {
        /* SOP & SOP1 & SOP2 */
        /* SOP1_Debug & SOP2_Debug not implemented */
        uint8_t settings = DPM_Settings[PortNum].PE_SupportedSOP;
        TLV_add(&send_tlv, GUI_PARAM_SOP, 1, &settings);
        if(0 != length)
        {
          break;
        }
      }
#if defined(USBPD_REV30_SUPPORT)
    case GUI_PARAM_FASTROLESWAP :
      {
        /*FastRoleSwap*/
        uint8_t settings = DPM_Settings[PortNum].PE_PD3_Support.d.PE_FastRoleSwapSupport;
        TLV_add(&send_tlv, GUI_PARAM_FASTROLESWAP, 1, &settings);
        if(0 != length)
        {
          break;
        }
      }
#endif /*USBPD_REV30_SUPPORT*/
    case GUI_PARAM_DATAROLESWAP :
      {
        /*DataRoleSwap*/
        uint8_t settings = DPM_USER_Settings[PortNum].PE_DataSwap;
        TLV_add(&send_tlv, GUI_PARAM_DATAROLESWAP, 1, &settings);
        if(0 != length)
        {
          break;
        }
      }
    case GUI_PARAM_DEFAULTPOWERROLE :
      {
        /*DefaultPowerRole*/
        uint8_t settings = DPM_Settings[PortNum].PE_DefaultRole;
        TLV_add(&send_tlv, GUI_PARAM_DEFAULTPOWERROLE, 1, &settings);
        if(0 != length)
        {
          break;
        }
      }
    case GUI_PARAM_DRP_SUPPORT :
      {
        /*DRP_Support*/
        uint8_t settings = DPM_Settings[PortNum].PE_RoleSwap;
        TLV_add(&send_tlv, GUI_PARAM_DRP_SUPPORT, 1, &settings);
        if(0 != length)
        {
          break;
        }
      }
    case GUI_PARAM_CADROLETOGGLE :
      {
        /*CADRoleToggle*/
        uint8_t settings = DPM_Settings[PortNum].CAD_RoleToggle;
        TLV_add(&send_tlv, GUI_PARAM_CADROLETOGGLE, 1, &settings);
        if(0 != length)
        {
          break;
        }
      }
    case GUI_PARAM_PE_SCAP_HR :
      {
        /*PE_SCAP_HR*/
        uint8_t settings = DPM_Settings[PortNum].PE_CapscounterSupport;
        TLV_add(&send_tlv, GUI_PARAM_PE_SCAP_HR,  1, &settings);
        if(0 != length)
        {
          break;
        }
      }
    case GUI_PARAM_VCONNSWAP :
      {
        /*VConnSwap*/
        uint8_t settings = DPM_USER_Settings[PortNum].PE_VconnSwap;
        TLV_add(&send_tlv, GUI_PARAM_VCONNSWAP, 1, &settings);
        if(0 != length)
        {
          break;
        }
      }
#if defined(_VDM)
    case GUI_PARAM_VDM_SUPPORT :
      {
        /* DRP Support */
        uint8_t settings = DPM_Settings[PortNum].PE_VDMSupport;
        TLV_add(&send_tlv, GUI_PARAM_VDM_SUPPORT, 1, &settings);
        if(0 != length)
        {
          break;
        }
      }
#endif /*_VDM*/
    case GUI_PARAM_PING_SUPPORT :
      {
        /* Ping Support */
        uint8_t settings = DPM_Settings[PortNum].PE_PingSupport;
        TLV_add(&send_tlv, GUI_PARAM_PING_SUPPORT, 1, &settings);
        if(0 != length)
        {
          break;
        }
      }
#if defined(USBPD_REV30_SUPPORT)
    case GUI_PARAM_PPS_SUPPORT :
      {
        /* PD3 Support */
        uint16_t settings = DPM_Settings[PortNum].PE_PD3_Support.PD3_Support;
        TLV_add(&send_tlv, GUI_PARAM_PPS_SUPPORT, 2, (uint8_t*)&settings);
        if(0 != length)
        {
          break;
        }
      }
#endif /*USBPD_REV30_SUPPORT*/
#if defined(_SNK) || defined(_DRP)
    case GUI_PARAM_SNK_PDO :
      {
        uint8_t* list_pdo;
        uint8_t nb_pdo;
        nb_pdo    = PWR_Port_PDO_Storage[PortNum].SinkPDO.NumberOfPDO;
        list_pdo  = (uint8_t*)PWR_Port_PDO_Storage[PortNum].SinkPDO.ListOfPDO;
        TLV_add(&send_tlv, GUI_PARAM_SNK_PDO, nb_pdo * 4, list_pdo);
      }
      if(0 != length)
      {
        break;
      }
#endif /* _SNK || _DRP */
#if defined(_SRC) || defined(_DRP)
    case GUI_PARAM_SRC_PDO :
      {
        uint8_t* list_pdo;
        uint8_t nb_pdo;
        nb_pdo    = PWR_Port_PDO_Storage[PortNum].SourcePDO.NumberOfPDO;
        list_pdo  = (uint8_t*)PWR_Port_PDO_Storage[PortNum].SourcePDO.ListOfPDO;
        TLV_add(&send_tlv, GUI_PARAM_SRC_PDO, nb_pdo * 4, list_pdo);
      }
      if(0 != length)
      {
        break;
      }
#endif /* _SRC || _DRP */
    case GUI_PARAM_TDRP :
      {
        /* Calculate the current value of tDRP (value between 50ms and 100ms) */
        uint8_t value = DPM_USER_Settings[PortNum].CAD_tDRP;
        TLV_add(&send_tlv, GUI_PARAM_TDRP, 1, &value);
      }
      if(0 != length)
      {
        break;
      }
    case GUI_PARAM_DCSRC_DRP :
      {
        /* Calculate the current value of tDRP (value between 50ms and 100ms) */
        uint8_t value = DPM_USER_Settings[PortNum].CAD_dcSRC_DRP;
        TLV_add(&send_tlv, GUI_PARAM_DCSRC_DRP, 1, &value);
      }
      if(0 != length)
      {
        break;
      }
    case GUI_PARAM_RESPONDS_TO_DISCOV_SOP :
      {
        uint8_t settings = DPM_Settings[PortNum].PE_RespondsToDiscovSOP;
        TLV_add(&send_tlv, GUI_PARAM_RESPONDS_TO_DISCOV_SOP, 1, &settings);
      }
      if(0 != length)
      {
        break;
      }
    case GUI_PARAM_ATTEMPTS_DISCOV_SOP :
      {
        uint8_t settings = DPM_Settings[PortNum].PE_AttemptsDiscovSOP;
        TLV_add(&send_tlv, GUI_PARAM_ATTEMPTS_DISCOV_SOP, 1, &settings);
      }
      if(0 != length)
      {
        break;
      }
#if defined(_VDM)
    case GUI_PARAM_XID_SOP :
      {
        uint32_t value = DPM_VDM_Settings[PortNum].VDM_XID_SOP;
        TLV_add(&send_tlv, GUI_PARAM_XID_SOP, 4, (uint8_t*)&value);
      }
      if(0 != length)
      {
        break;
      }
    case GUI_PARAM_DATA_CAPABLE_AS_USB_HOST_SOP :
      {
        uint8_t settings = DPM_VDM_Settings[PortNum].VDM_USBHostSupport;
        TLV_add(&send_tlv, GUI_PARAM_DATA_CAPABLE_AS_USB_HOST_SOP, 1, &settings);
      }
      if(0 != length)
      {
        break;
      }
    case GUI_PARAM_DATA_CAPABLE_AS_USB_DEVICE_SOP :
      {
        uint8_t settings = DPM_VDM_Settings[PortNum].VDM_USBDeviceSupport;
        TLV_add(&send_tlv, GUI_PARAM_DATA_CAPABLE_AS_USB_DEVICE_SOP, 1, &settings);
      }
      if(0 != length)
      {
        break;
      }
    case GUI_PARAM_PRODUCT_TYPE_SOP :
      {
        uint8_t settings = DPM_VDM_Settings[PortNum].VDM_ProductTypeUFPorCP;
        TLV_add(&send_tlv, GUI_PARAM_PRODUCT_TYPE_SOP, 1, &settings);
      }
      if(0 != length)
      {
        break;
      }
    case GUI_PARAM_MODAL_OPERATION_SUPPORTED_SOP :
      {
        uint8_t settings = DPM_VDM_Settings[PortNum].VDM_ModalOperation;
        TLV_add(&send_tlv, GUI_PARAM_MODAL_OPERATION_SUPPORTED_SOP, 1, &settings);
      }
      if(0 != length)
      {
        break;
      }
    case GUI_PARAM_USB_VID_SOP :
      {
        uint16_t value = DPM_VDM_Settings[PortNum].VDM_USB_VID_SOP;
        TLV_add(&send_tlv, GUI_PARAM_USB_VID_SOP, 2, (uint8_t*)&value);
      }
      if(0 != length)
      {
        break;
      }
    case GUI_PARAM_PID_SOP :
      {
        uint16_t value = DPM_VDM_Settings[PortNum].VDM_PID_SOP;
        TLV_add(&send_tlv, GUI_PARAM_PID_SOP, 2, (uint8_t*)&value);
      }
      if(0 != length)
      {
        break;
      }
    case GUI_PARAM_BCDDEVICE_SOP :
      {
        uint16_t value = DPM_VDM_Settings[PortNum].VDM_bcdDevice_SOP;
        TLV_add(&send_tlv, GUI_PARAM_BCDDEVICE_SOP, 2, (uint8_t*)&value);
      }
      if(0 != length)
      {
        break;
      }
#endif /* _VDM */
    case GUI_PARAM_MEASUREREPORTING :
      {
        uint8_t settings = GUI_USER_Params[PortNum].u.MeasurementReporting;
        TLV_add(&send_tlv, GUI_PARAM_MEASUREREPORTING, 1, &settings);
      }
      if(0 != length)
      {
        break;
      }
#if defined(USBPD_REV30_SUPPORT)
#if _MANU_INFO
    case GUI_PARAM_MANUINFOPORT_VID :
      {
        uint8_t size = sizeof(DPM_USER_Settings[PortNum].DPM_ManuInfoPort.ManuString) + 4;
        TLV_add(&send_tlv, GUI_PARAM_MANUINFOPORT_VID, size, (uint8_t*)&DPM_USER_Settings[PortNum].DPM_ManuInfoPort);
      }
      if(0 != length)
      {
        break;
      }
#endif /* _MANU_INFO */
#endif /*USBPD_REV30_SUPPORT*/
    default:
      if(0 != length)
      {
        /* At least 1 parameter does not exist */
        TLV_deinit_encode(&send_tlv);
        Send_DpmConfigGetRej(PortNum, pEncodedMsg, GUI_REJ_DPM_INVALID_MESSAGE);
        goto __end;
      }
      break;
    }
    index++;
    /*
    Loop until we haven't processed each specified parameter.
    If there weren't any (length = 0), we exit the loop after one execution.
    */
  } while(index < length);

__end:
  TLV_deinit_encode(&send_tlv);
}

static void Send_DpmConfigGetRej(uint8_t PortNum, uint8_t *pEncodedMsg, USBPD_GUI_Reject_Reason RejectReason)
{
  TLV_ToSend_Data send_tlv;
  TLV_init_encode(&send_tlv, DPM_CONFIG_REJ, TLV_SIZE_MAX, pEncodedMsg);
  TLV_add(&send_tlv, RejectReason, 0, NULL);
  TLV_deinit_encode(&send_tlv);
}

static void Send_DpmMessageRej(uint8_t PortNum, uint8_t *pEncodedMsg, USBPD_GUI_Reject_Reason RejectReason)
{
  TLV_ToSend_Data send_tlv;
  TLV_init_encode(&send_tlv, DPM_MESSAGE_REJ, TLV_SIZE_MAX, pEncodedMsg);
  TLV_add(&send_tlv, RejectReason, 0, NULL);
  TLV_deinit_encode(&send_tlv);
}

static void Send_DpmRegisterReadCnf(uint8_t PortNum, uint8_t *pEncodedMsg, uint8_t param)
{
  TLV_ToSend_Data ToSendTLV;
  TLV_init_encode(&ToSendTLV, __GUI_SET_TAG_ID((PortNum +1), DPM_REGISTER_READ_CNF), TLV_SIZE_MAX, pEncodedMsg);

  uint8_t stateMachineArray[GUI_REG_NUMBER_OF_REGISTERS] = {
    GUI_REG_VENDOR_ID, GUI_REG_PRODUCT_ID, GUI_REG_DEVICE_ID, GUI_REG_USBTYPEC_REV,
    GUI_REG_USBPD_REV_VER, GUI_REG_USBPD_INTERFACE_REV, GUI_REG_ALERT, GUI_REG_ALERT_MASK,
    GUI_REG_POWER_STATUS_MASK, GUI_REG_FAULT_STATUS_MASK, GUI_REG_CONFIG_STANDARD_OUTPUT,
    GUI_REG_TCPC_CONTROL, GUI_REG_ROLE_CONTROL, GUI_REG_FAULT_CONTROL, GUI_REG_POWER_CONTROL,
    GUI_REG_CC_STATUS, GUI_REG_POWER_STATUS, GUI_REG_FAULT_STATUS, GUI_REG_COMMAND, GUI_REG_DECIVE_CAP1,
    GUI_REG_DECIVE_CAP2, GUI_REG_STD_INPUT_CAP, GUI_REG_STD_OUTPUT_CAP, GUI_REG_MSG_HEADER_INFO,
    GUI_REG_RX_DETECT, GUI_REG_RX_BYTE_COUNT, GUI_REG_RX_BUFFER_FRAME_TYPE, GUI_REG_RX_HEADER,
    GUI_REG_RX_DATA, GUI_REG_TRANSMIT, GUI_REG_TX_BYTE_COUNT, GUI_REG_TX_HEADER, GUI_REG_TX_DATA,
    GUI_REG_VBUS_VOLTAGE, GUI_REG_VBUS_SINK_DISCONNECT_THRESHOLD, GUI_REG_VBUS_STOP_DISCHARGE_THRESHOLD,
    GUI_REG_VBUS_VOLTAGE_ALARM_HI_CFG, GUI_REG_VBUS_VOLTAGE_ALARM_LO_CFG, GUI_REG_VENDOR_DATA
  };
  uint8_t counter = 0;

  /*  If param is not at 0xFF, then it means we only want one parameter.
      We then have to get out of the state machine after the first state */
  if(param != 0xFF)
  {
    *stateMachineArray = param;
  }

  do
  {
    switch(stateMachineArray[counter])
      /*
        In each case, the register needs to be read. Inclusion in the TLV which
        is going to be sent has to be done with TLV_add().
      */
    {
      case GUI_REG_VENDOR_ID :

              break;
      case GUI_REG_PRODUCT_ID :

              break;
      case GUI_REG_DEVICE_ID :

              break;
      case GUI_REG_USBTYPEC_REV :

              break;
      case GUI_REG_USBPD_REV_VER :

              break;
      case GUI_REG_USBPD_INTERFACE_REV :

              break;
      case GUI_REG_ALERT :

              break;
      case GUI_REG_ALERT_MASK :

              break;
      case GUI_REG_POWER_STATUS_MASK :

              break;
      case GUI_REG_FAULT_STATUS_MASK :

              break;
      case GUI_REG_CONFIG_STANDARD_OUTPUT :

              break;
      case GUI_REG_TCPC_CONTROL :

              break;
      case GUI_REG_ROLE_CONTROL :

              break;
      case GUI_REG_FAULT_CONTROL :

              break;
      case GUI_REG_POWER_CONTROL :

              break;
      case GUI_REG_CC_STATUS :

              break;
      case GUI_REG_POWER_STATUS :

              break;
      case GUI_REG_FAULT_STATUS :

              break;
      case GUI_REG_COMMAND :

              break;
      case GUI_REG_DECIVE_CAP1 :

              break;
      case GUI_REG_DECIVE_CAP2 :

              break;
      case GUI_REG_STD_INPUT_CAP :

              break;
      case GUI_REG_STD_OUTPUT_CAP :

              break;
      case GUI_REG_MSG_HEADER_INFO :

              break;
      case GUI_REG_RX_DETECT :

              break;
      case GUI_REG_RX_BYTE_COUNT :

              break;
      case GUI_REG_RX_BUFFER_FRAME_TYPE :

              break;
      case GUI_REG_RX_HEADER :

              break;
      case GUI_REG_RX_DATA :

              break;
      case GUI_REG_TRANSMIT :

              break;
      case GUI_REG_TX_BYTE_COUNT :

              break;
      case GUI_REG_TX_HEADER :

              break;
      case GUI_REG_TX_DATA :

              break;
      case GUI_REG_VBUS_VOLTAGE :

              break;
      case GUI_REG_VBUS_SINK_DISCONNECT_THRESHOLD :

              break;
      case GUI_REG_VBUS_STOP_DISCHARGE_THRESHOLD :

              break;
      case GUI_REG_VBUS_VOLTAGE_ALARM_HI_CFG :

              break;
      case GUI_REG_VBUS_VOLTAGE_ALARM_LO_CFG :

              break;
      case GUI_REG_VENDOR_DATA :

              break;
      default :
              break;
    }
    counter++;
  } while(counter < GUI_REG_NUMBER_OF_REGISTERS && param != 0xFF);

  TLV_deinit_encode(&ToSendTLV);
}

static void Send_DpmRegisterWriteCnf(uint8_t PortNum, uint8_t *pEncodedMsg, uint8_t *toProcess)
{
  TLV_Received_Data ToProcessTLV;
  TLV_init_decode(&ToProcessTLV, toProcess);

  uint8_t registerName;
  uint16_t registerSize;
  uint8_t *registerValue;

  TLV_get(&ToProcessTLV, &registerName, &registerSize, &registerValue);

  switch(registerName)
    /*
      Each case should write the data pointed by registerValue in the appropriate register,
      while taking into account the size of the data with registerSize.
    */
  {
      case GUI_REG_VENDOR_ID :

              break;
      case GUI_REG_PRODUCT_ID :

              break;
      case GUI_REG_DEVICE_ID :

              break;
      case GUI_REG_USBTYPEC_REV :

              break;
      case GUI_REG_USBPD_REV_VER :

              break;
      case GUI_REG_USBPD_INTERFACE_REV :

              break;
      case GUI_REG_ALERT :

              break;
      case GUI_REG_ALERT_MASK :

              break;
      case GUI_REG_POWER_STATUS_MASK :

              break;
      case GUI_REG_FAULT_STATUS_MASK :

              break;
      case GUI_REG_CONFIG_STANDARD_OUTPUT :

              break;
      case GUI_REG_TCPC_CONTROL :

              break;
      case GUI_REG_ROLE_CONTROL :

              break;
      case GUI_REG_FAULT_CONTROL :

              break;
      case GUI_REG_POWER_CONTROL :

              break;
      case GUI_REG_CC_STATUS :

              break;
      case GUI_REG_POWER_STATUS :

              break;
      case GUI_REG_FAULT_STATUS :

              break;
      case GUI_REG_COMMAND :

              break;
      case GUI_REG_DECIVE_CAP1 :

              break;
      case GUI_REG_DECIVE_CAP2 :

              break;
      case GUI_REG_STD_INPUT_CAP :

              break;
      case GUI_REG_STD_OUTPUT_CAP :

              break;
      case GUI_REG_MSG_HEADER_INFO :

              break;
      case GUI_REG_RX_DETECT :

              break;
      case GUI_REG_RX_BYTE_COUNT :

              break;
      case GUI_REG_RX_BUFFER_FRAME_TYPE :

              break;
      case GUI_REG_RX_HEADER :

              break;
      case GUI_REG_RX_DATA :

              break;
      case GUI_REG_TRANSMIT :

              break;
      case GUI_REG_TX_BYTE_COUNT :

              break;
      case GUI_REG_TX_HEADER :

              break;
      case GUI_REG_TX_DATA :

              break;
      case GUI_REG_VBUS_VOLTAGE :

              break;
      case GUI_REG_VBUS_SINK_DISCONNECT_THRESHOLD :

              break;
      case GUI_REG_VBUS_STOP_DISCHARGE_THRESHOLD :

              break;
      case GUI_REG_VBUS_VOLTAGE_ALARM_HI_CFG :

              break;
      case GUI_REG_VBUS_VOLTAGE_ALARM_LO_CFG :

              break;
      case GUI_REG_VENDOR_DATA :

              break;
      default :
              break;
  }
  TLV_deinit_decode(&ToProcessTLV);

  TLV_ToSend_Data ToSendTLV;
  /* Generation of the empty response, as it should be done*/
  TLV_init_encode(&ToSendTLV, __GUI_SET_TAG_ID((PortNum + 1), DPM_REGISTER_WRITE_CNF), 12, pEncodedMsg);
  TLV_deinit_encode(&ToSendTLV);
}

/**
  * @}
  */

/**
  * @}
  */
#endif /* _GUI_INTERFACE */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
