#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <stdint.h>
#include "dmtimer.h"
/* -------------------------------------------------
   Core scheduler
   ------------------------------------------------- */

#define configUSE_PREEMPTION                    1
#define configCPU_CLOCK_HZ                      550000000UL
#define configTICK_RATE_HZ                      10
#define configMAX_PRIORITIES                    5
#define configMINIMAL_STACK_SIZE                128
#define configMAX_TASK_NAME_LEN                 10
#define configUSE_16_BIT_TICKS                  0
#define configUSE_TIME_SLICING                  1
#define configKERNEL_INTERRUPT_PRIORITY         63   /* Kernel tick runs at lowest priority */

#define pdMS_TO_TICKS( xTimeInMs )    ( ( TickType_t ) ( ( ( uint64_t ) (( xTimeInMs ) ) / ( configTICK_RATE_HZ ) ) ) )

/* -------------------------------------------------
   Memory
   ------------------------------------------------- */

#define configSUPPORT_STATIC_ALLOCATION         0
#define configSUPPORT_DYNAMIC_ALLOCATION        1
#define configTOTAL_HEAP_SIZE                   (8 * 1024)

/* -------------------------------------------------
   Cortex-A specifics
   ------------------------------------------------- */

#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1

/* -------------------------------------------------
   Hooks
   ------------------------------------------------- */

#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     1
#define configCHECK_FOR_STACK_OVERFLOW          1
#define configUSE_MALLOC_FAILED_HOOK            1
#define configCHECK_FOR_STACK_OVERFLOW          1
#define configUSE_TRACE_FACILITY                0


/* -------------------------------------------------
   Assertions
   ------------------------------------------------- */

#define configASSERT(x)                         \
    if(!(x)) {                                  \
        __asm volatile("cpsid i");              \
        for(;;);                                \
    }

/* -------------------------------------------------
   Timer setup (NO printf here!)
   ------------------------------------------------- */

#define configSETUP_TICK_INTERRUPT() \
    dmtimer_start(DMTIMER2_BASE)

#define configCLEAR_TICK_INTERRUPT() \
    dmtimer_clear_interrupt(DMTIMER2_BASE)

/* -------------------------------------------------
   API includes
   ------------------------------------------------- */

#define INCLUDE_vTaskDelay                      1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskSuspend                    1

#endif
