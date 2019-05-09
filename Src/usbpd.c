/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usbpd.c
  * @author  MCD Application Team
  * @brief   This file contains the device define.
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

/* Includes ------------------------------------------------------------------*/
#include "usbpd.h"

/* USER CODE BEGIN 0 */

#include "usbpd_core.h"
#include "usbpd_dpm_core.h"
#include "usbpd_dpm_conf.h"
#include "usbpd_dpm_user.h"
#include "usbpd_pwr_if.h"

#include "battery.h"
#include "printf.h"

/* USER CODE END 0 */

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/* Global variables ---------------------------------------------------------*/

TIM_HandleTypeDef        htim2;

/* USER CODE BEGIN 2 */

osMessageQId  USBPDMsgBox;
osThreadId USBPD_User_TaskHandle;

void vUSBPD_User(void const *pvParameters);

/* USER CODE END 2 */

/* USBPD init function */
void MX_USBPD_Init(void)
{

  htim2.Instance = TIMX;

  /* Global Init of USBPD HW */
  USBPD_HW_IF_GlobalHwInit();

  /* Initialize the Device Policy Manager */
  if(USBPD_OK != USBPD_DPM_InitCore())
  {
    while(1);
  }

  /* Initialise the DPM application */
  if (USBPD_OK != USBPD_DPM_UserInit())
  {
    while(1);
  }

  /* USER CODE BEGIN 3 */

	osThreadDef(usbpd_user_app, vUSBPD_User, USBPD_TASK_PRIORITY, 0, configMINIMAL_STACK_SIZE);
	USBPD_User_TaskHandle = osThreadCreate(osThread(usbpd_user_app), NULL);

  /* USER CODE END 3 */

}

/* USER CODE BEGIN 4 */


void vUSBPD_User(void const *pvParameters) {
	TickType_t xDelay = 500 / portTICK_PERIOD_MS;
	USBPD_StatusTypeDef status = USBPD_ERROR;

	vTaskDelay(xDelay);

	while (status != USBPD_OK) {
		status = USBPD_DPM_RequestGetSourceCapability(USBPD_PORT_0);
		//printf("USBPD_DPM_RequestGetSourceCapability Status: %d\r\n", status);

		if (status == USBPD_ERROR) {
			//printf("USBPD Error. Sending Hard Reset\r\n");
			//USBPD_DPM_RequestHardReset(USBPD_PORT_0);
		}

		vTaskDelay(xDelay);
	}

	printf("Number of received Source PDOs: %d\r\n", DPM_Ports[USBPD_PORT_0].DPM_NumberOfRcvSRCPDO);

	for (int i = 0; i < DPM_Ports[USBPD_PORT_0].DPM_NumberOfRcvSRCPDO; i++) {
		printf("PDO From Source: #%d PDO: %d\r\n", i, DPM_Ports[USBPD_PORT_0].DPM_ListOfRcvSRCPDO[i]);
	}

	for (;;) {

		if (Get_XT60_Connection_State() == CONNECTED) {
			switch(Get_Number_Of_Cells()) {
				case 2:
					status = USBPD_DPM_RequestMessageRequest(USBPD_PORT_0, 1, (uint16_t)5000);
					//printf("USBPD_DPM_RequestMessageRequest Status: %d\r\n", status);
					break;

				case 3:
					status = USBPD_DPM_RequestMessageRequest(USBPD_PORT_0, 2, (uint16_t)9000);
					//printf("USBPD_DPM_RequestMessageRequest Status: %d\r\n", status);
					break;

				case 4:
					status = USBPD_DPM_RequestMessageRequest(USBPD_PORT_0, 3, (uint16_t)15000);
					//printf("USBPD_DPM_RequestMessageRequest Status: %d\r\n", status);
//					status = USBPD_DPM_RequestMessageRequest(USBPD_PORT_0, 4, (uint16_t)20000);
//					printf("USBPD_DPM_RequestMessageRequest Status: %d\r\n", status);
					break;

				default:
					break;
			}
		}

		vTaskDelay(xDelay);
	}
}

/* USER CODE END 4 */

/**
  * @}
  */
 
/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
