/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "adc.h"
#include "crc.h"
#include "dma.h"
#include "fatfs.h"
#include "i2c.h"
#include "lwip.h"
#include "sdio.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "sram.h"  //sram_init
#include "mymalloc.h"  //my_mem_init

#include "FreeRTOS.h"
#include "task.h"

#include "delay.h"
#include "lcd.h"

#include "lwipopts.h"  //lwipopts.h

#include "GUI.h"  //GUI_PID_STATE
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

/* USER CODE BEGIN PV */
// �������������
HeapRegion_t xHeapRegions[] = {
    { ( uint8_t * ) 0x68000000, 0xa000 },  // �����0x68000000��ʼ��40K�ֽ��ڴ��
    { NULL, 0 }                            // ��ֹ����
};

void configureHeapRegions(void)
{
    vPortDefineHeapRegions(xHeapRegions);  // ���������
}

uint8_t paddr[20] = {0};                  /* ����ڴ�EXSRAMʹ���� */
uint16_t memused = 0;                     /* �ڴ�ʹ�ðٷֱ� */

uint8_t lcd_id[12];
uint16_t colors[12] = {WHITE, BLACK, BLUE, RED, MAGENTA, GREEN, CYAN, YELLOW, BRRED, GRAY, LGRAY, BROWN};uint8_t color_index = 0;
uint8_t cur_brightness;
uint8_t* str[23];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_FSMC_Init();
  MX_I2C1_Init();
  MX_CRC_Init();
  MX_SPI1_Init();
  MX_ADC3_Init();
  MX_SDIO_SD_Init();
  /* USER CODE BEGIN 2 */
		// �������ö�����ĺ���
	configureHeapRegions();  //FreeRTOS����heap5�Ķ�����ַ��Χ
	
	sram_init();                        /* ����SRAM��ʼ�� */

	//my_mem_init(SRAMIN);                /* ��ʼ���ڲ�SRAM�ڴ�� */
	my_mem_init(SRAMEX);                /* ��ʼ���ⲿSRAM�ڴ�� */
	my_mem_init(SRAMCCM);               /* ��ʼ���ڲ�CCM�ڴ�� */
	
	//��Ϊ������SRAM��ͷ��40KB��FreeRTOSʹ�ã�����Ҫ���ⲿ�����ڴ������ж�Ӧ�ı��ֵΪ���㣬��ʾ�Ѿ���ռ���ˣ���ֹӰ��FreeRTOS�Ķ���
	my_mem_occupy(SRAMEX, 40*1024);
	
	memused = my_mem_perused(SRAMEX);
	sprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
	printf("SRAMEX   USED: %s\n", (char *)paddr);
	
	//��Ϊ������SRAM������FreeRTOS�Ŀռ�����LWIP����MEM_SIZE��С�Ŀռ䣬����Ҫ���ⲿ�����ڴ������ж�Ӧ�ı��ֵΪ����
	my_mem_occupy_from(SRAMEX, 40*1024, MEM_SIZE);
	memused = my_mem_perused(SRAMEX);
	sprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
	printf("SRAMEX   USED: %s\n", (char *)paddr);
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init(); 

  /* Start scheduler */
  osKernelStart();
  
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/*
void emwin_test_touch(void){
	GUI_PID_STATE State;
	GUI_Init();
	GUI_SetBkColor(GUI_BLUE);
	GUI_SetFont(&GUI_Font20_ASCII);
	GUI_SetColor(GUI_YELLOW);
	GUI_Clear();
	while(1){
		//ִ�д����ʼ��
		GUI_TOUCH_Exec();
		//��ȡ������״ֵ̬
		GUI_TOUCH_GetState(&State);
		//�Ƿ���
		if(State.Pressed){
			//��ӡ������������Ϣ
			GUI_DispStringAt("X:", 0, 0);
			GUI_DispDecAt(State.x, 32, 0, 4);
			GUI_DispStringAt("Y:", 0, 24);
			GUI_DispDecAt(State.y, 32, 24, 4);
			
			GUI_DispStringAt("EXIT", lcddev.width - 40, 0);
		}

		delay_ms(10);
		if (State.x > lcddev.width - 40 && State.y < 20) //��EXIT 
		{
				GUI_SetBkColor(GUI_WHITE);
				GUI_Clear();
				return;  //�˳�������ѭ��
		}
	}
}
*/
/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
