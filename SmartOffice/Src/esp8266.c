#include "esp8266.h"
#include "gpio.h"

// ¿ØÖÆº¯Êý
void ESP8266_Enable(void) {
    HAL_GPIO_WritePin(CH_PD_GPIO_Port, CH_PD_Pin, GPIO_PIN_SET);
}

void ESP8266_Disable(void) {
    HAL_GPIO_WritePin(CH_PD_GPIO_Port, CH_PD_Pin, GPIO_PIN_RESET);
}

void ESP8266_Reset(void) {
    HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(100);  // ÑÓÊ±100ms
    HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_SET);
}
