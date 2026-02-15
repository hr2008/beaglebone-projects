#include "gpio.h"
#include "led.h"
#include "dmtimer.h"


/**
 * BBB LED Driver Implementation
 */




/* ========================================================================
 * BeagleBone Black User LED Functions
 * ======================================================================== */

/**
 * Initialize user LEDs on BeagleBone Black
 */
void user_led_init(void)
{
    /* Enable GPIO1 clock */
    gpio_enable_bank(GPIO_BANK_1);
    
    /* Configure all 4 user LEDs as outputs */
    gpio_set_direction(USER_LED0_BANK, USER_LED0_PIN, GPIO_DIR_OUTPUT);
    gpio_set_direction(USER_LED1_BANK, USER_LED1_PIN, GPIO_DIR_OUTPUT);
    gpio_set_direction(USER_LED2_BANK, USER_LED2_PIN, GPIO_DIR_OUTPUT);
    gpio_set_direction(USER_LED3_BANK, USER_LED3_PIN, GPIO_DIR_OUTPUT);
    
    /* Turn all LEDs off initially */
    user_led_off(0);
    user_led_off(1);
    user_led_off(2);
    user_led_off(3);
}
/**
 * Turn user LED on
 */
void user_led_on(uint8_t led_num)
{
    switch (led_num) {
        case 0:
            gpio_pin_set(USER_LED0_BANK, USER_LED0_PIN);
            break;
        case 1:
            gpio_pin_set(USER_LED1_BANK, USER_LED1_PIN);
            break;
        case 2:
            gpio_pin_set(USER_LED2_BANK, USER_LED2_PIN);
            break;
        case 3:
            gpio_pin_set(USER_LED3_BANK, USER_LED3_PIN);
            break;
        default:
            break;
    }
}

/**
 * Turn user LED off
 */
void user_led_off(uint8_t led_num)
{
    switch (led_num) {
        case 0:
            gpio_pin_clear(USER_LED0_BANK, USER_LED0_PIN);
            break;
        case 1:
            gpio_pin_clear(USER_LED1_BANK, USER_LED1_PIN);
            break;
        case 2:
            gpio_pin_clear(USER_LED2_BANK, USER_LED2_PIN);
            break;
        case 3:
            gpio_pin_clear(USER_LED3_BANK, USER_LED3_PIN);
            break;
        default:
            break;
    }
}

/**
 * Toggle user LED
 */
void user_led_toggle(uint8_t led_num)
{
    switch (led_num) {
        case 0:
            gpio_pin_toggle(USER_LED0_BANK, USER_LED0_PIN);
            break;
        case 1:
            gpio_pin_toggle(USER_LED1_BANK, USER_LED1_PIN);
            break;
        case 2:
            gpio_pin_toggle(USER_LED2_BANK, USER_LED2_PIN);
            break;
        case 3:
            gpio_pin_toggle(USER_LED3_BANK, USER_LED3_PIN);
            break;
        default:
            break;
    }
}

/**
 * Set user LED state
 */
void user_led_set(uint8_t led_num, gpio_pin_state_t state)
{
    if (state == GPIO_PIN_HIGH) {
        user_led_on(led_num);
    } else {
        user_led_off(led_num);
    }
}
/**
 * below are helper fucntions to directly control each LED without needing to specify the LED number
 * these can be used for testing or simple tracing execution flow of applications
 */
void toggle_leds(void)
{
    int i = 0;
    while(1)
    {
        for (i = 0; i < 4; i++)
        user_led_on(i);
        dmtimer_delay_ms(DMTIMER2_BASE, 1000);  
        for (i = 0; i < 4; i++)
        user_led_off(i);
        dmtimer_delay_ms(DMTIMER2_BASE, 1000);
    }
}
void user_led_on_0()
{
    gpio_pin_set(USER_LED0_BANK, USER_LED0_PIN);
}
void user_led_on_1()
{
    gpio_pin_set(USER_LED1_BANK, USER_LED1_PIN);
}
void user_led_on_2()
{
    gpio_pin_set(USER_LED2_BANK, USER_LED2_PIN);
}
void user_led_on_3()
{
    gpio_pin_set(USER_LED3_BANK, USER_LED3_PIN);
}           
void user_led_off_0()
{
    gpio_pin_clear(USER_LED0_BANK, USER_LED0_PIN);
}
void user_led_off_1()
{   
    gpio_pin_clear(USER_LED1_BANK, USER_LED1_PIN);
}
void user_led_off_2()
{
    gpio_pin_clear(USER_LED2_BANK, USER_LED2_PIN);
}
void user_led_off_3()
{
    gpio_pin_clear(USER_LED3_BANK, USER_LED3_PIN);
}

void user_led_toggle_0()
{
    while(1)
    {
        gpio_pin_toggle(USER_LED0_BANK, USER_LED0_PIN);
        delay_ms(2000);
    }
}
void user_led_toggle_1()
{
    while(1)
    {
        gpio_pin_toggle(USER_LED1_BANK, USER_LED1_PIN);
        delay_ms(2000);
    }
}
void user_led_toggle_2()
{
    while(1)
    {
        gpio_pin_toggle(USER_LED2_BANK, USER_LED2_PIN);
        delay_ms(2000);
    }
}
void user_led_toggle_3()
{
    while(1)
    {
        gpio_pin_toggle(USER_LED3_BANK, USER_LED3_PIN);
        delay_ms(2000);
    }
}

