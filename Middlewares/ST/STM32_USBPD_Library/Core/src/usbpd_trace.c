/**
  ******************************************************************************
  * @file    usbpd_trace.c
  * @author  MCD Application Team
  * @brief   This file contains trace control functions.
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
#if defined(_TRACE)||defined(_GUI_INTERFACE)
#define USBPD_TRACE_C

#include "stdint.h"
#include "usbpd_def.h"
#include "usbpd_core.h"
#include "usbpd_trace.h"
#include "usbpd_bsp_trace.h"

/** @addtogroup STM32_USBPD_LIBRARY
  * @{
  */

/** @addtogroup USBPD_CORE
  * @{
  */

/** @addtogroup USBPD_CORE_TRACE
  * @{
  */

/* Private enums -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @defgroup USBPD_CORE_TRACE_Private_Macros USBPD TRACE Private Defines
  * @{
  */
#define TRACE_SIZE_BUFFER_TX      1024u

#define TRACE_SIZE_HEADER_TRACE   9u      /* Type + Time x 2 + PortNum + Sop + Size */

#define TRACE_PORT_BIT_POSITION   5u      /* Bit position of port number in TAG id */

#define TLV_SOF                   0xFDu
#define TLV_EOF                   0xA5u
#define TLV_SIZE_MAX              256u
#define TLV_HEADER_SIZE           3u /* Size of TLV header (TAG(1) + LENGTH(2)  */
#define TLV_SOF_SIZE              4u /* TLV_SOF * 4                             */
#define TLV_EOF_SIZE              4u /* TLV_EOF * 4                             */

#define DEBUG_STACK_MESSAGE       0x12u
/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/** @defgroup USBPD_CORE_TRACE_Private_Macros USBPD TRACE Private Macros
  * @{
  */
#define __TRACE_SET_TAG_ID(_PORT_, _TAG_)  ((_PORT_) << TRACE_PORT_BIT_POSITION | (_TAG_))
#define TRACE_WRITE_DATA(_POSITION_,_DATA_)  PtrDataTx[_POSITION_] = _DATA_;                     \
                                             _POSITION_ = (_POSITION_ + 1) % TRACE_SIZE_BUFFER_TX;

#define TRACE_ENTER_CRITICAL_SECTION()  uint32_t primask= __get_PRIMASK();\
                                        __disable_irq();

#define TRACE_LEAVE_CRITICAL_SECTION() __set_PRIMASK(primask)


/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
/** @defgroup USBPD_CORE_TRACE_Private_Functions USBPD TRACE Private Functions
  * @{
  */

void      TRACE_CALLBACK_TX(void);
static int32_t   TRACE_AllocateBufer(uint32_t Size);
static void      TRACE_Lock(void);
static void      TRACE_UnLock(void);
static uint8_t   TRACE_CheckLook(uint32_t *Begin, uint32_t *End);

/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/
/** @defgroup USBPD_CORE_TRACE_Private_Variables USBPD TRACE Private Variables
  * @{
  */

uint8_t PtrDataTx[TRACE_SIZE_BUFFER_TX];
static uint32_t PtrReadTx, PtrWriteTx;
static volatile uint32_t Counter = 0;
static uint32_t SizeSent = 0;
extern uint32_t HAL_GetTick(void);

/**
  * @}
  */

/* Exported functions ---------------------------------------------------------*/


/** @addtogroup USBPD_CORE_TRACE_Exported_Functions
  * @{
  */
void USBPD_TRACE_Init(void)
{
  /* initialize the Ptr for Read/Write */
  PtrReadTx = PtrWriteTx = 0;
  /* Initialize trace BUS */
  BSP_TRACE_Init(TRACE_CALLBACK_TX, NULL);
  USBPD_PE_SetTrace((void*)USBPD_TRACE_Add, 3);
}

void USBPD_TRACE_Add(TRACE_EVENT Type, uint8_t PortNum, uint8_t Sop, uint8_t *Ptr, uint32_t Size)
{
  int32_t _time;
  int32_t _writepos;
  uint8_t *data_to_write;
  uint32_t index = 0;
  uint32_t total_size = 0;

  TRACE_Lock();

  /* If it's a trace, we encapsulate it in a TLV string*/
  if (USBPD_TRACE_FORMAT_TLV != Type)
  {
#if !defined(_TRACE)
    /* Do not send debug traces */
    return;
#else
    /* Allocate buffer Size */
    total_size = Size + TRACE_SIZE_HEADER_TRACE + TLV_HEADER_SIZE + TLV_SOF_SIZE + TLV_EOF_SIZE;
    _writepos = TRACE_AllocateBufer(total_size);
#endif /*_TRACE*/
  }
  else
  {
    /* If Type = 0, then it's a ready-to-go TLV string */
    _writepos = TRACE_AllocateBufer(Size);
  }

  /* Check allocation */
  if (_writepos  == -1)
  {
    /* Not enough space avaible to store the data */
    TRACE_UnLock();
    return;
  }

  data_to_write = Ptr;
  if (USBPD_TRACE_FORMAT_TLV != Type)
  {
    /* Copy SOF bytes */
    for(index = 0; index < TLV_SOF_SIZE; index++)
    {
      TRACE_WRITE_DATA(_writepos, TLV_SOF);
    }
    /* Copy the TAG */
    TRACE_WRITE_DATA(_writepos, __TRACE_SET_TAG_ID((PortNum + 1), DEBUG_STACK_MESSAGE));
    /* Copy the LENGTH */
    TRACE_WRITE_DATA(_writepos, ((total_size - TLV_HEADER_SIZE - TLV_SOF_SIZE - TLV_EOF_SIZE) >> 8) & 0xFFu);
    TRACE_WRITE_DATA(_writepos, (total_size - TLV_HEADER_SIZE - TLV_SOF_SIZE - TLV_EOF_SIZE) & 0xFFu);
    TRACE_WRITE_DATA(_writepos, Type);

    _time = HAL_GetTick();

    TRACE_WRITE_DATA(_writepos, _time         & 0xFFu);
    TRACE_WRITE_DATA(_writepos, (_time >> 8)  & 0xFFu);
    TRACE_WRITE_DATA(_writepos, (_time >> 16) & 0xFFu);
    TRACE_WRITE_DATA(_writepos, (_time >> 24) & 0xFFu);

    TRACE_WRITE_DATA(_writepos, PortNum);
    TRACE_WRITE_DATA(_writepos, Sop);

    TRACE_WRITE_DATA(_writepos, (Size >> 8) & 0xFFu);
    TRACE_WRITE_DATA(_writepos, Size & 0xFFu);

    /* initialize the Ptr for Read/Write */
    for (index = 0; index < Size; index++)
    {
      TRACE_WRITE_DATA(_writepos, data_to_write[index]);
    }

    /* Copy EOF bytes */
    for(index = 0; index < TLV_EOF_SIZE; index++)
    {
      TRACE_WRITE_DATA(_writepos, TLV_EOF);
    }
  }
  else
  {
    /* initialize the Ptr for Read/Write */
    for (index = 0; index < Size; index++)
    {
      TRACE_WRITE_DATA(_writepos, data_to_write[index]);
    }
  }
  TRACE_UnLock();
}

uint32_t USBPD_TRACE_TX_Process(void)
{
  uint32_t _timing = 2;
  uint32_t _begin, _end;

  if (TRACE_CheckLook(&_begin, &_end))
  {
    if (_begin == _end)
    {
      /* nothing to do */
      _timing = 0xFFFFFFFF;
    }
    else
    {
      TRACE_Lock();
      /*  */
      if (_end > _begin)
      {
        SizeSent = _end - _begin;
      }
      else  /* _begin > _end */
      {
        SizeSent = TRACE_SIZE_BUFFER_TX - _begin;
      }
      BSP_TRACE_SendData(&PtrDataTx[_begin], SizeSent);
    }
  }

  return _timing;
}

/**
  * @}
  */

/** @defgroup USBPD_CORE_TRACE_Private_Functions USBPD TRACE Private Functions
  * @{
  */

/**
  * @brief  callback called to end a transfert.
  * @param  None.
  * @retval None.
  */
void TRACE_CALLBACK_TX(void)
{
  TRACE_ENTER_CRITICAL_SECTION();
  PtrReadTx = (PtrReadTx + SizeSent) % TRACE_SIZE_BUFFER_TX;
  TRACE_LEAVE_CRITICAL_SECTION();
  TRACE_UnLock();
}

/**
  * @brief  Lock the trace buffer.
  * @param  None.
  * @retval None.
  */
static void TRACE_Lock(void)
{
  TRACE_ENTER_CRITICAL_SECTION();
  Counter++;
  TRACE_LEAVE_CRITICAL_SECTION();
}

/**
  * @brief  UnLock the trace buffer.
  * @param  None.
  * @retval None.
  */
static void TRACE_UnLock(void)
{
  TRACE_ENTER_CRITICAL_SECTION();
  Counter--;
  TRACE_LEAVE_CRITICAL_SECTION();
}

/**
  * @brief  if buffer is not locked return a Begin / end @ to transfert over the media.
  * @param  address begin of the data
  * @param  address end of the data
  * @retval USBPD_TRUE if a transfer can be execute else USBPD_FALSE.
  */
static uint8_t TRACE_CheckLook(uint32_t *Begin, uint32_t *End)
{
  uint8_t _status = USBPD_FALSE;

  TRACE_ENTER_CRITICAL_SECTION();

  if (0 == Counter)
  {
    *Begin = PtrReadTx;
    *End = PtrWriteTx;
    _status = USBPD_TRUE;
  }

  TRACE_LEAVE_CRITICAL_SECTION();

  return _status;
}

/**
  * @brief  allocate space inside the buffer to push data
  * @param  data size
  * @retval write position inside the buffer is -1 no space available.
  */
static int32_t TRACE_AllocateBufer(uint32_t Size)
{
  int32_t _pos = -1;
  uint32_t _freesize;
  TRACE_ENTER_CRITICAL_SECTION();

  if (PtrWriteTx == PtrReadTx)
  {
    _freesize = TRACE_SIZE_BUFFER_TX;
  }
  else
  {
    if (PtrWriteTx > PtrReadTx)
    {
      _freesize = TRACE_SIZE_BUFFER_TX - PtrWriteTx + PtrReadTx;
    }
    else
    {
      _freesize = PtrReadTx - PtrWriteTx;
    }
  }

  if (_freesize > Size)
  {
    _pos = PtrWriteTx;

    PtrWriteTx = (PtrWriteTx + Size) % TRACE_SIZE_BUFFER_TX;
  }

  TRACE_LEAVE_CRITICAL_SECTION();

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

#endif /* _TRACE || _GUI_INTERFACE*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

