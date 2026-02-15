/**
 * AM335x DMTimer Driver Implementation
 */

#include "dmtimer.h"

/* Clock Module registers for DMTimer2 */
#define CM_PER_BASE                 0x44E00000
#define CM_PER_TIMER2_CLKCTRL       (CM_PER_BASE + 0x80)
#define CM_DPLL_BASE                0x44E00500
#define CLKSEL_TIMER2_CLK           (CM_DPLL_BASE + 0x8)

/* Clock control bits */
#define MODULE_ENABLED              0x2
#define IDLEST_FUNCTIONAL           (0x0 << 16)
#define CLK_SEL_CLK_M_OSC           0x1  /* 24MHz oscillator */

/**
 * Enable clock for DMTimer2
 */
static void dmtimer2_enable_clock(void)
{
    volatile uint32_t *clksel = (volatile uint32_t *)CLKSEL_TIMER2_CLK;
    volatile uint32_t *clkctrl = (volatile uint32_t *)CM_PER_TIMER2_CLKCTRL;
    
    /* Select 24MHz oscillator as clock source */
    *clksel = CLK_SEL_CLK_M_OSC;
    
    /* Enable the module clock */
    *clkctrl = MODULE_ENABLED;
    
    /* Wait for clock to be enabled and functional */
    while ((*clkctrl & (0x3 << 16)) != IDLEST_FUNCTIONAL);
}

/**
 * Initialize a DMTimer
 * 
 * @param base: Timer base address
 */
void dmtimer_init(uint32_t base)
{
    /* Enable clock if this is DMTimer2 */
    if (base == DMTIMER2_BASE) {
        dmtimer2_enable_clock();
    }
    
    /* Perform soft reset */
    TIMER_REG(base, TIOCP_CFG) = 0x1;
    
    /* Wait for reset to complete (check TIDR to see if accessible) */
    while (TIMER_REG(base, TIOCP_CFG) & 0x1)       ;
    
    /* Configure for smart idle mode, no wakeup */
    TIMER_REG(base, TIOCP_CFG) = 0x2 << 3;  /* Smart idle */
    
    /* Stop the timer */
    TIMER_REG(base, TCLR) = 0;
    
    /* Clear any pending interrupts */
    TIMER_REG(base, IRQSTATUS) = 0x7;
    
    /* Disable all interrupts */
    TIMER_REG(base, IRQENABLE_CLR) = 0x7;
}

/**
 * Start the timer
 */
void dmtimer_start(uint32_t base)
{
    uint32_t tclr = TIMER_REG(base, TCLR);
    tclr |= TCLR_ST;
    TIMER_REG(base, TCLR) = tclr;
}

/**
 * Stop the timer
 */
void dmtimer_stop(uint32_t base)
{
    uint32_t tclr = TIMER_REG(base, TCLR);
    tclr &= ~TCLR_ST;
    TIMER_REG(base, TCLR) = tclr;
}

/**
 * Set timer period in microseconds
 * 
 * @param base: Timer base address
 * @param period_us: Period in microseconds
 */
void dmtimer_set_period_us(uint32_t base, uint32_t period_us)
{
    /* Calculate count value for period
     * Counter counts up from TLDR to 0xFFFFFFFF, then overflows
     * Count = 0xFFFFFFFF - (period_us * (CLOCK_FREQ / 1000000)) + 1
     */
    uint32_t count = period_us * (DMTIMER_CLOCK_FREQ / 1000000);
    uint32_t load_value = 0xFFFFFFFF - count + 1;
    
    /* Stop timer */
    dmtimer_stop(base);
    
    /* Set load value and counter value */
    TIMER_REG(base, TLDR) = load_value;
    TIMER_REG(base, TCRR) = load_value;
    
    /* Configure for auto-reload mode */
    TIMER_REG(base, TCLR) = TCLR_AR;
}

/**
 * Set timer period in milliseconds
 * 
 * @param base: Timer base address
 * @param period_ms: Period in milliseconds
 */
void dmtimer_set_period_ms(uint32_t base, uint32_t period_ms)
{
    dmtimer_set_period_us(base, period_ms * 1000);
}

/**
 * Enable overflow interrupt
 */
void dmtimer_enable_interrupt(uint32_t base)
{
    TIMER_REG(base, IRQENABLE_SET) = IRQ_OVF_IT_FLAG;
}

/**
 * Disable overflow interrupt
 */
void dmtimer_disable_interrupt(uint32_t base)
{
    TIMER_REG(base, IRQENABLE_CLR) = IRQ_OVF_IT_FLAG;
}

/**
 * Clear interrupt status
 */
void dmtimer_clear_interrupt(uint32_t base)
{
    TIMER_REG(base, IRQSTATUS) = IRQ_OVF_IT_FLAG;
    TIMER_REG(base, IRQ_EOI) = 0;
}

/**
 * Get current counter value
 */
uint32_t dmtimer_get_counter(uint32_t base)
{
    return TIMER_REG(base, TCRR);
}

/**
 * Set counter value
 */
void dmtimer_set_counter(uint32_t base, uint32_t value)
{
    TIMER_REG(base, TCRR) = value;
}

/**
 * Initialize DMTimer2 for FreeRTOS (1ms tick)
 * This is a convenience function that sets up everything needed
 * 
 * To change tick rate:
 * - For 1ms (1000 Hz):  dmtimer_set_period_ms(DMTIMER2_BASE, 1);
 * - For 10ms (100 Hz):  dmtimer_set_period_ms(DMTIMER2_BASE, 10);
 * - For 100μs (10 kHz): dmtimer_set_period_us(DMTIMER2_BASE, 100);
 * 
 * Remember to update configTICK_RATE_HZ in FreeRTOSConfig.h to match!
 */
void dmtimer2_init_for_freertos(int tickRateHz)
{
    /* Initialize the timer */
    dmtimer_init(DMTIMER2_BASE);
    
    /* Set period to 1ms (1000 microseconds) for 1000 Hz tick */
    dmtimer_set_period_ms(DMTIMER2_BASE, tickRateHz);
    
    /* Enable overflow interrupt */
    dmtimer_enable_interrupt(DMTIMER2_BASE);
    
    /* Timer is initialized but not started yet */
    /* Call dmtimer_start(DMTIMER2_BASE) when ready to start scheduler */
}

/**
 * Get current counter value (for debugging)
 */
uint32_t dmtimer_get_current_count(uint32_t base)
{
    return TIMER_REG(base, TCRR);
}

/**
 * Check if timer is running
 */
uint32_t dmtimer_is_running(uint32_t base)
{
    return (TIMER_REG(base, TCLR) & TCLR_ST) ? 1 : 0;
}


/**
 * Polling-based delay in microseconds
 * This is a blocking delay that uses the timer to measure time
 * 
 * @param base: Timer base address
 * @param microseconds: Number of microseconds to delay
 * 
 * Note: This function temporarily uses the timer for delay generation.
 *       The timer state is restored after the delay.
 */
void dmtimer_delay_us(uint32_t base, uint32_t microseconds)
{
    uint32_t ticks_needed;
    uint32_t start_value, current_value;
    uint32_t elapsed_ticks;
    
    /* Save current timer state */
    uint32_t saved_tclr = TIMER_REG(base, TCLR);
    uint32_t saved_tldr = TIMER_REG(base, TLDR);
    uint32_t saved_tcrr = TIMER_REG(base, TCRR);
    
    /* Calculate ticks needed for desired delay */
    /* At 24MHz: 1 microsecond = 24 ticks */
    ticks_needed = microseconds * (DMTIMER_CLOCK_FREQ / 1000000);
    
    /* Configure timer for one-shot mode (no auto-reload) */ 
    TIMER_REG(base, TCLR) = 0;  /* Stop timer */
    
    /* Set counter to 0 and start counting up */
    TIMER_REG(base, TCRR) = 0;
    TIMER_REG(base, TLDR) = 0;  /* Load value of 0 */
    
    /* Start timer (no auto-reload, just count up) */
    TIMER_REG(base, TCLR) = TCLR_ST;  /* Start bit only, no auto-reload */
    
    /* Record start value */
    start_value = TIMER_REG(base, TCRR);
    
    /* Poll until desired ticks have elapsed */
    do {
        current_value = TIMER_REG(base, TCRR);
        
        /* Handle overflow (counter wraps at 0xFFFFFFFF) */
        if (current_value >= start_value) {
            elapsed_ticks = current_value - start_value;
        } else {
            /* Overflow occurred */
            elapsed_ticks = (0xFFFFFFFF - start_value) + current_value + 1;
        }
    } while (elapsed_ticks < ticks_needed);
    
    /* Restore timer state */
    TIMER_REG(base, TCLR) = 0;  /* Stop timer */
    TIMER_REG(base, TLDR) = saved_tldr;
    TIMER_REG(base, TCRR) = saved_tcrr;
    TIMER_REG(base, TCLR) = saved_tclr;  /* Restore control (and running state) */
}

/**
 * Polling-based delay in milliseconds
 * This is a blocking delay that uses the timer to measure time
 * 
 * @param base: Timer base address
 * @param milliseconds: Number of milliseconds to delay
 */
void dmtimer_delay_ms(uint32_t base, uint32_t milliseconds)
{
    /* For large delays, break into smaller chunks to avoid overflow */
    while (milliseconds > 0) {
        if (milliseconds >= 1000) {
            /* Delay 1 second at a time */
            dmtimer_delay_us(base, 1000000);
            milliseconds -= 1000;
        } else {
            /* Delay remaining milliseconds */
            dmtimer_delay_us(base, milliseconds * 1000);
            milliseconds = 0;
        }
    }
}

/**
 * Simple millisecond delay using DMTimer2
 * This is a high-level convenience function
 * 
 * @param milliseconds: Number of milliseconds to delay
 * 
 * Note: This uses DMTimer2. If DMTimer2 is being used by FreeRTOS,
 *       use vTaskDelay() instead. This is primarily for use before
 *       the scheduler starts or in bare-metal applications.
 */
void delay_ms(uint32_t milliseconds)
{
    /* Check if DMTimer2 is configured for FreeRTOS (auto-reload enabled) */
    uint32_t tclr = TIMER_REG(DMTIMER2_BASE, TCLR);
    
    if (tclr & TCLR_AR) {
        /* Timer is in auto-reload mode (FreeRTOS is using it) */
        /* We can still use it for delay by polling the counter */
        
        /* Calculate how many timer overflows we need to wait for */
        /* Each overflow is 1ms when configured for FreeRTOS */
        uint32_t start_count = TIMER_REG(DMTIMER2_BASE, TCRR);
        uint32_t target_overflows = milliseconds;
        uint32_t overflow_count = 0;
        
        while (overflow_count < target_overflows) {
            uint32_t current_count = TIMER_REG(DMTIMER2_BASE, TCRR);
            
            /* Check if overflow occurred (counter wrapped around) */
            if (current_count < start_count) {
                overflow_count++;
            }
            
            start_count = current_count;
        }
    } else {
        /* Timer is not in auto-reload mode, safe to use for delay */
        dmtimer_delay_ms(DMTIMER2_BASE, milliseconds);
    }
}

/**
 * Simple microsecond delay using DMTimer2
 * This is a high-level convenience function
 * 
 * @param microseconds: Number of microseconds to delay
 * 
 * Note: For microsecond precision, timer must not be in auto-reload mode.
 *       If FreeRTOS is running, this may not be accurate.
 */
void delay_us(uint32_t microseconds)
{
    dmtimer_delay_us(DMTIMER2_BASE, microseconds);
}