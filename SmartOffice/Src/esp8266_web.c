#include "esp8266.h"  //ESP8266_Enable
#include "esp8266_web.h"  //enum
#include "stdint.h"
#include "usart.h"  //huart1
#include "main.h"   //RX_BUFFER_SIZE MAX_IP_LENGTH uart3_rx_buffer
#include "string.h" //strcmp memset
#include "delay.h"  //delay_ms

// 定义枚举类型
typedef enum {
    TCP,
    UDP
} enumTCP;

//extern uint8_t uart3_rx_buffer[];
extern volatile uint16_t uart3_rx_index;

void ESP8266_SendCmd(const char* str){
	//printf("send cmd\n");
	HAL_UART_Transmit(&huart3, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
}

static void ClearUart3ReceiveBuff(void){
	// 清空缓冲区
	memset(uart3_rx_buffer, 0, MAX_RX_BUFFER_SIZE);
	__HAL_UART_FLUSH_DRREGISTER(&huart3);  // 清空数据寄存器
	//Uart3FramFinishFlag = 0;  //接收完成标志置零
	uart3_rx_index = 0;
	
	// Re-enable UART1 receive interrupt
	// 重新开始接收数据
	if (HAL_UART_GetState(&huart3) == HAL_UART_STATE_READY)
	{
		HAL_UART_Receive_IT(&huart3, uart3_rx_buffer, MAX_RX_BUFFER_SIZE);
	}
	else
	{
		//printf("UART not ready to receive, aborting current receive operation\n");
		HAL_UART_AbortReceive_IT(&huart3);
		HAL_UART_Receive_IT(&huart3, uart3_rx_buffer, MAX_RX_BUFFER_SIZE);
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
#include <stdlib.h>  //atoi

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

/*
static void extractHTTPBody(char* response, char* extracted_ip) {
	printf("strlen(response):%d \n", strlen(response));
	printf("%s", response);
    // 解析响应，提取正文
    char* body_start = strstr(response, "\r\n\r\n");
    if (body_start != NULL) {
        body_start += 4; // 跳过"\r\n\r\n"
        char* body_end = strstr(body_start, "\r\nCLOSED");
        if (body_end != NULL) {
            int ip_length = body_end - body_start;
            if (ip_length < sizeof(extracted_ip)) {
                strncpy(extracted_ip, body_start, ip_length);
                extracted_ip[ip_length] = '\0';
                printf("Extracted IP: %s\n", extracted_ip);
            }
        }
    }
}
*/

uint8_t ESP8266_WaitResponseFor(const char* expected_response, uint32_t timeout){
	TickType_t startTime = xTaskGetTickCount();
    while ((xTaskGetTickCount() - startTime) < pdMS_TO_TICKS(timeout)){
		if (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_IDLE) != RESET) { //如果Uart3接收到了Esp8266的数据
			__HAL_UART_CLEAR_IDLEFLAG(&huart3);                            //接收标志置零
			//这个标志的清除是为了准备接收下一帧数据。如果不清除，系统将无法检测到下一次的空闲状态，从而可能错过数据帧的结束。
			//用于调试某个AT命令的返回值
			
			if(!strcmp(expected_response, "??")){
				printf("Uart3 Received data from ESP8266: %s\n", uart3_rx_buffer);  // 调试输出
				printf("Uart3 Received data length:%d\n", uart3_rx_index);
				
			}
			//将ESP8266的数据转发给Uart1
			if(!strcmp(expected_response, (const char*)uart3_rx_buffer) || strstr((const char*)uart3_rx_buffer, expected_response)) {
				//用于调试某个AT命令的返回值
				if(!strcmp(expected_response, "??")){
					printf("strlen(uart3_rx_buffer):%d \n", strlen((char*)uart3_rx_buffer));
					printf("%s", uart3_rx_buffer);
					// 提取并打印HTTP响应正文
					/*
					extractHTTPBody((char*)uart3_rx_buffer, FAN_ip_address);
					if (strlen(FAN_ip_address)) {
						printf("HTTP Response Body:\n%s\n", FAN_ip_address);
					} else {
						printf("Failed to extract HTTP body\n");
					}
					*/
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
/*
发送：
AT+CWAUTOCONN=0\r\n
接收：
AT+CWAUTOCONN=0\r\n\r\nOK\r\n
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
/*
AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password
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
 * @brief 开启或关闭多连接
 * @param enable 1开启,0关闭
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
static uint8_t _ESP8266_Enable_SERVER() {
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
 * @brief 使ESP8266开启服务端模式
 * @return 1 if successful, 0 if failed
 */
/*
AT+CIPMUX=1
AT+CIPSERVER=1,80
*/
void ESP8266_Enable_SERVER(){
	//等待禁用ESP8266多连接成功
	while( ! ESP8266_Enable_MultipleId(1) );
	
	//等待设置为客户端模式成功
	while( ! _ESP8266_Enable_SERVER() );
}


/**
 * @brief 使ESP8266开启客户端模式
 * @return 1 if successful, 0 if failed
 */

static uint8_t _ESP8266_Enable_Client(){
	ESP8266_SendCmd("AT+CIPSERVER=0\r\n");
    // 等待响应
    if (!ESP8266_WaitResponse("AT+CIPSERVER=0\r\n\r\nOK\r\n", 5000)){
		printf("Failed to set Client\r\n");
		return 0;
    }
	printf("Successfully set Client\r\n");
    return 1;
}

/*
AT+CIPSERVER=0
AT+CIPMUX=0
*/
void ESP8266_Enable_Client(){
	//等待设置为客户端模式成功
	while( ! _ESP8266_Enable_Client() );
	
	//等待禁用ESP8266多连接成功
	while( ! ESP8266_Enable_MultipleId(0) );
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
  
	//等待开启ESP8266多连接成功
	while( ! ESP8266_Enable_MultipleId(1) );
	
	//等待设置为服务器模式成功
	while( ! _ESP8266_Enable_SERVER() );
	
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
		//解析Fan ip地址
		/*
		char *pdest = strstr((char*)uart3_rx_buffer, "Fan:");
		if(pdest){
			strcpy(FAN_ip_address, pdest+4);
			printf("FAN_ip_address:%s\n", FAN_ip_address);
		}
		*/


		ClearUart3ReceiveBuff();
	 }
}

/*
AT+CWMODE=1  //设置ESP8266为STA模式 
AT+CWQAP  //断开可能存在的WiFi连接
AT+CWAUTOCONN=0  //禁用自动连接
AT+CWJAP="Yunshu_Drwells","yzy@0203yzy@0203"  //连接到指定的AP
AT+CIFSR  //查询IP地址以确认连接
AT+CIPMUX=1  //使用多连接(可以同时处理多个TCP/UDP连接)
AT+CIPSERVER=1,80  //设置为服务器模式，并在指定端口（例如80）上监听
AT+CIPSTO=60  //设置服务器超时时间（单位：秒，0表示永不超时）影响的是TCP连接的保持时间
*/







/*
AT+CIPSTART=0,"UDP","255.255.255.255",8080
AT+CIPSEND=0,16
> DISCOVER_DEVICES

AT+CIPSTART=0,"UDP","255.255.255.255",8080  //0: 这是连接ID，用于标识多连接模式下的不同连接。所有接收到广播数据包的设备必须在该端口上监听，才能接收到数据。
AT+CIPSEND=0,16  // 发送16字节的数据
> DISCOVER_DEVICES  // 输入要发送的数据
*/

/*
// 发起UDP广播
sendATCommand("AT+CIPSTART=0,\"UDP\",\"255.255.255.255\",8080");
sendATCommand("AT+CIPSEND=0,16");
sendATCommand("Hello, IoT Devices!");
*/


// 发送广播消息
char sendBroadcastCmd[] = "AT+CIPSTART=0,\"UDP\",\"255.255.255.255\",8080\r\n";

/*
发送：
AT+CIPSTART=0,"UDP","255.255.255.255",8080
成功接收：
AT+CIPSTART=0,"UDP","255.255.255.255",8080
0,CONNECT

OK
*/
uint8_t _ESP8266_sendBroadcastCmd(){
	ESP8266_SendCmd(sendBroadcastCmd);
    // 等待响应
    if (!ESP8266_WaitResponseFor("OK", 5000)) {
        printf("Failed to send Broadcast Cmd\r\n");
        return 0;
    }
	printf("Successfully sent Broadcast Cmd\r\n");
    return 1;
}

/*
发送：
AT+CIPSEND=0,16
成功接收：
RX：AT+CIPSEND=0,16

OK
> 
*/
uint8_t _ESP8266_sendDataCmd(char* sendDataCmd){
	ESP8266_SendCmd(sendDataCmd);
    // 等待响应
    if (!ESP8266_WaitResponseFor(">", 5000)) {
        printf("Failed to send Data Cmd: %s\r\n", sendDataCmd);
        return 0;
    }
	printf("Successfully sent Data Cmd: %s\r\n", sendDataCmd);
    return 1;
}

/*
发送：
DISCOVER_DEVICES
成功接收：
ES

busy s...

Recv 16 bytes

SEND OK
*/
uint8_t _ESP8266_broadcastMessage(char* broadcastMessage){
	ESP8266_SendCmd(broadcastMessage);
    // 等待响应
    if (!ESP8266_WaitResponseFor("SEND OK", 5000)) {
        printf("Failed to send broadcast Message: %s\r\n", broadcastMessage);
        return 0;
    }
	printf("Successfully sent broadcast Message: %s\r\n", broadcastMessage);
    return 1;
}

/**
* @brief  ESP8266 发送广播消息
* @param  无
* @retval 无
*/
//通过广播发起请求，然后让所有物联网设备返回自己的设备名称和ip地址，最后再通过ip地址的建立tcp从而与子模块通信
//这样会损耗cpu性能，直接使用广播的方式对所有物联网设备进行控制
void ESP8266_sendBroadcastCmd(char* broadcastMessage)
{
	char sendDataCmd[18] = {0};
	sprintf(sendDataCmd, "AT+CIPSEND=0,%d\r\n", strlen(broadcastMessage));
	
	//等待 AT+CIPSEND=0,16 命令成功
	while( ! _ESP8266_sendDataCmd(sendDataCmd) );
	
	//等待 发送信息成功
	while( ! _ESP8266_broadcastMessage(broadcastMessage) );
}

//#include "esp8266_fan.h"  //GetFanModuleIP

/*
FAN_ON
FAN_OFF

MasterLight_ON&R=<>&G=<>&B=<> (MasterLight_ON&R=255&G=255&B=255)
MasterLight_OFF

SpotLight_ON&R=<>&G=<>&B=<> (SpotLight_ON&R=255&G=255&B=255)
SpotLight_OFF

CONNECT_NEW_WIFI&ssid=Yunshu_Drwells&pwd=yzy@0203yzy@0203
*/

void ESP8266_startBroadCastCmd(){
	//等待 AT+CIPSTART=0,"UDP","255.255.255.255",8080 命令成功
	while( ! _ESP8266_sendBroadcastCmd() );
}


/**
* @brief  ESP8266 连接新wifi函数
 * @param ssid WiFi的SSID
 * @param password WiFi的密码 
* @retval 无
*/
/*
AT+CWQAP\r\n
AT+CWAUTOCONN=0\r\n
AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password
*/
uint8_t ESP8266_Connect_New_Wifi(const char* ssid, const char* password)
{
	//等待断开可能存在的WiFi连接成功
	//while( ! ESP8266_CWQAP() );
	//等待禁用自动连接成功
	//while( ! ESP8266_CWAUTOCONN() );
	//等待成功连接到某个指定的wifi并获取ip地址
	if( ! ESP8266_JoinAP(ssid, password) ){
		//失败
		return 0;
	}else{  
		//能成功连接
		return 1;
	}
}


/**
* @brief  ESP8266 连接新wifi函数
 * @param ssid WiFi的SSID
 * @param password WiFi的密码 
* @retval 无
*/
void ESP8266_Connect_New_Wifi_ALL(const char* ssid, const char* password)
{
	/*
	//在连接新wifi之前统计所有物联网设备数
	uint8_t devices = ESP8266_Count_Devices();

	//等待成功连接到某个指定的wifi并获取ip地址
	if( ! ESP8266_Connect_New_Wifi(ssid, password) ){
		//失败
		//连接回默认wifi
		while(!ESP8266_Connect_New_Wifi(macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd));
	}else{  //能成功连接
	    //检查空闲的ip地址数是否大于所有的物联网设备数
		uint8_t ips = ESP8266_Count_Free_ips();
		if(devices >= ips){
			//无法满足要求
			//连接回默认wifi
			while(!ESP8266_Connect_New_Wifi(macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd));
		}else{
			//可以满足要求
			//连接回旧wifi
			while(!ESP8266_Connect_New_Wifi(macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd));
			//通知所有物联网设备连接新wifi
			//...
		}
	}
	*/
	//等待成功连接到某个指定的wifi并获取ip地址
	if( ! ESP8266_Connect_New_Wifi(ssid, password) ){
		//失败
		//连接回默认wifi
		while(!ESP8266_Connect_New_Wifi(macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd));
	}else{  //能成功连接
		printf("start connect to old wifi\n");
		//连接回默认wifi
		ESP8266_Connect_New_Wifi(macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd);
		printf("connected to old wifi\n");
		//开启广播
		//ESP8266_startBroadCastCmd();
		
		//通知所有物联网设备连接新wifi
		//发送广播消息，让所有的物联网设备连接新wifi
		char sendDataCmd[128] = {0};
		sprintf(sendDataCmd, "CONNECT_NEW_WIFI&ssid=%s&pwd=%s", ssid, password);
		ESP8266_sendBroadcastCmd(sendDataCmd);
		printf("message all\n");
		
		//再连接回新wifi
		ESP8266_Connect_New_Wifi(ssid, password);
		printf("connected new wifi\n");
		//开启广播
		//ESP8266_startBroadCastCmd();
	}
}

void ESP8266_Lora_SenCmd(const char* str){
	HAL_UART_Transmit(&huart6, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
}

void Lora_OpenDoor(){
	ESP8266_Lora_SenCmd("Open_Door");
}




void test(){
	//测试风扇、主灯、射灯
	//ESP8266_startBroadCastCmd();
	
	//char broadcastMessage[] = "MasterLight_ON&R=255&G=255&B=255";
	//发送广播
	//ESP8266_sendBroadcastCmd(broadcastMessage);
	//ESP8266_sendBroadcastCmd("MasterLight_ON&R=255&G=255&B=255");
	/*	
	ESP8266_sendBroadcastCmd("FAN_ON");
	ESP8266_sendBroadcastCmd("MasterLight_ON&R=255&G=255&B=255");
	ESP8266_sendBroadcastCmd("SpotLight_ON&R=255&G=255&B=255");

	ESP8266_sendBroadcastCmd("FAN_OFF");
	ESP8266_sendBroadcastCmd("MasterLight_OFF");
	ESP8266_sendBroadcastCmd("SpotLight_OFF");
	*/
	
	//测试电磁门锁
	//Lora_OpenDoor();
	
	//测试wifi
	//ESP8266_Connect_New_Wifi_ALL(macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd);
	ESP8266_Connect_New_Wifi_ALL("Yunshu_Drwells", "yzy@0203yzy@0203");
}


/**
* @brief  统计连接了当前wifi的物联网设备数
 * @param ssid WiFi的SSID
 * @param password WiFi的密码 
* @retval 无
*/
/*
uint8_t ESP8266_Count_Devices(){
	return 0;
}
*/


/*
static uint8_t parseResponse(char *response)
{
  uint8_t freeIPCount = 0;
  // 解析响应，提取空闲IP地址数量
  // 这里只是一个简单的示例，实际解析逻辑可能更复杂
  char *token = strtok(response, ",");
  while (token != NULL)
  {
    if (strstr(token, "+CWLAP:") != NULL)
    {
      // 提取空闲IP地址数量
      freeIPCount = atoi(strtok(NULL, ":"));
      char countStr[10];
      sprintf(countStr, "%d", freeIPCount);
    }
    token = strtok(NULL, ",");
  }
  return freeIPCount;
}
*/

/*
static uint8_t ESP8266_WaitResponse_ForCWLAP(const char* expected_response, uint32_t timeout){
	//uint32_t startTime = HAL_GetTick();
	//while ((HAL_GetTick() - startTime) < timeout){
	TickType_t startTime = xTaskGetTickCount();
    while ((xTaskGetTickCount() - startTime) < pdMS_TO_TICKS(timeout)){
		if (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_IDLE) != RESET) { //如果Uart3接收到了Esp8266的数据
			__HAL_UART_CLEAR_IDLEFLAG(&huart3);                            //接收标志置零

			//用于调试某个AT命令的返回值
			if(!strcmp(expected_response, "OK")){
				printf("Uart3 Received data from ESP8266: %s\n", uart3_rx_buffer);  // 调试输出
				printf("Uart3 Received data length:%d\n", uart3_rx_index);
			}


			//将ESP8266的数据转发给Uart1
				if(!strcmp(expected_response, "OK")){		
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
*/


/**
* @brief  获取所有可用的wifi
 * @param ssid WiFi的SSID
 * @param password WiFi的密码 
* @retval 无
*/
/*
uint8_t ESP8266_Get_Enable_Wifi(){
    ESP8266_SendCmd("AT+CWLAP\r\n");
	uint8_t res = ESP8266_WaitResponse_ForCWLAP("OK", 5000);
    if (!res) {
        printf("Failed to get free ip or no free ip\r\n");
        return 0;
    }
	printf("Successfully get free ip\r\n");
	return res;
}
*/

/*
static int findNthOccurrence(const char *str, char ch, int n) {
    const char *ptr = str;
    int count = 0;

    while (*ptr != '\0') {
        if (*ptr == ch) {
            count++;
            if (count == n) {
                return ptr - str; // 返回相对于字符串起始位置的偏移量
            }
        }
        ptr++;
    }

    return -1; // 没有找到第n个字符
}
*/

/*
static uint8_t ESP8266_WaitResponse_ForPING(const char* expected_response, uint32_t timeout){
	//uint32_t startTime = HAL_GetTick();
	//while ((HAL_GetTick() - startTime) < timeout){
	TickType_t startTime = xTaskGetTickCount();
    while ((xTaskGetTickCount() - startTime) < pdMS_TO_TICKS(timeout)){
		if (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_IDLE) != RESET) { //如果Uart3接收到了Esp8266的数据
			__HAL_UART_CLEAR_IDLEFLAG(&huart3);                            //接收标志置零

			//用于调试某个AT命令的返回值
			if(!strcmp(expected_response, "OK")){
				printf("Uart3 Received data from ESP8266: %s\n", uart3_rx_buffer);  // 调试输出
				printf("Uart3 Received data length:%d\n", uart3_rx_index);
			}

			//将ESP8266的数据转发给Uart1
			if(!strcmp(expected_response, (const char*)uart3_rx_buffer) || strstr((const char*)uart3_rx_buffer, expected_response)) {
				if(!strcmp(expected_response, "OK")){		
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
*/

/*
static uint8_t ESP8266_WaitResponse_ForARP(const char* expected_response, uint32_t timeout){
	//uint32_t startTime = HAL_GetTick();
	//while ((HAL_GetTick() - startTime) < timeout){
	TickType_t startTime = xTaskGetTickCount();
    while ((xTaskGetTickCount() - startTime) < pdMS_TO_TICKS(timeout)){
		if (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_IDLE) != RESET) { //如果Uart3接收到了Esp8266的数据
			__HAL_UART_CLEAR_IDLEFLAG(&huart3);                            //接收标志置零

			//用于调试某个AT命令的返回值
			if(!strcmp(expected_response, "OK")){
				printf("Uart3 Received data from ESP8266: %s\n", uart3_rx_buffer);  // 调试输出
				printf("Uart3 Received data length:%d\n", uart3_rx_index);
			}

			//将ESP8266的数据转发给Uart1
			if(!strcmp(expected_response, (const char*)uart3_rx_buffer) || strstr((const char*)uart3_rx_buffer, expected_response)) {
				if(!strcmp(expected_response, "OK")){			
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
*/

/**
 * @brief  发送 ARP 请求
 * @param  ip_address: 目标 IP 地址
 * @return 成功返回 1，失败返回 0
 */
/*
int sendARPRequest(const char *ip_address) {
    char cmd[50];
    sprintf(cmd, "AT+CIPARP=\"%s\"\r\n", ip_address);
    ESP8266_SendCmd(cmd);
    return ESP8266_WaitResponse_ForARP("OK", 1000);
}
*/


/**
* @brief  统计连接的当前wifi的空闲ip数量
 * @param 无
 * @return 空闲ip数
*/
/*
uint8_t ESP8266_Count_Free_ips(){
	 // 发送AT指令执行ping命令
    //sendATCommand("AT+PING=\"www.google.com\"\r\n");
	
	//获取已经使用的ip数量
	uint8_t usedIPs = 0;
	uint8_t freeIps = 0;
	char temp_ip_address[MAX_IP_LENGTH] = {0};
	strcpy(temp_ip_address, ip_address);
	char str[4];
	//192.168.2.38
	//找到第三个.的下标
	int index = findNthOccurrence(temp_ip_address, '.', 3);
	//AT命令
	//char cmd[26] = {0};
	for(uint8_t i=0; i<255; i++){  //[0:254]
		// 使用sprintf将uint8_t转换为字符串
		sprintf(str, "%u", i);
		strncpy(temp_ip_address+index+1, str, 4);

		//printf("%s\n", temp_ip_address);
		strcpy(cmd, "AT+PING=\"");
		strcat(cmd, temp_ip_address);
		strcat(cmd, "\"\r\n");
		//printf("%s", cmd);

		ESP8266_SendCmd(cmd);  //AT+PING=\"ip\"\r\n
		if (!ESP8266_WaitResponse_ForPING("OK", 5000)) {
			freeIps++;
		}else{
			usedIPs++;
		}

		//Ping速度太慢了
		
		//采用ARP 扫描
		if (!sendARPRequest(temp_ip_address)) {
            freeIps++;
        } else {
            usedIPs++;
        }
		//速度依然很慢，因此放弃对空闲ip数量的统计
		
	}
	printf("free ip counts:%d", freeIps);
	return freeIps;
}
*/


/*
static uint8_t ESP8266_WaitResponseForCIPSTART(const char* expected_response, uint32_t timeout){
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

			//调试或者打印都无法打印正确值，printf太耗时，会导致uart3接收失败
			for(uint8_t i = 0; i<10; i++){
				printf("uart3_rx_buffer[%d]: %d\n", (uint8_t)uart3_rx_buffer[i]);
			}

			//将ESP8266的数据转发给Uart1
			if(!strcmp(expected_response, (const char*)uart3_rx_buffer) || strstr((const char*)uart3_rx_buffer, expected_response)) {
				if(!strcmp(expected_response, "OK")){
					//解析ip地址
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
*/

/*
void send_broadcast_request(void)
{
    // 发送广播请求
	ESP8266_SendCmd("AT+CIPSTART=\"TCP\",\"255.255.255.255\",80");
	uint8_t res = ESP8266_WaitResponseForCIPSTART("OK", 5000);
    if (!res) {
        printf("Failed to get free ip or no free ip\r\n");
        return 0;
    }
	printf("Successfully get free ip\r\n");
	return res;
	
    send_at_command("AT+CIPSTART=\"TCP\",\"255.255.255.255\",80");
    HAL_Delay(1000); // 等待连接建立
    send_at_command("AT+CIPSEND=0,35");
    HAL_Delay(1000); // 等待发送准备
    const char* request = "GET /get_ip?msg=GET_IP HTTP/1.1\r\nHost: 255.255.255.255\r\nConnection: close\r\n";
    HAL_UART_Transmit(&huart3, (uint8_t*)request, strlen(request), HAL_MAX_DELAY);
}
*/


