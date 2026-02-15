/**
 * AM335x Interrupt Controller (INTC) Driver Implementation
 */

#include "intc.h"

/**
 * Initialize the interrupt controller
 */
void intc_init(void)
{
    uint32_t i;
    
    /* Reset the INTC */
    REG(INTC_SYSCONFIG) = 0x2;  /* Soft reset */
    
    /* Wait for reset to complete */
    while (!(REG(INTC_SYSSTATUS) & 0x1))
        ;
    
    /* Disable all interrupts by setting all MIR bits */
    for (i = 0; i < 4; i++) {
        REG(INTC_MIR(i)) = 0xFFFFFFFF;
    }
    
    /* Set all interrupts to lowest priority and route to IRQ (not FIQ) */
    for (i = 0; i < 128; i++) {
        REG(INTC_ILR(i)) = 0;  /* Priority 0, IRQ */
    }
    
    /* Enable new IRQ/FIQ generation */
    REG(INTC_CONTROL) = 0x1;


    /* Enable in INTC (AM335x UART0 IRQ = 72) */
    intc_enable_interrupt(IRQ_UART0INT);

}

/**
 * Enable a specific interrupt
 * 
 * @param irq_num: Interrupt number (0-127)
 */
void intc_enable_interrupt(uint32_t irq_num)
{
    uint32_t reg_num = irq_num / 32;    /* Which MIR register (0-3) */
    uint32_t bit_num = irq_num % 32;    /* Which bit in that register */
    
    /* Clear the mask bit to enable the interrupt */
    REG(INTC_MIR_CLEAR(reg_num)) = (1 << bit_num);
}

/**
 * Disable a specific interrupt
 * 
 * @param irq_num: Interrupt number (0-127)
 */
void intc_disable_interrupt(uint32_t irq_num)
{
    uint32_t reg_num = irq_num / 32;
    uint32_t bit_num = irq_num % 32;
    
    /* Set the mask bit to disable the interrupt */
    REG(INTC_MIR_SET(reg_num)) = (1 << bit_num);
}

/**
 * Set interrupt priority
 * 
 * @param irq_num: Interrupt number (0-127)
 * @param priority: Priority level (0-63, 0 is highest)
 */
void intc_set_priority(uint32_t irq_num, uint32_t priority)
{
    uint32_t ilr_value;
    
    /* Read current ILR value */
    ilr_value = REG(INTC_ILR(irq_num));
    
    /* Clear priority bits and set new priority */
    ilr_value = (ilr_value & ~0xFC) | ((priority & 0x3F) << 2);
    
    /* Write back */
    REG(INTC_ILR(irq_num)) = ilr_value;
}

/**
 * Get the currently active IRQ number
 * 
 * @return: Active IRQ number (0-127) or 0x7F if spurious
 */
uint32_t intc_get_active_irq(void)
{
    /* Read SIR_IRQ register to get active interrupt number */
    return REG(INTC_SIR_IRQ) & 0x7F;
}

/**
 * Acknowledge interrupt processing complete
 * This must be called at the end of IRQ handler to allow new IRQs
 */
void intc_ack_interrupt(void)
{
    /* Write 1 to CONTROL register to signal new IRQ agreement */
    REG(INTC_CONTROL) = 0x1;
}
