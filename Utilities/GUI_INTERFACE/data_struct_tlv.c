/**
  ******************************************************************************
  * @file    data_struct_tlv.c
  * @author  MCD Application Team
  * @brief   data_struct_tlv provides function to manipulate TLV structures
  *          and/or strings.
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
#if defined(_GUI_INTERFACE)

#include <stdint.h>
#include <stdlib.h>
#include "data_struct_tlv.h"

/**
  * @brief    TLV_init_encode initializes a TLV_ToSend_Data structure, to have it ready to be used with TLV_add.
  * @note     TLV_add and TLV_deinit_encode must not be called without calling this function first.
  *           SOF and EOF are created too.
  * @param    ToSendTLV    The TLV_ToSend_Data structure that will be used.
  * @param    Tag          The Tag to put as the master Tag in this instruction.
  * @param    SizeMax      The maximal size of the instruction. Will be used to protect writting against overflow.
  * @param    Ptr          A pointer to the value. This function does not allocate memory, it is the user's responsibility to allocate this.
  * @return   0 if everything went fine, 0xFF otherwise.
  */
uint8_t TLV_init_encode(TLV_ToSend_Data *ToSendTLV, uint8_t Tag, uint16_t SizeMax, uint8_t *Ptr)
{
  if(Ptr == NULL) return 0xFF;                  /* Buffer is NULL*/
  if(SizeMax < 11) return 0xFF;                 /* Because of his small max size, the buffer can't even receive one empty TLV*/
  
  ToSendTLV->data = Ptr;
  ToSendTLV->maxSize = SizeMax;
   
  ToSendTLV->data[0]
    = ToSendTLV->data[1]
      = ToSendTLV->data[2]
        = ToSendTLV->data[3] = TLV_SOF;          /* SOF*/
  
  ToSendTLV->data[4] = Tag;                      /* Tag*/
  
  ToSendTLV->data[5]
    = ToSendTLV->data[6] = 0;                    /* Size*/
  
  ToSendTLV->data[7]
    = ToSendTLV->data[8]
      = ToSendTLV->data[9]
        = ToSendTLV->data[10] = TLV_EOF;         /* EOF*/
  
  ToSendTLV->EOFposition = 7;
  
  return 0;
}

/**
  * @brief    TLV_add inserts data in the TLV format inside the value of a host TLV instruction.
  * @note     Refer to the USB-PD GUI INTERFACE SPECIFICATION DOCUMENT for more info about this practice (part 5.3.1)
  *           TLV_init_encode must be called with the same TLV_ToSend_Data structure before calling this function.
  *           This function can add data to the same TLV_ToSend_Data structure indefinitely as long as the maximal size defined during initialization of the structure isn't reached.
  *           SOF and EOF keeps being handled as data is added.
  * @param    ToSendTLV    The TLV_ToSend_Data structure in which we add data.
  * @param    Tag          The byte to put as Tag of the data we add
  * @param    Size         The Size of the data, in bytes.
  * @param    Value        A pointer to the Value to add.
  * @return   The number of bytes written if everything went fine, 0xFFFF otherwise.
  *
  */
uint16_t TLV_add(TLV_ToSend_Data *ToSendTLV, uint8_t Tag, uint16_t Size, const uint8_t *Value)
{
  if(ToSendTLV->data[ToSendTLV->EOFposition] != TLV_EOF)                return 0xFFFF;       /* EOF has been lost. Has any TLV operation failed, or went interrupted ?*/
  if((ToSendTLV->EOFposition + 4 + 3 + Size) > ToSendTLV->maxSize)      return 0xFFFF;       /* Can't add this TLV, because it will overflow the provided buffer.*/
  if(ToSendTLV->data == NULL)                                          return 0xFFFF;        /* Data points to NULL. Has the encoding been initialized with TLV_init_encode ?*/
  if(ToSendTLV->EOFposition == 0xFFFF)                                 return 0xFFFF;        /* EOF at -1.  Has the decoding been initialized with TLV_init_decode ?*/
  
  ToSendTLV->data[ToSendTLV->EOFposition] = Tag;                           /* Tag*/
  ToSendTLV->EOFposition++;
  ToSendTLV->SizePosition = ToSendTLV->EOFposition;
  
  ToSendTLV->data[ToSendTLV->EOFposition]   = (Size >> 8) & 0x00FF;        /* Size*/
  ToSendTLV->data[ToSendTLV->EOFposition+1] = (Size     ) & 0x00FF;
  ToSendTLV->EOFposition += 2;
  
  for(uint32_t index = 0; index < Size; index++)                                          /* Value*/
  {
    ToSendTLV->data[ToSendTLV->EOFposition] = Value[index];
    ToSendTLV->EOFposition++;
  }
  
  ToSendTLV->data[ToSendTLV->EOFposition]
    = ToSendTLV->data[ToSendTLV->EOFposition+1]
      = ToSendTLV->data[ToSendTLV->EOFposition+2]
        = ToSendTLV->data[ToSendTLV->EOFposition+3] = TLV_EOF;             /* EOF*/
  
  uint16_t total_lgth = (ToSendTLV->data[5] << 8) + ToSendTLV->data[6] + 3 + Size; /* Update of the top level Size marker*/
  ToSendTLV->data[5] = total_lgth >> 8;
  ToSendTLV->data[6] = total_lgth;
  
  return 3 + Size;
}

/**
  * @brief    Inserts value inside the TLV packet
  * @param    ToSendTLV    The TLV_ToSend_Data structure in which we add data.
  * @param    Size         The Size of the data, in bytes.
  * @param    Value        A pointer to the Value to add.
  * @return   The number of bytes written if everything went fine, 0xFFFF otherwise.
  *
  */
uint16_t TLV_addValue(TLV_ToSend_Data *ToSendTLV, const uint8_t *Value, uint16_t Size)
{
  if(ToSendTLV->data[ToSendTLV->EOFposition] != TLV_EOF)                return 0xFFFF;       /* EOF has been lost. Has any TLV operation failed, or went interrupted ?*/
  if((ToSendTLV->EOFposition + 4 + 3 + Size) > ToSendTLV->maxSize)      return 0xFFFF;       /* Can't add this TLV, because it will overflow the provided buffer.*/
  if(ToSendTLV->data == NULL)                                          return 0xFFFF;        /* Data points to NULL. Has the encoding been initialized with TLV_init_encode ?*/
  if(ToSendTLV->EOFposition == 0xFFFF)                                 return 0xFFFF;        /* EOF at -1.  Has the decoding been initialized with TLV_init_decode ?*/
  
  for(uint32_t index = 0; index < Size; index++)                                          /* Value*/
  {
    ToSendTLV->data[ToSendTLV->EOFposition] = Value[index];
    ToSendTLV->EOFposition++;
  }
  
  ToSendTLV->data[ToSendTLV->EOFposition]
    = ToSendTLV->data[ToSendTLV->EOFposition+1]
      = ToSendTLV->data[ToSendTLV->EOFposition+2]
        = ToSendTLV->data[ToSendTLV->EOFposition+3] = TLV_EOF;             /* EOF*/
  
  uint16_t total_lgth = (ToSendTLV->data[5] << 8) + ToSendTLV->data[6] + Size; /* Update of the top level Size marker*/
  ToSendTLV->data[5] = total_lgth >> 8;
  ToSendTLV->data[6] = total_lgth;
  
  return 3 + Size;
}

/**
  * @brief    Update size of previous inserted TAG inside the TLV packet
  * @param    ToSendTLV    The TLV_ToSend_Data structure in which we add data.
  * @param    Size         The Size of the data, in bytes.
  * @return   None
  *
  */
void TLV_UpdateSizeTag(TLV_ToSend_Data *ToSendTLV, uint16_t Size)
{
  ToSendTLV->data[ToSendTLV->SizePosition]     = (Size >> 8) & 0x00FF;        /* Size*/
  ToSendTLV->data[ToSendTLV->SizePosition + 1] = (Size     ) & 0x00FF;
}
/**
  * @brief    TLV_deinit_encode deinitialize a TLV_ToSend_Data structure.
  * @note     After this, TLV_add can no longer be used with this structure (unless if it is reinitialized with TLV_init_encode).
  *           If a TLV_ToSend_Data structure is used without deinitialization, the associated string can be altered.
  *           Once deinitialization is done, the string used can directly be send.
  * @param    ToSendTLV    The TLV_ToSend_Data structure to deinitialize.
  * @return   None
  */
void TLV_deinit_encode(TLV_ToSend_Data *ToSendTLV)
{
  ToSendTLV->data        = NULL;
  ToSendTLV->EOFposition = 0xFFFF;
  ToSendTLV->maxSize     = 0;
}

/**
  * @brief    TLV_init_decode initializes a TLV_Received_Data structure, to have it ready to be used with TLV_get.
  *           TLV_get and TLV_deinit_decode must not be called without calling this function first.
  * @note     This function places a cursor on the first parameter.
  * @param    ToProcessTLV    The TLV_Received_Data that will be used.
  * @param    pReceivedString          The string which will be decoded.
  * @return   The position of the cursor if everything went fine, 0xFF otherwise.
  */
uint8_t TLV_init_decode(TLV_Received_Data *ToProcessTLV, uint8_t *pReceivedString)
{
  if(pReceivedString == NULL) return 0xFF;               /* Received string is NULL*/
  if(  (pReceivedString[0] != TLV_SOF)
     ||(pReceivedString[1] != TLV_SOF)
     ||(pReceivedString[2] != TLV_SOF)
     ||(pReceivedString[3] != TLV_SOF)) return 0xFF;       /* Incorrect SOF*/
  
  ToProcessTLV->data = pReceivedString;
  ToProcessTLV->cursor = 7; /* Cursor at start of value*/
  
  return ToProcessTLV->cursor;
}

/**
  * @brief    TLV_get gets data in TLV format from inside the value of a host TLV instruction.
  * @note     Refer to the USB-PD GUI INTERFACE SPECIFICATION DOCUMENT for more info about this practice (part 5.3.1)
  *           TLV_init_decode must be called with the same TLV_Received_Data structure before calling this function.
  *           Once this function has read its parameter, it places the cursor at the beginning of the next one. 
  *           Refer to the returned value to see if there is one or not.
  * @param    ToProcessTLV    The TLV_Received_Data structure which is associated to the string we want to get a parameter from.
  * @param    Tag            A pointer to where the decoded Tag should be stocked.
  * @param    Length            A pointer to where the decoded size should be stocked.
  * @param    Value            A double pointer, which will contain a pointer directly to the data in the reception buffer.
  * @return   0 if the reading was fine and there is another parameter after, 1 if the reading was fine and it was the last parameter, 0xFF otherwise.
  */
uint8_t TLV_get(TLV_Received_Data *ToProcessTLV, uint8_t *Tag, uint16_t *Length, uint8_t **Value)
{
  if(ToProcessTLV->data == NULL) return 0xFF;                           /* Data points to NULL. Has the decoding been initialized with TLV_init_decode ?*/
  if(ToProcessTLV->cursor == 0)  return 0xFF;                           /* The cursor is not positioned. Has the decoding been initialized with TLV_init_decode ?*/
  if(ToProcessTLV->data[ToProcessTLV->cursor] == TLV_EOF) return 0xFF;  /* EOF reached. There is not any parameter left to read.*/

  *Tag = ToProcessTLV->data[ToProcessTLV->cursor];                                                         /* Tag*/
  ToProcessTLV->cursor++;
  *Length = (ToProcessTLV->data[ToProcessTLV->cursor] << 8) + ToProcessTLV->data[ToProcessTLV->cursor+1];  /* Length*/
  ToProcessTLV->cursor += 2;
  *Value = &ToProcessTLV->data[ToProcessTLV->cursor];                                                      /* Value*/
  ToProcessTLV->cursor += *Length;

  return 0;
}

/**
  * @brief    TLV_deinit_decode deinitialize a TLV_Received_Data structure.
  * @note     After this, TLV_get can no longer be used with this structure (unless if it is reinitialized with TLV_init_decode).
  *           Once the cursor reach the EOF, there isn't anything to do except deinitializing the TLV_Received_Data structure with this function (eventually to reinitialize it after, to restart decoding from the beginning).
  *           This function do not handle any deallocation.
  * @param    ToProcessTLV    The TLV_Received_Data structure to deinitialize.
  * @return   None
  */
void TLV_deinit_decode(TLV_Received_Data *ToProcessTLV)
{
  ToProcessTLV->data = NULL;
  ToProcessTLV->cursor = 0;
}

/**
  * @brief    TLV_getStringLength returns the length of a 8 bit TLV string, in bytes.
  * @note     This function relies on the length marker in the string. Whether there is or not the SOF and/or the EOF, it will return the size of the string without them.
  * @param    pString      A uint8_t pString, under TLV format, with or without SOF or EOF.
  * @return   The size of the pString in bytes; including tag, length, and value; excluding SOF and EOF, if applicable.
  */
uint16_t TLV_get_string_length(const uint8_t* pString)
{
  uint16_t  length;      /* Variable to be return.*/
  uint8_t   start = 0;    /* To indicate the start of the real pString, in case there is a EOF. */

  while(pString[start] == TLV_SOF)
  {
    start++;
  } /* start variable is now after the SOF if there is one.*/

  length =   (uint16_t)pString[start+1] << 8
    |(uint16_t)pString[start+2];
  length += 3;
  
  return length;
}
#endif /* _GUI_INTERFACE */
