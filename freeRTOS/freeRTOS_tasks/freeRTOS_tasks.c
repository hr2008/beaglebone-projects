/**
 * freeRTOS_tasks.c - Example FreeRTOS application with multiple tasks
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>



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

/* External UART function (if available) */
extern void uart_init(void);
extern void uart_putc(char c);

/* Function prototypes */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName);
void vApplicationMallocFailedHook(void);

/* Task function prototypes */
void vTaskHeartbeat(void *pvParameters);
void vTaskLEDSequence(void *pvParameters);
void vTaskStatus(void *pvParameters);


/* ============================================================================
 * FreeRTOS Hook Functions
 * ========================================================================== */

/**
 * Stack overflow hook - called when a task overflows its stack
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
  (void)xTask;
  (void)pcTaskName;

  /* Disable interrupts */
  taskDISABLE_INTERRUPTS();

  /* Blink all LEDs rapidly to indicate error */
  while (1) {
    for (uint8_t i = 0; i < 4; i++) {
      user_led_toggle(i);
    }
    for (volatile uint32_t i = 0; i < 100000; i++);
  }
}

/**
 * Malloc failed hook - called when heap allocation fails
 */
void vApplicationMallocFailedHook(void) {
  /* Disable interrupts */
  taskDISABLE_INTERRUPTS();

  user_led_toggle_0();

  /* Turn on all LEDs to indicate error */
  user_led_on(0);
  user_led_on(1);
  user_led_on(2);
  user_led_on(3);

  while (1);
}

/* ============================================================================
 * FreeRTOS Task Implementations
 * ========================================================================== */

/**
 * Task 1: Heartbeat - Blinks LED0 with heartbeat pattern
 * Priority: 1 (Low)
 * Stack: 256 words (1KB)
 */
void vTaskHeartbeat(void *pvParameters) {
  (void)pvParameters;
  printf("HB entry\r\n");
  int beat_count = 0;

  while (1) {
    /* First beat */
    user_led_on(0);
    printf("vTaskHeartbeat: on %d\r\n", ++beat_count);
    vTaskDelay(pdMS_TO_TICKS(1000));
    
    /* Second beat */
    user_led_off(0);
    printf("vTaskHeartbeat:off %d\r\n", ++beat_count);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

/**
 * Task 2: LED Sequence - Cycles through LEDs 1-3
 * Priority: 1 (Low)
 * Stack: 256 words (1KB)
 */
void vTaskLEDSequence(void *pvParameters) {
  (void)pvParameters;

  uint8_t current_led = 1; /* Start with LED1 */
  TickType_t xLastWakeTime = xTaskGetTickCount();

  while (1) {
    /* Turn on current LED */
    user_led_on(current_led);
  
    /* Wait */
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1000));

    /* Turn off current LED */
    user_led_off(current_led);

    /* Move to next LED (1, 2, 3, then back to 1) */
    current_led++;
    if (current_led > 3) {
      current_led = 1;
    }
  }
}

/**
 * Task 3: Status - Prints system status periodically
 * Priority: 2 (Medium)
 * Stack: 384 words (1.5KB)
 */
void vTaskStatus(void *pvParameters) {
  (void)pvParameters;

  TickType_t xLastWakeTime = xTaskGetTickCount();
  uint32_t loop_count = 0;

  while (1) {
    /* Print status every 5 seconds */
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(5000));

    loop_count++;

    /* Print system information if UART is available */
    printf("\r\n\r\n==== System Status ===\r\n");
    printf("FreeRTOS is running!\r\n");
    printf("Loop count: %ld", loop_count);
    /* Note: Add uart_print_number function to print loop_count */
    printf("\r\n\r\n");

  }
}

/**
 * Example: Using queues to communicate between tasks
 */
QueueHandle_t led_queue;

void vTaskLEDProducer(void *pvParameters)
{ 
    (void)pvParameters;
    uint8_t led_num = 1;
    printf("LED Producer started\r\n");
    
     /* Create a queue to send LED numbers to the consumer */
    while (1) {
        xQueueSend(led_queue, &led_num, portMAX_DELAY);
        led_num = (led_num + 1) % 4;
        if(led_num == 0) led_num = 1; /* Skip LED0 which is used by heartbeat */
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}

void vTaskLEDConsumer(void *pvParameters)
{ 
    (void)pvParameters;
    uint8_t led_num;
    printf("LED Consumer started\r\n");
    while (1) {
        if (xQueueReceive(led_queue, &led_num, portMAX_DELAY) == pdTRUE) {
            user_led_toggle(led_num);
        }
    }
}


/**
 * Start the timer tick for FreeRTOS
 * Called by xPortStartScheduler() via configSETUP_TICK_INTERRUPT()
 *
 * Note: This is called BEFORE the first task runs, so interrupts
 * are still disabled at the CPU level when this executes.
 * The scheduler will enable interrupts when starting the first task.
 */
void vApplicationSetupTimerInterrupt(void) {
  /* Timer is already initialized and interrupt enabled in INTC */
  /* Just start the timer - it will begin generating interrupts */
  /* but CPU IRQs are still masked, so no interrupts fire yet */
  dmtimer_start(DMTIMER2_BASE);

  /* When the scheduler calls vPortRestoreTaskContext(), */
  /* it will restore CPSR with interrupts enabled, and */
  /* timer interrupts will start firing */
}

/* ============================================================================
 * Main Application Entry Point
 * ========================================================================== */

void vApplicationTickHook() {
  { 
    //user_led_toggle(3); 
  }
}

void vApplicationIdleHook() { printf("Idle Hook called\r\n"); }
/**
 * Main function - Entry point after startup code
 */
int create_tasks(void) {

  /* Create FreeRTOS tasks */
  BaseType_t result;

  printf("creating FreeRTOS apps...\r\n");
  TaskHandle_t pxCreatedTask;

  /* Task 1: Heartbeat on LED0 */
  result = xTaskCreate(vTaskHeartbeat, /* Task function */
                       "Heartbeat",    /* Task name (for debugging) */
                       256,            /* Stack size (words) */
                       NULL,           /* Task parameters */
                       2,              /* Priority */
                       &pxCreatedTask  /* Task handle */
  );

  if (result != pdPASS) {
    /* Failed to create task - blink LED0 rapidly */
    while (1) {
      user_led_toggle(0);
      for (volatile uint32_t i = 0; i < 500000; i++);
    }
  }

  /* Task 2: LED sequence on LEDs 1-3 */
  result = xTaskCreate(vTaskLEDProducer, "TaskLEDProducer", 256, NULL, 2, NULL);

  if (result != pdPASS) {
    printf("Failed to create LED Producer task!\r\n");
    while (1) {
      user_led_toggle(1);
      for (volatile uint32_t i = 0; i < 500000; i++);
    }
  }

    /* Task 3: LED sequence on LEDs 1-3 */
  result = xTaskCreate(vTaskLEDConsumer, "TaskLEDConsumer", 256, NULL, 1, NULL);

  if (result != pdPASS) {
    printf("Failed to create LED Consumer task!\r\n");
    while (1) {
      user_led_toggle(1);
      for (volatile uint32_t i = 0; i < 500000; i++);
    }
  }


  /* Task 4: Status output */
  result =
      xTaskCreate(vTaskStatus, "Status", 384, /* Larger stack for printing */
                  NULL, 2,                    /* Higher priority */
                  NULL);

  if (result != pdPASS) {
    while (1) {
      user_led_toggle(2);
      for (volatile uint32_t i = 0; i < 500000; i++);
    }
  }

  /* CREATE THE QUEUE*/
  led_queue = xQueueCreate(
      5,              // Can hold 5 items
      sizeof(uint8_t) // Each item is 1 byte
  );

  if (led_queue == NULL) {
      uart_puts("ERROR: Queue creation failed!\r\n");
      while (1);  // Halt - not enough heap
  }

  
  return 0; /* Never reached */
}

/* ============================================================================
 * Additional Example Tasks (Commented Out)
 * ========================================================================== */

#if 0 /* Uncomment to use these tasks instead */

/**
 * Example: Knight Rider effect on LEDs
 */
void vTaskKnightRider(void *pvParameters)
{ DEBUG_LINE();
    (void)pvParameters;
    
    int8_t current_led = 0;
    int8_t direction = 1;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    while (1) {
        user_led_on(current_led);
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));
        user_led_off(current_led);
        
        current_led += direction;
        
        if (current_led >= 3) {
            current_led = 3;
            direction = -1;
        } else if (current_led <= 0) {
            current_led = 0;
            direction = 1;
        }
    }
}

/**
 * Example: Binary counter on LEDs
 */
void vTaskBinaryCounter(void *pvParameters)
{ DEBUG_LINE();
    (void)pvParameters;
    
    uint8_t count = 0;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    while (1) {
        user_led_set(0, (count & 0x01) ? GPIO_PIN_HIGH : GPIO_PIN_LOW);
        user_led_set(1, (count & 0x02) ? GPIO_PIN_HIGH : GPIO_PIN_LOW);
        user_led_set(2, (count & 0x04) ? GPIO_PIN_HIGH : GPIO_PIN_LOW);
        user_led_set(3, (count & 0x08) ? GPIO_PIN_HIGH : GPIO_PIN_LOW);
        
        count = (count + 1) & 0x0F;
        
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(500));
    }
}

#endif /* End of commented example tasks */