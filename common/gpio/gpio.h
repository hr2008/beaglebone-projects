/**
 * AM335x GPIO Driver
 * 
 * The AM335x has 4 GPIO banks (GPIO0-GPIO3), each with 32 pins.
 * Total: 128 GPIO pins
 * 
 */

#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include <stdbool.h>

/* GPIO Bank Base Addresses */
#define GPIO0_BASE          0x44E07000
#define GPIO1_BASE          0x4804C000
#define GPIO2_BASE          0x481AC000
#define GPIO3_BASE          0x481AE000

/* GPIO Register Offsets (same for all banks) */
#define GPIO_REVISION           0x000   /* IP Revision Identifier */
#define GPIO_SYSCONFIG          0x010   /* System Configuration */
#define GPIO_EOI                0x020   /* End Of Interrupt */
#define GPIO_IRQSTATUS_RAW_0    0x024   /* Interrupt Status Raw (bank 0) */
#define GPIO_IRQSTATUS_RAW_1    0x028   /* Interrupt Status Raw (bank 1) */
#define GPIO_IRQSTATUS_0        0x02C   /* Interrupt Status (bank 0) */
#define GPIO_IRQSTATUS_1        0x030   /* Interrupt Status (bank 1) */
#define GPIO_IRQSTATUS_SET_0    0x034   /* Interrupt Status Set (bank 0) */
#define GPIO_IRQSTATUS_SET_1    0x038   /* Interrupt Status Set (bank 1) */
#define GPIO_IRQSTATUS_CLR_0    0x03C   /* Interrupt Status Clear (bank 0) */
#define GPIO_IRQSTATUS_CLR_1    0x040   /* Interrupt Status Clear (bank 1) */
#define GPIO_IRQWAKEN_0         0x044   /* Wakeup Enable (bank 0) */
#define GPIO_IRQWAKEN_1         0x048   /* Wakeup Enable (bank 1) */
#define GPIO_SYSSTATUS          0x114   /* System Status */
#define GPIO_CTRL               0x130   /* Module Control */
#define GPIO_OE                 0x134   /* Output Enable (0=output, 1=input) */
#define GPIO_DATAIN             0x138   /* Data Input */
#define GPIO_DATAOUT            0x13C   /* Data Output */
#define GPIO_LEVELDETECT0       0x140   /* Low-level Detect */
#define GPIO_LEVELDETECT1       0x144   /* High-level Detect */
#define GPIO_RISINGDETECT       0x148   /* Rising Edge Detect */
#define GPIO_FALLINGDETECT      0x14C   /* Falling Edge Detect */
#define GPIO_DEBOUNCENABLE      0x150   /* Debounce Enable */
#define GPIO_DEBOUNCINGTIME     0x154   /* Debouncing Time */
#define GPIO_CLEARDATAOUT       0x190   /* Clear Data Output */
#define GPIO_SETDATAOUT         0x194   /* Set Data Output */

/* Clock Module Base Addresses for enabling GPIO clocks */
#define CM_PER_BASE             0x44E00000
#define CM_WKUP_BASE            0x44E00400

/* Clock enable registers */
#define CM_PER_GPIO1_CLKCTRL    (CM_PER_BASE + 0xAC)
#define CM_PER_GPIO2_CLKCTRL    (CM_PER_BASE + 0xB0)
#define CM_PER_GPIO3_CLKCTRL    (CM_PER_BASE + 0xB4)
#define CM_WKUP_GPIO0_CLKCTRL   (CM_WKUP_BASE + 0x8)

/* Clock control values */
#define CM_CLKCTRL_MODULEMODE_ENABLE    0x2
#define CM_CLKCTRL_IDLEST_FUNCTIONAL    0x0
#define CM_CLKCTRL_IDLEST_MASK          (0x3 << 16)

/* GPIO Pin Definitions */
typedef enum {
    GPIO_BANK_0 = 0,
    GPIO_BANK_1 = 1,
    GPIO_BANK_2 = 2,
    GPIO_BANK_3 = 3
} gpio_bank_t;

typedef enum {
    GPIO_DIR_INPUT = 0,
    GPIO_DIR_OUTPUT = 1
} gpio_direction_t;

typedef enum {
    GPIO_PIN_LOW = 0,
    GPIO_PIN_HIGH = 1
} gpio_pin_state_t;

/* Interrupt trigger types */
typedef enum {
    GPIO_INT_NONE = 0,
    GPIO_INT_RISING = 1,
    GPIO_INT_FALLING = 2,
    GPIO_INT_BOTH = 3,
    GPIO_INT_LOW_LEVEL = 4,
    GPIO_INT_HIGH_LEVEL = 5
} gpio_int_type_t;


/* Helper macros */
#define GPIO_REG(base, offset)      (*(volatile uint32_t *)((base) + (offset)))
#define GPIO_PIN_MASK(pin)          (1U << (pin))

/* Function prototypes */

/* Initialization */
void gpio_init(void);
void gpio_enable_bank(gpio_bank_t bank);
uint32_t gpio_get_bank_base(gpio_bank_t bank);

/* Pin configuration */
void gpio_set_direction(gpio_bank_t bank, uint8_t pin, gpio_direction_t dir);
gpio_direction_t gpio_get_direction(gpio_bank_t bank, uint8_t pin);

/* Digital I/O */
void gpio_pin_set(gpio_bank_t bank, uint8_t pin);
void gpio_pin_clear(gpio_bank_t bank, uint8_t pin);
void gpio_pin_toggle(gpio_bank_t bank, uint8_t pin);
void gpio_pin_write(gpio_bank_t bank, uint8_t pin, gpio_pin_state_t state);
gpio_pin_state_t gpio_pin_read(gpio_bank_t bank, uint8_t pin);

/* Port operations (all 32 pins at once) */
void gpio_port_write(gpio_bank_t bank, uint32_t value);
uint32_t gpio_port_read(gpio_bank_t bank);
void gpio_port_set_bits(gpio_bank_t bank, uint32_t mask);
void gpio_port_clear_bits(gpio_bank_t bank, uint32_t mask);

/* Interrupt configuration */
void gpio_enable_interrupt(gpio_bank_t bank, uint8_t pin, gpio_int_type_t type);
void gpio_disable_interrupt(gpio_bank_t bank, uint8_t pin);
uint32_t gpio_get_interrupt_status(gpio_bank_t bank, uint8_t irq_line);
void gpio_clear_interrupt(gpio_bank_t bank, uint8_t irq_line, uint8_t pin);

/* Debouncing */
void gpio_enable_debounce(gpio_bank_t bank, uint8_t pin);
void gpio_disable_debounce(gpio_bank_t bank, uint8_t pin);
void gpio_set_debounce_time(gpio_bank_t bank, uint8_t time_ms);


#endif /* GPIO_H */
