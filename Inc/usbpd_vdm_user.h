/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usbpd_vdm_user.h
  * @author  MCD Application Team
  * @brief   Header file for usbpd_vdm_user.c file
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

#ifndef __USBPD_VDM_USER_H_
#define __USBPD_VDM_USER_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/** @addtogroup STM32_USBPD_APPLICATION
  * @{
  */

/** @addtogroup STM32_USBPD_APPLICATION_VDM_USER
  * @{
  */

/* Exported typedef ----------------------------------------------------------*/
typedef struct
{
  uint32_t VDM_XID_SOP                      :32; /*!< A decimal number assigned by USB-IF before certification */
  uint32_t VDM_USB_VID_SOP                  :16; /*!< A unique 16-bit number, assigned to the Vendor by USB-IF. */
  uint32_t VDM_PID_SOP                      :16; /*!< A unique number assigned by the Vendor ID holder identifying the product. */
  uint32_t VDM_bcdDevice_SOP                :16; /*!< A unique number assigned by the Vendor ID holder containing identity information relevant to the release version of the product. */
  USBPD_ModalOp_TypeDef VDM_ModalOperation  : 1; /*!< Product support Modes based on @ref USBPD_ModalOp_TypeDef */
  USBPD_USBCapa_TypeDef VDM_USBHostSupport  : 1; /*!< Indicates whether the UUT is capable of enumerating USB Host */
  USBPD_USBCapa_TypeDef VDM_USBDeviceSupport: 1; /*!< Indicates whether the UUT is capable of enumerating USB Devices */
  USBPD_ProductType_TypeDef VDM_ProductTypeUFPorCP : 3; /*!< Product type UFP or CablePlug of the UUT based on @ref USBPD_ProductType_TypeDef */
  USBPD_ProductType_TypeDef VDM_ProductTypeDFP : 3; /*!< Product type DFP of the UUT based on @ref USBPD_ProductType_TypeDef */
  uint32_t Reserved3                        : 7; /*!< Reserved bits */
} USBPD_VDM_SettingsTypeDef;
/* USER CODE BEGIN typedef */

/* USER CODE END typedef */

/* Exported define -----------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
USBPD_StatusTypeDef USBPD_VDM_UserInit(uint8_t PortNum);
void                USBPD_VDM_UserReset(uint8_t PortNum);
/* USER CODE BEGIN Exported functions */
/* USER CODE END Exported functions */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __USBPD_VDM_USER_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
