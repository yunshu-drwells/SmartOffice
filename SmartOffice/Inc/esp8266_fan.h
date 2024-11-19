

#include "stm32f4xx_hal.h"
#include "string.h"
#include "stdio.h"
#include "esp8266_http_client.h"  //ESP8266_SendHTTPRequest

// 示例：获取IP地址
void GetFanModuleIP(void) {
    ESP8266_SendHTTPRequest("192.168.2.37", "/get_ip?msg=GET_IP");
}

// 示例：打开风扇
void TurnOnFan(void) {
    ESP8266_SendHTTPRequest("192.168.2.37", "/fan_on?msg=FAN_ON");
}

// 示例：关闭风扇
void TurnOffFan(void) {
    ESP8266_SendHTTPRequest("192.168.2.37", "/fan_off?msg=FAN_OFF");
}


/*
// 示例：获取风扇模块IP
GetFanModuleIP();

// 示例：打开风扇
TurnOnFan();

HAL_Delay(5000);  // 等待5秒

// 示例：关闭风扇
TurnOffFan();
*/


