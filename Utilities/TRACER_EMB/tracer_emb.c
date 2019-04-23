/**
  ******************************************************************************
  * @file    tracer_emb.c
  * @author  MCD Application Team
  * @brief   This file contains embeded tracer control functions.
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

/* Includes ------------------------------------------------------------------*/
#include "tracer_emb.h"
#include "tracer_emb_hw.h"

/** @addtogroup TRACER_EMB
  * @{
  */

/** @addtogroup TRACER_EMB
  * @{
  */

/** @addtogroup TRACER_EMB
  * @{
  */

/* Private enums -------------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/
/** @defgroup USBPD_CORE_TRACER_EMB_Private_TypeDef EMB TRACER Private typedef
  * @{
  */
 typedef enum {
  TRACER_OK,
  TRACER_KO
} TRACER_StatusTypedef;
/**
  * @}
  */
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/** @defgroup USBPD_CORE_TRACER_EMB_Private_Macros USBPD TRACE Private Macros
  * @{
  */

#define TRACER_WRITE_DATA(_POSITION_,_DATA_)  PtrDataTx[(_POSITION_)% TRACER_EMB_BUFFER_SIZE] = (_DATA_);  \
                                             (_POSITION_) = ((_POSITION_) + 1u) % TRACER_EMB_BUFFER_SIZE;

#define TRACER_ENTER_CRITICAL_SECTION()  uint32_t primask= __get_PRIMASK();\
                                        __disable_irq();

#define TRACER_LEAVE_CRITICAL_SECTION() __set_PRIMASK(primask)


/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
/** @defgroup USBPD_CORE_TRACER_EMB_Private_Functions USBPD TRACE Private Functions
  * @{
  */
uint32_t                    TRACER_CallbackRX(uint8_t character, uint8_t error);
static TRACER_StatusTypedef TRACER_CheckLook(uint32_t *Begin, uint32_t *End);


/* Function prototypes -----------------------------------------------*/
uint8_t TRACER_EMB_ReadData(void);

/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/
/** @defgroup USBPD_CORE_TRACER_EMB_Private_Variables USBPD TRACE Private Variables
  * @{
  */
static uint8_t PtrDataTx[TRACER_EMB_BUFFER_SIZE];
static uint32_t PtrTx_Read, PtrTx_Write;
static volatile uint32_t Counter = 0u;
static uint32_t SizeSent = 0u;

/**
  * @}
  */

/* Exported functions ---------------------------------------------------------*/


/** @addtogroup USBPD_CORE_TRACER_EMB_Exported_Functions
  * @{
  */
void TRACER_EMB_Init(void)
{
  /* initialize the Ptr for Read/Write */
  PtrTx_Read = 0u;
  PtrTx_Write = 0u;
  /* Initialize trace BUS */
  HW_TRACER_EMB_Init();
}

void TRACER_EMB_Add(uint8_t *Ptr, uint32_t Size)
{
  int32_t _writepos;
  uint8_t *data_to_write = Ptr;
  uint32_t index;

  TRACER_EMB_Lock();

  _writepos = TRACER_EMB_AllocateBufer(Size);

  /* if allocation is ok, write data into the bufffer */
  if (_writepos  != -1)
  {
    /* initialize the Ptr for Read/Write */
    for (index = 0u; index < Size; index++)
    {
      TRACER_WRITE_DATA(_writepos, data_to_write[index]);
    }
  }

  TRACER_EMB_UnLock();
}

#if TRACER_EMB_DMA_MODE == 1UL
void TRACER_EMB_IRQHandlerDMA(void)
{
  HW_TRACER_EMB_IRQHandlerDMA();
}
#endif

void TRACER_EMB_IRQHandlerUSART(void)
{
  HW_TRACER_EMB_IRQHandlerUSART();
}

uint32_t TRACER_EMB_TX_Process(void)
{
  uint32_t _timing = 2u;
  uint32_t _begin, _end;

  if (TRACER_OK == TRACER_CheckLook(&_begin, &_end))
  {
    if (_begin == _end)
    {
      /* nothing to do */
      _timing = 0xFFFFFFFFu;
    }
    else
    {
      TRACER_EMB_Lock();
      /*  */
      if (_end > _begin)
      {
        SizeSent = _end - _begin;
      }
      else  /* _begin > _end */
      {
        SizeSent = TRACER_EMB_BUFFER_SIZE - _begin;
      }
      HW_TRACER_EMB_SendData(&PtrDataTx[_begin], SizeSent);
    }
  }

  return _timing;
}

void TRACER_EMB_WriteData(uint16_t pos, uint8_t data)
{
  PtrDataTx[pos % TRACER_EMB_BUFFER_SIZE] = data;
}

void TRACER_EMB_StartRX(void (*callbackRX)(uint8_t, uint8_t))
{
  HW_TRACER_EMB_RegisterRxCallback(callbackRX);
  HW_TRACER_EMB_StartRX();
}


uint8_t TRACER_EMB_ReadData()
{
  return LL_USART_ReceiveData8(TRACER_EMB_USART_INSTANCE); // -------------------------------------------------------------*/
}


/**
  * @}
  */

/** @defgroup USBPD_CORE_TRACER_EMB_Private_Functions USBPD TRACE Private Functions
  * @{
  */

/**
  * @brief  callback called to end a transfert.
  * @param  None.
  * @retval None.
  */
void TRACER_EMB_CALLBACK_TX(void)
{
  TRACER_ENTER_CRITICAL_SECTION();
  PtrTx_Read = (PtrTx_Read + SizeSent) % TRACER_EMB_BUFFER_SIZE;
  TRACER_LEAVE_CRITICAL_SECTION();
  TRACER_EMB_UnLock();
}

/**
  * @brief  Lock the trace buffer.
  * @param  None.
  * @retval None.
  */
void TRACER_EMB_Lock(void)
{
  TRACER_ENTER_CRITICAL_SECTION();
  Counter++;
  TRACER_LEAVE_CRITICAL_SECTION();
}

/**
  * @brief  UnLock the trace buffer.
  * @param  None.
  * @retval None.
  */
void TRACER_EMB_UnLock(void)
{
  TRACER_ENTER_CRITICAL_SECTION();
  Counter--;
  TRACER_LEAVE_CRITICAL_SECTION();
}

/**
  * @brief  if buffer is not locked return a Begin / end @ to transfert over the media.
  * @param  address begin of the data
  * @param  address end of the data
  * @retval USBPD_TRUE if a transfer can be execute else USBPD_FALSE.
  */
static TRACER_StatusTypedef TRACER_CheckLook(uint32_t *Begin, uint32_t *End)
{
  TRACER_StatusTypedef _status = TRACER_KO;
  TRACER_ENTER_CRITICAL_SECTION();

  if (0u == Counter)
  {
    *Begin = PtrTx_Read;
    *End = PtrTx_Write;
    _status = TRACER_OK;
  }

  TRACER_LEAVE_CRITICAL_SECTION();
  return _status;
}

/**
  * @brief  allocate space inside the buffer to push data
  * @param  data size
  * @retval write position inside the buffer is -1 no space available.
  */
int32_t TRACER_EMB_AllocateBufer(uint32_t Size)
{
  uint32_t _freesize;
  int32_t _pos = -1;

  TRACER_ENTER_CRITICAL_SECTION();

  if (PtrTx_Write == PtrTx_Read)
  {
    // need to add buffer full managment
    _freesize = TRACER_EMB_BUFFER_SIZE;
  }
  else
  {
    if (PtrTx_Write > PtrTx_Read)
    {
      _freesize = TRACER_EMB_BUFFER_SIZE - PtrTx_Write + PtrTx_Read;
    }
    else
    {
      _freesize = PtrTx_Read - PtrTx_Write;
    }
  }

  if (_freesize > Size)
  {
    _pos = (int32_t)PtrTx_Write;
    PtrTx_Write = (PtrTx_Write + Size) % TRACER_EMB_BUFFER_SIZE;
  }

  TRACER_LEAVE_CRITICAL_SECTION();
  return _pos;
}

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

