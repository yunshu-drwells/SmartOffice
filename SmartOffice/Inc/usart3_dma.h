#include <stdint.h>
#include "cmsis_os.h"

//extern SemaphoreHandle_t xBinarySemaphoreData;

//#define QUEUE_ITEM_SIZE 100  // 队列项大小
//#define QUEUE_LENGTH 10      // 队列长度

//extern QueueHandle_t xQueueData;  // 数据队列

void USART3_Init_DMA(void);
void USART3_SendData_DMA(uint8_t *pData, uint16_t Size);
