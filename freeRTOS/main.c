/**
 * Main Application for BeagleBone Black with FreeRTOS
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "printf.h"

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "freeRTOS_tasks.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

/* Hardware driver includes */
#include "dmtimer.h"
#include "gpio.h"
#include "intc.h"
#include "led.h"

/* ============================================================================
 * Hardware Initialization Functions
 * ========================================================================== */

/**
 * Initialize all hardware peripherals
 */
static void hardware_init(void) {
  /* Initialize serial & interrupt controller */
  uart_init();

  intc_init();

  /* Initialize GPIO and user LEDs */
  gpio_init();
  user_led_init();

  /* Turn off all LEDs initially */
  for (uint8_t i = 0; i < 4; i++) {
    user_led_off(i);
  }

  /* Initialize DMTimer2 for FreeRTOS tick (1ms) */
  dmtimer2_init_for_freertos(configTICK_RATE_HZ);

  /* Set timer interrupt priority to kernel priority (63 = lowest) */
  intc_set_priority(IRQ_TINT2, configKERNEL_INTERRUPT_PRIORITY);

  /* Enable DMTimer2 interrupt in INTC */
  intc_enable_interrupt(IRQ_TINT2);
}



/**
 * Main function - Entry point after startup code
 */
int main(void) {
  /* Initialize all hardware */
  hardware_init();

  dmtimer_delay_ms(DMTIMER2_BASE, 2000); /* Short delay to stabilize */
  /* Print startup banner */
  printf("\r\n");
  printf("======================================\r\n");
  printf("  BeagleBone Black - FreeRTOS Demo  \r\n");
  printf("======================================\r\n");
  printf("Initializing FreeRTOS...\r\n");

  /* Brief LED test - blink all LEDs once */ 
  for (uint8_t i = 0; i < 4; i++) {
    user_led_on(i);
  }
  for (volatile uint32_t i = 0; i < 1000000; i++); /* Short delay */
  for (uint8_t i = 0; i < 4; i++) {
    user_led_off(i);
  }
  for (volatile uint32_t i = 0; i < 1000000; i++); /* Short delay */
  /* Create FreeRTOS tasks */
  create_tasks();

  printf("Tasks created successfully!\r\n");
  printf("Starting scheduler...\r\n");
  printf("\r\n");

  /* Don't enable IRQs here - the scheduler will do it when it starts */
  /* The first task will run with interrupts enabled automatically */

  /* Start the FreeRTOS scheduler - this should never return */

  vTaskStartScheduler();

  /* If we reach here, there was insufficient heap memory */

  printf("ERROR: Failed to start scheduler!\r\n");

  configSETUP_TICK_INTERRUPT();

  /* Error state - blink all LEDs */
  while (1) {
    for (uint8_t i = 0; i < 3; i++) {
      user_led_toggle(i);
    }
    printf("Scheduler failed to start!\r\n");
    for (volatile uint32_t i = 0; i < 1000000; i++);
  }

  return 0; /* Never reached */
}