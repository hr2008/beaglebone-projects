#include <stdint.h>
#include "debug_uart.h"
#include "gpio.h"
#include "led.h"


void printf(const char *fmt, ...);

static unsigned delayVal = 1000000;

static void delay(volatile unsigned count) {
    while (count--) ;
}

int main(void)
{
    uart_init();
     /* Initialize GPIO and user LEDs */
    gpio_init();
    user_led_init();

    while (1) {
        printf("Hello world - Toggling leds\n\r");
        /* Brief LED test - blink all LEDs once */
        for (uint8_t i = 0; i < 4; i++) {
            user_led_on(i);
        }
        
        delay(delayVal);

        for (uint8_t i = 0; i < 4; i++) {
        user_led_off(i);
        }
    
        delay(delayVal);
    }
}

void default_handler()
{
    for (uint8_t i = 0; i < 4; i +=2) {
        user_led_on(i);
    }

    for (uint8_t i = 1; i < 4; i +=2) {
        user_led_on(i);
    }

}
