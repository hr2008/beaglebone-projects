#ifndef FREERTOS_TASKS_H
#define FREERTOS_TASKS_H 

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

/* Hardware driver & service includes */
#include "led.h"
#include "printf.h"

/* Task function prototypes */
void vTaskHeartbeat(void *pvParameters);
void vTaskLEDSequence(void *pvParameters);
void vTaskStatus(void *pvParameters);

/* FreeRTOS Hook Functions */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName);
void vApplicationMallocFailedHook(void);
void vApplicationTickHook();
void vApplicationIdleHook();

/* Task creation function */
int create_tasks(void);

 #endif /* FREERTOS_TASKS_H */
