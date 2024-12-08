#include "stm32f4xx_hal.h"
#include "usart.h"
#include "main.h"  //rx_buffer rx_data rx_index RX_BUFFER_SIZE

// 创建二值信号量句柄 
//SemaphoreHandle_t xBinarySemaphoreData;
// 创建互斥信号量句柄 
//SemaphoreHandle_t xMutexData;

/*
void USART3_Init_DMA(void)
{
    // 启动DMA接收
    HAL_UART_Receive_DMA(&huart3, uart3_rx_buffer, BUFFER_WINDOW);

    // 启用空闲中断
    __HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);
	
	// 创建二值信号量 
	//xBinarySemaphoreData = xSemaphoreCreateBinary();
	
	//创建互斥信号量
	//xMutexData = xSemaphoreCreateMutex();
}
*/

void USART3_SendData_DMA(uint8_t *pData, uint16_t Size)
{
    HAL_UART_Transmit_DMA(&huart3, pData, Size);
}
