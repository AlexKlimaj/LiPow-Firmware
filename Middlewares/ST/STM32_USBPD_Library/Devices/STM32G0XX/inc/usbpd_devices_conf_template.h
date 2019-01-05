/**
  ******************************************************************************
  * @file    usbpd_devices_conf.h
  * @author  MCD Application Team
  * @brief   This file contains the device define.
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

#ifndef USBPD_DEVICE_CONF_H
#define USBPD_DEVICE_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_ll_adc.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_dma.h"
#include "stm32g0xx_ll_gpio.h"
#include "stm32g0xx_ll_lptim.h"
#include "stm32g0xx_ll_usart.h"
#include "stm32g0xx_ll_ucpd.h"
#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_tim.h"

#if defined(USE_STM32G081B_EVAL_REVA) ||defined(USE_STM32G081B_EVAL_REVB) || defined(USE_STM32G081B_EVAL_REVC)
#include "stm32g081b_eval_pwr.h"
#else
#include "usbpd_bsp_pwr.h"
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* -----------------------------------------------------------------------------
      usbpd_hw.c
-------------------------------------------------------------------------------*/

/* defined used to configure function : USBPD_HW_GetUSPDInstance */
#define UCPD_INSTANCE0 UCPD1
#define UCPD_INSTANCE1 UCPD2

/* defined used to configure function : USBPD_HW_Init_DMARxInstance,USBPD_HW_DeInit_DMARxInstance */
#define UCPDDMA_INSTANCE0_CLOCKENABLE_RX  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1)
#define UCPDDMA_INSTANCE1_CLOCKENABLE_RX  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1)

#define UCPDDMA_INSTANCE0_DMA_RX  DMA1
#define UCPDDMA_INSTANCE1_DMA_RX  DMA1

#define UCPDDMA_INSTANCE0_REQUEST_RX   DMA_REQUEST_UCPD1_RX
#define UCPDDMA_INSTANCE1_REQUEST_RX   DMA_REQUEST_UCPD2_RX

#define UCPDDMA_INSTANCE0_LL_CHANNEL_RX   LL_DMA_CHANNEL_5
#define UCPDDMA_INSTANCE1_LL_CHANNEL_RX   LL_DMA_CHANNEL_2

#define UCPDDMA_INSTANCE0_CHANNEL_RX   DMA1_Channel5
#define UCPDDMA_INSTANCE1_CHANNEL_RX   DMA1_Channel2

/* defined used to configure function : USBPD_HW_Init_DMATxInstance, USBPD_HW_DeInit_DMATxInstance */
#define UCPDDMA_INSTANCE0_CLOCKENABLE_TX  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1)
#define UCPDDMA_INSTANCE1_CLOCKENABLE_TX  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1)

#define UCPDDMA_INSTANCE0_DMA_TX  DMA1
#define UCPDDMA_INSTANCE1_DMA_TX  DMA1

#define UCPDDMA_INSTANCE0_REQUEST_TX   DMA_REQUEST_UCPD1_TX
#define UCPDDMA_INSTANCE1_REQUEST_TX   DMA_REQUEST_UCPD2_TX

#define UCPDDMA_INSTANCE0_LL_CHANNEL_TX   LL_DMA_CHANNEL_3
#define UCPDDMA_INSTANCE1_LL_CHANNEL_TX   LL_DMA_CHANNEL_4

#define UCPDDMA_INSTANCE0_CHANNEL_TX   DMA1_Channel3
#define UCPDDMA_INSTANCE1_CHANNEL_TX   DMA1_Channel4

/* defined used to configure  USBPD_HW_SetFRSSignalling */
#define UCPDFRS_INSTANCE0_FRSCC1   {                                                                   \
                                     LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);                \
                                     LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_2, LL_GPIO_MODE_ALTERNATE); \
                                     LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_2, LL_GPIO_AF_4);         \
                                   }

#define UCPDFRS_INSTANCE0_FRSCC2   {                                                                   \
                                     LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);                \
                                     LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_0, LL_GPIO_MODE_ALTERNATE); \
                                     LL_GPIO_SetAFPin_0_7(GPIOB, LL_GPIO_PIN_0, LL_GPIO_AF_6);         \
                                   }
#define UCPDFRS_INSTANCE1_FRSCC1
#define UCPDFRS_INSTANCE1_FRSCC2

#define UCPD_INSTANCE0_ENABLEIRQ   do{                                                                 \
                                        NVIC_SetPriority(UCPD1_2_IRQn,0);                              \
                                        NVIC_EnableIRQ(UCPD1_2_IRQn);                                  \
                                    } while(0)

#define UCPD_INSTANCE1_ENABLEIRQ   do{                                                                 \
                                        NVIC_SetPriority(UCPD1_2_IRQn,0);                              \
                                        NVIC_EnableIRQ(UCPD1_2_IRQn);                                  \
                                    } while(0)

/* -----------------------------------------------------------------------------
      Definitions for TRACE feature
-------------------------------------------------------------------------------*/

/* Enable below USE_FULL_LL_DRIVER_USART compilation flag to use generic LL_USART_Init() function */
/* #define USE_FULL_LL_DRIVER_USART */

#define TRACE_BAUDRATE                          921600u

#define TRACE_USART_INSTANCE                    USART3

#define TRACE_TX_GPIO                           GPIOC
#define TRACE_TX_PIN                            LL_GPIO_PIN_10
#define TRACE_TX_AF                             LL_GPIO_AF_0
#define TRACE_RX_GPIO                           GPIOC
#define TRACE_RX_PIN                            LL_GPIO_PIN_11
#define TRACE_RX_AF                             LL_GPIO_AF_0
#define TRACE_GPIO_ENABLE_CLOCK()               LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC)

#define TRACE_ENABLE_CLK_USART()                LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3)
#define TRACE_SET_CLK_SOURCE_USART()            /* No need for clock source selection in case of USART3 // LL_RCC_SetUSARTClockSource(LL_RCC_USART3_CLKSOURCE_PCLK2) */
#define TRACE_USART_IRQ                         USART3_4_LPUART1_IRQn
#define TRACE_TX_AF_FUNCTION                    LL_GPIO_SetAFPin_8_15
#define TRACE_RX_AF_FUNCTION                    LL_GPIO_SetAFPin_8_15
#define TRACE_DMA_INSTANCE                      DMA1
#define TRACE_ENABLE_CLK_DMA()                  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1)
#define TRACE_TX_DMA_REQUEST                    LL_DMAMUX_REQ_USART3_TX
#define TRACE_TX_DMA_CHANNEL                    LL_DMA_CHANNEL_7
#define TRACE_TX_DMA_IRQ                        DMA1_Ch4_7_DMAMUX1_OVR_IRQn
#define TRACE_TX_DMA_IRQHANDLER                 DMA1_Ch4_7_DMAMUX1_OVR_IRQHandler
#define TRACE_TX_DMA_ACTIVE_FLAG                LL_DMA_IsActiveFlag_TC7
#define TRACE_TX_DMA_CLEAR_FLAG                 LL_DMA_ClearFlag_GI7

/* -----------------------------------------------------------------------------
      Definitions for timer service feature
-------------------------------------------------------------------------------*/
#define TIMX                           TIM2
#define TIMX_CLK_ENABLE                LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2)
#define TIMX_IRQ                       TIM2_IRQn
#define TIMX_CHANNEL_CH1               LL_TIM_CHANNEL_CH1
#define TIMX_CHANNEL_CH2               LL_TIM_CHANNEL_CH2
#define TIMX_CHANNEL_CH3               LL_TIM_CHANNEL_CH3
#define TIMX_CHANNEL_CH4               LL_TIM_CHANNEL_CH4
#define TIMX_CHANNEL1_SETEVENT         do{                                                                    \
                                          LL_TIM_OC_SetCompareCH1(TIMX, (us_time + TIMX->CNT) % TIM_MAX_TIME);\
                                          LL_TIM_ClearFlag_CC1(TIMX);                                         \
                                       }while(0)
#define TIMX_CHANNEL2_SETEVENT         do{                                                                    \
                                          LL_TIM_OC_SetCompareCH2(TIMX, (us_time + TIMX->CNT) % TIM_MAX_TIME);\
                                          LL_TIM_ClearFlag_CC2(TIMX);                                         \
                                       }while(0)
#define TIMX_CHANNEL3_SETEVENT         do{                                                                    \
                                          LL_TIM_OC_SetCompareCH3(TIMX, (us_time + TIMX->CNT) % TIM_MAX_TIME);\
                                          LL_TIM_ClearFlag_CC3(TIMX);                                         \
                                       }while(0)
#define TIMX_CHANNEL4_SETEVENT         do{                                                                    \
                                          LL_TIM_OC_SetCompareCH4(TIMX, (us_time + TIMX->CNT) % TIM_MAX_TIME);\
                                          LL_TIM_ClearFlag_CC4(TIMX);                                         \
                                       }while(0)
#define TIMX_CHANNEL1_GETFLAG          LL_TIM_IsActiveFlag_CC1
#define TIMX_CHANNEL2_GETFLAG          LL_TIM_IsActiveFlag_CC2
#define TIMX_CHANNEL3_GETFLAG          LL_TIM_IsActiveFlag_CC3
#define TIMX_CHANNEL4_GETFLAG          LL_TIM_IsActiveFlag_CC4

#ifdef __cplusplus
}
#endif

#endif /* USBPD_DEVICE_CONF_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

