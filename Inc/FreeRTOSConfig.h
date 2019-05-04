/* USER CODE BEGIN Header */
/*
 * FreeRTOS Kernel V10.0.1
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */
 /* USER CODE END Header */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * These parameters and more are described within the 'configuration' section of the
 * FreeRTOS API documentation available on the FreeRTOS.org web site.
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

/* USER CODE BEGIN Includes */   	      
/* Section where include file can be added */
/* USER CODE END Includes */ 

/* Ensure stdint is only used by the compiler, and not the assembler. */
#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
    #include <stdint.h>
    extern uint32_t SystemCoreClock;
/* USER CODE BEGIN 0 */   	      
    extern volatile unsigned long ulHighFrequencyTimerTicks;
/* USER CODE END 0 */       
#endif

#define configUSE_PREEMPTION                     1
#define configSUPPORT_STATIC_ALLOCATION          0
#define configSUPPORT_DYNAMIC_ALLOCATION         1
#define configUSE_IDLE_HOOK                      0
#define configUSE_TICK_HOOK                      0
#define configCPU_CLOCK_HZ                       ( SystemCoreClock )
#define configTICK_RATE_HZ                       ((TickType_t)1000)
#define configMAX_PRIORITIES                     ( 7 )
#define configMINIMAL_STACK_SIZE                 ((uint16_t)64)
#define configTOTAL_HEAP_SIZE                    ((size_t)12000)
#define configMAX_TASK_NAME_LEN                  ( 16 )
#define configGENERATE_RUN_TIME_STATS            1
#define configUSE_TRACE_FACILITY                 1
#define configUSE_16_BIT_TICKS                   0
#define configUSE_MUTEXES                        1
#define configQUEUE_REGISTRY_SIZE                8

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES                    0
#define configMAX_CO_ROUTINE_PRIORITIES          ( 2 )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet            1
#define INCLUDE_uxTaskPriorityGet           1
#define INCLUDE_vTaskDelete                 1
#define INCLUDE_vTaskCleanUpResources       0
#define INCLUDE_vTaskSuspend                1
#define INCLUDE_vTaskDelayUntil             0
#define INCLUDE_vTaskDelay                  1
#define INCLUDE_xTaskGetSchedulerState      1

/* Normal assert() semantics without relying on the provision of an assert.h
header file. */
/* USER CODE BEGIN 1 */
#define configASSERT( x ) if ((x) == 0) {taskDISABLE_INTERRUPTS(); for( ;; );} 
/* USER CODE END 1 */

/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
standard names. */
#define vPortSVCHandler    SVC_Handler
#define xPortPendSVHandler PendSV_Handler

/* IMPORTANT: This define is commented when used with STM32Cube firmware, when the timebase source is SysTick,
              to prevent overwriting SysTick_Handler defined within STM32Cube HAL */
#define xPortSysTickHandler SysTick_Handler

/* USER CODE BEGIN 2 */    
/* Definitions needed when configGENERATE_RUN_TIME_STATS is on */
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() ( ulHighFrequencyTimerTicks = 0UL )
#define portGET_RUN_TIME_COUNTER_VALUE() ulHighFrequencyTimerTicks
/* USER CODE END 2 */

/* USER CODE BEGIN Defines */   	      
/* Section where parameter definitions can be added (for instance, to override default ones in FreeRTOS.h) */
#define configCOMMAND_INT_MAX_OUTPUT_SIZE 1600
#define configUSE_STATS_FORMATTING_FUNCTIONS 1
#define configUSE_TASK_NOTIFICATIONS 1

/* Priorities at which the tasks are created. */
#define REGULATOR_TASK_PRIORITY			( tskIDLE_PRIORITY + 4 )
#define ADC_TASK_PRIORITY				( tskIDLE_PRIORITY + 3 )
#define	LED_TASK_PRIORITY				( tskIDLE_PRIORITY + 2 )
#define UART_CLI_TASK_PRIORITY			( tskIDLE_PRIORITY + 1 )
#define USBPD_TASK_PRIORITY				( tskIDLE_PRIORITY + 2 )

#define vcliSTACK_SIZE					( configMINIMAL_STACK_SIZE * 3 )
#define vRead_ADC_STACK_SIZE			( configMINIMAL_STACK_SIZE * 2 )
#define vRegulator_STACK_SIZE			( configMINIMAL_STACK_SIZE * 2 )

/* USER CODE END Defines */ 

#endif /* FREERTOS_CONFIG_H */
