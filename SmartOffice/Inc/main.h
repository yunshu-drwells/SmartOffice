/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
uint8_t sys_stm32_clock_init(uint32_t plln, uint32_t pllm, uint32_t pllp, uint32_t pllq);
void load_draw_dialog(void);
void lcd_draw_bline(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t size, uint16_t color);
void rtp_test(void);
void ctp_test(void);
void emwin_test_touch(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define GT9XXX_RST_Pin GPIO_PIN_13
#define GT9XXX_RST_GPIO_Port GPIOC
#define A0_Pin GPIO_PIN_0
#define A0_GPIO_Port GPIOF
#define A1_Pin GPIO_PIN_1
#define A1_GPIO_Port GPIOF
#define A2_Pin GPIO_PIN_2
#define A2_GPIO_Port GPIOF
#define A3_Pin GPIO_PIN_3
#define A3_GPIO_Port GPIOF
#define A4_Pin GPIO_PIN_4
#define A4_GPIO_Port GPIOF
#define A5_Pin GPIO_PIN_5
#define A5_GPIO_Port GPIOF
#define RST_Pin GPIO_PIN_6
#define RST_GPIO_Port GPIOF
#define Beep_Pin GPIO_PIN_8
#define Beep_GPIO_Port GPIOF
#define LED0_Pin GPIO_PIN_9
#define LED0_GPIO_Port GPIOF
#define LED1_Pin GPIO_PIN_10
#define LED1_GPIO_Port GPIOF
#define CH_PD_Pin GPIO_PIN_0
#define CH_PD_GPIO_Port GPIOC
#define GT9XXX_SCL_Pin GPIO_PIN_0
#define GT9XXX_SCL_GPIO_Port GPIOB
#define GT9XXX_INT_Pin GPIO_PIN_1
#define GT9XXX_INT_GPIO_Port GPIOB
#define GT9XXX_MISO_Pin GPIO_PIN_2
#define GT9XXX_MISO_GPIO_Port GPIOB
#define GT9XXX_MOSI_Pin GPIO_PIN_11
#define GT9XXX_MOSI_GPIO_Port GPIOF
#define LCD_RS_Pin GPIO_PIN_12
#define LCD_RS_GPIO_Port GPIOF
#define A7_Pin GPIO_PIN_13
#define A7_GPIO_Port GPIOF
#define A8_Pin GPIO_PIN_14
#define A8_GPIO_Port GPIOF
#define A9_Pin GPIO_PIN_15
#define A9_GPIO_Port GPIOF
#define A10_Pin GPIO_PIN_0
#define A10_GPIO_Port GPIOG
#define A11_Pin GPIO_PIN_1
#define A11_GPIO_Port GPIOG
#define D4_Pin GPIO_PIN_7
#define D4_GPIO_Port GPIOE
#define D5_Pin GPIO_PIN_8
#define D5_GPIO_Port GPIOE
#define D6_Pin GPIO_PIN_9
#define D6_GPIO_Port GPIOE
#define D7_Pin GPIO_PIN_10
#define D7_GPIO_Port GPIOE
#define D8_Pin GPIO_PIN_11
#define D8_GPIO_Port GPIOE
#define D9_Pin GPIO_PIN_12
#define D9_GPIO_Port GPIOE
#define D10_Pin GPIO_PIN_13
#define D10_GPIO_Port GPIOE
#define D11_Pin GPIO_PIN_14
#define D11_GPIO_Port GPIOE
#define D12_Pin GPIO_PIN_15
#define D12_GPIO_Port GPIOE
#define SPI1_CS_Pin GPIO_PIN_14
#define SPI1_CS_GPIO_Port GPIOB
#define LCD_BL_Pin GPIO_PIN_15
#define LCD_BL_GPIO_Port GPIOB
#define D13_Pin GPIO_PIN_8
#define D13_GPIO_Port GPIOD
#define D14_Pin GPIO_PIN_9
#define D14_GPIO_Port GPIOD
#define D15_Pin GPIO_PIN_10
#define D15_GPIO_Port GPIOD
#define A16_Pin GPIO_PIN_11
#define A16_GPIO_Port GPIOD
#define A17_Pin GPIO_PIN_12
#define A17_GPIO_Port GPIOD
#define A18_Pin GPIO_PIN_13
#define A18_GPIO_Port GPIOD
#define D0_Pin GPIO_PIN_14
#define D0_GPIO_Port GPIOD
#define D1_Pin GPIO_PIN_15
#define D1_GPIO_Port GPIOD
#define A12_Pin GPIO_PIN_2
#define A12_GPIO_Port GPIOG
#define A13_Pin GPIO_PIN_3
#define A13_GPIO_Port GPIOG
#define A14_Pin GPIO_PIN_4
#define A14_GPIO_Port GPIOG
#define A15_Pin GPIO_PIN_5
#define A15_GPIO_Port GPIOG
#define D2_Pin GPIO_PIN_0
#define D2_GPIO_Port GPIOD
#define D3_Pin GPIO_PIN_1
#define D3_GPIO_Port GPIOD
#define ETH_RESET_Pin GPIO_PIN_3
#define ETH_RESET_GPIO_Port GPIOD
#define LCD_RD_Pin GPIO_PIN_4
#define LCD_RD_GPIO_Port GPIOD
#define LCD_WR_Pin GPIO_PIN_5
#define LCD_WR_GPIO_Port GPIOD
#define DHT11_Pin GPIO_PIN_9
#define DHT11_GPIO_Port GPIOG
#define CS_Pin GPIO_PIN_10
#define CS_GPIO_Port GPIOG
#define LCD_CS_Pin GPIO_PIN_12
#define LCD_CS_GPIO_Port GPIOG
#define SPI1_SCK_Pin GPIO_PIN_3
#define SPI1_SCK_GPIO_Port GPIOB
#define SPI1_MISO_Pin GPIO_PIN_4
#define SPI1_MISO_GPIO_Port GPIOB
#define SPI1_MOSI_Pin GPIO_PIN_5
#define SPI1_MOSI_GPIO_Port GPIOB
#define LB_Pin GPIO_PIN_0
#define LB_GPIO_Port GPIOE
#define UB_Pin GPIO_PIN_1
#define UB_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */
//#define RX_BUFFER_SIZE 256  // Define the size of your receive buffer
#define MAX_RX_BUFFER_SIZE 10240  // Define the max size of your receive buffer
#define BUFFER_WINDOW 4096  //Define the increase size of buffer
extern uint8_t* uart3_rx_buffer;

#define MAX_IP_LENGTH 16

#define macUser_ESP8266_ApSsid "DUOBAO"        //要连接的热点名称（只支持2.4G网络�?
#define macUser_ESP8266_ApPwd  "yunshu666"  //要连接的热点密钥

#define macUser_ESP8266_TcpServer_IP   "192.168.3.3" //要连接的服务器的IP
#define macUser_ESP8266_TcpServer_Port 8000        //要连接的服务器的端口

//extern char FAN_ip_address[];  //风扇模块ip地址
//extern char MasterLight_ip_address[];  //主灯模块ip地址
//extern char SpotLight_ip_address[];  //射灯模块ip地址

extern char ETH_ip_address[];  //有线网络ip地址

void fmout_sd(uint8_t opt);
void fmout_norflash(uint8_t opt);

//外扩SRAM总共8MB
//外扩SRAM前0x20000 B(128KB)用于.ANY区 [0x68000000, 0x68020000]；843KB用于内存池；(843 * 1024 / 32) * 2 = 52KB用于内存管理表

//内存池中
//紧接着40KB用于FreeRTOS的heap5堆区 [0x68020000, 0x6802a000] 
// 在heap_5中，FreeRTOSConfig.h中configTOTAL_HEAP_SIZE宏定义不再用于指定总的堆大小，而是用于检查你定义的堆区域总大小是否超过了这个值。如果超过了，编译时会触发一个错误。）
//紧接着1600B用于lwip的


//FreeRTOS的heap5在外扩SRAM的起始地址
#define FreeRTOS_Heap5_ExRAM_ADDR 0x68020000
//FreeRTOS的heap5在外扩SRAM的大小(Bytes)
#define FreeRTOS_Heap5_ExRAM_SIZE 0xa000  //40KB
//#define FreeRTOS_Heap5_ExRAM_SIZE 0xCEC000  //827KB
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
