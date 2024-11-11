#include "esp8266.h"  //ESP8266_Enable
#include "esp8266_web.h"  //enum
#include "stdint.h"
#include "usart.h"  //huart1
#include "main.h"   //RX_BUFFER_SIZE MAX_IP_LENGTH
#include "string.h" //strcmp memset
#include "delay.h"  //delay_ms

extern uint8_t uart3_rx_buffer[];
extern volatile uint16_t uart3_rx_index;

static void ESP8266_SendCmd(const char* str){
	//printf("send cmd\n");
	HAL_UART_Transmit(&huart3, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
}

static void ClearUart3ReceiveBuff(void){
	// 清空缓冲区
	memset(uart3_rx_buffer, 0, RX_BUFFER_SIZE);
	__HAL_UART_FLUSH_DRREGISTER(&huart3);  // 清空数据寄存器
	//Uart3FramFinishFlag = 0;  //接收完成标志置零
	uart3_rx_index = 0;
	
	// Re-enable UART1 receive interrupt
	// 重新开始接收数据
	if (HAL_UART_GetState(&huart3) == HAL_UART_STATE_READY)
	{
		HAL_UART_Receive_IT(&huart3, uart3_rx_buffer, RX_BUFFER_SIZE);
	}
	else
	{
		//printf("UART not ready to receive, aborting current receive operation\n");
		HAL_UART_AbortReceive_IT(&huart3);
		HAL_UART_Receive_IT(&huart3, uart3_rx_buffer, RX_BUFFER_SIZE);
	}
}
#include <stdio.h>
#include <string.h>

char ip_address[MAX_IP_LENGTH] = {0};

char* extract_ip_address(const char* input, char* ip_address, size_t max_length) {
    const char* start = strstr(input, "+CIFSR:STAIP,\"");
    if (start == NULL) {
        return NULL;
    }
    
    start += 14; // 跳过 "+CIFSR:STAIP,\""
    const char* end = strchr(start, '"');
    if (end == NULL) {
        return NULL;
    }
    
    size_t length = end - start;
    if (length >= max_length) {
        return NULL;
    }
    
    strncpy(ip_address, start, length);
    ip_address[length] = '\0';
    
    return ip_address;
}

#include "FreeRTOS.h" //TickType_t
#include "task.h"  //xTaskGetTickCount

static uint8_t ESP8266_WaitResponse(const char* expected_response, uint32_t timeout){
	//uint32_t startTime = HAL_GetTick();
	//while ((HAL_GetTick() - startTime) < timeout){
	TickType_t startTime = xTaskGetTickCount();
    while ((xTaskGetTickCount() - startTime) < pdMS_TO_TICKS(timeout)){
		if (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_IDLE) != RESET) { //如果Uart3接收到了Esp8266的数据
			__HAL_UART_CLEAR_IDLEFLAG(&huart3);                            //接收标志置零

			//用于调试某个AT命令的返回值
			if(!strcmp(expected_response, "??")){
				printf("Uart3 Received data from ESP8266: %s\n", uart3_rx_buffer);  // 调试输出
				printf("Uart3 Received data length:%d\n", uart3_rx_index);
			}

			//printf("Uart3 Received data from ESP8266: %s\n", uart3_rx_buffer);  // 调试输出
			//printf("Uart3 Received data length:%d\n", uart3_rx_index);
			/*
			//调试或者打印都无法打印正确值，printf太耗时，会导致uart3接收失败
			for(uint8_t i = 0; i<10; i++){
				printf("uart3_rx_buffer[%d]: %d\n", (uint8_t)uart3_rx_buffer[i]);
			}
			*/
			//将ESP8266的数据转发给Uart1
			if(!strcmp(expected_response, (const char*)uart3_rx_buffer) || strstr((const char*)uart3_rx_buffer, expected_response)) {
				if(!strcmp(expected_response, "OK")){
					//解析ip地址
					/*
					AT+CIFSR
					+CIFSR:STAIP,"192.168.2.38"
					+CIFSR:STAMAC,"ec:fa:bc:8f:01:3a"

					OK
					*/
					if (extract_ip_address((const char*)uart3_rx_buffer, ip_address, MAX_IP_LENGTH) != NULL) {
						printf("Extracted IP address: %s\n", ip_address);
					} else {
						printf("Failed to extract IP address\n");
					}			
					//printf("identical \n");
				}
				//用于调试某个AT命令的返回值
				if(!strcmp(expected_response, "??")){
					printf("identical \n");
				}
				ClearUart3ReceiveBuff();
				//printf("identical \n");
				return 1;
			} 
			ClearUart3ReceiveBuff();
		 }
	}
	return 0;
}

static uint8_t ESP8266_AT_Test(void){
    const char* str = "AT\r\n";
    //HAL_UART_Transmit(&huart3, (uint8_t*)str, 4, HAL_MAX_DELAY);
	ESP8266_SendCmd(str);
	
	if (!ESP8266_WaitResponse("AT\r\n\r\nOK\r\n", 200))
    {
        printf("AT ERROR!\r\n");
        return 0;
    }
	printf("AT OK!\r\n");
	return 1;
}

/**
 * @brief 设置ESP8266为STA模式
 * @return 1 if successful, 0 if failed
 */
static uint8_t ESP8266_SetSTAMode(void)
{
    ESP8266_SendCmd("AT+CWMODE=1\r\n");
    if (!ESP8266_WaitResponse("AT+CWMODE=1\r\n\r\nOK\r\n", 1000))
    {
        //printf("Failed to set STA mode\r\n");
        return 0;
    }
	printf("Successed to set STA mode\r\n");
	return 1;
}

/**
 * @brief 断开可能存在的WiFi连接
 * @return 1 if successful, 0 if failed
 */
static uint8_t ESP8266_CWQAP(){
    ESP8266_SendCmd("AT+CWQAP\r\n");
    if (!ESP8266_WaitResponse("AT+CWQAP\r\n\r\nOK\r\n", 1000))
    {
        //printf("Failed to disconnect from AP\r\n");
        return 0;
    }
	printf("Successed to disconnect from AP\r\n");

    return 1;
}

/**
 * @brief 禁用自动连接
 * @return 1 if successful, 0 if failed
 */
uint8_t ESP8266_CWAUTOCONN(){
    ESP8266_SendCmd("AT+CWAUTOCONN=0\r\n");
    if (!ESP8266_WaitResponse("AT+CWAUTOCONN=0\r\n\r\nOK\r\n", 1000))
    {
        //printf("Failed to disable auto-connect\r\n");
        return 0;
    }
	printf("Successed to disable auto-connect\r\n");

    return 1;
}

/**
 * @brief 使ESP8266连接到指定的WiFi热点
 * @param ssid WiFi的SSID
 * @param password WiFi的密码
 * @return 1 if successful, 0 if failed
 */
uint8_t ESP8266_JoinAP(const char* ssid, const char* password)
{
    char cmd[128];
    
    // 1. 设置WiFi模式为Station模式
	//已经完成了
	
    // 2. 连接到指定的AP
    snprintf(cmd, sizeof(cmd), "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password);
    ESP8266_SendCmd(cmd);
    
	/*
    if (!ESP8266_WaitResponse(cmd, 20000)) {
        printf("Failed to connect to AP\r\n");
        return 0;
    }
	*/
	
	if (!ESP8266_WaitResponse("WIFI CONNECTED\r\n", 20000)) {
        printf("Failed to connect to AP\r\n");
        return 0;
    }
	
    if (!ESP8266_WaitResponse("WIFI GOT IP\r\n", 10000)) {
        printf("Connection confirmation WIFI GOT IP failed\r\n");
        return 0;
    }
    
    if (!ESP8266_WaitResponse("\r\nOK\r\n", 5000)) {
        printf("Connection confirmation OK failed\r\n");
        return 0;
    }
	
	
    
    // 3. 查询IP地址以确认连接
    ESP8266_SendCmd("AT+CIFSR\r\n");
    if (!ESP8266_WaitResponse("OK", 5000)) {
        printf("Failed to get IP address\r\n");
        return 0;
    }
    
    printf("Successfully connected to AP\r\n");
    return 1;
}


/**
 * @brief 使ESP8266连接到指定的WiFi热点
 * @param ssid WiFi的SSID
 * @param password WiFi的密码
 * @return 1 if successful, 0 if failed
 */
static uint8_t ESP8266_Enable_MultipleId(int enable) {
    char command[20] = {0};
	snprintf(command, sizeof(command), "AT+CIPMUX=%d\r\n", enable);
    //printf("command %s\n", command);
    
    ESP8266_SendCmd(command);
	snprintf(command, sizeof(command), "AT+CIPMUX=%d\r\n\r\nOK\r\n", enable);
    // 等待响应
    if (!ESP8266_WaitResponse(command, 5000)) {
        printf("Failed to set Multiple %d\r\n", enable);
        return 0;
    }
	printf("Successfully set Multiple %d \r\n", enable);
    return 1;
}


/**
 * @brief 使ESP8266开启服务器模式
 * @return 1 if successful, 0 if failed
 */
static uint8_t ESP8266_Enable_SERVER() {
    ESP8266_SendCmd("AT+CIPSERVER=1,80\r\n");
    // 等待响应 首次设置和第二次设置成功返回字符是不同的
    if (!ESP8266_WaitResponse("AT+CIPSERVER=1,80\r\n\r\nOK\r\n", 5000)){
		ESP8266_SendCmd("AT+CIPSERVER=1,80\r\n");
		if (!ESP8266_WaitResponse("AT+CIPSERVER=1,80\r\nno change\r\n\r\nOK\r\n", 5000)) {
			printf("Failed to set SERVER\r\n");
			return 0;
		}
    }
	printf("Successfully set SERVER\r\n");
    return 1;
}


/**
 * @brief 设置ESP8266服务器超时时间
 * @return 1 if successful, 0 if failed
 */
static uint8_t ESP8266_Enable_STO() {
    ESP8266_SendCmd("AT+CIPSTO=60\r\n");
    // 等待响应
    if (!ESP8266_WaitResponse("AT+CIPSTO=60\r\n\r\nOK\r\n", 5000)) {
        printf("Failed to set STO\r\n");
        return 0;
    }
	printf("Successfully set STO\r\n");
    return 1;
}

/**
* @brief  ESP8266 连接wifi函数
 * @param ssid WiFi的SSID
 * @param password WiFi的密码 
* @retval 无
*/
void ESP8266_Connect_Wifi(const char* ssid, const char* password)
{
	//等待AT命令成功
	while( ! ESP8266_AT_Test() );
	//等待设置WiFi模式为STA成功
	while( ! ESP8266_SetSTAMode() );
	//等待断开可能存在的WiFi连接成功
	while( ! ESP8266_CWQAP() );
	//等待禁用自动连接成功
	while( ! ESP8266_CWAUTOCONN() );
	//等待成功连接到某个指定的wifi并获取ip地址
	while( ! ESP8266_JoinAP(ssid, password) );
  
	//等待禁用ESP8266多连接成功
	while( ! ESP8266_Enable_MultipleId(1) );
	
	//等待设置为服务器模式成功
	while( ! ESP8266_Enable_SERVER() );
	
	//等待设置服务器超时时间成功
	while( ! ESP8266_Enable_STO() );
}

/**
* @brief  ESP8266 处理网络请求数据
* @param  无
* @retval 无
*/
void ESP8266_CheckRecvData(void)
{
	if (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_IDLE) != RESET) { //如果Uart3接收到了Esp8266的数据
		__HAL_UART_CLEAR_IDLEFLAG(&huart3);                            //接收标志置零

		printf("Uart3 Received data from ESP8266: %s\n", uart3_rx_buffer);  // 调试输出
		printf("Uart3 Received data length:%d\n", uart3_rx_index);

		ClearUart3ReceiveBuff();
	 }
}
