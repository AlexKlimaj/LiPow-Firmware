/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usbpd_dpm_user.c
  * @author  MCD Application Team
  * @brief   USBPD DPM user code
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

#define USBPD_DPM_USER_C
/* Includes ------------------------------------------------------------------*/
#include "usbpd_core.h"
#include "usbpd_pdo_defs.h"
#include "usbpd_dpm_core.h"
#include "usbpd_dpm_conf.h"
#include "usbpd_dpm_user.h"
#include "usbpd_vdm_user.h"
#include "usbpd_trace.h"
#include "usbpd_pwr_if.h"
#include "string.h"
#include "cmsis_os.h"
#include "usbpd_pwr_user.h"

/** @addtogroup STM32_USBPD_LIBRARY
  * @{
  */

/** @addtogroup USBPD_USER
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN Private_Typedef */

/* USER CODE END Private_Typedef */

/* Private define ------------------------------------------------------------*/
/** @defgroup USBPD_USER_PRIVATE_DEFINES USBPD USER Private Defines
  * @{
  */
/* USER CODE BEGIN Private_Define */

/* USER CODE END Private_Define */
/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/** @defgroup USBPD_USER_PRIVATE_MACROS USBPD USER Private Macros
  * @{
  */
/* USER CODE BEGIN Private_Macro */

/* USER CODE END Private_Macro */
/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/
/** @defgroup USBPD_USER_PRIVATE_VARIABLES USBPD USER Private Variables
  * @{
  */
/* USER CODE BEGIN Private_Variables */
extern USBPD_ParamsTypeDef DPM_Params[];
USBPD_HandleTypeDef DPM_Ports[USBPD_PORT_COUNT];
/* USER CODE END Private_Variables */
/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
/** @defgroup USBPD_USER_PRIVATE_FUNCTIONS USBPD USER Private Functions
  * @{
  */
/* USER CODE BEGIN USBPD_USER_PRIVATE_FUNCTIONS_Prototypes */

/* USER CODE END USBPD_USER_PRIVATE_FUNCTIONS_Prototypes */
/**
  * @}
  */

/* Exported functions ------- ------------------------------------------------*/
/** @defgroup USBPD_USER_EXPORTED_FUNCTIONS USBPD USER Exported Functions
  * @{
  */
/* USER CODE BEGIN USBPD_USER_EXPORTED_FUNCTIONS */

/* USER CODE END USBPD_USER_EXPORTED_FUNCTIONS */

/** @defgroup USBPD_USER_EXPORTED_FUNCTIONS_GROUP1 USBPD USER Exported Functions called by DPM CORE
  * @{
  */
/* USER CODE BEGIN USBPD_USER_EXPORTED_FUNCTIONS_GROUP1 */

/* USER CODE END USBPD_USER_EXPORTED_FUNCTIONS_GROUP1 */

/**
  * @brief  Initialize DPM (port power role, PWR_IF, CAD and PE Init procedures)
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_DPM_UserInit(void)
{
/* USER CODE BEGIN USBPD_DPM_UserInit */
  return USBPD_OK;
/* USER CODE END USBPD_DPM_UserInit */
}

/**
  * @brief  User delay implementation which is OS dependant
  * @param  Time time in ms
  * @retval None
  */
void USBPD_DPM_WaitForTime(uint32_t Time)
{
/* USER CODE BEGIN USBPD_DPM_WaitForTime */
#ifdef _RTOS
  osDelay(Time);
#else
  HAL_Delay(Time);
#endif
/* USER CODE END USBPD_DPM_WaitForTime */
}

/**
  * @brief  User processing time, it is recommended to avoid blocking task for long time
  * @param  None
  * @retval None
  */
void USBPD_DPM_UserExecute(void const *argument)
{
/* USER CODE BEGIN USBPD_DPM_UserExecute */

/* USER CODE END USBPD_DPM_UserExecute */
}

/**
  * @brief  UserCableDetection reporting events on a specified port from CAD layer.
  * @param  PortNum The handle of the port
  * @param  State CAD state
  * @retval None
  */
void USBPD_DPM_UserCableDetection(uint8_t PortNum, USBPD_CAD_EVENT State)
{
/* USER CODE BEGIN USBPD_DPM_UserCableDetection */

/* USER CODE END USBPD_DPM_UserCableDetection */
}

/**
  * @brief  function used to manage user timer.
  * @param  PortNum Port number
  * @retval None
  */
void USBPD_DPM_UserTimerCounter(uint8_t PortNum)
{
/* USER CODE BEGIN USBPD_DPM_UserTimerCounter */

/* USER CODE END USBPD_DPM_UserTimerCounter */
}

/**
  * @}
  */

/** @defgroup USBPD_USER_EXPORTED_FUNCTIONS_GROUP2 USBPD USER Exported Callbacks functions called by PE
  * @{
  */

/**
  * @brief  Callback function called by PE layer when HardReset message received from PRL
  * @param  PortNum The current port number
  * @param  CurrentRole the current role
  * @param  Status status on hard reset event
  * @retval None
  */
void USBPD_DPM_HardReset(uint8_t PortNum, USBPD_PortPowerRole_TypeDef CurrentRole, USBPD_HR_Status_TypeDef Status)
{
/* USER CODE BEGIN USBPD_DPM_HardReset */

/* USER CODE END USBPD_DPM_HardReset */
}

/**
  * @brief  Request the DPM to setup the new power level.
  * @param  PortNum The current port number
  * @retval USBPD status
  */
USBPD_StatusTypeDef USBPD_DPM_SetupNewPower(uint8_t PortNum)
{
/* USER CODE BEGIN USBPD_DPM_SetupNewPower */
  return USBPD_PWR_IF_SetProfile(PortNum);
/* USER CODE END USBPD_DPM_SetupNewPower */
}

/**
  * @brief  Evaluate the swap request from PE.
  * @param  PortNum The current port number
  * @retval USBPD_ACCEPT, USBPD_WAIT, USBPD_REJECT
  */
USBPD_StatusTypeDef USBPD_DPM_EvaluatePowerRoleSwap(uint8_t PortNum)
{
/* USER CODE BEGIN USBPD_DPM_EvaluatePowerRoleSwap */
  return USBPD_REJECT;
/* USER CODE END USBPD_DPM_EvaluatePowerRoleSwap */
}

/**
  * @brief  Callback function called by PE to inform DPM about PE event.
  * @param  PortNum The current port number
  * @param  EventType @ref USBPD_NotifyEvent_TypeDef
  * @param  EventVal @ref USBPD_NotifyEventValue_TypeDef
  * @retval None
  */
void USBPD_DPM_Notification(uint8_t PortNum, USBPD_NotifyEventValue_TypeDef EventVal)
{
/* USER CODE BEGIN USBPD_DPM_Notification */

/* USER CODE END USBPD_DPM_Notification */
}

/**
  * @brief  Request DPM to confirm if current contract is still valid.
  * @param  PortNum Port number
  * @retval USBPD_OK, USBPD_ERROR
*/
USBPD_StatusTypeDef USBPD_DPM_IsContractStillValid(uint8_t PortNum)
{
/* USER CODE BEGIN USBPD_DPM_IsContractStillValid */
  return USBPD_OK;
/* USER CODE END USBPD_DPM_IsContractStillValid */
}

/**
  * @brief  DPM callback to allow PE to retrieve information from DPM/PWR_IF.
  * @param  PortNum Port number
  * @param  DataId  Type of data to be updated in DPM based on @ref USBPD_CORE_DataInfoType_TypeDef
  * @param  Ptr     Pointer on address where DPM data should be written (u32 pointer)
  * @param  Size    Pointer on nb of u8 written by DPM
  * @retval None
  */
void USBPD_DPM_GetDataInfo(uint8_t PortNum, USBPD_CORE_DataInfoType_TypeDef DataId, uint8_t *Ptr, uint32_t *Size)
{
/* USER CODE BEGIN USBPD_DPM_GetDataInfo */

/* USER CODE END USBPD_DPM_GetDataInfo */
}

/**
  * @brief  DPM callback to allow PE to update information in DPM/PWR_IF.
  * @param  PortNum Port number
  * @param  DataId  Type of data to be updated in DPM based on @ref USBPD_CORE_DataInfoType_TypeDef
  * @param  Size    Nb of bytes to be updated in DPM
  * @retval None
  */
void USBPD_DPM_SetDataInfo(uint8_t PortNum, USBPD_CORE_DataInfoType_TypeDef DataId, uint8_t *Ptr, uint32_t Size)
{
/* USER CODE BEGIN USBPD_DPM_SetDataInfo */
  switch(DataId)
  {
	/* Case requested DO position Data information :
	*/
  case USBPD_CORE_DATATYPE_RDO_POSITION :
	if (Size == 4)
	{
	  uint8_t* temp;
	  temp = (uint8_t*)&DPM_Ports[PortNum].DPM_RDOPosition;
	  (void)memcpy(temp, Ptr, Size);
	  DPM_Ports[PortNum].DPM_RDOPositionPrevious = *Ptr;
	  temp = (uint8_t*)&DPM_Ports[PortNum].DPM_RDOPositionPrevious;
	  (void)memcpy(temp, Ptr, Size);
	}
	break;


//  case USBPD_CORE_DATATYPE_REQUEST_DO :
//    if (Size == 4)
//    {
//      uint8_t* rdo;
//      rdo = (uint8_t*)&DPM_Ports[PortNum].DPM_RcvRequestDOMsg;
//      (void)memcpy(rdo, Ptr, Size);
//    }
//
	/* Case Received Source PDO values Data information :
	*/
  case USBPD_CORE_DATATYPE_RCV_SRC_PDO :
	{
	  if (Size <= (USBPD_MAX_NB_PDO * 4))
	  {
		uint8_t* rdo;
		DPM_Ports[PortNum].DPM_NumberOfRcvSRCPDO = (Size / 4);
		/* Copy PDO data in DPM Handle field */
		for (uint32_t index = 0; index < (Size / 4); index++)
		{
		  rdo = (uint8_t*)&DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[index];
		  (void)memcpy(rdo, (Ptr + (index * 4u)), (4u * sizeof(uint8_t)));
		}
	  }
	  break;
	}

  default :
	break;
  }
/* USER CODE END USBPD_DPM_SetDataInfo */
}

/**
  * @brief  Evaluate received Request Message from Sink port
  * @param  pdhandle Pointer to USB PD handle
  * @param  PtrPowerObject  Pointer on the power data object
  * @retval USBPD status : USBPD_ACCEPT, USBPD_REJECT, USBPD_WAIT, USBPD_GOTOMIN
  */
USBPD_StatusTypeDef USBPD_DPM_EvaluateRequest(uint8_t PortNum, USBPD_CORE_PDO_Type_TypeDef *PtrPowerObject)
{
/* USER CODE BEGIN USBPD_DPM_EvaluateRequest */
  return USBPD_REJECT;
/* USER CODE END USBPD_DPM_EvaluateRequest */
}

/**
  * @brief  Evaluate received Capabilities Message from Source port and prepare the request message
  * @param  PortNum         Port number
  * @param  PtrRequestData  Pointer on selected request data object
  * @param  PtrPowerObject  Pointer on the power data object
  * @retval None
  */
void USBPD_DPM_SNK_EvaluateCapabilities(uint8_t PortNum, uint32_t *PtrRequestData, USBPD_CORE_PDO_Type_TypeDef *PtrPowerObject)
{
/* USER CODE BEGIN USBPD_DPM_SNK_EvaluateCapabilities */
  USBPD_PDO_TypeDef  pdo;
  USBPD_SNKRDO_TypeDef rdo;
  USBPD_HandleTypeDef *pdhandle = &DPM_Ports[PortNum];
  uint32_t size;
  uint32_t snkpdolist[USBPD_MAX_NB_PDO];
  USBPD_PDO_TypeDef snk_fixed_pdo;

  /* Initialize RDO */
  rdo.d32 = 0;

  /* Select the first PDO to get a 5V aligned with sink PDO */
  pdo.d32 = pdhandle->DPM_ListOfRcvSRCPDO[0];

  /* Read the sink PDO */
  USBPD_PWR_IF_GetPortPDOs(PortNum, USBPD_CORE_DATATYPE_SNK_PDO, (uint8_t*)snkpdolist, &size);

  /* Initialise sinkdpo with the first PDO*/
  snk_fixed_pdo.d32 = snkpdolist[0];

  if(USBPD_CORE_PDO_TYPE_FIXED == pdo.GenericPDO.PowerObject)
  {
	/* Prepare the requested pdo */
	rdo.FixedVariableRDO.ObjectPosition = 1;
	if (snk_fixed_pdo.SNKFixedPDO.OperationalCurrentIn10mAunits > pdo.SRCFixedPDO.MaxCurrentIn10mAunits)
	{
	  rdo.FixedVariableRDO.OperatingCurrentIn10mAunits  = pdo.SRCFixedPDO.MaxCurrentIn10mAunits;
	  rdo.FixedVariableRDO.MaxOperatingCurrent10mAunits = pdo.SRCFixedPDO.MaxCurrentIn10mAunits;
	  rdo.FixedVariableRDO.CapabilityMismatch = 1;
	}
	else
	{
	  rdo.FixedVariableRDO.OperatingCurrentIn10mAunits  = snk_fixed_pdo.SNKFixedPDO.OperationalCurrentIn10mAunits;
	  rdo.FixedVariableRDO.MaxOperatingCurrent10mAunits = snk_fixed_pdo.SNKFixedPDO.OperationalCurrentIn10mAunits;
	  rdo.FixedVariableRDO.CapabilityMismatch = 0;
	}

	rdo.FixedVariableRDO.USBCommunicationsCapable = snk_fixed_pdo.SNKFixedPDO.USBCommunicationsCapable;

#if defined(USBPD_REV30_SUPPORT) && defined(_UNCHUNKED_SUPPORT)
	if (USBPD_SPECIFICATION_REV2 < DPM_Params[PortNum].PE_SpecRevision)
	{
	  rdo.FixedVariableRDO.UnchunkedExtendedMessage = DPM_Settings[PortNum].PE_PD3_Support.d.PE_UnchunkSupport;
	  DPM_Params[PortNum].PE_UnchunkSupport   = USBPD_FALSE;
	  /* Set unchuncked bit if supported by port partner;*/
	  if (USBPD_TRUE == fixed_pdo.SRCFixedPDO.UnchunkedExtendedMessage)
	  {
		DPM_Params[PortNum].PE_UnchunkSupport   = USBPD_TRUE;
	  }
	}
#endif /* USBPD_REV30_SUPPORT && _UNCHUNKED_SUPPORT */

	*PtrPowerObject = USBPD_CORE_PDO_TYPE_FIXED;
	*PtrRequestData = rdo.d32;
	pdhandle->DPM_RequestDOMsg = rdo.d32;
	pdhandle->DPM_RequestedVoltage = 5000;
  }
  else {
	/* This case shall never occurs because any source must present a first PDO with 5V */
  }
/* USER CODE END USBPD_DPM_SNK_EvaluateCapabilities */
}

/**
  * @brief  Power role swap status update
  * @param  PortNum Port number
  * @param  CurrentRole the current role
  * @param  Status status on power role swap event
  */
void USBPD_DPM_PowerRoleSwap(uint8_t PortNum, USBPD_PortPowerRole_TypeDef CurrentRole, USBPD_PRS_Status_TypeDef Status)
{
/* USER CODE BEGIN USBPD_DPM_PowerRoleSwap */

/* USER CODE END USBPD_DPM_PowerRoleSwap */
}

/**
  * @brief  Callback to be used by PE to evaluate a Vconn swap
  * @param  PortNum Port number
  * @retval USBPD_ACCEPT, USBPD_REJECT, USBPD_WAIT
  */
USBPD_StatusTypeDef USBPD_DPM_EvaluateVconnSwap(uint8_t PortNum)
{
/* USER CODE BEGIN USBPD_DPM_EvaluateVconnSwap */
  return USBPD_REJECT;
/* USER CODE END USBPD_DPM_EvaluateVconnSwap */
}

/**
  * @brief  Callback to be used by PE to manage VConn
  * @param  PortNum Port number
  * @param  State Enable or Disable VConn on CC lines
  * @retval USBPD_ACCEPT, USBPD_REJECT
  */
USBPD_StatusTypeDef USBPD_DPM_PE_VconnPwr(uint8_t PortNum, USBPD_FunctionalState State)
{
/* USER CODE BEGIN USBPD_DPM_PE_VconnPwr */
  return USBPD_ERROR;
/* USER CODE END USBPD_DPM_PE_VconnPwr */
}

/**
  * @brief  DPM callback to allow PE to forward extended message information.
  * @param  PortNum Port number
  * @param  MsgType Type of message to be handled in DPM
  *         This parameter can be one of the following values:
  *           @arg @ref USBPD_EXT_SECURITY_REQUEST Security Request extended message
  *           @arg @ref USBPD_EXT_SECURITY_RESPONSE Security Response extended message
  * @param  Ptr Pointer on address Extended Message data could be read (u8 pointer)
  * @param  Size Nb of u8 that compose Extended message
  * @retval None
  */
void USBPD_DPM_ExtendedMessageReceived(uint8_t PortNum, USBPD_ExtendedMsg_TypeDef MsgType, uint8_t *ptrData, uint16_t DataSize)
{
/* USER CODE BEGIN USBPD_DPM_ExtendedMessageReceived */

/* USER CODE END USBPD_DPM_ExtendedMessageReceived */
}

/**
  * @brief  DPM callback used to know user choice about Data Role Swap.
  * @param  PortNum Port number
  * @retval USBPD_REJECT, UBPD_ACCEPT
  */
USBPD_StatusTypeDef USBPD_DPM_EvaluateDataRoleSwap(uint8_t PortNum)
{
/* USER CODE BEGIN USBPD_DPM_EvaluateDataRoleSwap */
  return USBPD_REJECT;
/* USER CODE END USBPD_DPM_EvaluateDataRoleSwap */
}

/**
  * @brief  Callback to be used by PE to check is VBUS is ready or present
  * @param  PortNum Port number
  * @param  Vsafe   Vsafe status based on @ref USBPD_VSAFE_StatusTypeDef
  * @retval USBPD_DISABLE or USBPD_ENABLE
  */
USBPD_FunctionalState USBPD_DPM_IsPowerReady(uint8_t PortNum, USBPD_VSAFE_StatusTypeDef Vsafe)
{
/* USER CODE BEGIN USBPD_DPM_IsPowerReady */
  return ((USBPD_OK == USBPD_PWR_IF_SupplyReady(PortNum, Vsafe)) ? USBPD_ENABLE : USBPD_DISABLE);
/* USER CODE END USBPD_DPM_IsPowerReady */
}
/**
  * @}
  */

/** @defgroup USBPD_USER_EXPORTED_FUNCTIONS_GROUP3 USBPD USER Functions PD messages requests
  * @{
  */

/**
  * @}
  */

/** @addtogroup USBPD_USER_PRIVATE_FUNCTIONS
  * @{
  */

/* USER CODE BEGIN USBPD_USER_PRIVATE_FUNCTIONS */

/* USER CODE END USBPD_USER_PRIVATE_FUNCTIONS */

/**
  * @}
  */

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
