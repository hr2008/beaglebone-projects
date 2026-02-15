/**
 * AM335x GPIO Driver Implementation
 */

#include "gpio.h"

/**
 * Get the base address for a GPIO bank
 */
uint32_t gpio_get_bank_base(gpio_bank_t bank)
{
    switch (bank) {
        case GPIO_BANK_0: return GPIO0_BASE;
        case GPIO_BANK_1: return GPIO1_BASE;
        case GPIO_BANK_2: return GPIO2_BASE;
        case GPIO_BANK_3: return GPIO3_BASE;
        default: return 0;
    }
}

/**
 * Enable clock for a specific GPIO bank
 */
void gpio_enable_bank(gpio_bank_t bank)
{
    volatile uint32_t *clkctrl_reg;
    
    /* Get the clock control register for this bank */
    switch (bank) {
        case GPIO_BANK_0:
            clkctrl_reg = (volatile uint32_t *)CM_WKUP_GPIO0_CLKCTRL;
            break;
        case GPIO_BANK_1:
            clkctrl_reg = (volatile uint32_t *)CM_PER_GPIO1_CLKCTRL;
            break;
        case GPIO_BANK_2:
            clkctrl_reg = (volatile uint32_t *)CM_PER_GPIO2_CLKCTRL;
            break;
        case GPIO_BANK_3:
            clkctrl_reg = (volatile uint32_t *)CM_PER_GPIO3_CLKCTRL;
            break;
        default:
            return;
    }
    
    /* Enable the module */
    *clkctrl_reg = CM_CLKCTRL_MODULEMODE_ENABLE;
    
    /* Wait for module to be fully functional */
    while ((*clkctrl_reg & CM_CLKCTRL_IDLEST_MASK) != CM_CLKCTRL_IDLEST_FUNCTIONAL)
        ;
}

/**
 * Initialize GPIO subsystem
 * This enables clocks for all GPIO banks
 */
void gpio_init(void)
{
    gpio_enable_bank(GPIO_BANK_0);
    gpio_enable_bank(GPIO_BANK_1);
    gpio_enable_bank(GPIO_BANK_2);
    gpio_enable_bank(GPIO_BANK_3);
}

/**
 * Set pin direction (input or output)
 */
void gpio_set_direction(gpio_bank_t bank, uint8_t pin, gpio_direction_t dir)
{
    uint32_t base = gpio_get_bank_base(bank);
    uint32_t oe_reg;
    
    if (base == 0 || pin > 31) {
        return;
    }
    
    oe_reg = GPIO_REG(base, GPIO_OE);
    
    if (dir == GPIO_DIR_OUTPUT) {
        /* Clear bit for output (0 = output) */
        oe_reg &= ~GPIO_PIN_MASK(pin);
    } else {
        /* Set bit for input (1 = input) */
        oe_reg |= GPIO_PIN_MASK(pin);
    }
    
    GPIO_REG(base, GPIO_OE) = oe_reg;
}

/**
 * Get pin direction
 */
gpio_direction_t gpio_get_direction(gpio_bank_t bank, uint8_t pin)
{
    uint32_t base = gpio_get_bank_base(bank);
    uint32_t oe_reg;
    
    if (base == 0 || pin > 31) {
        return GPIO_DIR_INPUT;
    }
    
    oe_reg = GPIO_REG(base, GPIO_OE);
    
    /* If bit is 1, it's input; if 0, it's output */
    return (oe_reg & GPIO_PIN_MASK(pin)) ? GPIO_DIR_INPUT : GPIO_DIR_OUTPUT;
}

/**
 * Set a GPIO pin high
 */
void gpio_pin_set(gpio_bank_t bank, uint8_t pin)
{
    uint32_t base = gpio_get_bank_base(bank);
    
    if (base == 0 || pin > 31) {
        return;
    }
    
    /* Write to SETDATAOUT register */
    GPIO_REG(base, GPIO_SETDATAOUT) = GPIO_PIN_MASK(pin);
}

/**
 * Set a GPIO pin low
 */
void gpio_pin_clear(gpio_bank_t bank, uint8_t pin)
{
    uint32_t base = gpio_get_bank_base(bank);
    
    if (base == 0 || pin > 31) {
        return;
    }
    
    /* Write to CLEARDATAOUT register */
    GPIO_REG(base, GPIO_CLEARDATAOUT) = GPIO_PIN_MASK(pin);
}

/**
 * Toggle a GPIO pin
 */
void gpio_pin_toggle(gpio_bank_t bank, uint8_t pin)
{
    uint32_t base = gpio_get_bank_base(bank);
    uint32_t dataout;
    
    if (base == 0 || pin > 31) {
        return;
    }
    
    /* Read current output state */
    dataout = GPIO_REG(base, GPIO_DATAOUT);
    
    if (dataout & GPIO_PIN_MASK(pin)) {
        /* Currently high, set it low */
        gpio_pin_clear(bank, pin);
    } else {
        /* Currently low, set it high */
        gpio_pin_set(bank, pin);
    }
}

/**
 * Write a value to a GPIO pin
 */
void gpio_pin_write(gpio_bank_t bank, uint8_t pin, gpio_pin_state_t state)
{
    if (state == GPIO_PIN_HIGH) {
        gpio_pin_set(bank, pin);
    } else {
        gpio_pin_clear(bank, pin);
    }
}

/**
 * Read a GPIO pin value
 */
gpio_pin_state_t gpio_pin_read(gpio_bank_t bank, uint8_t pin)
{
    uint32_t base = gpio_get_bank_base(bank);
    uint32_t datain;
    
    if (base == 0 || pin > 31) {
        return GPIO_PIN_LOW;
    }
    
    /* Read DATAIN register */
    datain = GPIO_REG(base, GPIO_DATAIN);
    
    return (datain & GPIO_PIN_MASK(pin)) ? GPIO_PIN_HIGH : GPIO_PIN_LOW;
}

/**
 * Write to entire GPIO port (all 32 pins)
 */
void gpio_port_write(gpio_bank_t bank, uint32_t value)
{
    uint32_t base = gpio_get_bank_base(bank);
    
    if (base == 0) {
        return;
    }
    
    GPIO_REG(base, GPIO_DATAOUT) = value;
}

/**
 * Read entire GPIO port (all 32 pins)
 */
uint32_t gpio_port_read(gpio_bank_t bank)
{
    uint32_t base = gpio_get_bank_base(bank);
    
    if (base == 0) {
        return 0;
    }
    
    return GPIO_REG(base, GPIO_DATAIN);
}

/**
 * Set specific bits in GPIO port
 */
void gpio_port_set_bits(gpio_bank_t bank, uint32_t mask)
{
    uint32_t base = gpio_get_bank_base(bank);
    
    if (base == 0) {
        return;
    }
    
    GPIO_REG(base, GPIO_SETDATAOUT) = mask;
}

/**
 * Clear specific bits in GPIO port
 */
void gpio_port_clear_bits(gpio_bank_t bank, uint32_t mask)
{
    uint32_t base = gpio_get_bank_base(bank);
    
    if (base == 0) {
        return;
    }
    
    GPIO_REG(base, GPIO_CLEARDATAOUT) = mask;
}

/**
 * Enable interrupt for a GPIO pin
 */
void gpio_enable_interrupt(gpio_bank_t bank, uint8_t pin, gpio_int_type_t type)
{
    uint32_t base = gpio_get_bank_base(bank);
    uint32_t pin_mask = GPIO_PIN_MASK(pin);
    
    if (base == 0 || pin > 31) {
        return;
    }
    
    /* First, clear all detection types for this pin */
    GPIO_REG(base, GPIO_LEVELDETECT0) &= ~pin_mask;
    GPIO_REG(base, GPIO_LEVELDETECT1) &= ~pin_mask;
    GPIO_REG(base, GPIO_RISINGDETECT) &= ~pin_mask;
    GPIO_REG(base, GPIO_FALLINGDETECT) &= ~pin_mask;
    
    /* Configure detection type */
    switch (type) {
        case GPIO_INT_RISING:
            GPIO_REG(base, GPIO_RISINGDETECT) |= pin_mask;
            break;
            
        case GPIO_INT_FALLING:
            GPIO_REG(base, GPIO_FALLINGDETECT) |= pin_mask;
            break;
            
        case GPIO_INT_BOTH:
            GPIO_REG(base, GPIO_RISINGDETECT) |= pin_mask;
            GPIO_REG(base, GPIO_FALLINGDETECT) |= pin_mask;
            break;
            
        case GPIO_INT_LOW_LEVEL:
            GPIO_REG(base, GPIO_LEVELDETECT0) |= pin_mask;
            break;
            
        case GPIO_INT_HIGH_LEVEL:
            GPIO_REG(base, GPIO_LEVELDETECT1) |= pin_mask;
            break;
            
        case GPIO_INT_NONE:
        default:
            /* Already cleared above */
            break;
    }
    
    /* Enable interrupt for this pin on IRQ line 0 */
    GPIO_REG(base, GPIO_IRQSTATUS_SET_0) = pin_mask;
}

/**
 * Disable interrupt for a GPIO pin
 */
void gpio_disable_interrupt(gpio_bank_t bank, uint8_t pin)
{
    uint32_t base = gpio_get_bank_base(bank);
    uint32_t pin_mask = GPIO_PIN_MASK(pin);
    
    if (base == 0 || pin > 31) {
        return;
    }
    
    /* Clear interrupt enable */
    GPIO_REG(base, GPIO_IRQSTATUS_CLR_0) = pin_mask;
    
    /* Clear all detection types */
    GPIO_REG(base, GPIO_LEVELDETECT0) &= ~pin_mask;
    GPIO_REG(base, GPIO_LEVELDETECT1) &= ~pin_mask;
    GPIO_REG(base, GPIO_RISINGDETECT) &= ~pin_mask;
    GPIO_REG(base, GPIO_FALLINGDETECT) &= ~pin_mask;
}

/**
 * Get interrupt status
 */
uint32_t gpio_get_interrupt_status(gpio_bank_t bank, uint8_t irq_line)
{
    uint32_t base = gpio_get_bank_base(bank);
    
    if (base == 0) {
        return 0;
    }
    
    if (irq_line == 0) {
        return GPIO_REG(base, GPIO_IRQSTATUS_0);
    } else {
        return GPIO_REG(base, GPIO_IRQSTATUS_1);
    }
}

/**
 * Clear interrupt for a specific pin
 */
void gpio_clear_interrupt(gpio_bank_t bank, uint8_t irq_line, uint8_t pin)
{
    uint32_t base = gpio_get_bank_base(bank);
    uint32_t pin_mask = GPIO_PIN_MASK(pin);
    
    if (base == 0 || pin > 31) {
        return;
    }
    
    if (irq_line == 0) {
        GPIO_REG(base, GPIO_IRQSTATUS_0) = pin_mask;
    } else {
        GPIO_REG(base, GPIO_IRQSTATUS_1) = pin_mask;
    }
}

/**
 * Enable debouncing for a pin
 */
void gpio_enable_debounce(gpio_bank_t bank, uint8_t pin)
{
    uint32_t base = gpio_get_bank_base(bank);
    
    if (base == 0 || pin > 31) {
        return;
    }
    
    GPIO_REG(base, GPIO_DEBOUNCENABLE) |= GPIO_PIN_MASK(pin);
}

/**
 * Disable debouncing for a pin
 */
void gpio_disable_debounce(gpio_bank_t bank, uint8_t pin)
{
    uint32_t base = gpio_get_bank_base(bank);
    
    if (base == 0 || pin > 31) {
        return;
    }
    
    GPIO_REG(base, GPIO_DEBOUNCENABLE) &= ~GPIO_PIN_MASK(pin);
}

/**
 * Set debounce time (applies to entire bank)
 * time_ms: debounce time in milliseconds (1-255)
 */
void gpio_set_debounce_time(gpio_bank_t bank, uint8_t time_ms)
{
    uint32_t base = gpio_get_bank_base(bank);
    
    if (base == 0) {
        return;
    }
    
    /* Debounce time register value = (debounce_time_ms * 32) - 1 */
    /* Range: 31-8191, corresponding to ~1ms to ~256ms @ 32kHz clock */
    uint32_t reg_value = (time_ms * 32) - 1;
    
    if (reg_value > 8191) {
        reg_value = 8191;
    }
    
    GPIO_REG(base, GPIO_DEBOUNCINGTIME) = reg_value;
}


