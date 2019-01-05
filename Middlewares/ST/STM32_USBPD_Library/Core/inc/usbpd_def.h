/**
  ******************************************************************************
  * @file    usbpd_def.h
  * @author  MCD Application Team
  * @brief   Global defines for USB-PD libarary
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
#ifndef __USBPD_DEF_H_
#define __USBPD_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#if   defined ( __CC_ARM )
#define __ASM            __asm                                      /*!< asm keyword for ARM Compiler          */
#define __INLINE         __inline                                   /*!< inline keyword for ARM Compiler       */
#define __STATIC_INLINE  static __inline
#define __PACKEDSTRUCTBEGIN __packed struct
#define __PACKEDSTRUCTEND
#elif defined ( __ICCARM__ )
#define __ASM            __asm                                      /*!< asm keyword for IAR Compiler          */
#define __INLINE         inline                                     /*!< inline keyword for IAR Compiler. Only available in High optimization mode! */
#define __STATIC_INLINE  static inline
#define __PACKEDSTRUCTBEGIN __packed struct
#define __PACKEDSTRUCTEND
#elif defined ( __GNUC__ )
#define __ASM            __asm                                      /*!< asm keyword for GNU Compiler          */
#define __INLINE         inline                                     /*!< inline keyword for GNU Compiler       */
#define __STATIC_INLINE  static inline
#ifndef __packed
#define __packed __attribute__((__packed__))
#endif /* __packed */
#define __PACKEDSTRUCTBEGIN struct
#define __PACKEDSTRUCTEND   __packed
#endif

#include <stdint.h>
#include <stddef.h>

#if !defined (_SIMULATOR)

/* Check if USBPD_CMSIS_CORE_VERSION is defined in compilation options. if not set it to default CMSIS Core v4.5 */
#if !defined(USBPD_CMSIS_CORE_VERSION)
#define USBPD_CMSIS_CORE_VERSION 4
#endif
#if (USBPD_CMSIS_CORE_VERSION == 5)
#include "cmsis_compiler.h"
#else
#include "core_cmFunc.h"
#endif
  
#endif /* _SIMULATOR */

/** @addtogroup STM32_USBPD_LIBRARY
  * @{
  */

/** @addtogroup USBPD_CORE
  * @{
  */

/** @addtogroup USBPD_CORE_DEF
  * @{
  */

/* Internal macros --------------------------------------------------------*/
/** @defgroup USBPD_CORE_DEF_Exported_Macros USBPD CORE DEF Exported Macros
  * @{
  */
#define DIV_ROUND_UP(x, y) (((x) + ((y) - 1)) / (y))
#define MV2ADC(__X__)           ( (__X__*4095) / 3300 )
#define ADC2MV(__X__)           ( (__X__*3300) / 4095 )

/* Macros for integer division with various rounding variants default integer
   division rounds down. */
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))

#define USPBPD_WRITE32(addr,data)   do {                                                                       \
                                         uint8_t bindex;                                                       \
                                         for(bindex = 0; bindex < 4; bindex++)                                 \
                                         {                                                                     \
                                           ((uint8_t *)addr)[bindex] = ((data >> (8 * bindex)) & 0x000000FF);  \
                                         }                                                                     \
                                       } while(0);

#define LE16(addr) (((uint16_t)(*((uint8_t *)(addr))))\
                                             + (((uint16_t)(*(((uint8_t *)(addr)) + 1))) << 8))

#define LE32(addr) ((((uint32_t)(*(((uint8_t *)(addr)) + 0))) + \
                                              (((uint32_t)(*(((uint8_t *)(addr)) + 1))) << 8) + \
                                              (((uint32_t)(*(((uint8_t *)(addr)) + 2))) << 16) + \
                                              (((uint32_t)(*(((uint8_t *)(addr)) + 3))) << 24)))

/**
  * @}
  */

/* Exported defines   --------------------------------------------------------*/
/** @defgroup USBPD_CORE_DEF_Exported_Defines USBPD CORE DEF Exported Defines
  * @{
  */
#define USBPD_TRUE  1
#define USBPD_FALSE 0

#define __DEBUG_PESTATEMACHINE

/** @defgroup USBPD_CORE_DEF_Exported_Defines_Swiches USBPD Compilations switches
  * @brief List of compilation switches which can be used to reduce size of the CORE library
  * @{
  */

/* Default Switch */
#define USBPDCORE_GOTOMIN
#define USBPDCORE_BIST
#define USBPDCORE_GETSNKCAP
#define USBPDCORE_GETSRCCAP
#define USBPDCORE_GIVESNKCAP

#if defined(USBPDCORE_LIB_PD3_FULL) || defined(USBPDCORE_LIB_PD3_CONFIG_1) || defined(USBPD_TCPM_LIB_PD3_FULL) || defined(USBPD_TCPM_LIB_PD3_CONFIG_1)
/*
   USBPDCORE_LIB_PD3_FULL
   USBPDCORE_LIB_PD3_CONFIG_1  : NO VDM
   USBPD_TCPM_LIB_PD3_FULL     : PD3.0 TCPM FULL
   USBPD_TCPM_LIB_PD3_CONFIG_1 : PD3.0 TCPM FULL without VDM
*/
#define USBPDCORE_SRC
#define USBPDCORE_SNK
#define USBPDCORE_DRP

#if defined(USBPDCORE_LIB_PD3_FULL) || defined(USBPD_TCPM_LIB_PD3_FULL)
#define USBPDCORE_SVDM
#endif /* USBPDCORE_LIB_PD3_FULL || USBPD_TCPM_LIB_PD3_FULL */

#define USBPDCORE_UVDM
#define USBPD_REV30_SUPPORT
#define USBPDCORE_CABLE
#define USBPDCORE_FASTROLESWAP
#define USBPDCORE_PPS
#define USBPDCORE_ALERT
#define USBPDCORE_SRC_CAPA_EXT
#define USBPDCORE_STATUS
#define USBPDCORE_BATTERY
#define USBPDCORE_MANU_INFO
#define USBPDCORE_SECURITY_MSG
#define USBPDCORE_FWUPD
#define USBPDCORE_COUNTRY_MSG
#define USBPDCORE_VCONN_SUPPORT
#define USBPDCORE_DATA_SWAP
#define USBPDCORE_ERROR_RECOVERY
#define USBPDCORE_UNCHUNCKED_MODE
#define USBPDCORE_PING_SUPPORT

#if defined(USBPD_TCPM_LIB_PD3_FULL) || defined(USBPD_TCPM_LIB_PD3_CONFIG_1)
#define USBPDCORE_TCPM_SUPPORT
#endif /* TCPM */

#endif /* PD3.0 */

/* List of compilation switches which can be used to reduce size of the CORE librairy */
#if defined(USBPDCORE_LIB_PD2_FULL) || defined(USBPDCORE_LIB_PD2_CONFIG_1) || defined(USBPDCORE_LIB_PD2_CONFIG_MINSRC) || defined(USBPDCORE_LIB_PD2_CONFIG_MINSNK)  || defined(USBPD_TCPM_LIB_PD2_FULL) || defined(USBPD_TCPM_LIB_PD2_CONFIG_1) || defined(USBPD_TCPM_LIB_PD2_MINSRC) || defined(USBPD_TCPM_LIB_PD2_MINSNK)
/*
   USBPDCORE_LIB_PD2_FULL
   USBPDCORE_LIB_PD2_CONFIG_1 : NO VDM
   USBPDCORE_LIB_PD2_CONFIG_MINSRC : ONLY SRC and  NO option
   USBPDCORE_LIB_PD2_CONFIG_MINSNK : ONLY SNK, and NO option
   USBPD_TCPM_LIB_PD2_FULL     : PD2.0 TCPM FULL
   USBPD_TCPM_LIB_PD2_CONFIG_1 : PD2.0 TCPM FULL without VDM
   USBPD_TCPM_LIB_PD2_MINSRC : PD2.0 TCPM Only SRC
   USBPD_TCPM_LIB_PD2_MINSNK : PD2.0 TCPM Only SNK
*/
#define USBPD_REV20_SUPPORT

#if !defined(USBPDCORE_LIB_PD2_CONFIG_MINSNK) && !defined(USBPD_TCPM_LIB_PD2_MINSNK)
#define USBPDCORE_SRC
#endif

#if !defined(USBPDCORE_LIB_PD2_CONFIG_MINSRC) && !defined(USBPD_TCPM_LIB_PD2_MINSRC)
#define USBPDCORE_SNK
#endif

#if defined(USBPDCORE_LIB_PD2_CONFIG_MINSRC) || defined(USBPDCORE_LIB_PD2_CONFIG_MINSNK) || defined(USBPD_TCPM_LIB_PD2_MINSRC) || defined(USBPD_TCPM_LIB_PD2_MINSNK)
#else
#define USBPDCORE_DRP
#define USBPDCORE_VCONN_SUPPORT
#define USBPDCORE_DATA_SWAP
#define USBPDCORE_CABLE
#define USBPDCORE_ERROR_RECOVERY
#define USBPDCORE_UVDM
#endif

#if defined(USBPDCORE_LIB_PD2_FULL) || defined(USBPD_TCPM_LIB_PD2_FULL)
#define USBPDCORE_SVDM
#endif /* USBPDCORE_LIB_PD3_FULL || USBPD_TCPM_LIB_PD2_FULL */

#if defined(USBPD_TCPM_LIB_PD2_FULL) || defined(USBPD_TCPM_LIB_PD2_CONFIG_1) || defined(USBPD_TCPM_LIB_PD2_MINSRC) || defined(USBPD_TCPM_LIB_PD2_MINSNK)
#define USBPDCORE_TCPM_SUPPORT
#endif /* TCPM */

#endif /* PD2.0 */

#if defined(USBPDCORE_LIB_PD3_CONFIG_2)
#undef USBPDCORE_GOTOMIN
#undef USBPDCORE_BIST
#undef USBPDCORE_GETSNKCAP
#undef USBPDCORE_GETSRCCAP
#undef USBPDCORE_GIVESNKCAP

#define USBPDCORE_SNK
#define USBPD_REV30_SUPPORT
#define USBPDCORE_FWUPD
#define USBPDCORE_UVDM
#define USBPDCORE_UNCHUNCKED_MODE
#endif /* USBPDCORE_LIB_PD3_CONFIG_2 */

/* _LIB_ID definition */
/*
  _LIB_ID constructs like this: 0xXYVVVWWW
  * X: 3 (PD3.0) or 2 (PD2.0)
  * Y: 0 (CORE) or 1 (TCPM)
  * VVV: Stack version (ex 200 for Stack 2.0.0)
  * WWW: 0 (FULL VERSION) or config_x
*/
/* Defines for PD revision */
#define LIB_PD_VERSION_POS  28u
#define LIB_PD_VERSION_MSK  (0xFu << LIB_PD_VERSION_POS)
#define LIB_PD2             (2u   << LIB_PD_VERSION_POS)
#define LIB_PD3             (3u   << LIB_PD_VERSION_POS)
/* Defines for CORE or TCPM */
#define LIB_CORE_TCPM_POS   24u
#define LIB_CORE_TCPM_MSK   (0xFu << LIB_CORE_TCPM_POS)
#define LIB_CORE            (0u   << LIB_CORE_TCPM_POS)
#define LIB_TCPM            (1u   << LIB_CORE_TCPM_POS)
/* Defines for STACK version */
#define LIB_STACK_VER_POS   12u
#define LIB_STACK_VER_MSK   (0xFFFu << LIB_STACK_VER_POS)
#define LIB_STACK_VER       (0x210  << LIB_STACK_VER_POS)
/* Defines for configuration */
#define LIB_CONFIG_MSK      0xFFFu
#define LIB_FULL            0x000u
#define LIB_CONFIG_1        0x001u
#define LIB_CONFIG_MINSRC   0x002u
#define LIB_CONFIG_MINSNK   0x004u
#define LIB_CONFIG_2        0x010u

#ifdef USBPDCORE_LIB_PD3_FULL
#define _LIB_ID   (LIB_PD3 | LIB_CORE | LIB_STACK_VER | LIB_FULL)
#endif
#ifdef USBPDCORE_LIB_PD3_CONFIG_1
#define _LIB_ID   (LIB_PD3 | LIB_CORE | LIB_STACK_VER | LIB_CONFIG_1)
#endif
#ifdef USBPDCORE_LIB_PD2_FULL
#define _LIB_ID   (LIB_PD2 | LIB_CORE | LIB_STACK_VER | LIB_FULL)
#endif
#ifdef USBPDCORE_LIB_PD2_CONFIG_1
#define _LIB_ID   (LIB_PD2 | LIB_CORE | LIB_STACK_VER | LIB_CONFIG_1)
#endif
#ifdef USBPDCORE_LIB_PD2_CONFIG_MINSRC
#define _LIB_ID   (LIB_PD2 | LIB_CORE | LIB_STACK_VER | LIB_CONFIG_MINSRC)
#endif
#ifdef USBPDCORE_LIB_PD2_CONFIG_MINSNK
#define _LIB_ID   (LIB_PD2 | LIB_CORE | LIB_STACK_VER | LIB_CONFIG_MINSNK)
#endif
#ifdef USBPD_TCPM_LIB_PD2_FULL
#define _LIB_ID   (LIB_PD2 | LIB_TCPM | LIB_STACK_VER | LIB_FULL)
#endif
#ifdef USBPD_TCPM_LIB_PD2_MINSRC
#define _LIB_ID   (LIB_PD2 | LIB_TCPM | LIB_STACK_VER | LIB_CONFIG_MINSRC)
#endif
#ifdef USBPD_TCPM_LIB_PD2_MINSNK
#define _LIB_ID   (LIB_PD2 | LIB_TCPM | LIB_STACK_VER | LIB_CONFIG_MINSNK)
#endif
#ifdef USBPD_TCPM_LIB_PD2_CONFIG_1
#define _LIB_ID   (LIB_PD2 | LIB_TCPM | LIB_STACK_VER | LIB_CONFIG_1)
#endif
#ifdef USBPD_TCPM_LIB_PD3_CONFIG_1
#define _LIB_ID   (LIB_PD3 | LIB_TCPM | LIB_STACK_VER | LIB_CONFIG_1)
#endif
#ifdef USBPD_TCPM_LIB_PD3_FULL
#define _LIB_ID   (LIB_PD3 | LIB_TCPM | LIB_STACK_VER | LIB_FULL)
#endif
#if defined(USBPDCORE_LIB_PD3_CONFIG_2)
#define _LIB_ID   (LIB_PD3 | LIB_TCPM | LIB_STACK_VER | LIB_CONFIG_2)
#endif
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup USBPD_CORE_VDM_Exported_Defines USBPD CORE VDM Exported Defines
  * @{
  */
#define MAX_MODES_PER_SVID  6
/**
  * @}
  */

#define USBPD_PORT_0                     (0U)              /*!< Port 0 identifier */
#define USBPD_PORT_1                     (1U)              /*!< Port 1 identifier */

#define USBPD_MAX_NB_PDO                 (7U)              /*!< Maximum number of supported Power Data Objects: fix by the Specification */
#define BIST_CARRIER_MODE_MS             (50U)             /*!< Time in ms of the BIST signal*/

/*
 * Maximum size of a Power Delivery packet (in bits on the wire) :
 *    16-bit header + 0..7 32-bit data objects  (+ 4b5b encoding)
 *    64-bit preamble + SOP (4x 5b) + header (16-bit) + message in 4b5b + 32-bit CRC  + EOP (1x 5b)
 * =  64bit           + 4*5bit      + 16bit * 5/4 + 7 * 32bit * 5/4 + 32bit * 5/4 + 5
 */
#define PHY_BIT_LEN             ((uint16_t)429)
#define PHY_MAX_RAW_SIZE        ((uint16_t)((PHY_BIT_LEN*2) + 3))
#define PHY_MAX_RAW_BYTE_SIZE   ((uint8_t)60) /* PHY_BIT_LEN / 8 + SAFE Bytes */

/** @defgroup USBPD_PDO_Index_And_Mask_Constants Index and Mask constants used in PDO bits handling
  * @{
  */
#define USBPD_PDO_TYPE_Pos                           (30U)                                                     /*!< PDO Type bits position                          */
#define USBPD_PDO_TYPE_Msk                           (0x3U << USBPD_PDO_TYPE_Pos)                              /*!< PDO Type bits mask : 0xC0000000                 */
#define USBPD_PDO_TYPE_FIXED                         (uint32_t)(USBPD_CORE_PDO_TYPE_FIXED << USBPD_PDO_TYPE_Pos)         /*!< PDO Type = FIXED                                */
#define USBPD_PDO_TYPE_BATTERY                       (uint32_t)(USBPD_CORE_PDO_TYPE_BATTERY << USBPD_PDO_TYPE_Pos)       /*!< PDO Type = BATTERY                              */
#define USBPD_PDO_TYPE_VARIABLE                      (uint32_t)(USBPD_CORE_PDO_TYPE_VARIABLE << USBPD_PDO_TYPE_Pos)      /*!< PDO Type = VARIABLE                             */
#if defined(USBPD_REV30_SUPPORT)
#define USBPD_PDO_TYPE_APDO                          (uint32_t)(USBPD_CORE_PDO_TYPE_APDO<< USBPD_PDO_TYPE_Pos)           /*!< PDO Type = APDO                                 */
#endif /* USBPD_REV30_SUPPORT */

/* Source Fixed type PDO elments */
#define USBPD_PDO_SRC_FIXED_DRP_SUPPORT_Pos          (29U)                                                     /*!< DRP Support bit position                        */
#define USBPD_PDO_SRC_FIXED_DRP_SUPPORT_Msk          (0x1U << USBPD_PDO_SRC_FIXED_DRP_SUPPORT_Pos)             /*!< DRP Support bit mask : 0x20000000               */
#define USBPD_PDO_SRC_FIXED_DRP_NOT_SUPPORTED        (0U)                                                      /*!< DRP not supported                               */
#define USBPD_PDO_SRC_FIXED_DRP_SUPPORTED            USBPD_PDO_SRC_FIXED_DRP_SUPPORT_Msk                       /*!< DRP supported                                   */

#define USBPD_PDO_SRC_FIXED_USBSUSPEND_Pos           (28U)                                                     /*!< USB Suspend Support bit position                */
#define USBPD_PDO_SRC_FIXED_USBSUSPEND_Msk           (0x1U << USBPD_PDO_SRC_FIXED_USBSUSPEND_Pos)              /*!< USB Suspend Support bit mask : 0x10000000       */
#define USBPD_PDO_SRC_FIXED_USBSUSPEND_NOT_SUPPORTED (0U)                                                      /*!< USB Suspend not supported                       */
#define USBPD_PDO_SRC_FIXED_USBSUSPEND_SUPPORTED     USBPD_PDO_SRC_FIXED_USBSUSPEND_Msk                        /*!< USB Suspend supported                           */

#define USBPD_PDO_SRC_FIXED_EXT_POWER_Pos            (27U)                                                     /*!< External Power available bit position           */
#define USBPD_PDO_SRC_FIXED_EXT_POWER_Msk            (0x1U << USBPD_PDO_SRC_FIXED_EXT_POWER_Pos)               /*!< External Power available bit mask : 0x08000000  */
#define USBPD_PDO_SRC_FIXED_EXT_POWER_NOT_AVAILABLE  (0U)                                                      /*!< External Power not available                    */
#define USBPD_PDO_SRC_FIXED_EXT_POWER_AVAILABLE      USBPD_PDO_SRC_FIXED_EXT_POWER_Msk                         /*!< External Power available                        */

#define USBPD_PDO_SRC_FIXED_USBCOMM_Pos              (26U)                                                     /*!< USB Communication Support bit position          */
#define USBPD_PDO_SRC_FIXED_USBCOMM_Msk              (0x1U << USBPD_PDO_SRC_FIXED_USBCOMM_Pos)                 /*!< USB Communication Support bit mask : 0x04000000 */
#define USBPD_PDO_SRC_FIXED_USBCOMM_NOT_SUPPORTED    (0U)                                                      /*!< USB Communication not supported                 */
#define USBPD_PDO_SRC_FIXED_USBCOMM_SUPPORTED        USBPD_PDO_SRC_FIXED_USBCOMM_Msk                           /*!< USB Communication supported                     */

#define USBPD_PDO_SRC_FIXED_DRD_SUPPORT_Pos          (25U)                                                     /*!< Dual Role Data Support bit position             */
#define USBPD_PDO_SRC_FIXED_DRD_SUPPORT_Msk          (0x1U << USBPD_PDO_SRC_FIXED_DRD_SUPPORT_Pos)             /*!< Dual Role Data Support bit mask : 0x02000000    */
#define USBPD_PDO_SRC_FIXED_DRD_NOT_SUPPORTED        (0U)                                                      /*!< Dual Role Data not supported                    */
#define USBPD_PDO_SRC_FIXED_DRD_SUPPORTED            USBPD_PDO_SRC_FIXED_DRD_SUPPORT_Msk                       /*!< Dual Role Data supported                        */

#if defined(USBPD_REV30_SUPPORT)
#define USBPD_PDO_SRC_FIXED_UNCHUNK_SUPPORT_Pos      (24U)                                                     /*!< Unchunked Extended Messages Support bit position             */
#define USBPD_PDO_SRC_FIXED_UNCHUNK_SUPPORT_Msk      (0x1U << USBPD_PDO_SRC_FIXED_UNCHUNK_SUPPORT_Pos)         /*!< Unchunked Extended Messages Support bit mask : 0x01000000    */
#define USBPD_PDO_SRC_FIXED_UNCHUNK_NOT_SUPPORTED    (0U)                                                      /*!< Unchunked Extended Messages not supported                    */
#define USBPD_PDO_SRC_FIXED_UNCHUNK_SUPPORTED        USBPD_PDO_SRC_FIXED_UNCHUNK_SUPPORT_Msk                   /*!< Unchunked Extended Messages supported                        */
#endif /* USBPD_REV30_SUPPORT */

#define USBPD_PDO_SRC_FIXED_PEAKCURRENT_Pos          (20U)                                                             /*!< Peak Current info bits position            */
#define USBPD_PDO_SRC_FIXED_PEAKCURRENT_Msk          (0x3U << USBPD_PDO_SRC_FIXED_PEAKCURRENT_Pos)                     /*!< Peak Current info bits mask : 0x00300000   */
#define USBPD_PDO_SRC_FIXED_PEAKCURRENT_EQUAL        (USBPD_CORE_PDO_PEAKEQUAL << USBPD_PDO_SRC_FIXED_PEAKCURRENT_Pos) /*!< Peak Current info : Equal to Ioc           */
#define USBPD_PDO_SRC_FIXED_PEAKCURRENT_OVER1        (USBPD_CORE_PDO_PEAKOVER1 << USBPD_PDO_SRC_FIXED_PEAKCURRENT_Pos) /*!< Peak Current info : Overload Cap 1         */
#define USBPD_PDO_SRC_FIXED_PEAKCURRENT_OVER2        (USBPD_CORE_PDO_PEAKOVER2 << USBPD_PDO_SRC_FIXED_PEAKCURRENT_Pos) /*!< Peak Current info : Overload Cap 2         */
#define USBPD_PDO_SRC_FIXED_PEAKCURRENT_OVER3        (USBPD_CORE_PDO_PEAKOVER3 << USBPD_PDO_SRC_FIXED_PEAKCURRENT_Pos) /*!< Peak Current info : Overload Cap 3         */

#define USBPD_PDO_SRC_FIXED_VOLTAGE_Pos              (10U)                                                     /*!< Voltage in 50 mV units bits position               */
#define USBPD_PDO_SRC_FIXED_VOLTAGE_Msk              (0x3FFU << USBPD_PDO_SRC_FIXED_VOLTAGE_Pos)               /*!< Voltage in 50 mV units bits mask : 0x000FFC00      */

#define USBPD_PDO_SRC_FIXED_MAX_CURRENT_Pos          (0U)                                                      /*!< Max current in 10 mA units bits position           */
#define USBPD_PDO_SRC_FIXED_MAX_CURRENT_Msk          (0x3FFU << USBPD_PDO_SRC_FIXED_MAX_CURRENT_Pos)           /*!< Max current in 10 mA units bits mask : 0x000003FF  */

/* Source Variable type PDO elments */
#define USBPD_PDO_SRC_VARIABLE_MAX_VOLTAGE_Pos       (20U)                                                     /*!< Max Voltage in 50 mV units bits position           */
#define USBPD_PDO_SRC_VARIABLE_MAX_VOLTAGE_Msk       (0x3FFU << USBPD_PDO_SRC_VARIABLE_MAX_VOLTAGE_Pos)        /*!< Max Voltage in 50 mV units bits mask : 0x3FF00000  */

#define USBPD_PDO_SRC_VARIABLE_MIN_VOLTAGE_Pos       (10U)                                                     /*!< Max Voltage in 50 mV units bits position           */
#define USBPD_PDO_SRC_VARIABLE_MIN_VOLTAGE_Msk       (0x3FFU << USBPD_PDO_SRC_VARIABLE_MIN_VOLTAGE_Pos)        /*!< Max Voltage in 50 mV units bits mask : 0x000FFC00  */

#define USBPD_PDO_SRC_VARIABLE_MAX_CURRENT_Pos       (0U)                                                      /*!< Max current in 10 mA units bits position           */
#define USBPD_PDO_SRC_VARIABLE_MAX_CURRENT_Msk       (0x3FFU << USBPD_PDO_SRC_VARIABLE_MAX_CURRENT_Pos)        /*!< Max current in 10 mA units bits mask : 0x000003FF  */

/* Source Battery type PDO elments */
#define USBPD_PDO_SRC_BATTERY_MAX_VOLTAGE_Pos        (20U)                                                     /*!< Max Voltage in 50 mV units bits position           */
#define USBPD_PDO_SRC_BATTERY_MAX_VOLTAGE_Msk        (0x3FFU << USBPD_PDO_SRC_BATTERY_MAX_VOLTAGE_Pos)         /*!< Max Voltage in 50 mV units bits mask : 0x3FF00000  */

#define USBPD_PDO_SRC_BATTERY_MIN_VOLTAGE_Pos        (10U)                                                     /*!< Max Voltage in 50 mV units bits position           */
#define USBPD_PDO_SRC_BATTERY_MIN_VOLTAGE_Msk        (0x3FFU << USBPD_PDO_SRC_BATTERY_MIN_VOLTAGE_Pos)         /*!< Max Voltage in 50 mV units bits mask : 0x000FFC00  */

#define USBPD_PDO_SRC_BATTERY_MAX_POWER_Pos          (0U)                                                      /*!< Max allowable power in 250mW units bits position          */
#define USBPD_PDO_SRC_BATTERY_MAX_POWER_Msk          (0x3FFU << USBPD_PDO_SRC_BATTERY_MAX_POWER_Pos)           /*!< Max allowable power in 250mW units bits mask : 0x000003FF */

/* Sink Fixed type PDO elments */
#define USBPD_PDO_SNK_FIXED_DRP_SUPPORT_Pos          (29U)                                                     /*!< DRP Support bit position                        */
#define USBPD_PDO_SNK_FIXED_DRP_SUPPORT_Msk          (0x1U << USBPD_PDO_SNK_FIXED_DRP_SUPPORT_Pos)             /*!< DRP Support bit mask : 0x20000000               */
#define USBPD_PDO_SNK_FIXED_DRP_NOT_SUPPORTED        (0U)                                                      /*!< DRP not supported                               */
#define USBPD_PDO_SNK_FIXED_DRP_SUPPORTED            USBPD_PDO_SNK_FIXED_DRP_SUPPORT_Msk                       /*!< DRP supported                                   */

#define USBPD_PDO_SNK_FIXED_HIGHERCAPAB_Pos           (28U)                                                    /*!< Higher capability support bit position          */
#define USBPD_PDO_SNK_FIXED_HIGHERCAPAB_Msk           (0x1U << USBPD_PDO_SNK_FIXED_HIGHERCAPAB_Pos)            /*!< Higher capability support bit mask : 0x10000000 */
#define USBPD_PDO_SNK_FIXED_HIGHERCAPAB_NOT_SUPPORTED (0U)                                                     /*!< Higher capability not supported                 */
#define USBPD_PDO_SNK_FIXED_HIGHERCAPAB_SUPPORTED     USBPD_PDO_SNK_FIXED_HIGHERCAPAB_Msk                      /*!< Higher capability supported                     */

#define USBPD_PDO_SNK_FIXED_EXT_POWER_Pos            (27U)                                                     /*!< External Power available bit position           */
#define USBPD_PDO_SNK_FIXED_EXT_POWER_Msk            (0x1U << USBPD_PDO_SNK_FIXED_EXT_POWER_Pos)               /*!< External Power available bit mask : 0x08000000  */
#define USBPD_PDO_SNK_FIXED_EXT_POWER_NOT_AVAILABLE  (0U)                                                      /*!< External Power not available                    */
#define USBPD_PDO_SNK_FIXED_EXT_POWER_AVAILABLE      USBPD_PDO_SNK_FIXED_EXT_POWER_Msk                         /*!< External Power available                        */

#define USBPD_PDO_SNK_FIXED_USBCOMM_Pos              (26U)                                                     /*!< USB Communication Support bit position          */
#define USBPD_PDO_SNK_FIXED_USBCOMM_Msk              (0x1U << USBPD_PDO_SNK_FIXED_USBCOMM_Pos)                 /*!< USB Communication Support bit mask : 0x04000000 */
#define USBPD_PDO_SNK_FIXED_USBCOMM_NOT_SUPPORTED    (0U)                                                      /*!< USB Communication not supported                 */
#define USBPD_PDO_SNK_FIXED_USBCOMM_SUPPORTED        USBPD_PDO_SNK_FIXED_USBCOMM_Msk                           /*!< USB Communication supported                     */

#define USBPD_PDO_SNK_FIXED_DRD_SUPPORT_Pos          (25U)                                                     /*!< Dual Role Data Support bit position             */
#define USBPD_PDO_SNK_FIXED_DRD_SUPPORT_Msk          (0x1U << USBPD_PDO_SNK_FIXED_DRD_SUPPORT_Pos)             /*!< Dual Role Data Support bit mask : 0x02000000    */
#define USBPD_PDO_SNK_FIXED_DRD_NOT_SUPPORTED        (0U)                                                      /*!< Dual Role Data not supported                    */
#define USBPD_PDO_SNK_FIXED_DRD_SUPPORTED            USBPD_PDO_SNK_FIXED_DRD_SUPPORT_Msk                       /*!< Dual Role Data supported                        */

#if defined(USBPD_REV30_SUPPORT)
#define USBPD_PDO_SNK_FIXED_FRS_SUPPORT_Pos          (23U)                                                     /*!< Fast Role Swap required Current bit position             */
#define USBPD_PDO_SNK_FIXED_FRS_SUPPORT_Msk          (0x3U << USBPD_PDO_SNK_FIXED_FRS_SUPPORT_Pos)             /*!< Fast Role Swap required Current bit mask : 0x01800000    */
#define USBPD_PDO_SNK_FIXED_FRS_NOT_SUPPORTED        (0U)                                                      /*!< Fast Role Swap not supported                             */
#define USBPD_PDO_SNK_FIXED_FRS_DEFAULT              (USBPD_CORE_PDO_FRS_DEFAULT_USB_POWER << USBPD_PDO_SNK_FIXED_FRS_SUPPORT_Pos)  /*!< Fast Role Swap required default USB power  */
#define USBPD_PDO_SNK_FIXED_FRS_1_5A                 (USBPD_CORE_PDO_FRS_1_5A_5V << USBPD_PDO_SNK_FIXED_FRS_SUPPORT_Pos)            /*!< Fast Role Swap 1.5A at 5V                  */
#define USBPD_PDO_SNK_FIXED_FRS_3A                   (USBPD_CORE_PDO_FRS_3A_5V << USBPD_PDO_SNK_FIXED_FRS_SUPPORT_Pos)              /*!< Fast Role Swap 3A at 5V                    */
#endif /* USBPD_REV30_SUPPORT */

#define USBPD_PDO_SNK_FIXED_VOLTAGE_Pos              (10U)                                                     /*!< Voltage in 50 mV units bits position               */
#define USBPD_PDO_SNK_FIXED_VOLTAGE_Msk              (0x3FFU << USBPD_PDO_SNK_FIXED_VOLTAGE_Pos)               /*!< Voltage in 50 mV units bits mask : 0x000FFC00      */

#define USBPD_PDO_SNK_FIXED_OP_CURRENT_Pos           (0U)                                                      /*!< Operational current in 10 mA units bits position           */
#define USBPD_PDO_SNK_FIXED_OP_CURRENT_Msk           (0x3FFU << USBPD_PDO_SNK_FIXED_OP_CURRENT_Pos)            /*!< Operational current in 10 mA units bits mask : 0x000003FF  */

/* Sink Variable type PDO elments */
#define USBPD_PDO_SNK_VARIABLE_MAX_VOLTAGE_Pos       (20U)                                                     /*!< Max Voltage in 50 mV units bits position           */
#define USBPD_PDO_SNK_VARIABLE_MAX_VOLTAGE_Msk       (0x3FFU << USBPD_PDO_SNK_VARIABLE_MAX_VOLTAGE_Pos)        /*!< Max Voltage in 50 mV units bits mask : 0x3FF00000  */

#define USBPD_PDO_SNK_VARIABLE_MIN_VOLTAGE_Pos       (10U)                                                     /*!< Max Voltage in 50 mV units bits position           */
#define USBPD_PDO_SNK_VARIABLE_MIN_VOLTAGE_Msk       (0x3FFU << USBPD_PDO_SNK_VARIABLE_MIN_VOLTAGE_Pos)        /*!< Max Voltage in 50 mV units bits mask : 0x000FFC00  */

#define USBPD_PDO_SNK_VARIABLE_OP_CURRENT_Pos        (0U)                                                      /*!< Operational current in 10 mA units bits position           */
#define USBPD_PDO_SNK_VARIABLE_OP_CURRENT_Msk        (0x3FFU << USBPD_PDO_SNK_VARIABLE_OP_CURRENT_Pos)         /*!< Operational current in 10 mA units bits mask : 0x000003FF  */

/* Sink Battery type PDO elments */
#define USBPD_PDO_SNK_BATTERY_MAX_VOLTAGE_Pos        (20U)                                                     /*!< Max Voltage in 50 mV units bits position           */
#define USBPD_PDO_SNK_BATTERY_MAX_VOLTAGE_Msk        (0x3FFU << USBPD_PDO_SNK_BATTERY_MAX_VOLTAGE_Pos)         /*!< Max Voltage in 50 mV units bits mask : 0x3FF00000  */

#define USBPD_PDO_SNK_BATTERY_MIN_VOLTAGE_Pos        (10U)                                                     /*!< Max Voltage in 50 mV units bits position           */
#define USBPD_PDO_SNK_BATTERY_MIN_VOLTAGE_Msk        (0x3FFU << USBPD_PDO_SNK_BATTERY_MIN_VOLTAGE_Pos)         /*!< Max Voltage in 50 mV units bits mask : 0x000FFC00  */

#define USBPD_PDO_SNK_BATTERY_OP_POWER_Pos           (0U)                                                      /*!< Operational power in 250mW units bits position          */
#define USBPD_PDO_SNK_BATTERY_OP_POWER_Msk           (0x3FFU << USBPD_PDO_SNK_BATTERY_OP_POWER_Pos)            /*!< Operational power in 250mW units bits mask : 0x000003FF */

#if defined(USBPD_REV30_SUPPORT)
#ifdef USBPDCORE_PPS
/* Source APDO type PDO elments */
#define USBPD_PDO_SRC_APDO_PPS_Pos                   (28U)                                                     /*!< Programmable Power Supply bit position           */
#define USBPD_PDO_SRC_APDO_PPS_Msk                   (0x3U << USBPD_PDO_SRC_APDO_PPS_Pos)                      /*!< Programmable Power Supply bit mask : 0x300000000 */
#define USBPD_PDO_SRC_APDO_PPS                       (0U)                                                      /*!< Programmable Power Supply field value 00         */

#define USBPD_PDO_SRC_APDO_MAX_VOLTAGE_Pos           (17U)                                                     /*!< APDO Max Voltage in 100 mV increments bits position  */
#define USBPD_PDO_SRC_APDO_MAX_VOLTAGE_Msk           (0xFFU << USBPD_PDO_SRC_APDO_MAX_VOLTAGE_Pos)             /*!< APDO Max Voltage in 100 mV increments bits mask : 0x01FE0000 */

#define USBPD_PDO_SRC_APDO_MIN_VOLTAGE_Pos           (8U)                                                      /*!< APDO Min Voltage in 100 mV increments bits position  */
#define USBPD_PDO_SRC_APDO_MIN_VOLTAGE_Msk           (0xFFU << USBPD_PDO_SRC_APDO_MIN_VOLTAGE_Pos)             /*!< APDO Min Voltage in 100 mV increments bits mask : 0x0000FF00 */

#define USBPD_PDO_SRC_APDO_MAX_CURRENT_Pos           (0U)                                                      /*!< APDO Max Current in 50 mA increments bits position  */
#define USBPD_PDO_SRC_APDO_MAX_CURRENT_Msk           (0x7FU << USBPD_PDO_SRC_APDO_MAX_CURRENT_Pos)             /*!< APDO Max Current in 50 mA increments bits mask : 0x0000007F */

/* Sink APDO type PDO elments */
#define USBPD_PDO_SNK_APDO_PPS_Pos                   (28U)                                                     /*!< Programmable Power Supply bit position           */
#define USBPD_PDO_SNK_APDO_PPS_Msk                   (0x3U << USBPD_PDO_SNK_APDO_PPS_Pos)                      /*!< Programmable Power Supply bit mask : 0x300000000 */
#define USBPD_PDO_SNK_APDO_PPS                       (0U)                                                      /*!< Programmable Power Supply field value 00         */

#define USBPD_PDO_SNK_APDO_MAX_VOLTAGE_Pos           (17U)                                                     /*!< APDO Max Voltage in 100 mV increments bits position  */
#define USBPD_PDO_SNK_APDO_MAX_VOLTAGE_Msk           (0xFFU << USBPD_PDO_SNK_APDO_MAX_VOLTAGE_Pos)             /*!< APDO Max Voltage in 100 mV increments bits mask : 0x01FE0000 */

#define USBPD_PDO_SNK_APDO_MIN_VOLTAGE_Pos           (8U)                                                      /*!< APDO Min Voltage in 100 mV increments bits position  */
#define USBPD_PDO_SNK_APDO_MIN_VOLTAGE_Msk           (0xFFU << USBPD_PDO_SNK_APDO_MIN_VOLTAGE_Pos)             /*!< APDO Min Voltage in 100 mV increments bits mask : 0x0000FF00 */

#define USBPD_PDO_SNK_APDO_MAX_CURRENT_Pos           (0U)                                                      /*!< APDO Max Current in 50 mA increments bits position  */
#define USBPD_PDO_SNK_APDO_MAX_CURRENT_Msk           (0x7FU << USBPD_PDO_SNK_APDO_MAX_CURRENT_Pos)             /*!< APDO Max Current in 50 mA increments bits mask : 0x0000007F */
#endif /* USBPDCORE_PPS */
#endif /* USBPD_REV30_SUPPORT */

#define USBPD_EXTENDED_MESSAGE                       (0x80U)                                                   /*!< Flag to indicate that it is a extended message     */

#if defined(USBPD_REV30_SUPPORT)
/** @defgroup USBPD_ADO_TYPE_ALERT USB-PD Type alert definition used for Alert Data Object
  * @{
  */
#define USBPD_ADO_TYPE_ALERT_BATTERY_STATUS (1 << 1) /*!< Battery Status Change Event(Attach/Detach/charging/discharging/idle) */
#define USBPD_ADO_TYPE_ALERT_OCP            (1 << 2) /*!< Over-Current Protection event when set (Source only, for Sink Reserved and Shall be set to zero) */
#define USBPD_ADO_TYPE_ALERT_OTP            (1 << 3) /*!< Over-Temperature Protection event when set  */
#define USBPD_ADO_TYPE_ALERT_OPERATING_COND (1 << 4) /*!< Operating Condition Change when set */
#define USBPD_ADO_TYPE_ALERT_SRC_INPUT      (1 << 5) /*!< Source Input Change Event when set */
#define USBPD_ADO_TYPE_ALERT_OVP            (1 << 6) /*!< Over-Voltage Protection event when set (Sink only, for Source Reserved and Shall be set to zero) */
/**
  * @}
  */

/** @defgroup USBPD_ADO_FIXED_BATT USB-PD Fixed Batteries definition used for Alert Data Object
  * @{
  */
#define USBPD_ADO_FIXED_BATT_BATTERY_0 (1 << 0) /*!< Fixed Batterie 0 had a status change */
#define USBPD_ADO_FIXED_BATT_BATTERY_1 (1 << 1) /*!< Fixed Batterie 1 had a status change */
#define USBPD_ADO_FIXED_BATT_BATTERY_2 (1 << 2) /*!< Fixed Batterie 2 had a status change */
#define USBPD_ADO_FIXED_BATT_BATTERY_3 (1 << 3) /*!< Fixed Batterie 3 had a status change */
/**
  * @}
  */

/** @defgroup USBPD_ADO_HOT_SWAP_BATT USB-PD Hot Swappable Batteries definition used for Alert Data Object
  * @{
  */
#define USBPD_ADO_HOT_SWAP_BATT_BATTERY_4 (1 << 0) /*!< Hot Swappable Batterie 4 had a status change */
#define USBPD_ADO_HOT_SWAP_BATT_BATTERY_5 (1 << 1) /*!< Hot Swappable Batterie 5 had a status change */
#define USBPD_ADO_HOT_SWAP_BATT_BATTERY_6 (1 << 2) /*!< Hot Swappable Batterie 6 had a status change */
#define USBPD_ADO_HOT_SWAP_BATT_BATTERY_7 (1 << 3) /*!< Hot Swappable Batterie 7 had a status change */
/**
  * @}
  */

/** @defgroup USBPD_MANUFINFO_TARGET USB-PD Manufacturer Info Target
  * @{
  */
#define USBPD_MANUFINFO_TARGET_PORT_CABLE_PLUG 0 /*!< Manufacturer Info Target Port/Cable Plug */
#define USBPD_MANUFINFO_TARGET_BATTERY 1         /*!< Manufacturer Info Target  Battery */
/**
  * @}
  */

/** @defgroup USBPD_MANUFINFO_REF USB-PD Manufacturer Info Ref
  * @{
  */
#define USBPD_MANUFINFO_REF_MAX_VALUES 7 /*!< Manufacturer Info Ref 0..3:Fixed Batteries and 4..7: Hot Swappable Batteries*/
/**
  * @}
  */

/** @defgroup USBPD_BSDO_BATT_INFO USB-PD Battery Status - Info
  * @{
  */
#define USBPD_BSDO_BATT_INFO_INVALID_REF              (1 << 0) /*!<  Battery Status - Info: Invalid Battery reference */
#define USBPD_BSDO_BATT_INFO_BATT_PRESENT             (1 << 1) /*!<  Battery Status - Info: Battery is present when set*/
#define USBPD_BSDO_BATT_INFO_BATT_ISCHARGING          (0 << 2) /*!<  Battery Status - Info: Battery is Charging*/
#define USBPD_BSDO_BATT_INFO_BATT_ISDISCHARGING       (1 << 2) /*!<  Battery Status - Info: Battery is Discharging*/
#define USBPD_BSDO_BATT_INFO_BATT_ISIDLE              (2 << 2) /*!<  Battery Status - Info: Battery is Idle*/
/**
  * @}
  */

#ifdef USBPDCORE_PPS
/** @defgroup USBPD_CORE_DEF_REAL_TIME_FLAGS USBPD CORE DEF Real Time Flags
  * @{
  */
#define USBPD_PPS_REALTIMEFLAGS_PTF_NOT_SUPPORTED     (00 << 1) /*!< PTF: 00 - Not Supported                              */
#define USBPD_PPS_REALTIMEFLAGS_PTF_NORMAL            (01 << 1) /*!< PTF: 01 - Normal                                     */
#define USBPD_PPS_REALTIMEFLAGS_PTF_WARNING           (10 << 1) /*!< PTF: 10 - Warning                                    */
#define USBPD_PPS_REALTIMEFLAGS_PTF_OVER_TEMPERATURE  (11 << 1) /*!< PTF: 11 - Over temperature                           */
#define USBPD_PPS_REALTIMEFLAGS_OMF_ENABLED           (1  << 3) /*!< OMF set when operating in Current Foldback mode      */
#define USBPD_PPS_REALTIMEFLAGS_OMF_DISABLED          (0  << 3) /*!< OMF set when operating in Current Foldback mode      */
/**
  * @}
  */
#endif /* USBPDCORE_PPS */
#endif /* USBPD_REV30_SUPPORT */
/**
  * @}
  */

/** @defgroup USBPD_SupportedSOP_TypeDef USB PD Supported SOP Types structure definition
  * @{
  */
#define USBPD_SUPPORTED_SOP_NONE       (     0) /*<! Not supported */
#define USBPD_SUPPORTED_SOP_SOP        (1 << 0) /*<! SOP           */
#define USBPD_SUPPORTED_SOP_SOP1       (1 << 1) /*<! SOP '         */
#define USBPD_SUPPORTED_SOP_SOP2       (1 << 2) /*<! SOP ''        */
#define USBPD_SUPPORTED_SOP_SOP1_DEBUG (1 << 3) /*<! SOP' Debug    */
#define USBPD_SUPPORTED_SOP_SOP2_DEBUG (1 << 4) /*<! SOP '' Debug  */
/**
  * @}
  */

#if defined(USBPDCORE_FWUPD)

/** @defgroup USBPD_FWUPD_MSGTYPE USB-PD Firmware Update Message Request and Responses Defines
  * @{
  */

typedef enum
{
  USBPD_FWUPD_MSGTYPE_NONE                = 0x00u, /*!< Reserved value                                                 */
  USBPD_FWUPD_MSGTYPE_RSP_GET_FW_ID       = 0x01u, /*!< Response is used to respond to a GET_FW_ID Request             */
  USBPD_FWUPD_MSGTYPE_RSP_PDFU_INITIATE   = 0x02u, /*!< Response is used to respond to a PDFU_INITIATE Request         */
  USBPD_FWUPD_MSGTYPE_RSP_PDFU_DATA       = 0x03u, /*!< Response is used to respond to a PDFU_DATA Request             */
  USBPD_FWUPD_MSGTYPE_RSP_PDFU_VALIDATE   = 0x05u, /*!< Response is used to respond to a PDFU_VALIDATE Request         */
  USBPD_FWUPD_MSGTYPE_RSP_PDFU_DATA_PAUSE = 0x07u, /*!< Response is used to respond to a PDFU_DATA_PAUSE Request       */
  USBPD_FWUPD_MSGTYPE_RSP_VENDOR_SPECIFIC = 0x7Fu, /*!< Response is used to respond to a VENDOR_SPECIFIC Request       */
  USBPD_FWUPD_MSGTYPE_REQ_GET_FW_ID       = 0x81u, /*!< Request is used to retrieve information about a PDFU Responder
                                                        and determine if a firmware update is necessary                */
  USBPD_FWUPD_MSGTYPE_REQ_PDFU_INITIATE   = 0x82u, /*!< Request is used to initiate firmware update                    */
  USBPD_FWUPD_MSGTYPE_REQ_PDFU_DATA       = 0x83u, /*!< Request is used to transfer a data block from a firmware image
                                                        (response required)                                            */
  USBPD_FWUPD_MSGTYPE_REQ_PDFU_DATA_NR    = 0x84u, /*!< Request is used to transfer a data block from a firmware image
                                                        (response not required)                                        */
  USBPD_FWUPD_MSGTYPE_REQ_PDFU_VALIDATE   = 0x85u, /*!< Request is used to request validation of a firmware image      */
  USBPD_FWUPD_MSGTYPE_REQ_PDFU_ABORT      = 0x86u, /*!< Request is used to end firmware image update prematurely       */
  USBPD_FWUPD_MSGTYPE_REQ_PDFU_DATA_PAUSE = 0x87u, /*!< Request is used to pause a firmware image update               */
  USBPD_FWUPD_MSGTYPE_REQ_VENDOR_SPECIFIC = 0xFFu, /*!< Request is for vendor-specific use                             */
}
USBPD_FWUPD_MsgType_TypeDef;

/**
  * @}
  */

/** @defgroup USBPD_FWUPD_PROT_VER USB-PD Firmware Update Message Protocol version Defines
  * @{
  */

#define  USBPD_FWUPD_PROT_VER_V1P0                0x01u /*!< USB PD Firmware Update Protocol Version 1.0 */

/**
  * @}
  */

/** @defgroup USBPD_FWUPD_SIZE_PAYLOAD USB-PD Firmware Update Size Payload Defines
  * @{
  */

#define  USBPD_FWUPD_SIZE_PAYLOAD_RSP_GET_FW_ID        (sizeof(USBPD_FWUPD_GetFwIDRspPayload_TypeDef)) /*!< Payload size of Response is used to respond to a GET_FW_ID Request             */
#define  USBPD_FWUPD_SIZE_PAYLOAD_RSP_PDFU_INITIATE    (sizeof(USBPD_FWUPD_PdfuInitRspPayload_TypeDef)) /*!< Payload size of Response is used to respond to a PDFU_INITIATE Request         */
#define  USBPD_FWUPD_SIZE_PAYLOAD_RSP_PDFU_DATA        (sizeof(USBPD_FWUPD_PdfuDataRspPayload_TypeDef)) /*!< Payload size of Response is used to respond to a PDFU_DATA Request             */
#define  USBPD_FWUPD_SIZE_PAYLOAD_RSP_PDFU_VALIDATE    (sizeof(USBPD_FWUPD_PdfuValidateRspPayload_TypeDef)) /*!< Payload size of Response is used to respond to a PDFU_VALIDATE Request         */
#define  USBPD_FWUPD_SIZE_PAYLOAD_RSP_PDFU_DATA_PAUSE  (sizeof(USBPD_FWUPD_PdfuDataPauseRspPayload_TypeDef)) /*!< Payload size of Response is used to respond to a PDFU_DATA_PAUSE Request       */
#define  USBPD_FWUPD_SIZE_PAYLOAD_RSP_VENDOR_SPECIFIC  (sizeof(USBPD_FWUPD_VendorSpecificRspPayload_TypeDef)) /*!< Payload size of Response is used to respond to a VENDOR_SPECIFIC Request       */
#define  USBPD_FWUPD_SIZE_PAYLOAD_REQ_GET_FW_ID        0u /*!< Payload size of Request is used to retrieve information */
#define  USBPD_FWUPD_SIZE_PAYLOAD_REQ_PDFU_INITIATE    8u /*!< Payload size of Request is used to initiate firmware update                    */
#define  USBPD_FWUPD_SIZE_PAYLOAD_REQ_PDFU_DATA_MAX    (2u + 256u) /*!< Payload size of Request is used to transfer a data block from a firmware image
                                                             (response required)                                            */
#define  USBPD_FWUPD_SIZE_PAYLOAD_REQ_PDFU_DATA_NR_MAX (2u + 256u) /*!< Payload size of Request is used to transfer a data block from a firmware image
                                                             (response not required)                                        */
#define  USBPD_FWUPD_SIZE_PAYLOAD_REQ_PDFU_VALIDATE    0u /*!< Payload size of Request is used to request validation of a firmware image      */
#define  USBPD_FWUPD_SIZE_PAYLOAD_REQ_PDFU_ABORT       0u /*!< Payload size of Request is used to end firmware image update prematurely       */
#define  USBPD_FWUPD_SIZE_PAYLOAD_REQ_PDFU_DATA_PAUSE  0u /*!< Payload size of Request is used to pause a firmware image update               */
#define  USBPD_FWUPD_SIZE_PAYLOAD_REQ_VENDOR_SPECIFIC_MAX  (2u + 256u) /*!< Payload size of Request is for vendor-specific use                */

/**
  * @}
  */
#endif /* USBPDCORE_FWUPD */

/**
  * @}
  */

/* Exported typedef ----------------------------------------------------------*/

/** @defgroup USBPD_CORE_DEF_Exported_TypeDef USBPD CORE DEF Exported TypeDef
  * @{
  */

/** @defgroup USBPD_CORE_PE_ET_HR_STATUS USBPD CORE PE Hard Reset Status
  * @{
  */
typedef enum
{
  USBPD_HR_STATUS_START_ACK,
  USBPD_HR_STATUS_START_REQ,
  USBPD_HR_STATUS_MSG_SENT,
  USBPD_HR_STATUS_MSG_RECEIVED,
  USBPD_HR_STATUS_WAIT_VBUS_VSAFE0V,
  USBPD_HR_STATUS_COMPLETED,
  USBPD_HR_STATUS_FAILED,
}
USBPD_HR_Status_TypeDef;
/**
  * @}
  */

/** @defgroup USBPD_CORE_PE_ET_PRS_STATUS USBPD CORE PE Power Role Swap status
  * @{
  */
typedef enum
{
  USBPD_PRS_STATUS_NA,
  USBPD_PRS_STATUS_START_ACK,
  USBPD_PRS_STATUS_START_REQ,
  USBPD_PRS_STATUS_ACCEPTED,
  USBPD_PRS_STATUS_REJECTED,
  USBPD_PRS_STATUS_WAIT,
  USBPD_PRS_STATUS_VBUS_OFF,
  USBPD_PRS_STATUS_SRC_RP2RD,
  USBPD_PRS_STATUS_SRC_PS_READY_SENT,
  USBPD_PRS_STATUS_SNK_PS_READY_RECEIVED,
  USBPD_PRS_STATUS_SNK_RD2RP,
  USBPD_PRS_STATUS_VBUS_ON,
  USBPD_PRS_STATUS_SNK_PS_READY_SENT,
  USBPD_PRS_STATUS_SRC_PS_READY_RECEIVED,
  USBPD_PRS_STATUS_COMPLETED,
  USBPD_PRS_STATUS_FAILED,
  USBPD_PRS_STATUS_ABORTED,
} USBPD_PRS_Status_TypeDef;
/**
  * @}
  */

/**
  * @brief Status of VSafe
  * @{
  */
typedef enum
{
  USBPD_VSAFE_0V          , /*!< USBPD VSAFE0V   */
  USBPD_VSAFE_5V          , /*!< USBPD VSAFE5V   */
}
USBPD_VSAFE_StatusTypeDef;
/**
  * @}
  */

/**
  * @brief USB Power Delivery Status structures definition
  */
typedef enum
{
  USBPD_OK     ,
  USBPD_NOTSUPPORTED,
  USBPD_ERROR  ,
  USBPD_BUSY   ,
  USBPD_TIMEOUT,

  /* PRL status */
  USBPD_PRL_GOODCRC,
  USBPD_PRL_DISCARDED,
  USBPD_PRL_SOFTRESET,
  USBPD_PRL_CABLERESET,
#if defined(USBPD_REV30_SUPPORT)
  USBPD_PRL_SNKTX,
#endif /* USBPD_REV30_SUPPORT */

  /* Message repply */
  USBPD_ACCEPT,
  USBPD_GOTOMIN,
  USBPD_REJECT,
  USBPD_WAIT,
  USBPD_NAK,
  USBPD_ACK,

  USBPD_FAIL   ,
  USBPD_RXEVENT_SOP,
  USBPD_RXEVENT_SOP1,
  USBPD_RXEVENT_SOP2,
  USBPD_NOEVENT,
  USBPD_DISCARDRX,

  USBPD_MALLOCERROR,
#if defined(USBPDCORE_FWUPD)
  USBPD_PDFU_NODATA,
  USBPD_PDFU_PAUSE,
  USBPD_PDFU_RESUME,
#endif /* USBPDCORE_FWUPD */
}
USBPD_StatusTypeDef;

/**
  * @brief USB PD CC lines structures definition
  */
typedef enum
{
  CCNONE      = 0,
  CC1         = 1,
  CC2         = 2
} CCxPin_TypeDef;


/** @defgroup USBPD_SpecRev_TypeDef USB PD Specification Revision structure definition
  * @brief  USB PD Specification Revision structure definition
  * @{
  */
typedef enum
{
  USBPD_SPECIFICATION_REV1               = 0x00,  /*!< Revision 1.0      */
  USBPD_SPECIFICATION_REV2               = 0x01,  /*!< Revision 2.0      */
  USBPD_SPECIFICATION_REV3               = 0x02   /*!< Revision 3.0      */
} USBPD_SpecRev_TypeDef;
/**
  * @}
  */

/** @defgroup USBPD_SpecRev_TypeDef USB PD Specification Revision structure definition
  * @brief  USB PD Specification Revision structure definition
  * @{
  */
typedef enum
{
  USBPD_VDMVERSION_REV1       = 0x00,  /*!< Revision 1.0      */
  USBPD_VDMVERSION_REV2       = 0x01,  /*!< Revision 2.0 only used if USBPD_SPECIFICATION_REV3 */
} USBPD_VDMVersion_TypeDef;
/**
  * @}
  */

/**
  * @brief CAD event value
  * @{
  */
typedef enum
{
  USBPD_CAD_EVENT_NONE      = 0  ,         /*!< USBPD CAD event None                                */
  USBPD_CAD_EVENT_DETACHED  = 1  ,         /*!< USBPD CAD event No cable detected                   */
  USBPD_CAD_EVENT_ATTACHED  = 2  ,         /*!< USBPD CAD event Port partner attached               */
  USBPD_CAD_EVENT_EMC       = 3  ,         /*!< USBPD CAD event Electronically Marked Cable detected*/
  USBPD_CAD_EVENT_ATTEMC    = 4  ,         /*!< USBPD CAD event Port Partner detected throug EMC    */
  USBPD_CAD_EVENT_ACCESSORY = 5  ,         /*!< USBPD CAD event Accessory detected                  */
  USBPD_CAD_EVENT_DEBUG     = 6  ,         /*!< USBPD CAD event Debug detected                      */
//  USBPD_CAD_EVENT_LEGACY    = 7  ,         /*!< USBPD CAD event legacy cables detected              */
  USPPD_CAD_EVENT_UNKNOW    = 8            /*!< USBPD CAD event unknow                              */
} USBPD_CAD_EVENT;
/**
  * @}
  */
/** @defgroup USBPD_PortDataRole_TypeDef USB PD Port Data Role Types structure definition
  * @brief  USB PD Port Data Role Types structure definition
  * @{
  */
typedef enum
{
  USBPD_PORTDATAROLE_UFP                 = 0x00,  /*!< UFP        */
  USBPD_PORTDATAROLE_SOP1_SOP2           = USBPD_PORTDATAROLE_UFP,  /*!<  For all other SOP* Packets the Port Data Role
                                                                          field is Reserved and shall be set to zero.  */
  USBPD_PORTDATAROLE_DFP                 = 0x01   /*!< DFP        */
} USBPD_PortDataRole_TypeDef;
/**
  * @}
  */

/**
  * @brief  USB PD Control Message Types structure definition
  *
  */
typedef enum
{
  USBPD_CONTROLMSG_GOODCRC               = 0x01,  /*!< GoodCRC Control Message         */
  USBPD_CONTROLMSG_GOTOMIN               = 0x02,  /*!< GotoMin Control Message         */
  USBPD_CONTROLMSG_ACCEPT                = 0x03,  /*!< Accept Control Message          */
  USBPD_CONTROLMSG_REJECT                = 0x04,  /*!< Reject Control Message          */
  USBPD_CONTROLMSG_PING                  = 0x05,  /*!< Ping Control Message            */
  USBPD_CONTROLMSG_PS_RDY                = 0x06,  /*!< PS_RDY Control Message          */
  USBPD_CONTROLMSG_GET_SRC_CAP           = 0x07,  /*!< Get_Source_Cap Control Message  */
  USBPD_CONTROLMSG_GET_SNK_CAP           = 0x08,  /*!< Get_Sink_Cap Control Message    */
  USBPD_CONTROLMSG_DR_SWAP               = 0x09,  /*!< DR_Swap Control Message         */
  USBPD_CONTROLMSG_PR_SWAP               = 0x0A,  /*!< PR_Swap Control Message         */
  USBPD_CONTROLMSG_VCONN_SWAP            = 0x0B,  /*!< VCONN_Swap Control Message      */
  USBPD_CONTROLMSG_WAIT                  = 0x0C,  /*!< Wait Control Message            */
  USBPD_CONTROLMSG_SOFT_RESET            = 0x0D,  /*!< Soft_Reset Control Message      */
#if defined(USBPD_REV30_SUPPORT)
  USBPD_CONTROLMSG_NOT_SUPPORTED         = 0x10,  /*!< Not supported                   */
  USBPD_CONTROLMSG_GET_SRC_CAPEXT        = 0x11,  /*!< Get source capability extended  */
  USBPD_CONTROLMSG_GET_STATUS            = 0x12,  /*!< Get status                      */
  USBPD_CONTROLMSG_FR_SWAP               = 0x13,  /*!< Fast role swap                  */
  USBPD_CONTROLMSG_GET_PPS_STATUS        = 0x14,  /*!< Get PPS Status                  */
  USBPD_CONTROLMSG_GET_COUNTRY_CODES     = 0x15,  /*!< Get Country codes               */
#endif /* USBPD_REV30_SUPPORT */
}USBPD_ControlMsg_TypeDef;

/**
  * @brief  USB PD Data Message Types structure definition
  *
  */
typedef enum
{
  USBPD_DATAMSG_SRC_CAPABILITIES         = 0x01,  /*!< Source Capabilities Data Message  */
  USBPD_DATAMSG_REQUEST                  = 0x02,  /*!< Request Data Message              */
  USBPD_DATAMSG_BIST                     = 0x03,  /*!< BIST Data Message                 */
  USBPD_DATAMSG_SNK_CAPABILITIES         = 0x04,  /*!< Sink_Capabilities Data Message    */
#if defined(USBPD_REV30_SUPPORT)
  USBPD_DATAMSG_BATTERY_STATUS           = 0x05,  /*!< Battery status                    */
  USBPD_DATAMSG_ALERT                    = 0x06,  /*!< Alert                             */
  USBPD_DATAMSG_GET_COUNTRY_INFO         = 0x07,  /*!< Get country info                  */
#endif /* USBPD_REV30_SUPPORT */
  USBPD_DATAMSG_VENDOR_DEFINED           = 0x0F   /*!< Vendor_Defined Data Message       */
}USBPD_DataMsg_TypeDef;

/**
  * @brief Sink CC pins Multiple Source Current Advertisements
  */
typedef enum
{
  vRd_Undefined     = 0x00,    /*!< Port Power Role Source   */
  vRd_USB           = 0x01,    /*!< Default USB Power   */
  vRd_1_5A          = 0x02,    /*!< USB Type-C Current @ 1.5 A   */
  vRd_3_0A          = 0x03,    /*!< USB Type-C Current @ 3 A   */
} CAD_SNK_Source_Current_Adv_Typedef;


/**
  * @brief Sink CC pins Multiple Source Current Advertisements
  */
typedef enum
{
  vRp_Default       = 0x00,    /*!< Default USB Power   */
  vRp_1_5A          = 0x01,    /*!< USB Type-C Current @ 1.5 A   */
  vRp_3_0A          = 0x02,    /*!< USB Type-C Current @ 3 A   */
} CAD_RP_Source_Current_Adv_Typedef;


/**
  * @brief USB PD SOP Message Types Structure definition
  */
typedef enum
{
  USBPD_SOPTYPE_SOP            = 0,     /**< SOP*  MESSAGES               */
  USBPD_SOPTYPE_SOP1           = 1,     /**< SOP'  MESSAGES               */
  USBPD_SOPTYPE_SOP2           = 2,     /**< SOP'' MESSAGES               */
  USBPD_SOPTYPE_SOP1_DEBUG     = 3,     /**< SOP'  DEBUG_MESSAGES         */
  USBPD_SOPTYPE_SOP2_DEBUG     = 4,     /**< SOP'' DEBUG_MESSAGES         */
  USBPD_SOPTYPE_HARD_RESET     = 5,     /**< HARD RESET MESSAGE           */
  USBPD_SOPTYPE_CABLE_RESET    = 6,     /**< CABLE RESET MESSAGE          */
  USBPD_SOPTYPE_BIST_MODE_2    = 7,     /**< BIST_MODE2 MESSAGE           */
  USBPD_SOPTYPE_INVALID        = 0xFF,  /**< Invalid type                 */
} USBPD_SOPType_TypeDef;

/**
  * @brief USB funtionnal state Types enum definition
  *
  */
typedef enum
{
  USBPD_DISABLE = 0,
  USBPD_ENABLE = !USBPD_DISABLE
} USBPD_FunctionalState;


/**
  * @brief USB PD Port Power Role Types structure definition
  *
  */
typedef enum
{
  USBPD_CABLEPLUG_FROMDFPUFP    = 0x00,                           /*!< Message originated from a DFP or UFP    */
  USBPD_PORTPOWERROLE_SNK       = USBPD_CABLEPLUG_FROMDFPUFP ,    /*!< Sink                                    */
  USBPD_CABLEPLUG_FROMCABLEPLUG = 0x01,                           /*!< Message originated from a Cable Plug    */
  USBPD_PORTPOWERROLE_SRC       = USBPD_CABLEPLUG_FROMCABLEPLUG,  /*!< Source                                  */
} USBPD_PortPowerRole_TypeDef;

/**
  * @brief  USB PD Extended Message Types structure definition
  *
  */
typedef enum
{
  USBPD_EXT_NONE                  = USBPD_EXTENDED_MESSAGE,
  USBPD_EXT_SOURCE_CAPABILITIES   = (USBPD_EXTENDED_MESSAGE | 0x01),  /*!< sent by Source or Dual-Role Power    - SOP only  */
  USBPD_EXT_STATUS                = (USBPD_EXTENDED_MESSAGE | 0x02),  /*!< sent by Source                       - SOP only  */
  USBPD_EXT_GET_BATTERY_CAP       = (USBPD_EXTENDED_MESSAGE | 0x03),  /*!< sent by Source or Sink               - SOP only  */
  USBPD_EXT_GET_BATTERY_STATUS    = (USBPD_EXTENDED_MESSAGE | 0x04),  /*!< sent by Source or Sink               - SOP only  */
  USBPD_EXT_BATTERY_CAPABILITIES  = (USBPD_EXTENDED_MESSAGE | 0x05),  /*!< sent by Source or Sink               - SOP only  */
  USBPD_EXT_GET_MANUFACTURER_INFO = (USBPD_EXTENDED_MESSAGE | 0x06),  /*!< sent by Source or Sink or Cable Plug - SOP*      */
  USBPD_EXT_MANUFACTURER_INFO     = (USBPD_EXTENDED_MESSAGE | 0x07),  /*!< sent by Source or Sink or Cable Plug - SOP*      */
  USBPD_EXT_SECURITY_REQUEST      = (USBPD_EXTENDED_MESSAGE | 0x08),  /*!< sent by Source or Sink               - SOP*      */
  USBPD_EXT_SECURITY_RESPONSE     = (USBPD_EXTENDED_MESSAGE | 0x09),  /*!< sent by Source or Sink or Cable Plug - SOP*      */
  USBPD_EXT_FIRM_UPDATE_REQUEST   = (USBPD_EXTENDED_MESSAGE | 0x0A),  /*!< sent by Source or Sink               - SOP*      */
  USBPD_EXT_FIRM_UPDATE_RESPONSE  = (USBPD_EXTENDED_MESSAGE | 0x0B),  /*!< sent by Source or Sink or Cable Plug - SOP*      */
  USBPD_EXT_PPS_STATUS            = (USBPD_EXTENDED_MESSAGE | 0x0C),  /*!< sent by Source                       - SOP only  */
  USBPD_EXT_COUNTRY_INFO          = (USBPD_EXTENDED_MESSAGE | 0x0D),  /*!< sent by Source or Sink or Cable Plug - SOP*      */
  USBPD_EXT_COUNTRY_CODES         = (USBPD_EXTENDED_MESSAGE | 0x0E),  /*!< sent by Source or Sink or Cable Plug - SOP*      */
} USBPD_ExtendedMsg_TypeDef;

/**
  * @brief  USB PD BIST Mode Types structure definition
  *
  */
typedef enum
{
  USBPD_BIST_RECEIVER_MODE               = 0x00,  /*!< BIST Receiver Mode      */
  USBPD_BIST_TRANSMIT_MODE               = 0x01,  /*!< BIST Transmit Mode      */
  USBPD_RETURNED_BIST_COUNTERS           = 0x02,  /*!< Returned BIST Counters  */
  USBPD_BIST_CARRIER_MODE0               = 0x03,  /*!< BIST Carrier Mode 0     */
  USBPD_BIST_CARRIER_MODE1               = 0x04,  /*!< BIST Carrier Mode 1     */
  USBPD_BIST_CARRIER_MODE2               = 0x05,  /*!< BIST Carrier Mode 2     */
  USBPD_BIST_CARRIER_MODE3               = 0x06,  /*!< BIST Carrier Mode 3     */
  USBPD_BIST_EYE_PATTERN                 = 0x07,  /*!< BIST Eye Pattern        */
  USBPD_BIST_TEST_DATA                   = 0x08   /*!< BIST Test Data          */
} USBPD_BISTMsg_TypeDef;

/** @defgroup USBPD_CORE_PDO_Type_TypeDef PDO type definition
  * @brief  PDO type values in PDO definition
  * @{
  */
typedef enum
{
  USBPD_CORE_PDO_TYPE_FIXED     = 0x00,            /*!< Fixed Supply PDO                             */
  USBPD_CORE_PDO_TYPE_BATTERY   = 0x01,            /*!< Battery Supply PDO                           */
  USBPD_CORE_PDO_TYPE_VARIABLE  = 0x02,            /*!< Variable Supply (non-battery) PDO            */
#if defined(USBPD_REV30_SUPPORT) && defined(USBPDCORE_PPS)
  USBPD_CORE_PDO_TYPE_APDO      = 0x03,            /*!< Augmented Power Data Object (APDO)           */
#endif /*_USBPD_REV30_SUPPORT && PPS*/
} USBPD_CORE_PDO_Type_TypeDef;
/**
  * @}
  */

/** @defgroup USBPD_CORE_POWER_Type_TypeDef USB-PD power state
  * @brief  USB Power state
  * @{
  */
typedef enum
{
  USBPD_POWER_NO               = 0x0, /*!< No power contract                      */
  USBPD_POWER_DEFAULT5V        = 0x1, /*!< Default 5V                             */
  USBPD_POWER_IMPLICITCONTRACT = 0x2, /*!< Implicit contract                      */
  USBPD_POWER_EXPLICITCONTRACT = 0x3, /*!< Explicit contract                      */
  USBPD_POWER_TRANSITION       = 0x4, /*!< Power transition                       */
} USBPD_POWER_StateTypedef;
/**
  * @}
  */

#if defined(USBPD_REV30_SUPPORT) && defined(USBPDCORE_PPS)
/**
  * @brief  USB PD Programmable Power Supply APDO Structure definition (same for SRC and SNK)
  *
  */
typedef struct
{
  uint32_t MaxCurrentIn50mAunits  : 7;  /*!< Maximum Current in 50mA increments       */
  uint32_t Reserved1              : 1;  /*!< Reserved  - Shall be set to zero         */
  uint32_t MinVoltageIn100mV      : 8;  /*!< Minimum Voltage in 100mV increments      */
  uint32_t Reserved2              : 1;  /*!< Reserved  - Shall be set to zero         */
  uint32_t MaxVoltageIn100mV      : 8;  /*!< Maximum Voltage in 100mV increments      */
  uint32_t Reserved3              : 3;  /*!< Reserved  - Shall be set to zero         */
  uint32_t ProgrammablePowerSupply: 2;  /*!< 00b - Programmable Power Supply          */
  uint32_t PPS_APDO               : 2;  /*!< 11b - Augmented Power Data Object (APDO) */
} USBPD_ProgrammablePowerSupplyAPDO_TypeDef;

#endif /*_USBPD_REV30_SUPPORT && PPS*/

/** @defgroup USBPD_CORE_PDO_DRPowerSupport_TypeDef DRP Support type
  * @brief  DRP support values in PDO definition (Source or Sink)
  * @{
  */
typedef enum
{
  USBPD_CORE_PDO_DRP_NOT_SUPPORTED = 0x00,          /*!< Dual Role Power not supported                */
  USBPD_CORE_PDO_DRP_SUPPORTED     = 0x01,          /*!< Dual Role Power supported                    */
} USBPD_CORE_PDO_DRPowerSupport_TypeDef;
/**
  * @}
  */

/** @defgroup USBPD_CORE_PDO_USBSuspendSupport_TypeDef USB Suspend type
  * @brief  USB Suspend support values in PDO definition (Source)
  * @{
  */
typedef enum
{
  USBPD_CORE_PDO_USBSUSP_NOT_SUPPORTED = 0x00,      /*!< USB Suspend not supported                    */
  USBPD_CORE_PDO_USBSUSP_SUPPORTED     = 0x01,      /*!< USB Suspend supported                        */
} USBPD_CORE_PDO_USBSuspendSupport_TypeDef;
/**
  * @}
  */

/** @defgroup USBPD_CORE_PDO_ExtPowered_TypeDef Externally Powered type
  * @brief  Fixed Power Source Externally Powered indication values in PDO definition (Source or Sink)
  * @{
  */
typedef enum
{
  USBPD_CORE_PDO_NOT_EXT_POWERED = 0x00,            /*!< No external power source is available        */
  USBPD_CORE_PDO_EXT_POWERED     = 0x01,            /*!< External power source is available           */
} USBPD_CORE_PDO_ExtPowered_TypeDef;
/**
  * @}
  */

/** @defgroup USBPD_CORE_PDO_USBCommCapable_TypeDef USB Communication capability type
  * @brief  USB Communication capability over USB Data lines indication values in PDO definition (Source or Sink)
  * @{
  */
typedef enum
{
  USBPD_CORE_PDO_USBCOMM_NOT_CAPABLE = 0x00,        /*!< Device not capable of communication over USB Data lines */
  USBPD_CORE_PDO_USBCOMM_CAPABLE     = 0x01,        /*!< Device capable of communication over USB Data lines     */
} USBPD_CORE_PDO_USBCommCapable_TypeDef;
/**
  * @}
  */

/** @defgroup USBPD_CORE_PDO_DRDataSupport_TypeDef Dual Role Data Support type
  * @brief  Dual Role Data support values in PDO definition (Source or Sink)
  * @{
  */
typedef enum
{
  USBPD_CORE_PDO_DRD_NOT_SUPPORTED = 0x00,          /*!< Dual Role Data not supported                 */
  USBPD_CORE_PDO_DRD_SUPPORTED     = 0x01,          /*!< Dual Role Data supported                     */
} USBPD_CORE_PDO_DRDataSupport_TypeDef;
/**
  * @}
  */

/** @defgroup USBPD_CORE_PDO_PeakCurr_TypeDef Peak Current Capability type
  * @brief  Fixed Power Source Peak Current Capability type structure definition (Source)
  * @{
  */
typedef enum
{
  USBPD_CORE_PDO_PEAKEQUAL = 0x00,                  /*!< Peak current equals                          */
  USBPD_CORE_PDO_PEAKOVER1 = 0x01,                  /*!< Overload Capabilities:
  1. Peak current equals 150% IOC for 1ms @ 5% duty cycle (low current equals 97% IOC for 19ms)
  2. Peak current equals 125% IOC for 2ms @ 10% duty cycle (low current equals 97% IOC for 18ms)
  3. Peak current equals 110% IOC for 10ms @ 50% duty cycle (low current equals 90% IOC for 10ms */
  USBPD_CORE_PDO_PEAKOVER2 = 0x02,                  /*!< Overload Capabilities:
  1. Peak current equals 200% IOC for 1ms @ 5% duty cycle (low current equals 95% IOC for 19ms)
  2. Peak current equals 150% IOC for 2ms @ 10% duty cycle (low current equals 94% IOC for 18ms)
  3. Peak current equals 125% IOC for 10ms @ 50% duty cycle (low current equals 75% IOC for 10ms)*/
  USBPD_CORE_PDO_PEAKOVER3 = 0x03,                  /*!< Overload Capabilities:
  1. Peak current equals 200% IOC for 1ms @ 5% duty cycle (low current equals 95% IOC for 19ms)
  2. Peak current equals 175% IOC for 2ms @ 10% duty cycle (low current equals 92% IOC for 18ms)
  3. Peak current equals 150% IOC for 10ms @ 50% duty cycle (low current equals 50% IOC for 10ms)*/
} USBPD_CORE_PDO_PeakCurr_TypeDef;
/**
  * @}
  */

/** @defgroup USBPD_CORE_PDO_HigherCapability_TypeDef USB Higher Capability type
  * @brief  Values in PDO definition (Sink) indicating if Sink needs more than vSafe5V to provide full functionality
  * @{
  */
typedef enum
{
  USBPD_CORE_PDO_NO_HIGHER_CAPABILITY  = 0x00,      /*!< No need for more than vSafe5V to provide full functionality */
  USBPD_CORE_PDO_HIGHER_CAPABILITY     = 0x01,      /*!< Sink needs more than vSafe5V to provide full functionality  */
} USBPD_CORE_PDO_HigherCapability_TypeDef;
/**
  * @}
  */

#if defined(USBPD_REV30_SUPPORT)
/** @defgroup USBPD_CORE_PDO_UnchunkSupport_TypeDef Unchunked Extended Messages Support type
  * @brief  Unchunked Extended Messages support values in PDO definition (Source)
  * @{
  */
typedef enum
{
  USBPD_CORE_PDO_UNCHUNK_NOT_SUPPORTED = 0x00,      /*!< Unchunked Extended Messages not supported    */
  USBPD_CORE_PDO_UNCHUNK_SUPPORTED     = 0x01,      /*!< Unchunked Extended Messages supported        */
} USBPD_CORE_PDO_UnchunkSupport_TypeDef;
/**
  * @}
  */

/** @defgroup USBPD_CORE_PDO_FastRoleSwapRequiredCurrent_TypeDef Fast Role Swap Required current type
  * @brief  Fast Role Swap Required Current values in PDO definition (Sink)
  * @{
  */
typedef enum
{
  USBPD_CORE_PDO_FRS_NOT_SUPPORTED = 0x00,          /*!< Fast Role Swap not supported    */
  USBPD_CORE_PDO_FRS_DEFAULT_USB_POWER = 0x01,      /*!< Default USB Power               */
  USBPD_CORE_PDO_FRS_1_5A_5V = 0x02,                /*!< 1_5A at 5V supported            */
  USBPD_CORE_PDO_FRS_3A_5V = 0x03,                  /*!< 3A at 5V supported              */
} USBPD_CORE_PDO_FastRoleSwapRequiredCurrent_TypeDef;
/**
  * @}
  */
#endif /*_USBPD_REV30_SUPPORT*/

/**
  * @brief  USB PD Source Fixed Supply Power Data Object Structure definition
  *
  */
typedef struct
{
  uint32_t MaxCurrentIn10mAunits :                                  10;
  uint32_t VoltageIn50mVunits :                                     10;
  USBPD_CORE_PDO_PeakCurr_TypeDef PeakCurrent :                     2;
#if defined(USBPD_REV30_SUPPORT)
  uint32_t Reserved22_23 :                                          2;
  uint32_t UnchunkedExtendedMessage :                               1;
#else
  uint32_t Reserved22_24 :                                          3;
#endif /* USBPD_REV30_SUPPORT */
  USBPD_CORE_PDO_DRDataSupport_TypeDef DataRoleSwap :               1;
  USBPD_CORE_PDO_USBCommCapable_TypeDef USBCommunicationsCapable :  1;
  USBPD_CORE_PDO_ExtPowered_TypeDef ExternallyPowered :             1;
  USBPD_CORE_PDO_USBSuspendSupport_TypeDef USBSuspendSupported :    1;
  USBPD_CORE_PDO_DRPowerSupport_TypeDef DualRolePower :             1;
  USBPD_CORE_PDO_Type_TypeDef FixedSupply :                         2;

} USBPD_SRCFixedSupplyPDO_TypeDef;

/**
  * @brief  USB PD Source Variable Supply Power Data Object Structure definition
  *
  */
typedef struct
{
  uint32_t MaxCurrentIn10mAunits :
      10;
  uint32_t MinVoltageIn50mVunits :
      10;
  uint32_t MaxVoltageIn50mVunits :
      10;
  uint32_t VariableSupply :
      2;
} USBPD_SRCVariableSupplyPDO_TypeDef;

/**
  * @brief  USB PD Source Battery Supply Power Data Object Structure definition
  *
  */
typedef struct
{
  uint32_t MaxAllowablePowerIn250mWunits :
      10;
  uint32_t MinVoltageIn50mVunits :
      10;
  uint32_t MaxVoltageIn50mVunits :
      10;
  uint32_t Battery :
      2;
} USBPD_SRCBatterySupplyPDO_TypeDef;

/**
  * @brief  USB PD Sink Fixed Supply Power Data Object Structure definition
  *
  */
typedef struct
{
  uint32_t OperationalCurrentIn10mAunits :
      10;
  uint32_t VoltageIn50mVunits :
      10;
#if defined(USBPD_REV30_SUPPORT)
  uint32_t FastRoleSwapRequiredCurrent :
      2;
  uint32_t Reserved20_22 :
      3;
#else
  uint32_t Reserved20_24 :
      5;
#endif /* USBPD_REV30_SUPPORT */
  uint32_t DataRoleSwap :
      1;
  uint32_t USBCommunicationsCapable :
      1;
  uint32_t ExternallyPowered :
      1;
  uint32_t HigherCapability :
      1;
  uint32_t DualRolePower :
      1;
  uint32_t FixedSupply :
      2;
} USBPD_SNKFixedSupplyPDO_TypeDef;

/**
  * @brief  USB PD Sink Variable Supply Power Data Object Structure definition
  *
  */
typedef struct
{
  uint32_t OperationalCurrentIn10mAunits :
      10;
  uint32_t MinVoltageIn50mVunits :
      10;
  uint32_t MaxVoltageIn50mVunits :
      10;
  uint32_t VariableSupply :
      2;
} USBPD_SNKVariableSupplyPDO_TypeDef;

/**
  * @brief  USB PD Sink Battery Supply Power Data Object Structure definition
  *
  */
typedef struct
{
  uint32_t OperationalPowerIn250mWunits :
      10;
  uint32_t MinVoltageIn50mVunits :
      10;
  uint32_t MaxVoltageIn50mVunits :
      10;
  uint32_t Battery :
      2;
} USBPD_SNKBatterySupplyPDO_TypeDef;

/**
  * @brief  USB PD Sink Generic Power Data Object Structure definition
  *
  */
typedef struct
{
  uint32_t Bits_0_10                      : 10; /*!< Specific Power Capabilities are described by the (A)PDOs in the following sections. */
  uint32_t VoltageIn50mVunits             : 10; /*!< Maximum Voltage in 50mV units valid for all PDO (not APDO) */
  uint32_t Bits_20_29                     : 10; /*!< Specific Power Capabilities are described by the (A)PDOs in the following sections. */
  USBPD_CORE_PDO_Type_TypeDef PowerObject : 2;  /*!< (A) Power Data Object  */
} USBPD_GenericPDO_TypeDef;

/**
  * @brief  USB PD Power Data Object Structure definition
  *
  */
typedef union
{
  uint32_t d32;

  USBPD_GenericPDO_TypeDef            GenericPDO;       /*!< Generic Power Data Object Structure            */

  USBPD_SRCFixedSupplyPDO_TypeDef     SRCFixedPDO;      /*!< Fixed Supply PDO - Source                      */

  USBPD_SRCVariableSupplyPDO_TypeDef  SRCVariablePDO;   /*!< Variable Supply (non-Battery) PDO - Source     */

  USBPD_SRCBatterySupplyPDO_TypeDef   SRCBatteryPDO;    /*!< Battery Supply PDO - Source                    */

  USBPD_SNKFixedSupplyPDO_TypeDef     SNKFixedPDO;      /*!< Fixed Supply PDO - Sink                        */

  USBPD_SNKVariableSupplyPDO_TypeDef  SNKVariablePDO;   /*!< Variable Supply (non-Battery) PDO - Sink       */

  USBPD_SNKBatterySupplyPDO_TypeDef   SNKBatteryPDO;    /*!< Battery Supply PDO - Sink                      */

#if defined(USBPD_REV30_SUPPORT)
#ifdef USBPDCORE_PPS
  USBPD_ProgrammablePowerSupplyAPDO_TypeDef  SRCSNKAPDO;/*!< Programmable Power Supply APDO - Source / Sink */
#endif /* USBPDCORE_PPS */
#endif /* USBPD_REV30_SUPPORT */

} USBPD_PDO_TypeDef;

/**
  * @brief  USB PD Sink Fixed and Variable Request Data Object Structure definition
  *
  */
typedef struct
{
  uint32_t MaxOperatingCurrent10mAunits : /* Corresponding to min if GiveBackFlag = 1 */
      10;
  uint32_t OperatingCurrentIn10mAunits :
      10;
#if defined(USBPD_REV30_SUPPORT)
  uint32_t Reserved20_22 :
      3;
  uint32_t UnchunkedExtendedMessage :
      1;
#else
  uint32_t Reserved20_23 :
      4;
#endif /* USBPD_REV30_SUPPORT */
  uint32_t NoUSBSuspend :
      1;
  uint32_t USBCommunicationsCapable :
      1;
  uint32_t CapabilityMismatch :
      1;
  uint32_t GiveBackFlag :
      1;
  uint32_t ObjectPosition :
      3;
  uint32_t Reserved31 :
      1;
} USBPD_SNKFixedVariableRDO_TypeDef;

/**
  * @brief  USB PD Sink Battery Request Data Object Structure definition
  *
  */
typedef struct
{
  uint32_t MaxOperatingPowerIn250mWunits :
      10;
  uint32_t OperatingPowerIn250mWunits :
      10;
#if defined(USBPD_REV30_SUPPORT)
  uint32_t Reserved20_22 :
      3;
  uint32_t UnchunkedExtendedMessage :
      1;                                      /*!< Unchunked Extended Messages Supported                    */
#else
  uint32_t Reserved20_23 :
      4;
#endif /* USBPD_REV30_SUPPORT */
  uint32_t NoUSBSuspend :
      1;
  uint32_t USBCommunicationsCapable :
      1;
  uint32_t CapabilityMismatch :
      1;
  uint32_t GiveBackFlag :
      1;
  uint32_t ObjectPosition :
      3;
  uint32_t Reserved31 :
      1;
} USBPD_SNKBatteryRDO_TypeDef;

#if defined(USBPD_REV30_SUPPORT)
/**
  * @brief  USB PD Sink Programmable Request Data Object Structure definition
  *
  */
typedef struct
{
  uint32_t OperatingCurrentIn50mAunits  : 7;  /*!< Operating Current 50mA units                             */
  uint32_t Reserved1                    : 2;  /*!< Reserved  - Shall be set to zero                         */
  uint32_t OutputVoltageIn20mV          : 11; /*!< Output Voltage in 20mV units                             */
  uint32_t Reserved2                    : 3;  /*!< Reserved  - Shall be set to zero                         */
  uint32_t UnchunkedExtendedMessage     : 1;  /*!< Unchunked Extended Messages Supported                    */
  uint32_t NoUSBSuspend                 : 1;  /*!< No USB Suspend                                           */
  uint32_t USBCommunicationsCapable     : 1;  /*!< USB Communications Capable                               */
  uint32_t CapabilityMismatch           : 1;  /*!< Capability Mismatch                                      */
  uint32_t Reserved3                    : 1;  /*!< Reserved  - Shall be set to zero                         */
  uint32_t ObjectPosition               : 3;  /*!< Object position (000b is Reserved and Shall Not be used) */
  uint32_t Reserved4                    : 1;  /*!< USB Communications Capable                               */
} USBPD_SNKProgrammableRDO_TypeDef;
#endif /* USBPD_REV30_SUPPORT */


/**
  * @brief  USB PD Sink Generic Request Data Object Structure definition
  *
  */
typedef struct
{
#if defined(USBPD_REV30_SUPPORT)
  uint32_t Bits_0_22                    : 23; /*!< Bits 0 to 22 of RDO                                      */
  uint32_t UnchunkedExtendedMessage     : 1;  /*!< Unchunked Extended Messages Supported                    */
#else
  uint32_t Bits_0_23                    : 24;  /*!< Bits 0 to 23 of RDO                                     */
#endif /* USBPD_REV30_SUPPORT */
  uint32_t NoUSBSuspend                 : 1;  /*!< No USB Suspend                                           */
  uint32_t USBCommunicationsCapable     : 1;  /*!< USB Communications Capable                               */
  uint32_t CapabilityMismatch           : 1;  /*!< Capability Mismatch                                      */
  uint32_t Bit_27                       : 1;  /*!< Reserved  - Shall be set to zero                         */
  uint32_t ObjectPosition               : 3;  /*!< Object position (000b is Reserved and Shall Not be used) */
  uint32_t Bit_31                       : 1;  /*!< USB Communications Capable                               */
} USBPD_SNKGenericRDO_TypeDef;

/**
  * @brief  USB PD Sink Request Data Object Structure definition
  *
  */
typedef union
{
  uint32_t d32;

  USBPD_SNKGenericRDO_TypeDef       GenericRDO;       /*!<  Generic Request Data Object Structure           */

  USBPD_SNKFixedVariableRDO_TypeDef FixedVariableRDO; /*!< Fixed and Variable Request Data Object Structure */

  USBPD_SNKBatteryRDO_TypeDef       BatteryRDO;       /*!< Battery Request Data Object Structure            */

#if defined(USBPD_REV30_SUPPORT)
  USBPD_SNKProgrammableRDO_TypeDef  ProgRDO;          /*!< Programmable Request Data Object Structure       */
#endif /* USBPD_REV30_SUPPORT */

} USBPD_SNKRDO_TypeDef;

#if 0 /* Same structure as USBPD_SNKBatteryRDO_TypeDef */
/**
  * @brief  USB PD Sink Battery with GiveBack Request Data Object Structure definition
  *
  */
typedef union
{
  uint32_t d32;
  struct
  {
    uint32_t MinOperatingPowerIn250mWunits :
        10;
    uint32_t OperatingPowerIn250mWunits :
        10;
    uint32_t Reserved20_23 :
        4;
    uint32_t NoUSBSuspend :
        1;
    uint32_t USBCommunicationsCapable :
        1;
    uint32_t CapabilityMismatch :
        1;
    uint32_t GiveBackFlag :
        1;
    uint32_t ObjectPosition :
        3;
    uint32_t Reserved31 :
        1;
  }
  b;
} USBPD_SNKBatteryGiveBackRDO_TypeDef;
#endif

#if defined(USBPD_REV30_SUPPORT) && defined(USBPDCORE_PPS)
/**
  * @brief  USBPD Port APDO Structure definition
  *
  */
typedef struct
{
  uint32_t *ListOfAPDO;                          /*!< Pointer on Augmented Power Data Objects list, defining
                                                     port capabilities */
  uint8_t  NumberOfAPDO;                         /*!< Number of Augmented Power Data Objects defined in ListOfAPDO
                                                     This parameter must be set at max to @ref USBPD_MAX_NB_PDO value */
}USBPD_PortAPDO_TypeDef;
#endif /* USBPD_REV30_SUPPORT && USBPDCORE_PPS */

/**
  * @brief  USB PD BIST Data Object Structure definition
  *
  */
typedef union
{
  uint32_t d32;
  struct
  {
    uint32_t BistErrorCounter :
      16;
    uint32_t Reserved16_27 :
      12;
    uint32_t BistMode :
      4;
  }
  b;
} USBPD_BISTDataObject_TypeDef;

/** @brief  Sink requested power profile Structure definition
  *
  */
typedef struct
{
  uint32_t MaxOperatingCurrentInmAunits;           /*!< Sink board Max operating current in mA units   */
  uint32_t OperatingVoltageInmVunits;              /*!< Sink board operating voltage in mV units       */
  uint32_t MaxOperatingVoltageInmVunits;           /*!< Sink board Max operating voltage in mV units   */
  uint32_t MinOperatingVoltageInmVunits;           /*!< Sink board Min operating voltage in mV units   */
  uint32_t OperatingPowerInmWunits;                /*!< Sink board operating power in mW units         */
  uint32_t MaxOperatingPowerInmWunits;             /*!< Sink board Max operating power in mW units     */
} USBPD_SNKPowerRequest_TypeDef;


/** @defgroup USBPD_NotifyEventValue_TypeDef USBPD notification event type value
  * @brief notification envent used inside PE callbacks (USBPD_PE_NotifyDPM) to inform DPM
  * @{
  */
typedef enum
{
  USBPD_NOTIFY_REQUEST_ACCEPTED        = 1,
  USBPD_NOTIFY_REQUEST_REJECTED        = 2,
  USBPD_NOTIFY_REQUEST_WAIT            = 3,
  USBPD_NOTIFY_REQUEST_GOTOMIN         = 4,
  USBPD_NOTIFY_GETSNKCAP_SENT          = 5,
  USBPD_NOTIFY_GETSNKCAP_RECEIVED      = 6,
  USBPD_NOTIFY_GETSNKCAP_ACCEPTED      = 7,
  USBPD_NOTIFY_GETSNKCAP_REJECTED      = 8,
  USBPD_NOTIFY_GETSNKCAP_TIMEOUT       = 9,
  USBPD_NOTIFY_SNKCAP_SENT             =10,
  USBPD_NOTIFY_GETSRCCAP_SENT          =11,
  USBPD_NOTIFY_GETSRCCAP_RECEIVED      =12,
  USBPD_NOTIFY_GETSRCCAP_ACCEPTED      =13,
  USBPD_NOTIFY_GETSRCCAP_REJECTED      =14,
  USBPD_NOTIFY_SRCCAP_SENT             =15,
  USBPD_NOTIFY_POWER_EXPLICIT_CONTRACT =16,
  USBPD_NOTIFY_POWER_SRC_READY         =17,
  USBPD_NOTIFY_POWER_SNK_READY         =18,
  USBPD_NOTIFY_POWER_SNK_STOP          =19,
  USBPD_NOTIFY_POWER_SWAP_TO_SNK_DONE  =20,
  USBPD_NOTIFY_POWER_SWAP_TO_SRC_DONE  =21,
  USBPD_NOTIFY_POWER_SWAP_REJ          =22,
  USBPD_NOTIFY_POWER_SWAP_NOT_SUPPORTED=23,
  USBPD_NOTIFY_RESISTOR_ASSERT_RP      =24,
  USBPD_NOTIFY_RESISTOR_ASSERT_RD      =25,
  USBPD_NOTIFY_SVDM_ACK                =26,
  USBPD_NOTIFY_SVDM_NACK               =27,
  USBPD_NOTIFY_SVDM_BUSY               =28,
  USBPD_NOTIFY_SVDM_TIMEOUT            =29,
  USBPD_NOTIFY_HARDRESET_RX            =30,
  USBPD_NOTIFY_HARDRESET_TX            =31,
  USBPD_NOTIFY_STATE_SNK_READY         =32,
  USBPD_NOTIFY_STATE_SRC_DISABLED      =33,
  USBPD_NOTIFY_DATAROLESWAP_SENT       =34,
  USBPD_NOTIFY_DATAROLESWAP_RECEIVED   =35,
  USBPD_NOTIFY_DATAROLESWAP_UFP        =36,
  USBPD_NOTIFY_DATAROLESWAP_DFP        =37,
  USBPD_NOTIFY_DATAROLESWAP_WAIT       =38,
  USBPD_NOTIFY_DATAROLESWAP_REJECTED   =39,
  USBPD_NOTIFY_DATAROLESWAP_NOT_SUPPORTED=40,
  USBPD_NOTIFY_GOTOMIN_SENT            =41,
  USBPD_NOTIFY_GOTOMIN_POWERREADY      =42,
  USBPD_NOTIFY_SNK_GOTOMIN             =43,
  USBPD_NOTIFY_SNK_GOTOMIN_READY       =44,
  USBPD_NOTIFY_REQUEST_ERROR           =45,
  USBPD_NOTIFY_REQUEST_COMPLETE        =46,
  USBPD_NOTIFY_REQUEST_CANCELED        =47,
  USBPD_NOTIFY_SOFTRESET_SENT          =48,
  USBPD_NOTIFY_SOFTRESET_ACCEPTED      =49,
  USBPD_NOTIFY_SOFTRESET_RECEIVED      =50,
  USBPD_NOTIFY_PING_RECEIVED           =51,
  USBPD_NOTIFY_REQUEST_ENTER_MODE      =52,
  USBPD_NOTIFY_REQUEST_ENTER_MODE_ACK  =53,
  USBPD_NOTIFY_REQUEST_ENTER_MODE_NAK  =54,
  USBPD_NOTIFY_REQUEST_ENTER_MODE_BUSY =55,
  USBPD_NOTIFY_PD_SPECIFICATION_CHANGE =56,
  USBPD_NOTIFY_POWER_SWAP_SENT         =57,
  USBPD_NOTIFY_POWER_SWAP_ACCEPTED     =58,
  USBPD_NOTIFY_POWER_SWAP_WAIT         =59,
  USBPD_NOTIFY_POWER_SWAP_RECEIVED     =60,
  USBPD_NOTIFY_VCONN_SWAP_RECEIVED     =61,
  USBPD_NOTIFY_VCONN_SWAP_SENT         =62,
  USBPD_NOTIFY_VCONN_SWAP_ACCEPTED     =63,
  USBPD_NOTIFY_VCONN_SWAP_WAIT         =64,
  USBPD_NOTIFY_VCONN_SWAP_REJECTED     =65,
  USBPD_NOTIFY_VCONN_SWAP_COMPLETE     =66,
  USBPD_NOTIFY_VCONN_SWAP_NOT_SUPPORTED=67,
  USBPD_NOTIFY_CTRL_MSG_SENT           =68,
  USBPD_NOTIFY_DATA_MSG_SENT           =69,
  USBPD_NOTIFY_GET_SRC_CAP_EXT_RECEIVED=70,
  USBPD_NOTIFY_SRC_CAP_EXT_RECEIVED    =71,
  USBPD_NOTIFY_SRC_CAP_EXT_SENT        =72,
  USBPD_NOTIFY_GET_PPS_STATUS_RECEIVED =73,
  USBPD_NOTIFY_GET_PPS_STATUS_SENT     =74,
  USBPD_NOTIFY_PPS_STATUS_RECEIVED     =75,
  USBPD_NOTIFY_PPS_STATUS_SENT         =76,
  USBPD_NOTIFY_GET_STATUS_RECEIVED     =77,
  USBPD_NOTIFY_STATUS_RECEIVED         =78,
  USBPD_NOTIFY_STATUS_SENT             =79,
  USBPD_NOTIFY_ALERT_RECEIVED          =80,
  USBPD_NOTIFY_VDM_IDENTIFY_RECEIVED   =81,
  USBPD_NOTIFY_VDM_CABLE_IDENT_RECEIVED=82,
  USBPD_NOTIFY_VDM_SVID_RECEIVED       =83,
  USBPD_NOTIFY_VDM_MODE_RECEIVED       =84,
  USBPD_NOTIFY_REQUEST_EXIT_MODE       =85,
  USBPD_NOTIFY_REQUEST_EXIT_MODE_ACK   =86,
  USBPD_NOTIFY_REQUEST_EXIT_MODE_NAK   =87,
  USBPD_NOTIFY_REQUEST_EXIT_MODE_BUSY  =88,
  USBPD_NOTIFY_MSG_NOT_SUPPORTED       =89,
  USBPD_NOTIFY_POWER_STATE_CHANGE      =90,
  USBPD_NOTIFY_REQUEST_DISCARDED       =91,
  USBPD_NOTIFY_AMS_INTERRUPTED         =92,

  USBPD_NOTIFY_ALL                     = USBPD_NOTIFY_AMS_INTERRUPTED + 1,
} USBPD_NotifyEventValue_TypeDef;
/**
  * @}
  */

/** @defgroup USBPD_ProductVdo_TypeDef USB PD VDM Product VDO
  * @brief USB PD Product VDO Structure definition
  * @{
  */
typedef union
{
  uint32_t d32;
  struct
  {
    uint32_t bcdDevice :      /*!< Device version             */
    16;
    uint32_t USBProductId :   /*!< USB Product ID             */
    16;
  }
  b;
}USBPD_ProductVdo_TypeDef;

/**
  * @}
  */
/**
  * @brief Product Type field in ID Header
  * @{
  */
typedef enum
{
  PRODUCT_TYPE_UNDEFINED     = 0, /*!< Undefined                              */
  PRODUCT_TYPE_HUB           = 1, /*!< PDUSB Hub (UFP)                        */
  PRODUCT_TYPE_PERIPHERAL    = 2, /*!< PDUSB Host (UFP)                       */
#if defined(USBPD_REV30_SUPPORT)
  PRODUCT_TYPE_HOST          = 2, /*!< PDUSB Host  (DFP)                      */
  PRODUCT_TYPE_POWER_BRICK   = 3, /*!< Power Brick (DFP)                      */
#endif /* USBPD_REV30_SUPPORT */
  PRODUCT_TYPE_PASSIVE_CABLE = 3, /*!< Passive Cable (Cable Plug)             */
  PRODUCT_TYPE_ACTIVE_CABLE  = 4, /*!< Active Cable (Cable Plug)              */
#if defined(USBPD_REV30_SUPPORT)
  PRODUCT_TYPE_AMC           = 4, /*!<  Alternate Mode Controller (AMC) (DFP) */
#endif /* USBPD_REV30_SUPPORT */
  PRODUCT_TYPE_AMA           = 5, /*!< Alternate Mode Adapter (AMA) (UFP)     */
} USBPD_ProductType_TypeDef;

/**
  * @}
  */

/**
  * @brief USB Host or Device Capability field in ID Header
  * @{
  */
typedef enum
{
  USB_NOTCAPABLE     = 0, /*!< Not USB capable                                     */
  USB_CAPABLE        = 1, /*!< Capable of being enumerated as a USB host or device */
} USBPD_USBCapa_TypeDef;

/**
  * @}
  */

/**
  * @brief Modal operation field in ID Header
  * @{
  */
typedef enum
{
  MODAL_OPERATION_NONSUPP     = 0, /*!< Product not supports Modal Operation. */
  MODAL_OPERATION_SUPPORTED   = 1, /*!< Product supports Modal Operation.     */
} USBPD_ModalOp_TypeDef;

/**
  * @}
  */

/**
  * @brief Cable to USB field in Active/Passive cable
  * @{
  */
typedef enum
{
  CABLE_TO_TYPE_A   = 0, /*!< USB Type-A (PD 2.0 only)  */
  CABLE_TO_TYPE_B   = 1, /*!< USB Type-B (PD 2.0 only)  */
  CABLE_TO_TYPE_C   = 2, /*!< USB Type-C                */
  CABLE_CAPTIVE     = 3  /*!< Captive                   */
} USBPD_CableToType;

/**
  * @}
  */

/**
  * @brief  cable latency values in nanoseconds (max) in Active/Passive cable
  * @{
  */
typedef enum
{
  CABLE_LATENCY_10NS      = 1,  /*!< <10ns (~1m)        */
  CABLE_LATENCY_20NS      = 2,  /*!< 10ns to 20ns (~2m) */
  CABLE_LATENCY_30NS      = 3,  /*!< 20ns to 30ns (~3m) */
  CABLE_LATENCY_40NS      = 4,  /*!< 30ns to 40ns (~4m) */
  CABLE_LATENCY_50NS      = 5,  /*!< 40ns to 50ns (~5m) */
  CABLE_LATENCY_60NS      = 6,  /*!< 50ns to 60ns (~6m) */
  CABLE_LATENCY_70NS      = 7,  /*!< 60ns to 70ns (~7m) */
  CABLE_LATENCY_1000NS    = 8   /*!< > 70ns (>~7m) for P2.0 or 1000ns  (~100m) for P3.0    */
#if defined(USBPD_REV30_SUPPORT)
 ,CABLE_LATENCY_2000NS    = 9,  /*!< 2000ns (~200m)     */
  CABLE_LATENCY_3000NS    = 10  /*!< 3000ns (~300m)     */
#endif /* USBPD_REV30_SUPPORT */
} USBPD_CableLatency;

/**
  * @}
  */

/**
  * @brief  Cable maximun VBUS voltage
  * @{
  */
typedef enum
{
  VBUS_MAX_20V = 0,
  VBUS_MAX_30V = 1,
  VBUS_MAX_40V = 2,
  VBUS_MAX_50V = 3
} USBPD_CableMaxVoltage;

/**
  * @}
  */

/**
  * @brief  Cable Termination Type in Active/Passive cable
  * @{
  */
typedef enum
{
  CABLE_TERM_BOTH_PASSIVE_NO_VCONN = 0,  /*!< VCONN not required (PD2.0 only) */
  CABLE_TERM_BOTH_PASSIVE_VCONN    = 1   /*!< VCONN required (PD2.0 only)     */
#if defined(USBPD_REV30_SUPPORT)
 ,CABLE_TERM_ONE_EACH_VCONN        = 2,  /*!< One end Active, one end passive, VCONN required */
  CABLE_TERM_BOTH_ACTIVE_VCONN     = 3   /*!< Both ends Active, VCONN required  */
#endif /* USBPD_REV30_SUPPORT */
} USBPD_CableTermType;

/**
  * @}
  */

#if defined(USBPD_REV30_SUPPORT)
/**
  * @brief  Maximum Cable VBUS Voltage in Active/Passive cable
  * @{
  */
typedef enum
{
  VBUS_20V            = 0, /*!< Maximum Cable VBUS Voltage 20V */
  VBUS_30V            = 1, /*!< Maximum Cable VBUS Voltage 30V */
  VBUS_40V            = 2, /*!< Maximum Cable VBUS Voltage 40V */
  VBUS_50V            = 3  /*!< Maximum Cable VBUS Voltage 50V */
} USBPD_VBUSMaxVoltage;

/**
  * @}
  */
#endif /* USBPD_REV30_SUPPORT */

/**
  * @brief  configurability of SS Directionality in Active/Passive cable and AMA VDO (PD2.0 only)
  * @{
  */
typedef enum
{
  SS_DIR_FIXED         = 0, /*!< SSTX Directionality Support Fixed        */
  SS_DIR_CONFIGURABLE  = 1, /*!< SSTX Directionality Support Configurable */
} USBPD_SsDirectionality;

/**
  * @}
  */

/**
  * @brief SVDM commands definition
  */
typedef enum
{
  SVDM_RESERVEDCOMMAND,
  SVDM_DISCOVER_IDENTITY,
  SVDM_DISCOVER_SVIDS,
  SVDM_DISCOVER_MODES,
  SVDM_ENTER_MODE,
  SVDM_EXIT_MODE,
  SVDM_ATTENTION,
  SVDM_SPECIFIC_1 = 0x10,
  SVDM_SPECIFIC_2 = 0x11,
  SVDM_SPECIFIC_3 = 0x12,
  SVDM_SPECIFIC_4 = 0x13,
  SVDM_SPECIFIC_5,
  SVDM_SPECIFIC_6,
  SVDM_SPECIFIC_7,
  SVDM_SPECIFIC_8,
  SVDM_SPECIFIC_9,
  SVDM_SPECIFIC_10,
  SVDM_SPECIFIC_11,
  SVDM_SPECIFIC_12,
  SVDM_SPECIFIC_13,
  SVDM_SPECIFIC_14,
  SVDM_SPECIFIC_15,
  SVDM_SPECIFIC_16
} USBPD_VDM_Command_Typedef;

#if 0
/**
  * @brief  VBUS through cable-ness in Active/Passive cable VDO
  * @{
  */
typedef enum
{
  VBUS_THRU_CABLE_NO_ = 0, /*!< No VBUS Through Cable */
  VBUS_THRU_CABLE_YES = 1, /*!< VBUS Through Cable    */
} USBPD_VBUSThruCable;
#endif

/**
  * @brief  VBUS Current Handling Capability in Active/Passive cable VDO
  * @{
  */
typedef enum
{
  VBUS_3A    = 1, /*!< VBUS  Current Handling Capability 3A */
  VBUS_5A    = 2, /*!< VBUS  Current Handling Capability 5A */
} USBPD_VBUSCurrentHandCap;

/**
  * @}
  */

#if 0
/**
  * @brief  SOP" presence in Active cable VDO
  * @{
  */
typedef enum
{
  SOP2_NOT_PRESENT = 0, /*!< No SOP" controller present */
  SOP2_PRESENT     = 1, /*!< SOP" controller present    */
} USBPD_Sop2Presence;

/**
  * @}
  */
#endif

/**
  * @brief  USB Superspeed Signaling Support in Active/Passive cable VDO
  * @{
  */
typedef enum
{
  USB2P0_ONLY    = 0, /*!< USB2.0 only*/
  USB3P1_GEN1    = 1, /*!< USB3.1 Gen1 and USB2.0 */
  USB3P1_GEN1N2  = 2, /*!< USB3.1 Gen1, Gen2 and USB2.0*/
} USBPD_UsbSsSupport;

/**
  * @}
  */

/**
  * @brief  Power needed by adapter for full functionality in AMA VDO header
  * @{
  */
typedef enum
{
  VCONN_1W    = 0, /*!< VCONN  power 1W   */
  VCONN_1P5W  = 1, /*!< VCONN  power 1.5W */
  VCONN_2W    = 2, /*!< VCONN  power 2W   */
  VCONN_3W    = 3, /*!< VCONN  power 3W   */
  VCONN_4W    = 4, /*!< VCONN  power 4W   */
  VCONN_5W    = 5, /*!< VCONN  power 5W   */
  VCONN_6W    = 6, /*!< VCONN  power 6W   */
} USBPD_VConnPower;

/**
  * @}
  */

/**
  * @brief  VCONN being required by an adapter in AMA VDO header
  * @{
  */
typedef enum
{
  VCONN_NOT_REQUIRED =  0, /*!< VCONN not required  */
  VCONN_REQUIRED     =  1, /*!< VCONN required      */
} USBPD_VConnRequirement;

/**
  * @}
  */

/**
  * @brief  VBUS being required by an adapter in AMA VDO header
  * @{
  */
typedef enum
{
  VBUS_NOT_REQUIRED = 0, /*!< VBUS not required */
  VBUS_REQUIRED     = 1, /*!< VBUS required     */
} USBPD_VBusRequirement;

/**
  * @}
  */

typedef enum
{
  SVDM_INITIATOR,
  SVDM_RESPONDER_ACK,
  SVDM_RESPONDER_NAK,
  SVDM_RESPONDER_BUSY
}USBPD_VDM_CommandType_Typedef;


/** @defgroup USBPD_IDHeaderVDOStructure_definition USB SVDM ID header VDO Structure definition
  * @brief USB SVDM ID header VDO Structure definition
  * @{
  */
typedef union
{
  uint32_t d32;
  struct
  {
    uint32_t VID : 16;                                 /*!< SVDM Header's SVDM Version                 */
    uint32_t Reserved : 10;                            /*!< Reserved                                   */
    USBPD_ModalOp_TypeDef ModalOperation : 1;          /*!< Modal Operation Supported based on
                                                            @ref USBPD_ModalOp_TypeDef                 */
    USBPD_ProductType_TypeDef ProductTypeUFPorCP : 3;  /*!< Product Type (UFP or Cable Plug)based on
                                                            @ref USBPD_ProductType_TypeDef             */
    USBPD_USBCapa_TypeDef USBDevCapability : 1;        /*!< USB Communications Capable as a USB Device based on
                                                            @ref USBPD_USBCapa_TypeDef                 */
    USBPD_USBCapa_TypeDef USBHostCapability : 1;       /*!< USB Communications Capable as USB Host based on
                                                            @ref USBPD_USBCapa_TypeDef                 */
  }b20;
#if defined(USBPD_REV30_SUPPORT)
  struct
  {
    uint32_t VID : 16;                                 /*!< SVDM Header's SVDM Version                 */
    uint32_t Reserved : 7;                             /*!< Reserved                                   */
    uint32_t ProductTypeDFP : 3;                       /*!< Product Type (DFP) based on
                                                            @ref USBPD_ProductType_TypeDef             */
    USBPD_ModalOp_TypeDef ModalOperation : 1;          /*!< Modal Operation Supported based on
                                                            @ref USBPD_ModalOp_TypeDef                 */
    USBPD_ProductType_TypeDef ProductTypeUFPorCP : 3;  /*!< Product Type (UFP or Cable Plug)based on
                                                            @ref USBPD_ProductType_TypeDef             */
    USBPD_USBCapa_TypeDef USBDevCapability : 1;        /*!< USB Communications Capable as a USB Device based on
                                                            @ref USBPD_USBCapa_TypeDef                 */
    USBPD_USBCapa_TypeDef USBHostCapability : 1;       /*!< USB Communications Capable as USB Host based on
                                                            @ref USBPD_USBCapa_TypeDef                 */
  }b30;
#endif /* USBPD_REV30_SUPPORT */
}USBPD_IDHeaderVDO_TypeDef;
/**
  * @}
  */

typedef union
{
  struct /* PD 2.0*/
  {
    USBPD_SsDirectionality    SSRX2_DirSupport    : 1;  /*!< SSRX2 Directionality Support (PD2.0)     */
    USBPD_SsDirectionality    SSRX1_DirSupport    : 1;  /*!< SSRX1 Directionality Support (PD2.0)     */
    USBPD_SsDirectionality    SSTX2_DirSupport    : 1;  /*!< SSTX2 Directionality Support (PD2.0)     */
    USBPD_SsDirectionality    SSTX1_DirSupport    : 1;  /*!< SSTX1 Directionality Support (PD2.0)     */
  }
  pd_v20;
#if defined(USBPD_REV30_SUPPORT)
  struct /* PD 3.0*/
  {
    uint8_t                   Reserved            : 2;  /*!< Reserved                                 */
    USBPD_VBUSMaxVoltage      MaxVBUS_Voltage     : 2;  /*!< Maximum Cable VBUS Voltage               */
  }
  pd_v30;
#endif /* USBPD_REV30_SUPPORT */
} USBPD_CableVdo_Field1TypeDef;

/** @defgroup USBPD_AttentionInfo_TypeDef USB PD Attention Info object Structure definition
  * @brief USB PD Attention Info object Structure definition
  * @{
  */
typedef struct
{
  uint32_t  VDO;
  uint16_t  SVID;
  USBPD_VDM_Command_Typedef Command;
  uint8_t   ModeIndex;
}USBPD_AttentionInfo_TypeDef;

/**
  * @}
  */

/**
  * @brief  AMA USB Superspeed Signaling Support in AMA VDO header
  * @{
  */
typedef enum
{
  AMA_USB2P0_ONLY       = 0, /*!< USB2.0 only                   */
  AMA_USB3P1_GEN1       = 1, /*!< USB3.1 Gen1 and USB2.0        */
  AMA_USB3P1_GEN1N2     = 2, /*!< USB3.1 Gen1, Gen2 and USB2.0  */
  AMA_USB2P0_BILLBOARD  = 3, /*!< USB2.0 billboard only         */
} USBPD_AmaUsbSsSupport;

/**
  * @}
  */

#if defined(USBPDCORE_FWUPD)
/**
  * @brief  USBPD Firmware Update Status Information
  * @{
  */
typedef enum
{
  USBPD_FWUPD_STATUS_OK                         = 0x00u,     /*!< Request completed successfully or delayed                             */
  USBPD_FWUPD_STATUS_ERR_TARGET                 = 0x01u,     /*!< FW not targeted for this device                                       */
  USBPD_FWUPD_STATUS_ERR_FILE                   = 0x02u,     /*!< Fails vendor-specific verification test                               */
  USBPD_FWUPD_STATUS_ERR_WRITE                  = 0x03u,     /*!< Unable to write memory                                                */
  USBPD_FWUPD_STATUS_ERR_ERASE                  = 0x04u,     /*!< Memory erase function failed                                          */
  USBPD_FWUPD_STATUS_ERR_CHECK_ERASED           = 0x05u,     /*!< Memory erase check failed                                             */
  USBPD_FWUPD_STATUS_ERR_PROG                   = 0x06u,     /*!< Program memory function failed                                        */
  USBPD_FWUPD_STATUS_ERR_VERIFY                 = 0x07u,     /*!< Program memory failed verification                                    */
  USBPD_FWUPD_STATUS_ERR_ADDRESS                = 0x08u,     /*!< Received address is out of range                                      */
  USBPD_FWUPD_STATUS_ERR_NOTDONE                = 0x09u,     /*!< Received PDFU_DATA Request with a zero length Data Block, but the
                                                                  PDFU Responder expects more data                                      */
  USBPD_FWUPD_STATUS_ERR_FIRMWARE               = 0x0Au,     /*!< Device's firmware is corrupt. It cannot return to normal operations.  */
  USBPD_FWUPD_STATUS_ERR_POR                    = 0x0Du,     /*!< Unexpected power on reset                                             */
  USBPD_FWUPD_STATUS_ERR_UNKNOWN                = 0x0Eu,     /*!< Something went wrong                                                  */
  USBPD_FWUPD_STATUS_ERR_UNEXPECTED_HARD_RESET  = 0x80u,     /*!< Used when firmware update starts after a hard reset (enumeration, etc.)
                                                                  that occurred in the middle of firmware update */
  USBPD_FWUPD_STATUS_ERR_UNEXPECTED_SOFT_RESET  = 0x81u,     /*!< Used when firmware update starts after soft reset (new power contract, etc.)
                                                                  that occurred in the middle of firmware update */
  USBPD_FWUPD_STATUS_ERR_UNEXPECTED_REQUEST     = 0x82u,     /*!< PDFU Responder received a request that is not appropriate for the current Phase */
  USBPD_FWUPD_STATUS_ERR_REJECT_PAUSE           = 0x83u,     /*!< PDFU Responder is unable or unwilling to pause a firmware image transfer */
} USBPD_FWUPD_Status_TypeDef;
/**
  * @}
  */
#endif /* USBPDCORE_FWUPD */

/** @defgroup USBPD_CORE_VDM_Exported_Structures USBPD CORE VDM Exported Structures
  * @{
  */

typedef union
{
  struct /* PD 2.0*/
  {
    uint8_t                   Reserved            : 4;  /*!< Reserved                               */
  }
  pd_v20;
#if defined(USBPD_REV30_SUPPORT)
  struct /* PD 3.0*/
  {
    uint8_t                   VDOVersion          : 3;  /*!< Version Number of the VDO              */
    uint8_t                   Reserved            : 1;  /*!< Reserved                               */
  }
  pd_v30;
#endif /* USBPD_REV30_SUPPORT */
}USBPD_CableVdo_Field2TypeDef;

typedef enum
{
  VDM_UNSTRUCTUREDVDM_TYPE= 0x0,
  VDM_STRUCTUREDVDM_TYPE= 0x1
}USBPD_VDM_VDMType_Typedef;

/** @defgroup USBPD_SVDMHeaderStructure_definition USB SVDM Message header Structure definition
  * @brief USB SVDM Message header Structure definition
  * @{
  */
typedef union
{
  uint32_t d32;
  struct
  {
    USBPD_VDM_Command_Typedef Command :         /*!< SVDM Header's Command          */
    5;
    uint32_t Reserved5 :                        /*!< Reserved                       */
    1;
    USBPD_VDM_CommandType_Typedef CommandType : /*!< SVDM Header's Command Type     */
    2;
    uint32_t ObjectPosition :                   /*!< SVDM Header's Object Position  */
    3;
    uint32_t Reserved11 :                       /*!< Reserved                       */
    2;
    uint32_t SVDMVersion :                      /*!< SVDM Header's SVDM Version     */
    2;
    USBPD_VDM_VDMType_Typedef VDMType :         /*!< SVDM Header's VDM Type         */
    1;
    uint32_t SVID :                             /*!< SVDM Header's SVID             */
    16;
  }
  b;
}USBPD_SVDMHeader_TypeDef;

/**
  * @}
  */

#ifdef USBPDCORE_UVDM
/** @defgroup USBPD_UVDMHeaderStructure_definition USB UVDM Message header Structure definition
  * @brief USB UVDM Message header Structure definition
  * @{
  */
typedef union
{
  uint32_t d32;
  struct
  {
    uint32_t VendorUse                : 15;  /*!< Content of this field is defined by the vendor.  */
    USBPD_VDM_VDMType_Typedef VDMType : 1;   /*!< VDM Type                                         */
    uint32_t VID                      : 16;  /*!< Vendor ID (VID)                                  */
  }
  b;
}USBPD_UVDMHeader_TypeDef;

/**
  * @}
  */
#endif /* USBPDCORE_UVDM */

/** @defgroup USBPD_CableVdo_TypeDef USB PD VDM Cable VDO
  * @brief USB PD Cable VDO Structure definition
  * @{
  */
typedef union
{
  uint32_t d32;
  struct 
  {
    USBPD_UsbSsSupport        USB_SS_Support      : 3;  /*!< USB SuperSpeed Signaling Support           */
#if 0
    USBPD_Sop2Presence        SOP2_Presence       : 1;  /*!< SOP" controller present? (Active cable)    */
    USBPD_VBUSThruCable       VBUS_ThruCable      : 1;  /*!< VBUS through cable                         */
#else
    uint32_t                  reserved            : 2;
#endif
    USBPD_VBUSCurrentHandCap  VBUS_CurrentHandCap : 2;  /*!< VBUS Current Handling Capability           */
    uint32_t                  Fields1             : 2;  /*!< Based on @ref USBPD_CableVdo_Field1TypeDef */
    USBPD_CableMaxVoltage     CableMaxVoltage     : 2;  /*!< Cable maximun voltage                      */
    USBPD_CableTermType       CableTermType       : 2;  /*!< Cable Termination Type                     */
    uint32_t                  CableLatency        : 4;  /*!< Cable Latency                              */
    uint32_t                  Reserved            : 1;  /*!< Reserved                                   */
    USBPD_CableToType         CableToType         : 2;  /*!< USB Type-C plug to USB Type-A/B/C/Captive (PD 2.0)
                                                             USB Type-C plug to USB Type-C/Captive (PD 3.0) */
    uint32_t                  Fields2             : 1;  /*!< Based on @ref USBPD_CableVdo_Field2TypeDef */
    uint32_t                  VDO_Version         : 3;  /*!< Version number  of the VDO                 */
    uint32_t                  CableFWVersion      : 4;  /*!< Cable FW version number (vendor defined)   */
    uint32_t                  CableHWVersion      : 4;  /*!< Cable HW version number (vendor defined)   */
  }
  b;
}USBPD_CableVdo_TypeDef;

/**
  * @}
  */

/** @defgroup USBPD_CertStatVdo_TypeDef USB PD VDM Cert stat VDO
  * @brief USB PD Cert stat VDO Structure definition
  * @{
  */
typedef union
{
  uint32_t d32;
  struct
  {
    uint32_t XID :          /*!< USB-IF assigned XID */
    32;
  }
  b;
}USBPD_CertStatVdo_TypeDef;

/**
  * @}
  */

/** @defgroup USBPD_AMAVdo_TypeDef USB PD VDM Alternate Mode Adapter VDO
  * @brief USB PD Alternate Mode Adapter VDO Structure definition
  * @{
  */
typedef union
{
  uint32_t d32;
  struct
  {
    uint32_t                AMA_USB_SS_Support  : 3;  /*!< AMA USB SuperSpeed Signaling Support based on
                                                           @ref USBPD_AmaUsbSsSupport               */
    uint32_t                VBUSRequirement     : 1;  /*!< VBUS  required  based on
                                                           @ref USBPD_VBusRequirement               */
    uint32_t                VCONNRequirement    : 1;  /*!< VCONN  required  based on
                                                           @ref USBPD_VConnRequirement              */
    uint32_t                VCONNPower          : 3;  /*!< VCONN  power  based on
                                                           @ref USBPD_VConnPower                    */
#if defined(USBPD_REV30_SUPPORT)
    uint32_t                Reserved            : 13; /*!< Reserved                                 */
    uint32_t                VDO_Version         : 3;  /*!< Version Number of the VDO                */
#else
    uint32_t                SSRX2_DirSupport    : 1;  /*!< SSRX2 Directionality Support (PD2.0) based on
                                                           @ref USBPD_SsDirectionality              */
    uint32_t                SSRX1_DirSupport    : 1;  /*!< SSRX1 Directionality Support (PD2.0) based on
                                                           @ref USBPD_SsDirectionality              */
    uint32_t                SSTX2_DirSupport    : 1;  /*!< SSTX2 Directionality Support (PD2.0) based on
                                                           @ref USBPD_SsDirectionality              */
    uint32_t                SSTX1_DirSupport    : 1;  /*!< SSTX1 Directionality Support (PD2.0) based on
                                                           @ref USBPD_SsDirectionality              */
    uint32_t                Reserved            : 12; /*!< Reserved                                 */
#endif /* USBPD_REV30_SUPPORT */
    uint32_t                AMAFWVersion        : 4;  /*!< AMA FW version number (vendor defined)   */
    uint32_t                AMAHWVersion        : 4;  /*!< AMA HW version number (vendor defined)   */
  }
  b;
}USBPD_AMAVdo_TypeDef;
/**
  * @}
  */

/** @defgroup USBPD_DiscoveryIdentity_TypeDef USB PD Discovery identity Structure definition
  * @brief Data received from Discover Identity messages
  * @{
  */
typedef struct
{

  USBPD_IDHeaderVDO_TypeDef IDHeader;             /*!< ID Header VDO                              */
  USBPD_CertStatVdo_TypeDef CertStatVDO;          /*!< Cert Stat VDO                              */
  USBPD_ProductVdo_TypeDef  ProductVDO;           /*!< Product VDO                                */
  USBPD_CableVdo_TypeDef    CableVDO;             /*!< Passive Cable VDO                          */
  USBPD_AMAVdo_TypeDef      AMA_VDO;              /*!< Alternate Mode Adapter VDO                 */
  uint8_t                   CableVDO_Presence:1;    /*!< Indicate Passive Cable VDO presence or not */
  uint8_t                   AMA_VDO_Presence:1;     /*!< Indicate Alternate Mode Adapter VDO presence or not    */
  uint8_t                   Reserved:6;           /*!< Reserved bits                              */
} USBPD_DiscoveryIdentity_TypeDef;
/**
  * @}
  */

/** @defgroup USBPD_ModeInfo_TypeDef USB PD Mode Info object Structure definition
  * @brief USB PD Mode Info object Structure definition
  * @{
  */
typedef struct
{
  uint32_t  NumModes;
  uint32_t  Modes[MAX_MODES_PER_SVID];
  uint16_t  SVID;
}USBPD_ModeInfo_TypeDef;

/**
  * @}
  */

/** @defgroup USBPD_SVID_TypeDef USB PD Discovery SVID Structure definition
  * @brief Data received from Discover Identity messages
  * @{
  */
/*
 * Structure to SVID supported by the devices
 */
typedef struct
{
  uint16_t  SVIDs[12];
  uint8_t  NumSVIDs;
  uint8_t  AllSVID_Received; /* Flag to indicate that all the SVIDs have been received.
                                No need to send new SVDM Discovery SVID message */
}USBPD_SVIDInfo_TypeDef;
/**
  * @}
  */

/** @defgroup USBPD_CORE_DataInfoType_TypeDef USB CORE Data information type
  * @brief Data Infor types used in PE callbacks (USBPD_PE_GetDataInfo and USBPD_PE_SetDataInfo)
  * @{
  */
typedef enum
{
  USBPD_CORE_DATATYPE_SRC_PDO          = 0x00,      /*!< Handling of port Source PDO                  */
  USBPD_CORE_DATATYPE_SNK_PDO          = 0x01,      /*!< Handling of port Sink PDO                    */
  USBPD_CORE_DATATYPE_RDO_POSITION     = 0x02,      /*!< Storage of requested DO position in PDO list */
  USBPD_CORE_DATATYPE_REQ_VOLTAGE      = 0x03,      /*!< Storage of requested voltage value */
  USBPD_CORE_DATATYPE_RCV_SRC_PDO      = 0x04,      /*!< Storage of Received Source PDO values        */
  USBPD_CORE_DATATYPE_RCV_SNK_PDO      = 0x05,      /*!< Storage of Received Sink PDO values          */
  USBPD_CORE_DATATYPE_RCV_REQ_PDO      = 0x06,      /*!< Storage of Received Sink Request PDO value   */
  USBPD_CORE_DATATYPE_REQUEST_DO       = 0x07,      /*!< Storage of DO to be used in request message (from Sink to Source) */
  USBPD_CORE_EXTENDED_CAPA             = 0x08,      /*!< Extended capability                          */
  USBPD_CORE_INFO_STATUS               = 0x09,      /*!< Information status                           */
  USBPD_CORE_PPS_STATUS                = 0x0A,      /*!< PPS Status data                              */
  USBPD_CORE_ALERT                           ,      /*!< Alert                                        */
  USBPD_CORE_GET_MANUFACTURER_INFO           ,      /*!< Get Manufacturer Info                        */
  USBPD_CORE_MANUFACTURER_INFO               ,      /*!< Manufacturer Info                            */
  USBPD_CORE_GET_BATTERY_STATUS              ,      /*!< Get Battery Status                           */
  USBPD_CORE_BATTERY_STATUS                  ,      /*!< Battery Status                               */
  USBPD_CORE_GET_BATTERY_CAPABILITY          ,      /*!< Get Battery Capability                       */
  USBPD_CORE_BATTERY_CAPABILITY              ,      /*!< Battery Capability                           */
  USBPD_CORE_UNSTRUCTURED_VDM                ,      /*!< Unstructured VDM messag                      */
} USBPD_CORE_DataInfoType_TypeDef;
/**
  * @}
  */

#if defined(USBPD_REV30_SUPPORT)
/**
  * @brief  USBPD Alert Data Object Structure definition
  *
  */
typedef union
{
  uint32_t d32;
  struct
  {
    uint32_t Reserved             : 16; /*!< Reserved */
    uint32_t HotSwappableBatteries: 4;  /*!< Hot Swappable Batteries is a combination of @ref USBPD_ADO_HOT_SWAP_BATT */
    uint32_t FixedBatteries       : 4;  /*!< Fixed Batteries is a combination of @ref USBPD_ADO_FIXED_BATT */
    uint32_t TypeAlert            : 8;  /*!< Type of Alert is a combination of @ref USBPD_ADO_TYPE_ALERT */
  }
  b;
} USBPD_ADO_TypeDef;

/**
  * @brief  USBPD Battery Status Data Object Structure definition
  *
  */
typedef union
{
  uint32_t d32;
  struct
  {
    uint32_t Reserved: 8;
    uint32_t BatteryInfo: 8; /*!< Based on @ref USBPD_BSDO_BATT_INFO */
    uint32_t BatteryPC: 16;
  }
  b;
} USBPD_BSDO_TypeDef;

/**
  * @brief  USBPD Source Capabilities Extended Message Structure definition
  *
  */
typedef struct USBPD_SCEDB_TypeDef
{
  uint16_t VID;                 /*!< Vendor ID (assigned by the USB-IF)                   */
  uint16_t PID;                 /*!< Product ID (assigned by the manufacturer)            */
  uint32_t XID;                 /*!< Value provided by the USB-IF assigned to the product */
  uint8_t  FW_revision;         /*!< Firmware version number                              */
  uint8_t  HW_revision;         /*!< Hardware version number                              */
  uint8_t  Voltage_regulation;  /*!< Voltage Regulation                                   */
  uint8_t  Holdup_time;         /*!< Holdup Time                                          */
  uint8_t  Compliance;          /*!< Compliance                                           */
  uint8_t  TouchCurrent;        /*!< Touch Current                                        */
  uint16_t PeakCurrent1;        /*!< Peak Current1                                        */
  uint16_t PeakCurrent2;        /*!< Peak Current2                                        */
  uint16_t PeakCurrent3;        /*!< Peak Current3                                        */
  uint8_t  Touchtemp;           /*!< Touch Temp                                           */
  uint8_t  Source_inputs;       /*!< Source Inputs                                        */
  uint8_t  NbBatteries;         /*!< Number of Batteries/Battery Slots                    */
  uint8_t  SourcePDP;           /*!< Source PDP                                           */
} USBPD_SCEDB_TypeDef;

/**
  * @brief  USBPD Source Status Extended Message Structure definition
  *
  */
typedef __PACKEDSTRUCTBEGIN
{
  uint8_t InternalTemp;          /*!< Source or Sink internal temperature in degrees centigrade */
  uint8_t PresentInput;          /*!< Present Input                                             */
  uint8_t PresentBatteryInput;   /*!< Present Battery Input                                     */
  uint8_t EventFlags;            /*!< Event Flags                                               */
  uint8_t TemperatureStatus;     /*!< Temperature                                               */
} __PACKEDSTRUCTEND USBPD_SDB_TypeDef;

/**
  * @brief  USBPD Get Battery Capabilities Data Block Extended Message Structure definition
  *
  */
typedef __PACKEDSTRUCTBEGIN
{
  uint8_t BatteryCapRef;     /*!< Number of the Battery indexed from zero    */
} __PACKEDSTRUCTEND USBPD_GBCDB_TypeDef;

/**
  * @brief  USBPD Get Battery Status Data Block Extended Message Structure definition
  *
  */
typedef __PACKEDSTRUCTBEGIN
{
  uint8_t BatteryStatusRef;     /*!< Number of the Battery indexed from zero  */
} __PACKEDSTRUCTEND USBPD_GBSDB_TypeDef;

/**
  * @brief  USBPD  Battery Capability Data Block Extended Message Structure definition
  *
  */
typedef __PACKEDSTRUCTBEGIN
{
  uint16_t VID;                       /*!< Vendor ID (assigned by the USB-IF)         */
  uint16_t PID;                       /*!< Product ID (assigned by the manufacturer)  */
  uint16_t BatteryDesignCapa;         /*!< Battery Design Capacity                    */
  uint16_t BatteryLastFullChargeCapa; /*!< Battery last full charge capacity        */
  uint8_t  BatteryType;               /*!< Battery Type                               */
} __PACKEDSTRUCTEND USBPD_BCDB_TypeDef;

/**
  * @brief  USBPD Get Manufacturer Info Info Data Block Extended Message Structure definition
  *
  */
typedef __PACKEDSTRUCTBEGIN
{
  uint8_t ManufacturerInfoTarget;     /*!< Manufacturer Info Target based on @ref USBPD_MANUFINFO_TARGET                      */
  uint8_t ManufacturerInfoRef;        /*!< Manufacturer Info Ref between Min_Data=0 and Max_Data=7 (@ref USBPD_MANUFINFO_REF) */
} __PACKEDSTRUCTEND USBPD_GMIDB_TypeDef;

/**
  * @brief  USBPD Manufacturer Info Data Block Extended Message Structure definition
  *
  */
typedef struct
{
  uint16_t VID;                       /*!< Vendor ID (assigned by the USB-IF)        */
  uint16_t PID;                       /*!< Product ID (assigned by the manufacturer) */
  uint8_t ManuString[22];             /*!< Vendor defined byte array                 */
} USBPD_MIDB_TypeDef;

#if defined(USBPDCORE_FWUPD)
/**
  * @brief  USBPD Firmware Update GET_FW_ID Response Payload Structure definition
  *
  */
typedef __PACKEDSTRUCTBEGIN
{
  USBPD_FWUPD_Status_TypeDef   Status;  /*!< Status Information during Firmware Update      */
  uint16_t  VID;         /*!< USB-IF assigned Vendor ID                                     */
  uint16_t  PID;         /*!< USB-IF assigned Product ID                                    */
  uint8_t   HWVersion;   /*!< Hardware Version                                              */
  uint8_t   SiVersion;   /*!< Silicon Version                                               */
  uint16_t  FWVersion1;  /*!< Most significant component of the firmware version            */
  uint16_t  FWVersion2;  /*!< Second-most significant component of the firmware version     */
  uint16_t  FWVersion3;  /*!< Third-most significant component of the firmware version      */
  uint16_t  FWVersion4;  /*!< Least significant component of the firmware version           */
  uint8_t   ImageBank;   /*!< Image bank for which firmware is requested                    */
  uint8_t   Flags1;      /*!< Flags1                                                        */
  uint8_t   Flags2;      /*!< Flags2                                                        */
  uint8_t   Flags3;      /*!< Flags3                                                        */
  uint8_t   Flags4;      /*!< Flags4                                                        */
} __PACKEDSTRUCTEND USBPD_FWUPD_GetFwIDRspPayload_TypeDef;

/**
  * @brief  USBPD Firmware Update PDFU_INITIATE Request Payload Structure definition
  *
  */
typedef __PACKEDSTRUCTBEGIN
{
  uint16_t  FWVersion1;  /*!< Most significant component of the firmware version            */
  uint16_t  FWVersion2;  /*!< Second-most significant component of the firmware version     */
  uint16_t  FWVersion3;  /*!< Third-most significant component of the firmware version      */
  uint16_t  FWVersion4;  /*!< Least significant component of the firmware version           */
} __PACKEDSTRUCTEND USBPD_FWUPD_PdfuInitReqPayload_TypeDef;

/**
  * @brief  USBPD Firmware Update PDFU_INITIATE Response Payload Structure definition
  *
  */
typedef __PACKEDSTRUCTBEGIN
{
  USBPD_FWUPD_Status_TypeDef   Status;  /*!< Status Information during Firmware Update      */
  uint8_t   WaitTime;         /*!< Wait time                                                */
  uint8_t   MaxImageSize[3];  /*!< Max image size                                           */
} __PACKEDSTRUCTEND USBPD_FWUPD_PdfuInitRspPayload_TypeDef;

/**
  * @brief  USBPD Firmware Update PDFU_DATA Response Payload Structure definition
  *
  */
typedef __PACKEDSTRUCTBEGIN
{
  USBPD_FWUPD_Status_TypeDef   Status;  /*!< Status Information during Firmware Update      */
  uint8_t   WaitTime;         /*!< Wait time                                                */
  uint8_t   NumDataNR;        /*!< Number of PDFU_DATA_NR Requests                          */
  uint16_t  DataBlockNum;     /*!< Data Block Number of the next PDFU_DATA or PDFU_DATA_NR  */
} __PACKEDSTRUCTEND USBPD_FWUPD_PdfuDataRspPayload_TypeDef;

/**
  * @brief  USBPD Firmware Update PDFU_VALIDATE Response Payload Structure definition
  *
  */
typedef __PACKEDSTRUCTBEGIN
{
  USBPD_FWUPD_Status_TypeDef   Status;  /*!< Status Information during Firmware Update      */
  uint8_t   WaitTime;         /*!< Wait time                                                */
  uint8_t   Flags;            /*!< Flags                                                    */
} __PACKEDSTRUCTEND USBPD_FWUPD_PdfuValidateRspPayload_TypeDef;

/**
  * @brief  USBPD Firmware Update PDFU_DATA_PAUSE Response Payload Structure definition
  *
  */
typedef __PACKEDSTRUCTBEGIN
{
  USBPD_FWUPD_Status_TypeDef   Status;  /*!< Status Information during Firmware Update      */
} __PACKEDSTRUCTEND USBPD_FWUPD_PdfuDataPauseRspPayload_TypeDef;

/**
  * @brief  USBPD Firmware Update VENDOR_SPECIFIC Request Payload Structure definition
  *
  */
typedef __PACKEDSTRUCTBEGIN
{
  uint16_t  VID;                /*!< USB-IF assigned Vendor ID                              */
  uint8_t   VendorDefined[256]; /*!< Vendor defined                                         */
} __PACKEDSTRUCTEND USBPD_FWUPD_VendorSpecificReqPayload_TypeDef;

/**
  * @brief  USBPD Firmware Update VENDOR_SPECIFIC Response Payload Structure definition
  *
  */
typedef __PACKEDSTRUCTBEGIN
{
  USBPD_FWUPD_Status_TypeDef   Status;  /*!< Status Information during Firmware Update      */
  uint16_t  VID;                /*!< USB-IF assigned Vendor ID                              */
  uint8_t   VendorDefined[255]; /*!< Vendor defined                                         */
} __PACKEDSTRUCTEND USBPD_FWUPD_VendorSpecificRspPayload_TypeDef;

/**
  * @brief  USBPD Firmware Update Request Data Block Extended Message Structure definition
  *
  */
typedef __PACKEDSTRUCTBEGIN
{
  uint8_t   ProtocolVersion;  /*!< Protocol Version (@ref USBPD_FWUPD_PROT_VER)             */
  uint8_t   MessageType;      /*!< Firmware Update Message type (@ref USBPD_FWUPD_MSGTYPE)  */
  uint8_t   Payload[258];     /*!< Payload                                         */
} __PACKEDSTRUCTEND USBPD_FRQDB_TypeDef;

#endif /* USBPDCORE_FWUPD */

#ifdef USBPDCORE_PPS
/**
  * @brief  USBPD PPS Status Data Block Extended Message Structure definition
  *
  */
typedef union
{
  uint32_t d32;
  struct
  {
    uint16_t OutputVoltageIn20mVunits;  /*!< Source output voltage in 20mV units.
                                             When set to 0xFFFF, the Source does not support this field.          */
    uint8_t  OutputCurrentIn50mAunits;  /*!< Source output current in 50mA units.
                                             When set to 0xFF, the Source does not support this field.            */
    uint8_t  RealTimeFlags;             /*!< Real Time Flags, combination of @ref USBPD_CORE_DEF_REAL_TIME_FLAGS  */
  } fields;
} USBPD_PPSSDB_TypeDef;
#endif /* USBPDCORE_PPS */

/**
  * @brief  USBPD Country Code Data Block Extended Message Structure definition
  *
  */
typedef struct
{
  uint32_t Length;            /*!< Number of country codes in the message */
  uint16_t *PtrCountryCode;   /*!< Pointer of the country codes (1 to n)  */
} USBPD_CCDB_TypeDef;

/**
  * @brief  USBPD Country Info Data Block Extended Message Structure definition
  *
  */
typedef struct
{
  uint16_t CountryCode;             /*!< 1st and 2nd character of the Alpha-2 Country Code defined by [ISO 3166]  */
  uint32_t Reserved;                /*!< Reserved - Shall be set to 0.   */
  uint8_t  PtrCountrySpecificData;  /*!< Pointer on Content defined by the country authority (0 t 256 bytes). */
} USBPD_CIDB_TypeDef;

#endif /* USBPD_REV30_SUPPORT */

/**
  * @brief  USBPD Settings Structure definition
  *
  */
#if defined(USBPD_REV30_SUPPORT)
typedef union {
  uint16_t PD3_Support;
  struct {
    uint16_t PE_UnchunkSupport                : 1; /*!< Unchunked support */
    uint16_t PE_FastRoleSwapSupport           : 1; /*!< Fast role swap support */
    uint16_t Is_GetPPSStatus_Supported        : 1; /*!< Get PPS status message supported by PE */
    uint16_t Is_SrcCapaExt_Supported          : 1; /*!< Source_Capabilities_Extended message supported by PE */
    uint16_t Is_Alert_Supported               : 1; /*!< Alert message supported by PE */
    uint16_t Is_GetStatus_Supported           : 1; /*!< Get_Status message supported by PE (Is_Alert_Supported should be enabled) */
    uint16_t Is_GetManufacturerInfo_Supported : 1; /*!< Manufacturer_Info message supported by PE */
    uint16_t Is_GetCountryCodes_Supported     : 1; /*!< Get_Country_Codes message supported by PE */
    uint16_t Is_GetCountryInfo_Supported      : 1; /*!< Get_Country_Info message supported by PE */
    uint16_t Is_SecurityRequest_Supported     : 1; /*!< Security_Response message supported by PE */
    uint16_t Is_FirmUpdateRequest_Supported   : 1; /*!< Firmware update response message supported by PE */
    uint16_t reserved                         : 4; /*!< Reserved bits */
  } d;
}USBPD_PD3SupportTypeDef;
#endif /* USBPD_REV30_SUPPORT */

typedef struct
{
  uint32_t PE_SupportedSOP;

  USBPD_SpecRev_TypeDef PE_SpecRevision     : 2; /* spec revision value                                     */
  USBPD_PortPowerRole_TypeDef PE_DefaultRole: 1; /* Default port role                                       */
  uint32_t PE_RoleSwap                      : 1; /* support Power role swap                                 */
  uint32_t PE_DataSwap                      : 1; /* support Data role swap                                  */
  uint32_t PE_VDMSupport                    : 1; /* support VDM                                             */
  uint32_t PE_PingSupport                   : 1; /* support Ping (only for PD3.0)                           */
  uint32_t PE_CapscounterSupport            : 1;
  uint32_t PE_RespondsToDiscovSOP           : 1; /*!< Can respond successfully to a Discover Identity */
  uint32_t PE_AttemptsDiscovSOP             : 1; /*!< Can send a Discover Identity */
  uint32_t CAD_TryFeature                   : 2;
  uint32_t CAD_AccesorySupport              : 1;
#if defined(USBPD_REV30_SUPPORT)
  USBPD_PD3SupportTypeDef PE_PD3_Support;
#else
  uint16_t reserved                         : 16; /*!< Reserved bits */
#endif /* USBPD_REV30_SUPPORT */
  uint32_t                          CAD_SRCToogleTime    : 8;
  uint32_t                          CAD_SNKToogleTime    : 8;
  uint32_t                          CAD_RoleToggle       : 1;
  CAD_RP_Source_Current_Adv_Typedef CAD_DefaultResistor  : 2;
  uint32_t Reserved2                        :13; /*!< Reserved bits */
} USBPD_SettingsTypeDef;

/**
  * @brief  USBPD Settings Structure definition
  *
  */
typedef struct
{
  USBPD_SpecRev_TypeDef               PE_SpecRevision : 2;  /*!< PE Specification revision                                */
  USBPD_PortPowerRole_TypeDef         PE_PowerRole    : 1;  /*!< PE Power role                                            */
  USBPD_PortDataRole_TypeDef          PE_DataRole     : 1;  /*!< PE Data role                                             */
  uint32_t                            PE_SwapOngoing  : 1;  /*!< Power role swap ongoing flag                             */
  USBPD_VDMVersion_TypeDef            VDM_Version     : 2;  /*!< VDM version                                              */
  USBPD_VDMVersion_TypeDef            VDM_ModalOpe    : 1;  /*!< VDM operation mode support                               */
  CCxPin_TypeDef                      ActiveCCIs      : 2;  /*!< Active CC line based on @ref CCxPin_TypeDef              */
  uint32_t                            DPM_Initialized : 1;  /*!< DPM initialized flag                                     */
  uint32_t                            PE_IsConnected  : 1;  /*!< USB-PD PE stack is connected to CC line                  */
  USBPD_POWER_StateTypedef            PE_Power        : 3;  /*!< Power status based on @ref USBPD_POWER_StateTypedef      */
  CCxPin_TypeDef                      VconnCCIs       : 2;  /*!< VConn  CC line based on @ref CCxPin_TypeDef              */
  uint32_t                            VconnStatus     : 1;  /*!< VConnStatus USBP_TRUE = vconn on USBPD_FALSE = vconn off */
  CAD_RP_Source_Current_Adv_Typedef   RpResistor      : 2;  /*!< RpResistor presented                                     */
#if defined(USBPD_REV30_SUPPORT) && defined(USBPDCORE_UNCHUNCKED_MODE)
  uint32_t                    PE_UnchunkSupport: 1; /*!< Unchunked support                                        */
  uint32_t                    Reserved        : 12; /*!< Reserved bits                                            */
#else
  uint32_t                    Reserved        : 13 ; /*!< Reserved bits                                            */
#endif /* USBPD_REV30_SUPPORT && USBPDCORE_UNCHUNCKED_MODE */
} USBPD_ParamsTypeDef;

/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup USBPD_CORE_DEF_Exported_Macros USBPD CORE DEF Exported Macros
  * @{
  */
/**
  * @brief  Compare two variables and return the smallest
  * @param  __VAR1__ First variable to be compared
  * @param  __VAR2__ Second variable to be compared
  * @retval Returns the smallest variable
  */
#define USBPD_MIN(__VAR1__, __VAR2__) (((__VAR1__) > (__VAR2__))?(__VAR2__):(__VAR1__))

/**
  * @brief  Check if the requested voltage is valid
  * @param  __MV__    Requested voltage in mV units
  * @param  __MAXMV__ Max Requested voltage in mV units
  * @param  __MINMV__ Min Requested voltage in mV units
  * @retval 1 if valid voltage else 0
  */
#define USBPD_IS_VALID_VOLTAGE(__MV__, __MAXMV__, __MINMV__) ((((__MV__) <= (__MAXMV__)) && ((__MV__) >= (__MINMV__)))? 1: 0)

/**
  * @}
  */

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

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

#endif /* __USBPD_DEF_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
