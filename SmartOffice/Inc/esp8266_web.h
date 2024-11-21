#include <stdio.h>
#include <stdint.h>
#include <string.h>



// 定义Single_ID_0，假设它是一个常量
#define Single_ID_0 0

extern char ip_address[];

void ESP8266_SendCmd(const char* str);
void ESP8266_Connect_Wifi(const char* ssid, const char* password);
void ESP8266_startBroadCastCmd(void);
void ESP8266_sendBroadcastCmd(char* broadcastMessage);
void test(void);
//void ESP8266_Enable_SERVER(void);
//void ESP8266_Enable_Client(void);
void ESP8266_CheckRecvData(void);

