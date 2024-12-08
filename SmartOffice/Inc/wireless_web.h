#include "usart.h"

extern uint8_t dataReadyFlag;  //stm32f4xx_it.c
extern uint8_t AT_Flag;  //stm32f4xx_it.c
extern uint16_t uart3_rx_index;  //freertos.c
extern uint8_t* uart3_rx_data;  //main.c

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// 定义消息队列句柄
QueueHandle_t xQueue_UART3_RX;

// 定义接收缓冲区
#define UART3_RX_BUFFER_SIZE 1024

// 初始化消息队列
void init_uart3_rx_queue(void) {
    // 创建一个消息队列，队列长度为 10，每个元素的大小为 sizeof(uint8_t)
    xQueue_UART3_RX = xQueueCreate(10, sizeof(uint8_t));
    if (xQueue_UART3_RX == NULL) {
        // 消息队列创建失败
        //while (1);
		printf("Creat xQueue failed\n");
    }else{
		printf("Successfully Created xQueue\n");
	}
}

void vTaskProcessUART3RX(void *pvParameters) {
    uint8_t received_data;
    while (1) {
        // 从消息队列中获取数据
        if (xQueueReceive(xQueue_UART3_RX, &received_data, portMAX_DELAY) == pdPASS) {
            // 处理接收到的数据
            // 例如：打印数据到终端
            printf("Received: %c\n", received_data);
        }
    }
}


void Listen_Thread_Wireless(){
	if (!AT_Flag && dataReadyFlag) { //如果Uart3接收到了Esp8266的数据
		//__HAL_UART_CLEAR_IDLEFLAG(&huart3);                            //接收标志置零

		printf("Uart3 Received data from ESP8266: %s\n", uart3_rx_data);  // 调试输出
		printf("Uart3 Received data length:%d\n", uart3_rx_index);
		dataReadyFlag = 0;
	 }
}
