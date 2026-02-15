/*
*    irq handlers for am335x
*/


#include "intc.h"
#include "debug_uart.h"
#include "printf.h"
#include "dmtimer.h"

#ifdef FREE_RTOS
#include "FreeRTOS.h"
#include "task.h"
#endif

/* Global variables */
static volatile uint32_t system_tick_count = 0;

/*
 * Main IRQ Handler - Called from assembly stub in crt0.s
 * This handles all interrupt sources
 */
void irq_handler(void) {
    /* Get the active interrupt number from INTC */
    uint32_t irq_num = intc_get_active_irq();
    /* Handle the interrupt based on its source */
    /* Handle based on interrupt source */
    #ifdef FREE_RTOS
        BaseType_t xSwitch = pdFALSE;
    #endif

    switch (irq_num) {
        case IRQ_TINT2: /* DMTimer2 overflow - FreeRTOS tick */
        /* Clear the timer interrupt */
        #ifdef FREE_RTOS
        configCLEAR_TICK_INTERRUPT();
        #else
        dmtimer_clear_interrupt(DMTIMER2_BASE);
        #endif  
        /* Increment system tick counter */
        system_tick_count++;
        // if(system_tick_count % 1000 == 0)
        //     printf("Tick: %d\r\n", system_tick_count);
        #ifdef FREE_RTOS
        if (xTaskIncrementTick() != pdFALSE)
        {
            xSwitch = pdTRUE;
        }
        #endif

        break;

    case IRQ_GPIOINT0A: /* GPIO0 Bank interrupt A */
    case IRQ_GPIOINT0B: /* GPIO0 Bank interrupt B */
      /* Handle GPIO0 interrupts here if needed */
        break;

    case IRQ_GPIOINT1A: /* GPIO1 Bank interrupt A */
    case IRQ_GPIOINT1B: /* GPIO1 Bank interrupt B */
      /* Handle GPIO1 interrupts here if needed */
        break;

    case IRQ_UART0INT:   /* UART0 IRQ */
        uart_tx_isr();
        break;

    default:
      /* Spurious or unhandled interrupt */
        break;
    }

    /* Acknowledge interrupt to INTC */
    intc_ack_interrupt();

    #ifdef FREE_RTOS
      /* Perform context switch if needed */
    portEND_SWITCHING_ISR(xSwitch);
    #endif
}

/*
* For FreeRTOS port
*/
void vApplicationIRQHandler(void) {
    /* Call the C IRQ handler */
    irq_handler();
}
/*
* other Exception handlers
*/
void svc_handler_print() { 
    printf("SVC Handler called\r\n"); 
    while(1);
}

void prefetch_handler(void) {
    uint32_t lr, spsr;

    __asm volatile(
        "mov %0, lr\n"
        "mrs %1, spsr\n"
        : "=r"(lr), "=r"(spsr));

    uint32_t pc = lr - 4;

    printf("Prefetch abort: PC=%lx LR=%lx SPSR=%lx\n", pc, lr, spsr);

    while (1);
}
