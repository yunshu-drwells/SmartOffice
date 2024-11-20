

//使用ESP8266的AT指令通过广播的方式从STM32中发起HTTP请求来获取ESP8266控制的主灯、射灯、风扇模块的IP地址

#include "esp8266_web.h"  //ESP8266_SendCmd ESP8266_WaitResponseFor


// 发送HTTP GET请求
void ESP8266_SendHTTPRequest(const char* ip, const char* endpoint) {
	ESP8266_Enable_Client();
    char at_command[100];
    char http_request[200];
    
    // 建立TCP连接
    snprintf(at_command, sizeof(at_command), "AT+CIPSTART=0,\"TCP\",\"%s\",80", ip);
    ESP8266_SendCmd(at_command);
    if(ESP8266_WaitResponseFor("OK", 5000)){
		printf("connect tcp\r\n");
	}
	/*
	AT+CIPSTART="TCP","192.168.2.37",80\r\n请求成功后会返回
	AT+CIPSTART="TCP","192.168.2.37",80
	CONNECT

	OK
	*/
    /*  
    // 构造HTTP请求
    snprintf(http_request, sizeof(http_request), 
             "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", 
             endpoint, ip);

    // 发送HTTP请求
    snprintf(at_command, sizeof(at_command), "AT+CIPSEND=0,%d", strlen(http_request));
    ESP8266_SendCmd(at_command);
    if(ESP8266_WaitResponseFor(">", 1000)){
		printf("ok\r\n");
	}
   
    ESP8266_SendCmd(http_request);
    if(ESP8266_WaitResponseFor("GET", 5000)){
		printf("send ok\r\n");
	}

    // 等待响应并关闭连接
    if(ESP8266_WaitResponseFor("CLOSED", 10000)){
		printf("send closed\r\n");
	}
	*/
	ESP8266_Enable_SERVER();
}
