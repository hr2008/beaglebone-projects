/**
 * AM335x GPIO Driver
 * 
 * The AM335x has 4 GPIO banks (GPIO0-GPIO3), each with 32 pins.
 * Total: 128 GPIO pins
 * 
 * BeagleBone Black User LEDs:
 * - USR0: GPIO1_21 (P8_7)
 * - USR1: GPIO1_22 (P8_8)
 * - USR2: GPIO1_23 (P8_9)
 * - USR3: GPIO1_24 (P8_10)
 */

#ifndef LED_H
#define LED_H

#include <stdint.h>
#include <stdbool.h>
#include "gpio.h"

/* BeagleBone Black User LED definitions */
#define USER_LED0_BANK      GPIO_BANK_1
#define USER_LED0_PIN       21

#define USER_LED1_BANK      GPIO_BANK_1
#define USER_LED1_PIN       22

#define USER_LED2_BANK      GPIO_BANK_1
#define USER_LED2_PIN       23

#define USER_LED3_BANK      GPIO_BANK_1
#define USER_LED3_PIN       24




/* User LED convenience functions for BeagleBone Black */
void user_led_init(void);
void user_led_on(uint8_t led_num);
void user_led_off(uint8_t led_num);
void user_led_toggle(uint8_t led_num);
void user_led_set(uint8_t led_num, gpio_pin_state_t state);
void user_led_on_0();
void user_led_on_1();
void user_led_on_2();
void user_led_on_3();    
void user_led_toggle_0();
void user_led_toggle_1();
void user_led_toggle_2();
void user_led_toggle_3();

#endif /* LED_H */ 