/**
  ******************************************************************************
  * @file    usbpd_bsp_trace.c
  * @author  MCD Application Team
  * @brief   This file contains the low level interface to control trace system.
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

/* Includes ------------------------------------------------------------------*/
#include "usbpd_devices_conf.h"
#include "usbpd_bsp_trace.h"

/* Private typedef -----------------------------------------------------------*/
/* Private Defines */
/* Private Variable */
void (*fptr_tx)(void) = NULL;
void (*fptr_rx)(uint8_t, uint8_t) = NULL;

/* Private function prototypes -----------------------------------------------*/
/* Exported functions ---------------------------------------------------------*/

/**
  * @brief  Trace init
  * @param  callbackTX
  * @param  callbackRX
  * @retval none
  */
void BSP_TRACE_Init(void (*callbackTX)(void), void (*callbackRX)(uint8_t, uint8_t))
{
#if defined(USE_FULL_LL_DRIVER_USART)
  LL_USART_InitTypeDef usart_initstruct;
#endif /* USE_FULL_LL_DRIVER_USART */

  fptr_tx = callbackTX;
  fptr_rx = callbackRX;

  /* Enable the peripheral clock of GPIO Port */
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);
#if defined(_DISCO_TRACE_CN11) /* to use CN11 for trace instead of STLink VC */
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
#endif
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOD);

  /* Configure Tx Pin as : Alternate function, High Speed, Push pull, Pull up */
  TRACE_TX_AF_FUNCTION(TRACE_TX_GPIO, TRACE_TX_PIN, TRACE_TX_AF);
  LL_GPIO_SetPinMode(TRACE_TX_GPIO, TRACE_TX_PIN, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetPinSpeed(TRACE_TX_GPIO, TRACE_TX_PIN, LL_GPIO_SPEED_HIGH);
  LL_GPIO_SetPinOutputType(TRACE_TX_GPIO, TRACE_TX_PIN, LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_SetPinPull(TRACE_TX_GPIO, TRACE_TX_PIN, LL_GPIO_PULL_UP);

  /* Configure Rx Pin as : Alternate function, High Speed, Push pull, Pull up */
  TRACE_RX_AF_FUNCTION(TRACE_RX_GPIO, TRACE_RX_PIN, TRACE_RX_AF);
  LL_GPIO_SetPinMode(TRACE_RX_GPIO, TRACE_RX_PIN, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetPinSpeed(TRACE_RX_GPIO, TRACE_RX_PIN, LL_GPIO_SPEED_HIGH);
  LL_GPIO_SetPinOutputType(TRACE_RX_GPIO, TRACE_RX_PIN, LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_SetPinPull(TRACE_RX_GPIO, TRACE_RX_PIN, LL_GPIO_PULL_UP);

  /* Enable the peripheral clock for USART */
  TRACE_ENABLE_CLK_USART();

  /* Set clock source */
  TRACE_SET_CLK_SOURCE_USART();

  /* Configure USART */

  /* Disable USART prior modifying configuration registers */
  LL_USART_Disable(TRACE_USART_INSTANCE);

#if defined(USE_FULL_LL_DRIVER_USART)
  /* Set fields of initialization structure                   */
  /*  - Prescaler           : LL_USART_PRESCALER_DIV1         */
  /*  - BaudRate            : TRACE_BAUDRATE                  */
  /*  - DataWidth           : LL_USART_DATAWIDTH_8B           */
  /*  - StopBits            : LL_USART_STOPBITS_1             */
  /*  - Parity              : LL_USART_PARITY_NONE            */
  /*  - TransferDirection   : LL_USART_DIRECTION_TX_RX        */
  /*  - HardwareFlowControl : LL_USART_HWCONTROL_NONE         */
  /*  - OverSampling        : LL_USART_OVERSAMPLING_16        */
  usart_initstruct.PrescalerValue      = LL_USART_PRESCALER_DIV1;
  usart_initstruct.BaudRate            = TRACE_BAUDRATE;
  usart_initstruct.DataWidth           = LL_USART_DATAWIDTH_8B;
  usart_initstruct.StopBits            = LL_USART_STOPBITS_1;
  usart_initstruct.Parity              = LL_USART_PARITY_NONE;
  usart_initstruct.TransferDirection   = LL_USART_DIRECTION_TX_RX;
  usart_initstruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  usart_initstruct.OverSampling        = LL_USART_OVERSAMPLING_16;

  /* Initialize USART instance according to parameters defined in initialization structure */
  LL_USART_Init(TRACE_USART_INSTANCE, &usart_initstruct);
#else
  /* TX/RX direction */
  LL_USART_SetTransferDirection(TRACE_USART_INSTANCE, LL_USART_DIRECTION_TX_RX);
  /* 8 data bit, 1 start bit, 1 stop bit, no parity */
  LL_USART_ConfigCharacter(TRACE_USART_INSTANCE, LL_USART_DATAWIDTH_8B, LL_USART_PARITY_NONE, LL_USART_STOPBITS_1);
  /* Oversampling by 16 */
  LL_USART_SetOverSampling(TRACE_USART_INSTANCE, LL_USART_OVERSAMPLING_16);
  /* Set Baudrate to 921600 using APB frequency set to 64000000 Hz */
  /* This frequency can also be calculated through LL RCC macro */
  /* Ex :
      pllclk = __LL_RCC_CALC_PLLCLK_FREQ(__LL_RCC_CALC_MSI_FREQ(LL_RCC_MSIRANGESEL_RUN, LL_RCC_MSIRANGE_6), LL_RCC_PLLM_DIV1, 40, LL_RCC_PLLR_DIV2);
      hclk = __LL_RCC_CALC_HCLK_FREQ(pllclk, LL_RCC_GetAHBPrescaler());
      periphclk = __LL_RCC_CALC_PCLKx_FREQ(hclk, LL_RCC_GetAPBxPrescaler());  x=1 or 2 depending on USART instance

      periphclk is expected to be equal to 64000000 Hz

      In this example, Peripheral Clock is equal to SystemCoreClock
  */

  uint32_t _pllclk = __LL_RCC_CALC_PLLCLK_FREQ(HSI_VALUE, LL_RCC_PLLM_DIV_4, 64, LL_RCC_PLLR_DIV_4);
  uint32_t _hclk = __LL_RCC_CALC_HCLK_FREQ(_pllclk, LL_RCC_GetAHBPrescaler());
  uint32_t _periphclk = __LL_RCC_CALC_PCLK1_FREQ(_hclk, LL_RCC_GetAPB1Prescaler());

  LL_USART_SetBaudRate(TRACE_USART_INSTANCE, _periphclk, LL_USART_PRESCALER_DIV1, LL_USART_OVERSAMPLING_16, TRACE_BAUDRATE);
#endif /* USE_FULL_LL_DRIVER_USART */

  LL_USART_Enable(TRACE_USART_INSTANCE);

  /* Polling USART initialisation */
  while((!(LL_USART_IsActiveFlag_TEACK(TRACE_USART_INSTANCE))) || (!(LL_USART_IsActiveFlag_REACK(TRACE_USART_INSTANCE))))
  {
  }

  /* Configure TX DMA */
  TRACE_ENABLE_CLK_DMA();

  /* (3) Configure the DMA functional parameters for transmission */
  LL_DMA_ConfigTransfer(TRACE_DMA_INSTANCE, TRACE_TX_DMA_CHANNEL,
                        LL_DMA_DIRECTION_MEMORY_TO_PERIPH |
                        LL_DMA_PRIORITY_HIGH              |
                        LL_DMA_MODE_NORMAL                |
                        LL_DMA_PERIPH_NOINCREMENT         |
                        LL_DMA_MEMORY_INCREMENT           |
                        LL_DMA_PDATAALIGN_BYTE            |
                        LL_DMA_MDATAALIGN_BYTE);
  LL_DMA_SetPeriphRequest(TRACE_DMA_INSTANCE, TRACE_TX_DMA_CHANNEL, TRACE_TX_DMA_REQUEST);

  LL_DMA_EnableIT_TC(TRACE_DMA_INSTANCE, TRACE_TX_DMA_CHANNEL);

  /* Configure the interrupt for TX */
  NVIC_SetPriority(TRACE_TX_DMA_IRQ, 0);
  NVIC_EnableIRQ(TRACE_TX_DMA_IRQ);

  return;
}

/**
  * @brief  Trace Deinit
  * @retval none
  */
void BSP_TRACE_DeInit(void)
{
  return;
}

/**
  * @brief  Allow to update the RX callback
  * @param  callbackRX
  * @retval none
  */
void BSP_TRACE_RegisterRxCallback(void (*callbackRX)(uint8_t, uint8_t))
{
  fptr_rx = callbackRX;
}

/**
  * @brief  Start RX reception only when OSKernel have been started
  * @retval none
  */
void BSP_TRACE_StartRX(void)
{
  /* Enable USART IT for RX */
  LL_USART_EnableIT_RXNE(TRACE_USART_INSTANCE);
  LL_USART_EnableIT_ERROR(TRACE_USART_INSTANCE);

  /* Configure the interrupt for RX */
  NVIC_SetPriority(TRACE_USART_IRQ, 3);
  NVIC_EnableIRQ(TRACE_USART_IRQ);
}

/**
  * @brief  Function to handle reception in DMA mode
  * @retval none
  */
void BSP_TRACE_IRQHandlerDMA(void)
{
  if (TRACE_TX_DMA_ACTIVE_FLAG(TRACE_DMA_INSTANCE))
  {
    TRACE_TX_DMA_CLEAR_FLAG(TRACE_DMA_INSTANCE);
    LL_DMA_DisableChannel(TRACE_DMA_INSTANCE, TRACE_TX_DMA_CHANNEL);
    /* call the callback */
    if (NULL != fptr_tx)
    {
      fptr_tx();
    }
  }
}

void BSP_TRACE_IRQHandlerUSART(void)
{
  /* Ready to read reception*/
  if (LL_USART_IsActiveFlag_RXNE(TRACE_USART_INSTANCE) && LL_USART_IsEnabledIT_RXNE(TRACE_USART_INSTANCE))
  {
    __IO uint32_t received_char;

    /* Read Received character. RXNE flag is cleared by reading of RDR register */
    received_char = LL_USART_ReceiveData8(TRACE_USART_INSTANCE);
    if (fptr_rx != NULL)
    {
      fptr_rx(received_char, 0);
    }
  }

  if ( ((LL_USART_IsActiveFlag_PE(TRACE_USART_INSTANCE) && LL_USART_IsEnabledIT_PE(TRACE_USART_INSTANCE)))            /* Parity error    */
     || ((LL_USART_IsActiveFlag_RTO(TRACE_USART_INSTANCE) && LL_USART_IsEnabledIT_RTO(TRACE_USART_INSTANCE)))         /* Receiver timeout*/
     || ((LL_USART_IsEnabledIT_ERROR(TRACE_USART_INSTANCE) && LL_USART_IsActiveFlag_FE(TRACE_USART_INSTANCE)))        /* Framing error   */
     || ((LL_USART_IsEnabledIT_ERROR(TRACE_USART_INSTANCE) && LL_USART_IsActiveFlag_ORE(TRACE_USART_INSTANCE)))       /* Overrun error   */
     || ((LL_USART_IsEnabledIT_ERROR(TRACE_USART_INSTANCE) && LL_USART_IsActiveFlag_NE(TRACE_USART_INSTANCE))))       /* Noise detection */
  {
    /* Flags clearing */
    LL_USART_ClearFlag_PE(TRACE_USART_INSTANCE);
    LL_USART_ClearFlag_RTO(TRACE_USART_INSTANCE);
    LL_USART_ClearFlag_FE(TRACE_USART_INSTANCE);
    LL_USART_ClearFlag_ORE(TRACE_USART_INSTANCE);
    LL_USART_ClearFlag_NE(TRACE_USART_INSTANCE);

    if (fptr_rx != NULL)
    {
      fptr_rx(1, 1);  /* 1 indicate a reception error */
    }
  }
}

void BSP_TRACE_SendData(uint8_t *pData, uint32_t Size)
{

  LL_DMA_ConfigAddresses(TRACE_DMA_INSTANCE, TRACE_TX_DMA_CHANNEL,
                         (uint32_t)pData,
                         LL_USART_DMA_GetRegAddr(TRACE_USART_INSTANCE, LL_USART_DMA_REG_DATA_TRANSMIT),
                         LL_DMA_GetDataTransferDirection(TRACE_DMA_INSTANCE, TRACE_TX_DMA_CHANNEL));
  LL_DMA_SetDataLength(TRACE_DMA_INSTANCE, TRACE_TX_DMA_CHANNEL, Size);

  /* Enable DMA TX Interrupt */
  LL_USART_EnableDMAReq_TX(TRACE_USART_INSTANCE);

  /* Enable DMA Channel Rx */
  LL_DMA_EnableChannel(TRACE_DMA_INSTANCE, TRACE_TX_DMA_CHANNEL);

  LL_USART_EnableDirectionTx(TRACE_USART_INSTANCE);

  return;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
