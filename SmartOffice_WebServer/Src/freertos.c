/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "stdio.h"
#include "GUI.h"
#include "httpserver-socket.h" //包含webserver头文件
#include "string.h"
#include "protocol.h"
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

uint8_t ZigbeeBuff[40];
osMessageQId ZigbeeQueueHandle;

/* USER CODE END Variables */
osThreadId TouchTaskHandle;
osThreadId GuiTaskHandle;
osThreadId WebServerHandle;
osThreadId ZigbeeTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */


extern void MainTask(void);
   
/* USER CODE END FunctionPrototypes */

void Touch_Task(void const * argument);
void Gui_Task(void const * argument);
void Web_Server(void const * argument);
void Zigbee_Task(void const * argument);

extern void MX_FATFS_Init(void);
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
	osMessageQDef(ZigbeeQueue, 40, uint8_t);
  ZigbeeQueueHandle = osMessageCreate(osMessageQ(ZigbeeQueue), NULL);
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of TouchTask */
  osThreadDef(TouchTask, Touch_Task, osPriorityAboveNormal, 0, 1024);
  TouchTaskHandle = osThreadCreate(osThread(TouchTask), NULL);

  /* definition and creation of GuiTask */
  osThreadDef(GuiTask, Gui_Task, osPriorityBelowNormal, 0, 2048);
  GuiTaskHandle = osThreadCreate(osThread(GuiTask), NULL);
	
  /* definition and creation of WebServer */
	//websever任务分配的栈空间由4096变成2048 = 8KB
	//原因 我在读取网页文件时，需要动态分配内存空间，不够用了，在这边提取一点
  osThreadDef(WebServer, Web_Server, osPriorityHigh, 0, 2048);
  WebServerHandle = osThreadCreate(osThread(WebServer), NULL);

  /* definition and creation of ZigbeeTask */
  osThreadDef(ZigbeeTask, Zigbee_Task, osPriorityNormal, 0, 128);
  ZigbeeTaskHandle = osThreadCreate(osThread(ZigbeeTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_Touch_Task */
/**
  * @brief  Function implementing the TouchTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_Touch_Task */
void Touch_Task(void const * argument)
{
    
  //原有的Fatfs和lwip初始化工作移植到webserver中               

  /* USER CODE BEGIN Touch_Task */
	printf("system is runing!\r\n");
  /* Infinite loop */
  for(;;)
  {
		//ִ触摸屏需要轮询检测，不然emWin没有办法触发事件
		GUI_TOUCH_Exec();	
		osDelay(20);
  }
  /* USER CODE END Touch_Task */
}

/* USER CODE BEGIN Header_Gui_Task */
/**
* @brief Function implementing the GuiTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Gui_Task */
void Gui_Task(void const * argument)
{
  /* USER CODE BEGIN Gui_Task */

  /* Infinite loop */
	MainTask();
  /* USER CODE END Gui_Task */
}

/* USER CODE BEGIN Header_Web_Server */
/**
* @brief Function implementing the WebServer thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Web_Server */
void Web_Server(void const * argument)
{
  /* USER CODE BEGIN Web_Server */
	//webserver中用到了socket和fatfs，在webserver任务运行前，需要完成前两项内容初始化工作
  /* init code for FATFS */
  MX_FATFS_Init();

  /* init code for LWIP */
  MX_LWIP_Init();
  /* Infinite loop */
  for(;;)
  {
		//Webserver主线程任务
    http_server_socket_thread(NULL);
  }
  /* USER CODE END Web_Server */
}

/* USER CODE BEGIN Header_Zigbee_Task */
/**
* @brief Function implementing the ZigbeeTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Zigbee_Task */
void Zigbee_Task(void const * argument)
{
  /* USER CODE BEGIN Zigbee_Task */
	uint8_t u8Index;
  /* Infinite loop */
  for(;;)
  {
	  //每次读取消息之前，把索引初始化为0
	  u8Index = 0;
	  //1、一直等待接收消息,第一个消息应该放在消息缓冲区的第一个元素上
		if(xQueueReceive(ZigbeeQueueHandle,&ZigbeeBuff[u8Index++],portMAX_DELAY)==pdPASS){
			while(xQueueReceive(ZigbeeQueueHandle,&ZigbeeBuff[u8Index++],50)){}
			//完成解析以后，要清空接收缓冲区，不然会出现问题
			  ProtocolParse(ZigbeeBuff,u8Index-1);
		    memset(ZigbeeBuff,0,40);
				
		
		}
  }
  /* USER CODE END Zigbee_Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
