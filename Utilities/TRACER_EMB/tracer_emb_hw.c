/**
  ******************************************************************************
  * @file    tracer_emb_hw.c
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
#include "tracer_emb.h"
#include "tracer_emb_hw.h"

#if TRACER_EMB_DMA_MODE == 1UL && TRACER_EMB_IT_MODE == 1UL
#error "tracer emb hw please select DMA or IT mode"
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private Defines */
/* Private Variable */
static void (*fptr_rx)(uint8_t, uint8_t) = NULL;
#if TRACER_EMB_IT_MODE == 1UL
  uint8_t *txData = NULL;
  uint32_t txSize = 0;
#endif

/* Private function prototypes -----------------------------------------------*/
/* Exported functions ---------------------------------------------------------*/

/**
  * @brief  Trace init
  * @param  callbackTX
  * @param  callbackRX
  * @retval none
  */
void HW_TRACER_EMB_Init(void)
{
  LL_USART_InitTypeDef usart_initstruct;

  /* Enable the peripheral clock of GPIO Port */
  TRACER_EMB_TX_GPIO_ENABLE_CLOCK();
  TRACER_EMB_RX_GPIO_ENABLE_CLOCK();

  /* Configure Tx Pin as : Alternate function, High Speed, Push pull, Pull up */
  TRACER_EMB_TX_AF_FUNCTION(TRACER_EMB_TX_GPIO, TRACER_EMB_TX_PIN, TRACER_EMB_TX_AF);
  LL_GPIO_SetPinMode(TRACER_EMB_TX_GPIO, TRACER_EMB_TX_PIN, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetPinSpeed(TRACER_EMB_TX_GPIO, TRACER_EMB_TX_PIN, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetPinOutputType(TRACER_EMB_TX_GPIO, TRACER_EMB_TX_PIN, LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_SetPinPull(TRACER_EMB_TX_GPIO, TRACER_EMB_TX_PIN, LL_GPIO_PULL_UP);

  /* Configure Rx Pin as : Alternate function, High Speed, Push pull, Pull up */
  TRACER_EMB_RX_AF_FUNCTION(TRACER_EMB_RX_GPIO, TRACER_EMB_RX_PIN, TRACER_EMB_RX_AF);
  LL_GPIO_SetPinMode(TRACER_EMB_RX_GPIO, TRACER_EMB_RX_PIN, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetPinSpeed(TRACER_EMB_RX_GPIO, TRACER_EMB_RX_PIN, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetPinOutputType(TRACER_EMB_RX_GPIO, TRACER_EMB_RX_PIN, LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_SetPinPull(TRACER_EMB_RX_GPIO, TRACER_EMB_RX_PIN, LL_GPIO_PULL_UP);

  /* Enable the peripheral clock for USART */
  TRACER_EMB_ENABLE_CLK_USART();

  /* Set clock source */
  TRACER_EMB_SET_CLK_SOURCE_USART();

  /* Configure USART */

  /* Disable USART prior modifying configuration registers */
  LL_USART_Disable(TRACER_EMB_USART_INSTANCE);

  /* Set fields of initialization structure                   */
  /*  - Prescaler           : LL_USART_PRESCALER_DIV1         */
  /*  - BaudRate            : TRACE_BAUDRATE                  */
  /*  - DataWidth           : LL_USART_DATAWIDTH_8B           */
  /*  - StopBits            : LL_USART_STOPBITS_1             */
  /*  - Parity              : LL_USART_PARITY_NONE            */
  /*  - TransferDirection   : LL_USART_DIRECTION_TX_RX        */
  /*  - HardwareFlowControl : LL_USART_HWCONTROL_NONE         */
  /*  - OverSampling        : LL_USART_OVERSAMPLING_16        */
#if defined(USART_PRESC_PRESCALER)
  usart_initstruct.PrescalerValue      = LL_USART_PRESCALER_DIV1;
#endif
  usart_initstruct.BaudRate            = TRACER_EMB_BAUDRATE;
  usart_initstruct.DataWidth           = LL_USART_DATAWIDTH_8B;
  usart_initstruct.StopBits            = LL_USART_STOPBITS_1;
  usart_initstruct.Parity              = LL_USART_PARITY_NONE;
  usart_initstruct.TransferDirection   = LL_USART_DIRECTION_TX;
  usart_initstruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  usart_initstruct.OverSampling        = LL_USART_OVERSAMPLING_16;

  /* Initialize USART instance according to parameters defined in initialization structure */
  LL_USART_Init(TRACER_EMB_USART_INSTANCE, &usart_initstruct);

  LL_USART_Enable(TRACER_EMB_USART_INSTANCE);

  uint32_t _temp1 = LL_USART_IsActiveFlag_TEACK(TRACER_EMB_USART_INSTANCE);
  /* Polling USART initialisation */
  while(0u == _temp1)
  {
    _temp1 = LL_USART_IsActiveFlag_TEACK(TRACER_EMB_USART_INSTANCE);
  }


#if TRACER_EMB_DMA_MODE == 1UL
  /* Configure TX DMA */
  TRACER_EMB_ENABLE_CLK_DMA();

  /* (3) Configure the DMA functional parameters for transmission */
  LL_DMA_ConfigTransfer(TRACER_EMB_DMA_INSTANCE, TRACER_EMB_TX_DMA_CHANNEL,
                        LL_DMA_DIRECTION_MEMORY_TO_PERIPH |
                        LL_DMA_PRIORITY_HIGH              |
                        LL_DMA_MODE_NORMAL                |
                        LL_DMA_PERIPH_NOINCREMENT         |
                        LL_DMA_MEMORY_INCREMENT           |
                        LL_DMA_PDATAALIGN_BYTE            |
                        LL_DMA_MDATAALIGN_BYTE);

#if defined(DMAMUX_CxCR_DMAREQ_ID)
  LL_DMA_SetPeriphRequest(TRACER_EMB_DMA_INSTANCE, TRACER_EMB_TX_DMA_CHANNEL, TRACER_EMB_TX_DMA_REQUEST);
#endif

  LL_DMA_EnableIT_TC(TRACER_EMB_DMA_INSTANCE, TRACER_EMB_TX_DMA_CHANNEL);

  /* Configure the interrupt for TX */
  NVIC_SetPriority(TRACER_EMB_TX_DMA_IRQ, 0);
  NVIC_EnableIRQ(TRACER_EMB_TX_DMA_IRQ);
#endif
  
#if TRACER_EMB_IT_MODE == 1UL  
  /* Configure the interrupt for TX */
  NVIC_SetPriority(TRACER_EMB_USART_IRQ, 0);
  NVIC_EnableIRQ(TRACER_EMB_USART_IRQ);
#endif

  return;
}

/**
  * @brief  Trace Deinit
  * @retval none
  */
void HW_TRACER_EMB_DeInit(void)
{
  return;
}

/**
  * @brief  Allow to update the RX callback
  * @param  callbackRX
  * @retval none
  */
void HW_TRACER_EMB_RegisterRxCallback(void (*callbackRX)(uint8_t, uint8_t))
{
  fptr_rx = callbackRX;
}

/**
  * @brief  Start RX reception only when OSKernel have been started
  * @retval none
  */
void HW_TRACER_EMB_StartRX(void)
{
  /* Enable USART IT for RX */
  LL_USART_EnableIT_RXNE(TRACER_EMB_USART_INSTANCE);
  LL_USART_EnableIT_ERROR(TRACER_EMB_USART_INSTANCE);

  /* Enable RX/TX */
  LL_USART_EnableDirectionRx(TRACER_EMB_USART_INSTANCE);
  
  /* Configure the interrupt for RX */
  NVIC_SetPriority(TRACER_EMB_USART_IRQ, 3);
  NVIC_EnableIRQ(TRACER_EMB_USART_IRQ);
}

#if TRACER_EMB_DMA_MODE == 1UL
/**
  * @brief  Function to handle reception in DMA mode
  * @retval none
  */
void HW_TRACER_EMB_IRQHandlerDMA(void)
{
  if (1UL == TRACER_EMB_TX_DMA_ACTIVE_FLAG(TRACER_EMB_DMA_INSTANCE))
  {
    TRACER_EMB_TX_DMA_CLEAR_FLAG(TRACER_EMB_DMA_INSTANCE);
    LL_DMA_DisableChannel(TRACER_EMB_DMA_INSTANCE, TRACER_EMB_TX_DMA_CHANNEL);
    /* call the callback */
    TRACER_EMB_CALLBACK_TX();
  }
}
#endif

void HW_TRACER_EMB_IRQHandlerUSART(void)
{
  uint32_t _temp1, _temp2;
  uint8_t _error = 0u;

#if TRACER_EMB_IT_MODE == 1UL
  /* Ready to handle Tx interrupt */
  _temp1 = LL_USART_IsActiveFlag_TXE(TRACER_EMB_USART_INSTANCE);
  _temp2 = LL_USART_IsEnabledIT_TXE(TRACER_EMB_USART_INSTANCE);
  if ((1UL == _temp1) && (1UL == _temp2))
  {
    __IO uint32_t received_char;

    /* Write data inside the data register */
    LL_USART_TransmitData8(TRACER_EMB_USART_INSTANCE, *txData);
    txData++;
    txSize--;
    if (txSize == 0)
    {
      LL_USART_DisableIT_TXE(TRACER_EMB_USART_INSTANCE);

      /* call the callback */
      TRACER_EMB_CALLBACK_TX();
    }
  }
#endif
  
  /* Ready to read reception*/
  _temp1 = LL_USART_IsActiveFlag_RXNE(TRACER_EMB_USART_INSTANCE);
  _temp2 = LL_USART_IsEnabledIT_RXNE(TRACER_EMB_USART_INSTANCE);
  if ((1UL == _temp1) && (1UL == _temp2))
  {
    __IO uint32_t received_char;

    /* Read Received character. RXNE flag is cleared by reading of RDR register */
    received_char = LL_USART_ReceiveData8(TRACER_EMB_USART_INSTANCE);
    if (fptr_rx != NULL)
    {
      fptr_rx(received_char, 0);
    }
  }

  /* Parity error    */
  _temp1 = LL_USART_IsActiveFlag_PE(TRACER_EMB_USART_INSTANCE);
  _temp2 = LL_USART_IsEnabledIT_PE(TRACER_EMB_USART_INSTANCE);
  if ((1UL == _temp1) && (1UL == _temp2))
  {
    /* Flags clearing */
    LL_USART_ClearFlag_PE(TRACER_EMB_USART_INSTANCE);
    _error = 1;
  }

  /* Receiver timeout*/
  _temp1 = LL_USART_IsActiveFlag_RTO(TRACER_EMB_USART_INSTANCE);
  _temp2 = LL_USART_IsEnabledIT_RTO(TRACER_EMB_USART_INSTANCE);
  if ((1u == _temp1) && (1u == _temp2))
  {
    /* Flags clearing */
    LL_USART_ClearFlag_RTO(TRACER_EMB_USART_INSTANCE);
    _error = 1;
  }

  /* Generic Errors*/
  _temp1 = LL_USART_IsEnabledIT_ERROR(TRACER_EMB_USART_INSTANCE);
  if (1u == _temp1)
  {
    /* Framing error   */
    _temp2 = LL_USART_IsActiveFlag_FE(TRACER_EMB_USART_INSTANCE);
    if (1u == _temp2)
    {
      /* Flags clearing */
      LL_USART_ClearFlag_FE(TRACER_EMB_USART_INSTANCE);
      _error = 1;
    }

    /* Overrun error   */
    _temp2 = LL_USART_IsActiveFlag_ORE(TRACER_EMB_USART_INSTANCE);
    if (1u == _temp2)
    {
      /* Flags clearing */
      LL_USART_ClearFlag_ORE(TRACER_EMB_USART_INSTANCE);
      _error = 1;
    }

    /* Noise detection */
    _temp2 = LL_USART_IsActiveFlag_NE(TRACER_EMB_USART_INSTANCE);
    if (1u == _temp2)
    {
      /* Flags clearing */
      LL_USART_ClearFlag_NE(TRACER_EMB_USART_INSTANCE);
      _error = 1;
    }
  }

  if ((1u == _error) && (fptr_rx != NULL))
  {
    fptr_rx(1, 1);  /* 1 indicate a reception error */
  }
}

void HW_TRACER_EMB_SendData(uint8_t *pData, uint32_t Size)
{
#if TRACER_EMB_DMA_MODE == 1UL
  LL_DMA_ConfigAddresses(TRACER_EMB_DMA_INSTANCE, TRACER_EMB_TX_DMA_CHANNEL,
                         (uint32_t)pData,
                         LL_USART_DMA_GetRegAddr(TRACER_EMB_USART_INSTANCE, LL_USART_DMA_REG_DATA_TRANSMIT),
                         LL_DMA_GetDataTransferDirection(TRACER_EMB_DMA_INSTANCE, TRACER_EMB_TX_DMA_CHANNEL));
  LL_DMA_SetDataLength(TRACER_EMB_DMA_INSTANCE, TRACER_EMB_TX_DMA_CHANNEL, Size);

  /* Enable DMA TX Interrupt */
  LL_USART_EnableDMAReq_TX(TRACER_EMB_USART_INSTANCE);

  /* Enable DMA Channel Rx */
  LL_DMA_EnableChannel(TRACER_EMB_DMA_INSTANCE, TRACER_EMB_TX_DMA_CHANNEL);
#endif

#if TRACER_EMB_IT_MODE == 1UL
  /* keep information about the data to transfert */
  txData = pData;
  txSize = Size;

  /* Enable the TXEIE */
  LL_USART_EnableIT_TXE(TRACER_EMB_USART_INSTANCE);
#endif

}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
