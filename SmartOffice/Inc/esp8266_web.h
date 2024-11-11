#include <stdio.h>
#include <string.h>

// 定义枚举类型
typedef enum {
    TCP,
    UDP
} enumTCP;

// 定义Single_ID_0，假设它是一个常量
#define Single_ID_0 0

void ESP8266_Connect_Wifi(const char* ssid, const char* password);
void ESP8266_CheckRecvData(void);
