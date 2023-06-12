/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : app_freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "timers.h"
#include "usart.h"
#include "gpio.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
struct Period_t {
  uint32_t LD2_Period;
  uint32_t YELLOW_Period;

} Led = { 1000, 50

};
uint16_t vPeriod = 1000;
uint32_t key_status = 0x0;
uint32_t LedStatus = 0x1;
BaseType_t xHigherPriorityTaskWoken = pdFALSE;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = { .name = "defaultTask",
    .priority = (osPriority_t) osPriorityNormal, .stack_size = 128 * 4 };
/* Definitions for _vRecieverLD2 */
osThreadId_t _vRecieverLD2Handle;
const osThreadAttr_t _vRecieverLD2_attributes = { .name = "_vRecieverLD2",
    .priority = (osPriority_t) osPriorityNormal, .stack_size = 128 * 4 };
/* Definitions for _vReceiverYELLOW */
osThreadId_t _vReceiverYELLOWHandle;
const osThreadAttr_t _vReceiverYELLOW_attributes = { .name = "_vReceiverYELLOW",
    .priority = (osPriority_t) osPriorityNormal, .stack_size = 128 * 4 };
/* Definitions for myQueue01 */
osMessageQueueId_t myQueue01Handle;
const osMessageQueueAttr_t myQueue01_attributes = { .name = "myQueue01" };
/* Definitions for myTimer01 */
osTimerId_t myTimer01Handle;
const osTimerAttr_t myTimer01_attributes = { .name = "myTimer01" };

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
extern void vPrintStringAndNumber(uint32_t value);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void vRecieverLD2_Func(void *argument);
void _vReceiverYELLOW_Func(void *argument);
void Callback01(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);
void vApplicationMallocFailedHook(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void) {

}

__weak unsigned long getRunTimeCounterValue(void) {
  return xTaskGetTickCount();
}

/* USER CODE END 1 */

/* USER CODE BEGIN 5 */
void vApplicationMallocFailedHook(void) {
  /* vApplicationMallocFailedHook() will only be called if
   configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h. It is a hook
   function that will get called if a call to pvPortMalloc() fails.
   pvPortMalloc() is called internally by the kernel whenever a task, queue,
   timer or semaphore is created. It is also called by various parts of the
   demo application. If heap_1.c or heap_2.c are used, then the size of the
   heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
   FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
   to query the size of free heap space that remains (although it does not
   provide information on how the remaining heap might be fragmented). */
}
/* USER CODE END 5 */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* creation of myTimer01 */
  myTimer01Handle = osTimerNew(Callback01, osTimerPeriodic, NULL,
      &myTimer01_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of myQueue01 */
  myQueue01Handle = osMessageQueueNew(10, sizeof(uint16_t),
      &myQueue01_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL,
      &defaultTask_attributes);

  /* creation of _vRecieverLD2 */
  _vRecieverLD2Handle = osThreadNew(vRecieverLD2_Func, NULL,
      &_vRecieverLD2_attributes);

  /* creation of _vReceiverYELLOW */
  _vReceiverYELLOWHandle = osThreadNew(_vReceiverYELLOW_Func, NULL,
      &_vReceiverYELLOW_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument) {
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */

  if (xTimerChangePeriod(myTimer01Handle, 4000 / portTICK_PERIOD_MS,
      0) == pdPASS) {
    xTimerStart(myTimer01Handle, 0);
    vPrintStringAndNumber(xTaskGetTickCount() / configTICK_RATE_HZ);
    for (;;) {

      osDelay(1);
    }
  }
  //osThreadYield();
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_vRecieverLD2_Func */
/**
 * @brief Function implementing the _vRecieverLD2 thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_vRecieverLD2_Func */
void vRecieverLD2_Func(void *argument) {
  /* USER CODE BEGIN vRecieverLD2_Func */
  /* Infinite loop */
  for (;;) {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    osDelay(Led.LD2_Period);
  }
  /* USER CODE END vRecieverLD2_Func */
}

/* USER CODE BEGIN Header__vReceiverYELLOW_Func */
/**
 * @brief Function implementing the _vReceiverYELLO thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header__vReceiverYELLOW_Func */
void _vReceiverYELLOW_Func(void *argument) {
  /* USER CODE BEGIN _vReceiverYELLOW_Func */
  /* Infinite loop */
  for (;;) {
    HAL_GPIO_TogglePin(Yellow_GPIO_Port, Yellow_Pin);
    osDelay(Led.YELLOW_Period);
  }
  /* USER CODE END _vReceiverYELLOW_Func */
}

/* Callback01 function */
void Callback01(void *argument) {
  /* USER CODE BEGIN Callback01 */

  Led.YELLOW_Period = 1000;
  Led.LD2_Period = 50;
  IWDG->KR = 0xAAAA;
  if (xTimerIsTimerActive(myTimer01Handle)) {
    xTimerChangePeriodFromISR(myTimer01Handle, pdMS_TO_TICKS( 500 ),
        &xHigherPriorityTaskWoken);
  }
  vPrintStringAndNumber(xTimerGetPeriod(myTimer01Handle));
  char *statistic;
  statistic = pvPortMalloc(1024);
  if (statistic) {
    vTaskGetRunTimeStats(statistic);
    // HAL_UART_Transmit(&huart3, (uint8_t*) statistic, strlen(statistic),
//    HAL_MAX_DELAY
//    );
    vPortFree(statistic);
    osThreadYield();
  }

  /* USER CODE END Callback01 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

