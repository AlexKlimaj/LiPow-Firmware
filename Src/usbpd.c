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
#include "bq25703a_regulator.h"
#include "printf.h"
#include <stdlib.h>

/* USER CODE END 0 */

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/* Global variables ---------------------------------------------------------*/

/* USER CODE BEGIN 2 */

/* Private typedef -----------------------------------------------------------*/
struct USB_PD_Received_Source_PDO {
	uint32_t voltage_mv;
	uint32_t current_ma;
	uint32_t power_mw;
};

/* Private variables ---------------------------------------------------------*/
volatile struct USB_PD_Received_Source_PDO source_pdo[USBPD_MAX_NB_PDO];
volatile uint32_t max_source_power_mw = 0;
volatile uint8_t max_source_power_pdo = 0;

volatile uint8_t selected_source_pdo = 0;
volatile uint8_t power_ready = NOT_READY;
volatile uint8_t match_found = 0;

volatile uint16_t voltage_choice_list_mv[3][VOLTAGE_CHOICE_ARRAY_SIZE] = {
		{9000, 12000, 15000, 5000, 20000}, //Two S voltage choice list
		{12000, 15000, 20000, 9000, 5000}, //Three S voltage choice list
		{20000, 15000, 12000, 9000, 5000}  //Four S voltage choice list
};

osMessageQId  USBPDMsgBox;
osThreadId USBPD_User_TaskHandle;

void vUSBPD_User(void const *pvParameters);
uint8_t check_if_power_ready(void);

/* USER CODE END 2 */

/* USBPD init function */
void MX_USBPD_Init(void)
{

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

/**
 * @brief Gets the state of the input power supply
 * @retval READY or NOT_READY
 */
uint8_t Get_Input_Power_Ready(void) {
	return power_ready;
}

/**
 * @brief Gets the max input power for the selected PDO
 * @retval Max input power in mW
 */
uint32_t Get_Max_Input_Power(void){
	return source_pdo[selected_source_pdo].power_mw;
}

/**
 * @brief Gets the max input current for the selected PDO
 * @retval Max input current in mA
 */
uint32_t Get_Max_Input_Current(void) {
	return source_pdo[selected_source_pdo].current_ma;
}

/**
 * @brief Gets the input voltage for the selected PDO
 * @retval Input Voltage in mV
 */
uint32_t Get_Input_Voltage(void) {
	return source_pdo[selected_source_pdo].voltage_mv;
}

void vUSBPD_User(void const *pvParameters) {
	TickType_t xDelay = 500 / portTICK_PERIOD_MS;
	USBPD_StatusTypeDef status = USBPD_ERROR;

	vTaskDelay(xDelay);

/*	while (status != USBPD_OK) {
		//status = USBPD_DPM_RequestGetSourceCapability(USBPD_PORT_0);
		//printf("USBPD_DPM_RequestGetSourceCapability Status: %d\r\n", status);

		if (status == USBPD_ERROR) {
			//printf("USBPD Error. Sending Hard Reset\r\n");
			//USBPD_DPM_RequestHardReset(USBPD_PORT_0);
		}

		vTaskDelay(xDelay);
	}
*/

	printf("Number of received Source PDOs: %d\r\n", DPM_Ports[USBPD_PORT_0].DPM_NumberOfRcvSRCPDO);

	USBPD_PDO_TypeDef srcpdo;
	uint32_t nbsnkpdo;
	uint32_t snkpdo_array[USBPD_MAX_NB_PDO];
	USBPD_PWR_IF_GetPortPDOs(USBPD_PORT_0, USBPD_CORE_DATATYPE_SNK_PDO, (uint8_t*)snkpdo_array, &nbsnkpdo);

	for (int i = 0; i < DPM_Ports[USBPD_PORT_0].DPM_NumberOfRcvSRCPDO; i++) {

		printf("PDO From Source: #%d PDO: %d  ", i, DPM_Ports[USBPD_PORT_0].DPM_ListOfRcvSRCPDO[i]);

		srcpdo.d32 = DPM_Ports[USBPD_PORT_0].DPM_ListOfRcvSRCPDO[i];
		switch(srcpdo.GenericPDO.PowerObject)
		{
			/* SRC Fixed Supply PDO */
			case USBPD_CORE_PDO_TYPE_FIXED:
				source_pdo[i].voltage_mv = (srcpdo.SRCFixedPDO.VoltageIn50mVunits * 50);
				source_pdo[i].current_ma = (srcpdo.SRCFixedPDO.MaxCurrentIn10mAunits * 10);
				source_pdo[i].power_mw = (source_pdo[i].current_ma * source_pdo[i].voltage_mv) / 1000;

				if (source_pdo[i].power_mw > max_source_power_mw){
					max_source_power_mw = source_pdo[i].power_mw;
					max_source_power_pdo = i;
				}
			  break;
//			/* SRC Variable Supply (non-battery) PDO */
			case USBPD_CORE_PDO_TYPE_VARIABLE:
				printf("USBPD_CORE_PDO_TYPE_VARIABLE ");
//			  srcmaxvoltage50mv = srcpdo.SRCVariablePDO.MaxVoltageIn50mVunits;
//			  srcminvoltage50mv = srcpdo.SRCVariablePDO.MinVoltageIn50mVunits;
//			  srcmaxcurrent10ma = srcpdo.SRCVariablePDO.MaxCurrentIn10mAunits;
			  break;
//			/* SRC Battery Supply PDO */
			case USBPD_CORE_PDO_TYPE_BATTERY:
				printf("USBPD_CORE_PDO_TYPE_BATTERY ");
//			  srcmaxvoltage50mv = srcpdo.SRCBatteryPDO.MaxVoltageIn50mVunits;
//			  srcminvoltage50mv = srcpdo.SRCBatteryPDO.MinVoltageIn50mVunits;
//			  srcmaxpower250mw  = srcpdo.SRCBatteryPDO.MaxAllowablePowerIn250mWunits;
			  break;
//			/* Augmented Power Data Object (APDO) */
			case USBPD_CORE_PDO_TYPE_APDO:
				printf("USBPD_CORE_PDO_TYPE_BATTERY ");
//				srcmaxvoltage100mv = srcpdo.SRCSNKAPDO.MaxVoltageIn100mV;
//				srcmaxcurrent50ma = srcpdo.SRCSNKAPDO.MaxCurrentIn50mAunits;
				break;
		    default:
		      break;
		}
		printf("Voltage: %dmV  Current: %dmA  Power: %dmW\r\n", source_pdo[i].voltage_mv, source_pdo[i].current_ma, source_pdo[i].power_mw);
	}

	if (DPM_Ports[USBPD_PORT_0].DPM_NumberOfRcvSRCPDO == 0) {
		power_ready = NO_USB_PD_SUPPLY;
		for(;;) {
			vTaskDelay(xDelay);
		}
	}

	for (;;) {

		//Find the best PDO from the source for the highest regulator efficiency
		if ((Get_Balance_Connection_State() == CONNECTED)) {
			if (match_found == 0) {
				for (int i = 0; i < VOLTAGE_CHOICE_ARRAY_SIZE; i++) {
					for (int t = 0; t < DPM_Ports[USBPD_PORT_0].DPM_NumberOfRcvSRCPDO; t++) {
						if (voltage_choice_list_mv[Get_Number_Of_Cells() - 2][i] == source_pdo[t].voltage_mv) {
							printf("Voltage match found: %d\r\n", source_pdo[t].voltage_mv);
							selected_source_pdo = t;
							match_found = 1;
							break;
						}
					}
					if (match_found != 0) {
						break;
					}
				}
			}
		}
		else {
			match_found = 0;
		}

		if ((Get_XT60_Connection_State() == CONNECTED) && (Get_Balance_Connection_State() == CONNECTED) && (power_ready == NOT_READY) && (match_found == 1) && (Get_Requires_Charging_State() == 1)) {
			printf("Requesting %dV, Result: ", (source_pdo[selected_source_pdo].voltage_mv/1000));
			status = USBPD_DPM_RequestMessageRequest(USBPD_PORT_0, (selected_source_pdo + 1), (uint16_t)source_pdo[selected_source_pdo].voltage_mv);
			vTaskDelay(400 / portTICK_PERIOD_MS);
			if (status == USBPD_OK) {
				if (check_if_power_ready() != READY) {
					printf("Waiting for input voltage to be ready\r\n");
					power_ready = NOT_READY;
				}
				else {
					printf("Success\r\n");
					power_ready = READY;
				}
			}
			else {
				printf("Failed\r\n");
				power_ready = NOT_READY;
			}
		}
		else if ((Get_XT60_Connection_State() == NOT_CONNECTED) || (Get_Balance_Connection_State() == NOT_CONNECTED)){
			if (Get_VBUS_ADC_Reading() > (6 * REG_ADC_MULTIPLIER)) {
				printf("Requesting 5V, Result: ");
				selected_source_pdo = 0;
				status = USBPD_DPM_RequestMessageRequest(USBPD_PORT_0, selected_source_pdo + 1, (uint16_t)source_pdo[selected_source_pdo].voltage_mv);
				vTaskDelay(100 / portTICK_PERIOD_MS);
				if (status == USBPD_OK) {
					printf("Success\r\n");
				}
				else {
					printf("Failed\r\n");
				}
			}
			power_ready = NOT_READY;
			vTaskDelay(1000 / portTICK_PERIOD_MS);
		}

		vTaskDelay(xDelay);
	}
}

/**
 * @brief Compares the selected source PDO voltage to the measured input voltage
 * @retval READY or NOT_READY
 */
uint8_t check_if_power_ready() {
	if (Get_VBUS_ADC_Reading() > ((source_pdo[selected_source_pdo].voltage_mv * (REG_ADC_MULTIPLIER/1000)) - (2 * REG_ADC_MULTIPLIER))) {
		return READY;
	}
	return NOT_READY;
}

/* USER CODE END 4 */

/**
  * @}
  */
 
/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
