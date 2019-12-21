/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2019 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"
#include "stm32g0xx_ll_ucpd.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_cortex.h"
#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_system.h"
#include "stm32g0xx_ll_utils.h"
#include "stm32g0xx_ll_pwr.h"
#include "stm32g0xx_ll_gpio.h"
#include "stm32g0xx_ll_dma.h"

#include "stm32g0xx_ll_exti.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "cmsis_os.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Cell_4S_ADC_Pin GPIO_PIN_0
#define Cell_4S_ADC_GPIO_Port GPIOA
#define Cell_3S_ADC_Pin GPIO_PIN_1
#define Cell_3S_ADC_GPIO_Port GPIOA
#define Cell_2S_ADC_Pin GPIO_PIN_2
#define Cell_2S_ADC_GPIO_Port GPIOA
#define Cell_1S_ADC_Pin GPIO_PIN_3
#define Cell_1S_ADC_GPIO_Port GPIOA
#define BAT_ADC_Pin GPIO_PIN_4
#define BAT_ADC_GPIO_Port GPIOA
#define Blue_LED_Pin GPIO_PIN_5
#define Blue_LED_GPIO_Port GPIOA
#define Green_LED_Pin GPIO_PIN_7
#define Green_LED_GPIO_Port GPIOA
#define EN_OTG_Pin GPIO_PIN_0
#define EN_OTG_GPIO_Port GPIOB
#define PROTCHOT_Pin GPIO_PIN_1
#define PROTCHOT_GPIO_Port GPIOB
#define Red_LED_Pin GPIO_PIN_2
#define Red_LED_GPIO_Port GPIOB
#define ILIM_HIZ_Pin GPIO_PIN_11
#define ILIM_HIZ_GPIO_Port GPIOB
#define CHRG_OK_Pin GPIO_PIN_12
#define CHRG_OK_GPIO_Port GPIOB
#define CELL_1S_DIS_EN_Pin GPIO_PIN_4
#define CELL_1S_DIS_EN_GPIO_Port GPIOB
#define CELL_2S_DIS_EN_Pin GPIO_PIN_5
#define CELL_2S_DIS_EN_GPIO_Port GPIOB
#define CELL_3S_DIS_EN_Pin GPIO_PIN_8
#define CELL_3S_DIS_EN_GPIO_Port GPIOB
#define CELL_4S_DIS_EN_Pin GPIO_PIN_9
#define CELL_4S_DIS_EN_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

#define LIPOW_MAJOR_VERSION	(uint8_t)1
#define LIPOW_MINOR_VERSION	(uint8_t)3

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
