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
#include "delay.h"
#include "lcd.h"
#include "stdio.h"  //sprintf

#include "touch.h"  //tp_dev
#include "norflash.h"  //norflash_init

#include "dht11.h"  //dht11_init、dht11_read_data

#include "lsens.h"  //lsens_init

#include "GUI.h"  //GUI_TOUCH_Exec

#include "diskio.h"

#include "fatfs.h"  //SDFatFS、USERFatFS
#include "fonts.h"  //fonts_update_font
#include "mymalloc.h"  //mymalloc
#include "icon.h"

#include "icon_read.h"  //read_icons

#include "esp8266.h"
#include <string.h>
#include "esp8266_web.h"

#include "ip4_addr.h"  //ip4_addr_t
#include "lwip/netif.h"  //struct netif
#include "ip4_addr.h"  //ip4_addr_isvalid

//#include "init.h"
//#include "tcp.h"
//#include "dhcp.h"
//#include "api.h"
//#include "httpd.h"

#include "eth_web.h"
#include "web.h"  //webs_init
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
// 手动声明 ip4_addr_isvalid 函数
int ip4_addr_isvalid(const ip4_addr_t *addr);
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
//lcd
extern uint8_t lcd_id[12]; //main.c
//extern uint16_t colors[];  //main.c
//extern uint8_t color_index;  //main.c

uint16_t id;  //norflash id

uint8_t t = 0; 
uint16_t temperature; 
uint16_t humidity;

uint16_t adcx;

unsigned long recv = 0;

//TaskHandle_t xMountDisksTaskHandle;
// 创建二值信号量句柄 
SemaphoreHandle_t xBinarySemaphoreCheckFontsAndIconBin;
//更新字库相关
extern uint8_t fonts_update_res;

// 创建二值信号量句柄 
SemaphoreHandle_t xBinarySemaphoreFont;

// 创建二值信号量句柄
SemaphoreHandle_t xBinarySemaphoreICON;

//esp8266 uart3
//uint8_t uart3_rx_buffer[RX_BUFFER_SIZE];
volatile uint16_t uart3_rx_index = 0;
uint8_t Uart3FramFinishFlag = 0;

// 创建互斥信号量句柄
SemaphoreHandle_t xMutexEsp8266;

extern struct netif gnetif;  //lwip.c

extern uint8_t* paddr;
extern uint16_t memused;                     /* 内存使用百分比 */
/* USER CODE END Variables */
osThreadId WebServerHandle;
osThreadId TouchHandle;
osThreadId IOTHandle;
osThreadId GUIHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
extern void MainTask(void); 

void init_disks(){
	while(disk_initialize(0)){  //SD卡初始化
		//printf("SD Card Error!\n");
		delay_ms(500);
		//printf("Please Check!\n");
		delay_ms(500);
	}
	//printf("SD Card init OK!\n");
	disk_ioctl(0, 1, (void*)&recv);
	//printf("SD sector count: %d\n", (int)recv);
	while(disk_initialize(1)){  //NORFlash初始化
			//printf("Noflash Error!\n");
			delay_ms(500);
			//printf("Please Check!\n");
			delay_ms(500);
	}
	//printf("Noflash init OK!\n");
	disk_ioctl(1, 1, (void*)&recv);
	//printf("noflash sector count: %d\n", (int)recv);
}

void fmout_sd(uint8_t opt){
	//uint8_t work_buff[512] = {0};  //缓冲区
	uint8_t * work_buff = (uint8_t *)mymalloc(2, 512);
	uint8_t res = 0;
	res = f_mount(SDFatFS, "0:", opt);        // 挂载SD卡
	printf("f_mount sd res:%u\n", res);
	if(FR_OK == res){
		printf("SD Disk Mount Successed!\n");     //SD卡成功挂载
	}
	if (res == 0X0D) {               // SD卡挂载失败 文件系统错误
			//printf("sd fs error!\n");
			//printf("SD Disk Formatting...\n");
			res = f_mkfs("0:", 0, 0, work_buff, _MAX_SS);                                            /* 格式化SD,0:,盘符;0,使用默认格式化参数 */

			if (res == 0){
					f_setlabel((const TCHAR *)"0:ALIENTEK_SD");                                    /* 设置SD磁盘的名字为：ALIENTEK_SD */
					//printf("SD Disk Format Finish\n");     // 格式化成功
			}	else	{
					//printf("SD Disk Format Error\n");     // 格式化失败
			}
	}
	myfree(2, work_buff);
}

void fmout_norflash(uint8_t opt){
	uint8_t * work_buff = (uint8_t *)mymalloc(2, 512);
	uint8_t res = 0;
	res = f_mount(USERFatFS, "1:", opt);  // 挂载NORFlash
	printf("f_mount flash res:%u\n", res);
	if(FR_OK == res){
		printf("Flash Disk Mount Successed!\n");     // 挂载NORFlash成功
	}
	if (res == 0X0D) {                // NORFlash文件系统损坏
			//printf("flash fs error!\n");
			//printf("Flash Disk Formatting...\n");
			res = f_mkfs("1:", 0, 0, work_buff, _MAX_SS);                                            /* 格式化FLASH,1:,盘符;1,使用默认格式化参数 */

			if (res == 0)	{
					f_setlabel((const TCHAR *)"1:ALIENTEK_FLASH");                                    /* 设置Flash磁盘的名字为：ALIENTEK_FLASH */
					//printf("Flash Disk Format Finish\n");     /* 格式化完成 */
			}	else {
					//printf("Flash Disk Format Error \n");     /* 格式化失败 */
			}
	}
	myfree(2, work_buff);
}

//void fmout_disks(void *pvParameters){
void fmout_disks(uint8_t opt){
	fmout_sd(0);
	fmout_norflash(1);
}

// 定义一个任务来获取 IP 地址
void vGetIPTask(void *pvParameters) {
    struct netif *netif;
    ip4_addr_t ip_addr;

    // 等待网络接口准备就绪
    while (1) {
        netif = &gnetif; // 获取网络接口指针
        if (netif != NULL && netif_is_up(netif)) {
            // 获取 IP 地址
            ip_addr = netif->ip_addr;
			//使用 ip4addr_ntoa 函数，它将 ip4_addr_t 结构体转换为字符串形式的 IP 地址，如果转换成功则表示 IP 地址是有效的
            if (ip4addr_ntoa(&ip_addr)) { //验证 IP 地址是否有效
				// 打印 IP 地址(成功获取到有线ip地址)
				//printf("IP Address: %s\n", ip4addr_ntoa(&ip_addr));
				strcpy(ETH_ip_address, ip4addr_ntoa(&ip_addr));
				printf("ETH ip address:%s\n", (char*)ETH_ip_address);
				break;
            }
        }

        // 延迟一段时间后再次检测
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // 任务完成，删除自己
    vTaskDelete(NULL);
}
/* USER CODE END FunctionPrototypes */

void WebServer_Task(void const * argument);
void Touch_Task(void const * argument);
void IOT_Task(void const * argument);
void GUI_Task(void const * argument);

extern void MX_LWIP_Init(void);
extern void MX_FATFS_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

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
  osThreadDef(WebServer, WebServer_Task, osPriorityHigh, 0, 256);
  WebServerHandle = osThreadCreate(osThread(WebServer), NULL);

  /* definition and creation of Touch */
  osThreadDef(Touch, Touch_Task, osPriorityAboveNormal, 0, 128);
  TouchHandle = osThreadCreate(osThread(Touch), NULL);

  /* definition and creation of IOT */
  osThreadDef(IOT, IOT_Task, osPriorityNormal, 0, 128);
  IOTHandle = osThreadCreate(osThread(IOT), NULL);

  /* definition and creation of GUI */
  osThreadDef(GUI, GUI_Task, osPriorityBelowNormal, 0, 512);
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

  /* init code for FATFS */
  MX_FATFS_Init();

  /* USER CODE BEGIN WebServer_Task */
	taskENTER_CRITICAL();           /* 进入临界段 */
	
	printf("WebServer_Task-------------------->\n");
	
	printf("MX_LWIP_Init!\n");
	printf("MX_FATFS_Init!\n");
	
	// 创建二值信号量 
	xBinarySemaphoreFont = xSemaphoreCreateBinary();
	
	//初始化norflash和SD卡
	init_disks();
	printf("Init NORFlash and SD cards!\n");

	//挂载norflash和SD卡
	fmout_disks(1);
	
	printf("Mout NORFlash and SD cards!\n");

	delay_init(168);                    // 初始化自定义延时函数
	lcd_init();                             // 初始化LCD
	printf("Init lcd screen!\n");
	sprintf((char *)lcd_id, "LCD ID:%04X", lcddev.id);
	
	while (dht11_init())    /* DHT11初始化* */
	{
			printf("DHT11 Error!\n");
			delay_ms(200);
	}
	printf("DHT11 init successed!\n");
	lsens_init();                           /* 初始化光敏传感器 */
	printf("Lighter lsens init down!\n");
	
	// 释放信号量，通知任务2可以执行了 
	xSemaphoreGive(xBinarySemaphoreFont);
	
	//创建互斥信号量
	xMutexEsp8266 = xSemaphoreCreateMutex();
	
	//使能esp8266并开启中断接收
	ESP8266_Enable();  //CH使能
	ESP8266_Reset();  //复位引脚拉高
	printf("Enable ESP8266 and Reset!\n");
	 
	//启用串口1和串口3中断接收
	/*
	printf("usart1 ok\n");
	HAL_UART_Receive_IT(&huart1, uart1_rx_buffer, RX_BUFFER_SIZE);
	*/
	//HAL_UART_Receive_IT(&huart3, uart3_rx_buffer, RX_BUFFER_SIZE);
	HAL_UART_Receive_IT(&huart3, uart3_rx_buffer, MAX_RX_BUFFER_SIZE);
	
	//获取有线网络ip地址
	//创建一个FreeRTOS任务来获取 IP 地址
    xTaskCreate(vGetIPTask, "GetIP", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);

	/*
	// 等待网络接口准备就绪
	ip4_addr_t ip_addr;
    while (1) {
        if (netif_is_up(&gnetif)) {  //检查网络接口是否已经启动并准备获取ip
            // 获取 IP 地址
            ip_addr = gnetif.ip_addr;
			// 打印 IP 地址
			printf("IP Address: %s\n", ip4addr_ntoa(&ip_addr));
			break;
        }

        // 延迟一段时间后再次检测
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
	*/

	/* Initialize HTTP server */
	//httpd_init();
	
	WebServer();  //创建一个 TCP 套接字
	
	memused = my_mem_perused(SRAMEX);
	sprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
	printf("SRAMEX   USED: %s\n", (char *)paddr);
	
	printf("----------------------------------|\n\n");
	taskEXIT_CRITICAL();            /* 出临界段 */
	//vTaskDelete(xMountDisksTaskHandle);
    //xMountDisksTaskHandle = NULL;
  /* Infinite loop */
  for(;;)
  {
	/* Poll the HTTP server to handle incoming requests */
    //httpd_poll();
	Listen_Thread();  
    osDelay(1);
	  
    //测试打开文件
    /*
	if (xSemaphoreTake(xBinarySemaphoreFont, portMAX_DELAY) == pdTRUE) {
		taskENTER_CRITICAL();           // 进入临界段
		FIL *fftemp;
		fftemp = (FIL *)mymalloc(SRAMEX, sizeof(FIL));  // 给文件描述符开辟空间
		if(NULL == fftemp){
			printf("fftemp is NULL\n");
		}
		uint8_t res = 0;  
		res = f_open(fftemp, "1:SmartOfficeWeb/index.html", FA_READ);
		printf("NORFlash f_open return :%d\n", res);
		f_close(fftemp);
		myfree(SRAMEX, fftemp);	
		// 释放信号量
		xSemaphoreGive(xBinarySemaphoreFont);
		taskEXIT_CRITICAL();            // 出临界区 
	}
	osDelay(1000);
	*/
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
	printf("Touch_Task------------------------>\n");
	// 等待任务1完成 
	if (xSemaphoreTake(xBinarySemaphoreFont, portMAX_DELAY) == pdTRUE) {
		taskENTER_CRITICAL();           /* 进入临界段 */

		if(fonts_init()){  //初始化字库
			printf("Init font failed!\n");
		}else{
			printf("Init font successed!\n");
		}

		if(icons_init()){  //初始化图库
			printf("Init icons failed!\n");
		}else{
			printf("Init icons successed!\n");
		}
		
		if(webs_init()){  //初始化图库
			printf("Init web files failed!\n");
		}else{
			printf("Init web files successed!\n");
		}
		//show_webs_info(webstinfo);

		uint8_t res = 0;
		
		//测试NORFlash打开文件

		FIL *fftemp;
		fftemp = (FIL *)mymalloc(SRAMCCM, sizeof(FIL));  // 给文件描述符开辟空间

		//res = f_open(fftemp, "1:AlarmOn.bin", FA_READ);
		//printf("NORFlash f_open return :%d\n", res);
		//f_close(fftemp);

		//res = f_open(fftemp, "1:/img.jpg", FA_READ);
		//printf("NORFlash f_open return :%d\n", res);
		//f_close(fftemp);
		

		
		//测试SD打开文件
		/*
		res = f_open(fftemp, "0:/SYSTEM/SmartOfficeWeb/index.html", FA_READ);
		printf("SD f_open return :%d\n", res);
		f_close(fftemp);
		*/
		
		//res = f_open(fftemp, "0:/img.jpg", FA_READ);
		//printf("SD f_open return :%d\n", res);


		res = f_open(fftemp, "1:SmartOfficeWeb/index.html", FA_READ);
		printf("NORFlash f_open return :%d\n", res);
		f_close(fftemp);

		myfree(SRAMCCM, fftemp);
		
		//在WebServer_Task中完成磁盘初始化及挂载之后，只有Touch_Task中可以打开文件
		//也就是在GUI启动之后f_open都会失败(很扯淡)
		//因此也像字库和图库那样维护网页文件
		
		
		
		// 创建二值信号量 
		xBinarySemaphoreICON = xSemaphoreCreateBinary();
		//将图库加载到外扩SRAM中
		read_icons();
		//初始化位图结构体信息
		InitDynamicImage();


		lcd_set_backlight_by_pwm(0xFF); // 设置占空比为255，开启背光最亮
		lcd_clear(WHITE);  //清屏
		
		//screen touch init
		res = tp_dev.init();                      // 触摸屏初始化
		if(!res){
			printf("LCD Touch init Successful!\n");
		}
		
		// 释放信号量，通知任务GUI_Task可以执行了 
		xSemaphoreGive(xBinarySemaphoreICON); 	
		//emwin_test_touch();  //emWin坐标获取
		
		// 释放信号量
		xSemaphoreGive(xBinarySemaphoreFont);
		
		/*
		printf("webstinfo:%p\n", webstinfo);
		printf("webstinfo->index_html_size:%u\n", webstinfo->index_html_size);
		*/
		
		memused = my_mem_perused(SRAMEX);
		sprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
		printf("SRAMEX   USED: %s\n", (char *)paddr);
		
		printf("----------------------------------|\n\n");
		taskEXIT_CRITICAL();            /* 出临界区 */
	}
  /* Infinite loop */
  for(;;)
  {
	  /*
		if (xSemaphoreTake(xBinarySemaphoreFont, portMAX_DELAY) == pdTRUE) {
			taskENTER_CRITICAL();           // 进入临界段
			FIL *fftemp;
			fftemp = (FIL *)mymalloc(SRAMEX, sizeof(FIL));  // 给文件描述符开辟空间
			if(NULL == fftemp){
				printf("fftemp is NULL\n");
			}
			uint8_t res = 0;  
			res = f_open(fftemp, "1:SmartOfficeWeb/index.html", FA_READ);
			printf("NORFlash f_open return :%d\n", res);
			f_close(fftemp);
			myfree(SRAMEX, fftemp);	
			// 释放信号量
			xSemaphoreGive(xBinarySemaphoreFont);
			taskEXIT_CRITICAL();            // 出临界区 
	    }
	  */
		if (t % 5 == 0) /* 每200ms读取一次 */ { 
			dht11_read_data(&temperature, &humidity); /* 读取温湿度值 */
			
			//printf("temperature: %d.%d\n", temperature>>8, (temperature & 0xFF));/* 显示温度 */ 
			//printf("humidity: %d.%d", humidity>>8, (humidity & 0xFF)); /* 显示湿度 */ 
		}
		if(t % 10 == 0) /* 每400ms读取一次 */{ 
				adcx = lsens_get_val();                                 /* 获取亮度 */
				//printf("bright:%d\n", adcx);
		}
		t++; 
		//osDelay(10);
		//触摸屏需要轮询检测，否则emWin没有办法触发事件
		GUI_TOUCH_Exec();
		osDelay(40);
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
  printf("IOT_Task-------------------------->\n");
  //使用互斥信号量保护esp8266的初始化及配置过程
  if (xSemaphoreTake(xMutexEsp8266, portMAX_DELAY) == pdTRUE) {
	/*
	printf("webstinfo:%p\n", webstinfo);
	printf("webstinfo->index_html_size:%u\n", webstinfo->index_html_size);
	*/
	  

	//taskENTER_CRITICAL();           // 进入临界段
	ESP8266_Connect_Wifi(macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd);  //"DUOBAO", "yunshu666"
	//ESP8266_Connect_Wifi("Yunshu_Drwells", "yzy@0203yzy@0203");    //对ESP8266进行配置并连接到指定wifi
	//发起udp广播，所有在线的物联网子设备会主动连接过来从而获取它们的ip地址(废弃)
	ESP8266_startBroadCastCmd();  
	//test();
	//扫描所有wifi列表...
	  
	xSemaphoreGive(xMutexEsp8266);
	
	/*  
	printf("webstinfo:%p\n", webstinfo);
	printf("webstinfo->index_html_size:%u\n", webstinfo->index_html_size);
	*/
	  
	memused = my_mem_perused(SRAMEX);
	sprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
	printf("SRAMEX   USED: %s\n", (char *)paddr);
	printf("----------------------------------|\n\n");
	//taskEXIT_CRITICAL();            // 出临界区   
  }
  /*
  if (xSemaphoreTake(xBinarySemaphoreFont, portMAX_DELAY) == pdTRUE) {
		taskENTER_CRITICAL();           // 进入临界段
		FIL *fftemp;
		fftemp = (FIL *)mymalloc(SRAMEX, sizeof(FIL));  // 给文件描述符开辟空间
		if(NULL == fftemp){
			printf("fftemp is NULL\n");
		}
		uint8_t res = 0;  
		res = f_open(fftemp, "1:SmartOfficeWeb/index.html", FA_READ);
		printf("NORFlash f_open return :%d\n", res);
		f_close(fftemp);
		myfree(SRAMEX, fftemp);	
		// 释放信号量
		xSemaphoreGive(xBinarySemaphoreFont);
		taskEXIT_CRITICAL();            // 出临界区 
  }
  */
  /* Infinite loop */
  for(;;)
  {
	osDelay(10);
	//taskENTER_CRITICAL();           /* 进入临界段 */
	ESP8266_CheckRecvData(); // 处理网络请求 (可以成功收到)
	//taskEXIT_CRITICAL();            /* 出临界区 */
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
	/*
	taskENTER_CRITICAL();           // 进入临界段
	
	// 等待任务1完成 
	if (xSemaphoreTake(xBinarySemaphoreCheckFontsAndIconBin, portMAX_DELAY) == pdTRUE) { 
		// 执行任务2的操作 // ...
		MainTask();
	}
	taskEXIT_CRITICAL();            // 退出临界段
	*/
	printf("GUI_Task-------------------------->\n");
	if (xSemaphoreTake(xBinarySemaphoreICON, portMAX_DELAY) == pdTRUE) {
		/*
		//在WebServer_Task中完成磁盘初始化及挂载之后，只有Touch_Task中可以打开文件
		if (xSemaphoreTake(xBinarySemaphoreFont, portMAX_DELAY) == pdTRUE) {
			taskENTER_CRITICAL();           // 进入临界段
			FIL *fftemp;
			fftemp = (FIL *)mymalloc(SRAMEX, sizeof(FIL));  // 给文件描述符开辟空间
			if(NULL == fftemp){
				printf("fftemp is NULL\n");
			}
			uint8_t res = 0;  
			res = f_open(fftemp, "1:SmartOfficeWeb/index.html", FA_READ);
			printf("NORFlash f_open return :%d\n", res);
			f_close(fftemp);
			myfree(SRAMEX, fftemp);	
			// 释放信号量
			xSemaphoreGive(xBinarySemaphoreFont);
			taskEXIT_CRITICAL();            // 出临界区 
		}
		*/
		/*
		printf("webstinfo:%p\n", webstinfo);
		printf("webstinfo->index_html_size:%u\n", webstinfo->index_html_size);
		*/
		
		MainTask();
	}
  /* Infinite loop */
	/*
  for(;;)
  {
    osDelay(1);
  }
	*/
  /* USER CODE END GUI_Task */
}
/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
