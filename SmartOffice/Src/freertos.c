/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#include "usart.h"
#include "delay.h"
#include "lcd.h"
#include "touch.h"
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
//lcd
extern uint8_t lcd_id[12]; //main.c
/* USER CODE END Variables */
osThreadId WebServerHandle;
osThreadId TouchHandle;
osThreadId IOTHandle;
osThreadId GUIHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
   
/* USER CODE END FunctionPrototypes */

void WebServer_Task(void const * argument);
void Touch_Task(void const * argument);
void IOT_Task(void const * argument);
void GUI_Task(void const * argument);

extern void MX_LWIP_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];
  
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}                   
/* USER CODE END GET_IDLE_TASK_MEMORY */

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

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of WebServer */
  osThreadDef(WebServer, WebServer_Task, osPriorityHigh, 0, 4096);
  WebServerHandle = osThreadCreate(osThread(WebServer), NULL);

  /* definition and creation of Touch */
  osThreadDef(Touch, Touch_Task, osPriorityAboveNormal, 0, 1024);
  TouchHandle = osThreadCreate(osThread(Touch), NULL);

  /* definition and creation of IOT */
  osThreadDef(IOT, IOT_Task, osPriorityNormal, 0, 256);
  IOTHandle = osThreadCreate(osThread(IOT), NULL);

  /* definition and creation of GUI */
  osThreadDef(GUI, GUI_Task, osPriorityBelowNormal, 0, 2048);
  GUIHandle = osThreadCreate(osThread(GUI), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_WebServer_Task */
/**
  * @brief  Function implementing the WebServer thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_WebServer_Task */
void WebServer_Task(void const * argument)
{
    
                 
  /* init code for LWIP */
  MX_LWIP_Init();

  /* USER CODE BEGIN WebServer_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END WebServer_Task */
}

/* USER CODE BEGIN Header_Touch_Task */
/**
* @brief Function implementing the Touch thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Touch_Task */
void Touch_Task(void const * argument)
{
  /* USER CODE BEGIN Touch_Task */
	//lcd init
	delay_init(168);                    // ��ʱ��ʼ�� 
	lcd_init();                             // ��ʼ��LCD 
  sprintf((char *)lcd_id, "LCD ID:%04X", lcddev.id);  // ��LCD ID��ӡ��lcd_id����
  
	//screen touch init
	tp_dev.init();                      // ��������ʼ��
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Touch_Task */
}

/* USER CODE BEGIN Header_IOT_Task */
/**
* @brief Function implementing the IOT thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_IOT_Task */
void IOT_Task(void const * argument)
{
  /* USER CODE BEGIN IOT_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END IOT_Task */
}

/* USER CODE BEGIN Header_GUI_Task */
/**
* @brief Function implementing the GUI thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_GUI_Task */
void GUI_Task(void const * argument)
{
  /* USER CODE BEGIN GUI_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END GUI_Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
