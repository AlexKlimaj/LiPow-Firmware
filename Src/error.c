/**
 ******************************************************************************
 * @file           : error.c
 * @brief          : Handles error information
 ******************************************************************************
 */

#include "error.h"

volatile uint32_t error_state;

/**
 * @brief Returns the error state
 * @retval uint32_t NO_ERROR if no error. Other errors are defined in battery.h
 */
uint32_t Get_Error_State(void) {
	return error_state;
}

void Set_Error_State(uint32_t error_bitmask) {
	error_state |= error_bitmask;
}

void Clear_Error_State(uint32_t error_bitmask) {
	error_state &= ~error_bitmask;
}
