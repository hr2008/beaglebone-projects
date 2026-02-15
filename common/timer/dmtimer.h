/**
 * AM335x DMTimer Driver
 * 
 * The AM335x has 8 DMTimers (DMTimer0-7). 
 * DMTimer2 is commonly used for OS tick as it's always available.
 */

#ifndef DMTIMER_H
#define DMTIMER_H

#include <stdint.h>

/* DMTimer Base Addresses */
#define DMTIMER0_BASE       0x44E05000
#define DMTIMER1_1MS_BASE   0x44E31000  /* 1ms timer */
#define DMTIMER2_BASE       0x48040000
#define DMTIMER3_BASE       0x48042000
#define DMTIMER4_BASE       0x48044000
#define DMTIMER5_BASE       0x48046000
#define DMTIMER6_BASE       0x48048000
#define DMTIMER7_BASE       0x4804A000

/* DMTimer Register Offsets (same for all timers) */
#define TIDR                0x00    /* Identification Register */
#define TIOCP_CFG           0x10    /* Timer OCP Configuration */
#define IRQ_EOI             0x20    /* Timer IRQ End-Of-Interrupt */
#define IRQSTATUS_RAW       0x24    /* Timer Status Raw Register */
#define IRQSTATUS           0x28    /* Timer Status Register */
#define IRQENABLE_SET       0x2C    /* Timer Interrupt Enable Set */
#define IRQENABLE_CLR       0x30    /* Timer Interrupt Enable Clear */
#define IRQWAKEEN           0x34    /* Timer Wakeup Enable Register */
#define TCLR                0x38    /* Timer Control Register */
#define TCRR                0x3C    /* Timer Counter Register */
#define TLDR                0x40    /* Timer Load Register */
#define TTGR                0x44    /* Timer Trigger Register */
#define TWPS                0x48    /* Timer Write Posted Status */
#define TMAR                0x4C    /* Timer Match Register */
#define TCAR1               0x50    /* Timer Capture Register 1 */
#define TSICR               0x54    /* Timer Synchronous Interface Control */
#define TCAR2               0x58    /* Timer Capture Register 2 */

/* TCLR (Control Register) Bit Definitions */
#define TCLR_ST             (1 << 0)    /* Start/Stop timer */
#define TCLR_AR             (1 << 1)    /* Auto-reload mode */
#define TCLR_PTV_SHIFT      2           /* Pre-scaler value shift */
#define TCLR_PRE            (1 << 5)    /* Pre-scaler enable */
#define TCLR_CE             (1 << 6)    /* Compare enable */
#define TCLR_SCPWM          (1 << 7)    /* PWM output pin behavior */
#define TCLR_TCM_SHIFT      8           /* Transition capture mode */
#define TCLR_TRG_SHIFT      10          /* Trigger output mode */
#define TCLR_PT             (1 << 12)   /* Pulse or toggle mode */
#define TCLR_CAPT_MODE      (1 << 13)   /* Capture mode (1st/2nd) */
#define TCLR_GPO_CFG        (1 << 14)   /* PWM output/GP output */

/* IRQSTATUS/IRQENABLE Bit Definitions */
#define IRQ_MAT_IT_FLAG     (1 << 0)    /* Match interrupt */
#define IRQ_OVF_IT_FLAG     (1 << 1)    /* Overflow interrupt */
#define IRQ_TCAR_IT_FLAG    (1 << 2)    /* Capture interrupt */

/* Clock frequencies */
#define DMTIMER_CLOCK_FREQ  24000000    /* 24MHz - default clock source */

/* Register access macros */
#define TIMER_REG(base, offset)     (*(volatile uint32_t *)((base) + (offset)))

/* Function prototypes */
void dmtimer_init(uint32_t base);
void dmtimer_start(uint32_t base);
void dmtimer_stop(uint32_t base);
void dmtimer_set_period_us(uint32_t base, uint32_t period_us);
void dmtimer_set_period_ms(uint32_t base, uint32_t period_ms);
void dmtimer_enable_interrupt(uint32_t base);
void dmtimer_disable_interrupt(uint32_t base);
void dmtimer_clear_interrupt(uint32_t base);
uint32_t dmtimer_get_counter(uint32_t base);
void dmtimer_set_counter(uint32_t base, uint32_t value);

/* Convenience functions for common operations */
void dmtimer2_init_for_freertos(int);  /* Initialize DMTimer2 for 1ms tick */
/* Polling-based delay functions (blocking) */
void dmtimer_delay_ms(uint32_t base, uint32_t milliseconds);
void dmtimer_delay_us(uint32_t base, uint32_t microseconds);

/* High-level delay using DMTimer2 (easier to use) */
void delay_ms(uint32_t milliseconds);
void delay_us(uint32_t microseconds);

/* Debug helpers */
uint32_t dmtimer_get_current_count(uint32_t base);  /* Get current counter value */
uint32_t dmtimer_is_running(uint32_t base);         /* Check if timer is running */


#endif /* DMTIMER_H */
