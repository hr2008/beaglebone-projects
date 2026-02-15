#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*-----------------------------------------------------------
 * Basic types
 *----------------------------------------------------------*/

#define portSTACK_TYPE      uint32_t
typedef portSTACK_TYPE     StackType_t;

typedef long               BaseType_t;
typedef unsigned long      UBaseType_t;

#if( configUSE_16_BIT_TICKS == 1 )
    typedef uint16_t TickType_t;
    #define portMAX_DELAY 0xFFFF
#else
    typedef uint32_t TickType_t;
    #define portMAX_DELAY 0xFFFFFFFFUL
    #define portTICK_TYPE_IS_ATOMIC 1
#endif

/*-----------------------------------------------------------
 * Architecture properties
 *----------------------------------------------------------*/

#define portSTACK_GROWTH        (-1)
#define portBYTE_ALIGNMENT      8
#define portTICK_PERIOD_MS      ( ( TickType_t ) 1000 / configTICK_RATE_HZ )

/*-----------------------------------------------------------
 * Assembly helpers
 *----------------------------------------------------------*/
extern volatile uint32_t ulPortYieldRequired;

/*-----------------------------------------------------------
 * Interrupt control (Cortex-A CPSR I-bit)
 *----------------------------------------------------------*/
#define portDISABLE_INTERRUPTS()                     \
    __asm volatile (                                \
        "cpsid i      \n"                           \
        "dsb          \n"                           \
        "isb          \n"                           \
        ::: "memory" )

#define portENABLE_INTERRUPTS()                      \
    __asm volatile (                                \
        "dsb          \n"                           \
        "cpsie i      \n"                           \
        "isb          \n"                           \
        ::: "memory" )


/*-----------------------------------------------------------
 * Critical section control
 *----------------------------------------------------------*/

extern void vPortEnterCritical( void );
extern void vPortExitCritical( void );
extern uint32_t ulPortSetInterruptMask( void );
extern void vPortClearInterruptMask( uint32_t );

#define portENTER_CRITICAL()                vPortEnterCritical()
#define portEXIT_CRITICAL()                 vPortExitCritical()

#define portSET_INTERRUPT_MASK_FROM_ISR()   ulPortSetInterruptMask()
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(x) vPortClearInterruptMask(x)


/* Called at the end of an ISR that can cause a context switch. */
#define portEND_SWITCHING_ISR( xSwitchRequired ) \
    {                                            \
                                                 \
        if( xSwitchRequired != pdFALSE )         \
        {                                      \
            ulPortYieldRequired = pdTRUE;        \
        }                                        \
    }

#define portYIELD_FROM_ISR( x )    portEND_SWITCHING_ISR( x )
#define portYIELD()                __asm volatile ( "SWI 0" ::: "memory" );


/*-----------------------------------------------------------
 * Task function prototypes
 *----------------------------------------------------------*/

#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters )       void vFunction( void *pvParameters )

/*-----------------------------------------------------------
 * Optimised ready priority selection
 *----------------------------------------------------------*/

#ifndef configUSE_PORT_OPTIMISED_TASK_SELECTION
    #define configUSE_PORT_OPTIMISED_TASK_SELECTION 1
#endif

#if configUSE_PORT_OPTIMISED_TASK_SELECTION == 1

    #define portRECORD_READY_PRIORITY( uxPriority, uxReadyPriorities ) \
        ( uxReadyPriorities ) |= ( 1UL << ( uxPriority ) )

    #define portRESET_READY_PRIORITY( uxPriority, uxReadyPriorities ) \
        ( uxReadyPriorities ) &= ~( 1UL << ( uxPriority ) )

    #define portGET_HIGHEST_PRIORITY( uxTopPriority, uxReadyPriorities ) \
        uxTopPriority = 31UL - __builtin_clz( uxReadyPriorities )

#endif

/*-----------------------------------------------------------
 * Tickless idle stub
 *----------------------------------------------------------*/

#ifndef portSUPPRESS_TICKS_AND_SLEEP
    extern void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime );
    #define portSUPPRESS_TICKS_AND_SLEEP(x) vPortSuppressTicksAndSleep(x)
#endif

/*-----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */
