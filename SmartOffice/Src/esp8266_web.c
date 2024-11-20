#include "esp8266.h"  //ESP8266_Enable
#include "esp8266_web.h"  //enum
#include "stdint.h"
#include "usart.h"  //huart1
#include "main.h"   //RX_BUFFER_SIZE MAX_IP_LENGTH xBinarySemaphoreData
#include "string.h" //strcmp memset
#include "delay.h"  //delay_ms
#include "usart3_dma.h"  //USART3_SendData_DMA

// 定义枚举类型
typedef enum {
    TCP,
    UDP
} enumTCP;

static void ESP8266_SendCmd(const char* str){
	USART3_SendData_DMA((uint8_t*)str, strlen(str));
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

static uint8_t ESP8266_WaitResponseDMA(const char* expected_response, uint32_t timeout){
	uint32_t startTime = HAL_GetTick();
	
	//TickType_t xTicksToWait = pdMS_TO_TICKS(timeout);
    //TickType_t xStartTime = xTaskGetTickCount();
	while ((HAL_GetTick() - startTime) < timeout){
		if (dataReadyFlag) {
			//处理接收到的数据
			//用于调试某个AT命令的返回值
			//if(!strcmp(expected_response, "AT+CIFSR")){
			//	printf("Received %d bytes: %s\n", rx_index, rx_data);
			//}
			dataReadyFlag = 0;
			
			//printf("Received %d bytes: %s\n", rx_index, rx_data);
			if(!strcmp(expected_response, (const char*)uart3_rx_data) || strstr((const char*)uart3_rx_data, expected_response)) {
				if(!strcmp(expected_response, "AT+CIFSR")){
					//printf("rx_data: %s\n", (char*)uart3_rx_data);
					//解析ip地址
					/*
					AT+CIFSR
					+CIFSR:STAIP,"192.168.2.38"
					+CIFSR:STAMAC,"ec:fa:bc:8f:01:3a"

					OK
					*/
					if (extract_ip_address((const char*)uart3_rx_data, ip_address, MAX_IP_LENGTH) != NULL) {
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
				//printf("identical \n");
				return 1;
			} 
			
		}

	}
	return 0;
}

/*
发送：
AT\r\n
成功回复：
AT

OK
*/
static uint8_t ESP8266_AT_Test(void){
    const char* str = "AT\r\n";
    //HAL_UART_Transmit(&huart3, (uint8_t*)str, 4, HAL_MAX_DELAY);
	ESP8266_SendCmd(str);
	
	if (!ESP8266_WaitResponseDMA("AT\r\n\r\nOK\r\n", 200))
    {
        //printf("AT ERROR!\r\n");
        return 0;
    }
	printf("AT OK!\r\n");
	return 1;
}

/**
 * @brief 设置ESP8266为STA模式
 * @return 1 if successful, 0 if failed
 */
/*
发送：
AT+CWMODE=1\r\n
成功回复：
AT+CWMODE=1

OK
*/
static uint8_t ESP8266_SetSTAMode(void)
{
    ESP8266_SendCmd("AT+CWMODE=1\r\n");
    if (!ESP8266_WaitResponseDMA("AT+CWMODE=1\r\n\r\nOK\r\n", 1000))
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
    if (!ESP8266_WaitResponseDMA("AT+CWQAP\r\n\r\nOK\r\n", 1000))
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
    if (!ESP8266_WaitResponseDMA("AT+CWAUTOCONN=0\r\n\r\nOK\r\n", 1000))
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
uint8_t _ESP8266_JoinAP(const char* ssid, const char* password)
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
	
	if (!ESP8266_WaitResponseDMA("WIFI CONNECTED\r\n", 20000)) {
        printf("Failed to connect to AP\r\n");
        return 0;
    }

    if (!ESP8266_WaitResponseDMA("WIFI GOT IP\r\n", 10000)) {
        printf("Connection confirmation WIFI GOT IP failed\r\n");
        return 0;
    }

    if (!ESP8266_WaitResponseDMA("\r\nOK\r\n", 5000)) {
        printf("Connection confirmation OK failed\r\n");
        return 0;
    }
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
    if(!_ESP8266_JoinAP(ssid, password)){
		printf("Failed to connect AP\r\n");
		return 0;
	}
	printf("Successfully connected to AP\r\n");
	return 1;
}

/**
 * @brief 获取ESP8266连接wifi所分配ip
 * @return 1 if successful, 0 if failed
 */
uint8_t ESP8266_GetIp()
{
	// 3. 查询IP地址以确认连接
    ESP8266_SendCmd("AT+CIFSR\r\n");
    if (!ESP8266_WaitResponseDMA("AT+CIFSR", 5000)) {
        printf("Failed to get IP address\r\n");
        return 0;
    }
    
    printf("Successfully get IP address\r\n");
    return 1;
}

/**
 * @brief 开启或关闭多连接
 * @return 1 if successful, 0 if failed
 */
static uint8_t ESP8266_Enable_MultipleId(int enable) {
    char command[20] = {0};
	snprintf(command, sizeof(command), "AT+CIPMUX=%d\r\n", enable);
    //printf("command %s\n", command);
    
    ESP8266_SendCmd(command);
	snprintf(command, sizeof(command), "AT+CIPMUX=%d\r\n\r\nOK\r\n", enable);
    // 等待响应
    if (!ESP8266_WaitResponseDMA(command, 5000)) {
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
/*
AT+CIPSERVER=1,80
*/
static uint8_t _ESP8266_Enable_SERVER() {
    ESP8266_SendCmd("AT+CIPSERVER=1,80\r\n");
    // 等待响应 首次设置和第二次设置成功返回字符是不同的
    if (!ESP8266_WaitResponseDMA("AT+CIPSERVER=1,80\r\n\r\nOK\r\n", 5000)){
		ESP8266_SendCmd("AT+CIPSERVER=1,80\r\n");
		if (!ESP8266_WaitResponseDMA("AT+CIPSERVER=1,80\r\nno change\r\n\r\nOK\r\n", 5000)) {
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
    if (!ESP8266_WaitResponseDMA("AT+CIPSTO=60\r\n\r\nOK\r\n", 5000)) {
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

	//等待成功连接到某个指定的wifi
	while( ! ESP8266_JoinAP(ssid, password) );

	//获取ip地址
	while( ! ESP8266_GetIp() );

	//等待设置ESP8266多连接成功
	while( ! ESP8266_Enable_MultipleId(1) );
	
	//等待设置为服务器模式成功
	while( ! _ESP8266_Enable_SERVER() );
	
	//等待设置服务器超时时间成功
	while( ! ESP8266_Enable_STO() );
}

/**
* @brief  ESP8266 判断是不是web请求并处理网络请求数据
* @param  无
* @retval 无
*/
void ESP8266_CheckRecvData(void)
{
	if (dataReadyFlag) {
		printf("Received %d bytes: %s\n", rx_index, uart3_rx_data);
		dataReadyFlag = 0;
	}
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
    if (!ESP8266_WaitResponseDMA("AT+CIPSERVER=0\r\n\r\nOK\r\n", 5000)){
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

uint8_t ESP8266_WaitResponseFor(const char* expected_response, uint32_t timeout){
	return 0;
}

/**
* @brief  ESP8266 连接新wifi函数
 * @param ssid WiFi的SSID
 * @param password WiFi的密码 
* @retval 无
*/
uint8_t ESP8266_Connect_New_Wifi(const char* ssid, const char* password)
{
	//在连接新wifi之前统计所有物联网设备数
	//uint8_t devices = ESP8266_Count_Devices();
	//等待断开可能存在的WiFi连接成功
	while( ! ESP8266_CWQAP() );
	//等待禁用自动连接成功
	while( ! ESP8266_CWAUTOCONN() );
	//等待成功连接到某个指定的wifi并获取ip地址
	if( ! ESP8266_JoinAP(ssid, password) ){
		//失败
		return 0;
	}else{  //能成功连接
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
		//连接回默认wifi
		while(!ESP8266_Connect_New_Wifi(macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd));
		//通知所有物联网设备连接新wifi
		//。。。
		//再连接回新wifi
		while(!ESP8266_Connect_New_Wifi(ssid, password));
	}
}


#include "esp8266_fan.h"  //GetFanModuleIP

void test(){
	//while( ! ESP8266_Count_Free_ips() );
	//获取所有物联网设备的ip地址
	GetFanModuleIP();
	//TurnOnFan();
	//TurnOffFan();
}



