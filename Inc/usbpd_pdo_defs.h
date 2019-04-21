/**
  ******************************************************************************
  * @file    usbpd_pdo_defs.h
  * @author  MCD Application Team
  * @brief   Header file for definition of PDO/APDO values for 2 ports(DRP/SNK) configuration
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

#ifndef __USBPD_PDO_DEF_H_
#define __USBPD_PDO_DEF_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Define   ------------------------------------------------------------------*/
/* Exported typedef ----------------------------------------------------------*/
/**
  * @brief  USBPD Port PDO Structure definition
  *
  */
typedef struct
{
  uint32_t ListOfPDO[USBPD_MAX_NB_PDO];         /*!< List of Power Data Objects list, defining
                                                     port capabilities */
  uint8_t  NumberOfPDO;                         /*!< Number of Power Data Objects defined in ListOfPDO
                                                     This parameter must be set at max to @ref USBPD_MAX_NB_PDO value */
} USBPD_PortPDO_TypeDef;

/**
  * @brief  USBPD Port PDO Storage Structure definition
  */
typedef struct
{
  USBPD_PortPDO_TypeDef    SourcePDO;        /*!< SRC Power Data Objects */
  USBPD_PortPDO_TypeDef    SinkPDO;          /*!< SNK Power Data Objects */
}USBPD_PWR_Port_PDO_Storage_TypeDef;

/* Exported define -----------------------------------------------------------*/
#define USBPD_PDP_SRC_IN_WATTS          45  /* SRC PD Power in Watts */
#define USBPD_PDP_SNK_IN_WATTS          7.5 /* SINK PD Power in Watts */

/* Define board operating power and max power */
/* Request 5V and 1500mA in sink mode */
#define USBPD_BOARD_REQUESTED_VOLTAGE_MV       5000
#define USBPD_BOARD_MIN_VOLTAGE_MV_PORT0       5000
#define USBPD_BOARD_MIN_VOLTAGE_MV_PORT1       3300
#define USBPD_BOARD_MAX_VOLTAGE_MV             15000

/* PDP = 45W */
#define USBPD_PDO_APDO_9VPROG_45W_MAX_CURRENT  3   /* Max Current in A */
#define USBPD_PDO_APDO_15VPROG_45W_MAX_CURRENT 3   /* Max Current in A (PDP / 5) */

/* Programmable Power Supply Voltage Ranges */
#define USBPD_PDO_APDO_5VPROG_MIN_VOLTAGE 3.3   /* Min voltage in V */
#define USBPD_PDO_APDO_5VPROG_MAX_VOLTAGE 5.9 /* Max voltage in V */

/* Definitions of nb of PDO and APDO for each port */
#define USBPD_CORE_PDO_SRC_FIXED_MAX_CURRENT 3
#define USBPD_CORE_PDO_SNK_FIXED_MAX_CURRENT 3000

#define USBPD_PDO_APDO_5VPROG_15W_MAX_CURRENT  3   /* Max Current in A (PDP / 5) */

/* Definitions of nb of PDO and APDO for each port */
#define PORT0_NB_SOURCEPDO         4   /* Number of Source PDOs (applicable for port 0)   */
#define PORT1_NB_SOURCEPDO         0   /* Number of Source PDOs (applicable for port 1)   */
#define PORT0_NB_SINKPDO           3   /* Number of Sink PDOs (applicable for port 0)     */
#define PORT1_NB_SINKPDO           4   /* Number of Sink PDOs (applicable for port 1)     */
#define PORT0_NB_SOURCEAPDO        0   /* Number of Source APDOs (applicable for port 0)  */
#define PORT0_NB_SINKAPDO          0   /* Number of Sink APDOs (applicable for port 0)    */
#define PORT1_NB_SOURCEAPDO        0   /* Number of Source APDOs (applicable for port 1)  */
#define PORT1_NB_SINKAPDO          0   /* Number of Sink APDOs (applicable for port 1)    */

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
#ifndef __USBPD_PWR_IF_C

extern uint8_t USBPD_NbPDO[4];

extern uint32_t PORT0_PDO_ListSRC[USBPD_MAX_NB_PDO];
extern uint32_t PORT0_PDO_ListSNK[USBPD_MAX_NB_PDO];
extern uint32_t PORT1_PDO_ListSRC[USBPD_MAX_NB_PDO];
extern uint32_t PORT1_PDO_ListSNK[USBPD_MAX_NB_PDO];
#else

uint8_t USBPD_NbPDO[4] = {(PORT0_NB_SINKPDO + PORT0_NB_SINKAPDO), 
                          ((PORT0_NB_SOURCEPDO + PORT0_NB_SOURCEAPDO)),
                          ((PORT1_NB_SINKPDO + PORT1_NB_SINKAPDO)),
                          ((PORT1_NB_SOURCEPDO + PORT1_NB_SOURCEAPDO))};

/* Definition of Source PDO for Port 0 */
uint32_t PORT0_PDO_ListSRC[USBPD_MAX_NB_PDO] =
{
  /* PDO 1 */
        ( ((PWR_A_10MA(USBPD_CORE_PDO_SRC_FIXED_MAX_CURRENT)) << USBPD_PDO_SRC_FIXED_MAX_CURRENT_Pos) |
          ((PWR_V_50MV(5)) << USBPD_PDO_SRC_FIXED_VOLTAGE_Pos)                                        |
          USBPD_PDO_SRC_FIXED_PEAKCURRENT_EQUAL                                                       |
          USBPD_PDO_SRC_FIXED_UNCHUNK_NOT_SUPPORTED                                                   |
          USBPD_PDO_SRC_FIXED_DRD_SUPPORTED                                                           |
          USBPD_PDO_SRC_FIXED_USBCOMM_NOT_SUPPORTED                                                   |
          USBPD_PDO_SRC_FIXED_EXT_POWER_NOT_AVAILABLE                                                 |
          USBPD_PDO_SRC_FIXED_USBSUSPEND_NOT_SUPPORTED                                                |
          USBPD_PDO_SRC_FIXED_DRP_SUPPORTED                                                           |
          USBPD_PDO_TYPE_FIXED
        ),
  /* PDO 2 */
        ( ((PWR_A_10MA(USBPD_CORE_PDO_SRC_FIXED_MAX_CURRENT)) << USBPD_PDO_SRC_FIXED_MAX_CURRENT_Pos) |
          ((PWR_V_50MV(9)) << USBPD_PDO_SRC_FIXED_VOLTAGE_Pos)                                        |
          USBPD_PDO_SRC_FIXED_PEAKCURRENT_EQUAL                                                       |
          USBPD_PDO_SRC_FIXED_DRD_NOT_SUPPORTED                                                       |
          USBPD_PDO_SRC_FIXED_USBCOMM_NOT_SUPPORTED                                                   |
          USBPD_PDO_SRC_FIXED_EXT_POWER_NOT_AVAILABLE                                                 |
          USBPD_PDO_SRC_FIXED_USBSUSPEND_NOT_SUPPORTED                                                |
          USBPD_PDO_SRC_FIXED_DRP_NOT_SUPPORTED                                                       |
          USBPD_PDO_TYPE_FIXED
        ),
  /* PDO 3 */
        ( ((PWR_A_10MA(USBPD_CORE_PDO_SRC_FIXED_MAX_CURRENT)) << USBPD_PDO_SRC_FIXED_MAX_CURRENT_Pos) |
          ((PWR_V_50MV(15)) << USBPD_PDO_SRC_FIXED_VOLTAGE_Pos)                                       |
          USBPD_PDO_SRC_FIXED_PEAKCURRENT_EQUAL                                                       |
          USBPD_PDO_SRC_FIXED_DRD_NOT_SUPPORTED                                                       |
          USBPD_PDO_SRC_FIXED_USBCOMM_NOT_SUPPORTED                                                   |
          USBPD_PDO_SRC_FIXED_EXT_POWER_NOT_AVAILABLE                                                 |
          USBPD_PDO_SRC_FIXED_USBSUSPEND_NOT_SUPPORTED                                                |
          USBPD_PDO_SRC_FIXED_DRP_NOT_SUPPORTED                                                       |
          USBPD_PDO_TYPE_FIXED
        ),
  /* PDO 4 */ 
        ( (((PWR_A_50MA(USBPD_PDO_APDO_9VPROG_45W_MAX_CURRENT)) << USBPD_PDO_SRC_APDO_MAX_CURRENT_Pos) & (USBPD_PDO_SRC_APDO_MAX_CURRENT_Msk))  |
          (((PWR_V_100MV(USBPD_PDO_APDO_5VPROG_MIN_VOLTAGE)) << USBPD_PDO_SRC_APDO_MIN_VOLTAGE_Pos) & (USBPD_PDO_SRC_APDO_MIN_VOLTAGE_Msk)) |
          (((PWR_V_100MV(USBPD_PDO_APDO_5VPROG_MAX_VOLTAGE)) << USBPD_PDO_SRC_APDO_MAX_VOLTAGE_Pos) & (USBPD_PDO_SRC_APDO_MAX_VOLTAGE_Msk)) |
           USBPD_PDO_TYPE_APDO
        ),
  /* PDO 5 */
        (0x00000000U),
  /* PDO 6 */
        (0x00000000U),
  /* PDO 7 */
        (0x00000000U)
};

/* Definition of Sink PDO for Port 0 */
uint32_t PORT0_PDO_ListSNK[USBPD_MAX_NB_PDO] =
{
  /* PDO 1 */
        ( ((PWR_A_10MA(USBPD_CORE_PDO_SRC_FIXED_MAX_CURRENT)) << USBPD_PDO_SRC_FIXED_MAX_CURRENT_Pos) |
          ((PWR_V_50MV(5)) << USBPD_PDO_SRC_FIXED_VOLTAGE_Pos)                                        |
           USBPD_PDO_SNK_FIXED_FRS_NOT_SUPPORTED                                                     |
           USBPD_PDO_SNK_FIXED_DRD_SUPPORTED                                                         |
           USBPD_PDO_SNK_FIXED_USBCOMM_NOT_SUPPORTED                                                 |
           USBPD_PDO_SNK_FIXED_EXT_POWER_NOT_AVAILABLE                                               |
           USBPD_PDO_SNK_FIXED_HIGHERCAPAB_NOT_SUPPORTED                                             |
           USBPD_PDO_SNK_FIXED_DRP_SUPPORTED                                                         |
           USBPD_PDO_TYPE_FIXED
        ),
  /* PDO 2 */
        ( ((PWR_A_10MA(1)) << USBPD_PDO_SRC_FIXED_MAX_CURRENT_Pos) |
          ((PWR_V_50MV(9)) << USBPD_PDO_SRC_FIXED_VOLTAGE_Pos)
        ),
  /* PDO 3 */
        ( ((PWR_A_10MA(1)) << USBPD_PDO_SRC_FIXED_MAX_CURRENT_Pos) |
          ((PWR_V_50MV(15)) << USBPD_PDO_SRC_FIXED_VOLTAGE_Pos)
        ),
  /* PDO 4 */ (0x00000000U),
  /* PDO 5 */ (0x00000000U),
  /* PDO 6 */ (0x00000000U),
  /* PDO 7 */ (0x00000000U)
};

/* Definition of Source PDO for Port 1 */
uint32_t PORT1_PDO_ListSRC[USBPD_MAX_NB_PDO] =
{
  /* PDO 1 */ (0x00000000U),
  /* PDO 2 */ (0x00000000U),
  /* PDO 3 */ (0x00000000U),
  /* PDO 4 */ (0x00000000U),
  /* PDO 5 */ (0x00000000U),
  /* PDO 6 */ (0x00000000U),
  /* PDO 7 */ (0x00000000U)
};

/* Definition of Sink PDO for Port 1 */
uint32_t PORT1_PDO_ListSNK[USBPD_MAX_NB_PDO] =
{
  /* PDO 1 */
        ( ((PWR_A_10MA(USBPD_CORE_PDO_SRC_FIXED_MAX_CURRENT)) << USBPD_PDO_SRC_FIXED_MAX_CURRENT_Pos) |
          ((PWR_V_50MV(5)) << USBPD_PDO_SRC_FIXED_VOLTAGE_Pos)                                        |
           USBPD_PDO_SNK_FIXED_FRS_NOT_SUPPORTED                                                     |
           USBPD_PDO_SNK_FIXED_DRD_SUPPORTED                                                         |
           USBPD_PDO_SNK_FIXED_USBCOMM_NOT_SUPPORTED                                                 |
           USBPD_PDO_SNK_FIXED_EXT_POWER_NOT_AVAILABLE                                               |
           USBPD_PDO_SNK_FIXED_HIGHERCAPAB_NOT_SUPPORTED                                             |
           USBPD_PDO_SNK_FIXED_DRP_NOT_SUPPORTED                                                     |
           USBPD_PDO_TYPE_FIXED
        ),
  /* PDO 2 */
        ( ((PWR_A_10MA(1)) << USBPD_PDO_SRC_FIXED_MAX_CURRENT_Pos) |
          ((PWR_V_50MV(9)) << USBPD_PDO_SRC_FIXED_VOLTAGE_Pos)
        ),
  /* PDO 3 */
        ( ((PWR_A_10MA(1)) << USBPD_PDO_SRC_FIXED_MAX_CURRENT_Pos) |
          ((PWR_V_50MV(15)) << USBPD_PDO_SRC_FIXED_VOLTAGE_Pos)
        ),
  /* PDO 4 */
        ( (((PWR_A_50MA(USBPD_PDO_APDO_5VPROG_15W_MAX_CURRENT)) << USBPD_PDO_SNK_APDO_MAX_CURRENT_Pos) & (USBPD_PDO_SNK_APDO_MAX_CURRENT_Msk))  |
          (((PWR_V_100MV(USBPD_PDO_APDO_5VPROG_MIN_VOLTAGE)) << USBPD_PDO_SNK_APDO_MIN_VOLTAGE_Pos) & (USBPD_PDO_SNK_APDO_MIN_VOLTAGE_Msk)) |
          (((PWR_V_100MV(USBPD_PDO_APDO_5VPROG_MAX_VOLTAGE)) << USBPD_PDO_SNK_APDO_MAX_VOLTAGE_Pos) & (USBPD_PDO_SNK_APDO_MAX_VOLTAGE_Msk)) |
           USBPD_PDO_TYPE_APDO
        ),
  /* PDO 5 */ (0x00000000U),
  /* PDO 6 */ (0x00000000U),
  /* PDO 7 */ (0x00000000U)
};
#endif

/* Exported functions --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __USBPD_PDO_DEF_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
