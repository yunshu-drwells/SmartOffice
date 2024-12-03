#ifndef __ICON_H
#define __ICON_H

#include "stm32f4xx.h"
#include "core_cm4.h"
#include "stm32f4xx_hal.h"

/* 图片信息保存首地址
 * 占145个字节,第1个字节用于标记字库是否存在.后续每8个字节一组,分别保存起始地址和文件大小
 */
extern uint32_t ICONINFOADDR;

/* 字库信息结构体定义
 * 用来保存字库基本信息，地址，大小等
 */
typedef __PACKED_STRUCT
{
    uint8_t icontok;             /* 图库存在标志，0XAA，图库正常；其他，图库不存在 */
    uint32_t AlarmOnaddr;          /* AlarmOn.bin的地址 */
    uint32_t AlarmOnsize;          /* AlarmOn.bin的大小 */
    uint32_t BackLightaddr;          /* BackLight.bin的地址 */
    uint32_t BackLightsize;          /* BackLight.bin的大小 */
    uint32_t blueaddr;          /* blue.bin的地址 */
    uint32_t bluesize;          /* blue.bin的大小 */
    uint32_t Brightnesssaddr;          /* Brightnesss.bin的地址 */
    uint32_t Brightnessssize;          /* Brightnesss.bin的大小 */
    uint32_t FanOnaddr;          /* FanOn.bin的地址 */
    uint32_t FanOnsize;          /* FanOn.bin的大小 */
    uint32_t Humidityaddr;          /* Humidity.bin的地址 */
    uint32_t Humiditysize;          /* Humidity.bin的大小 */
    uint32_t LightingMasterOnaddr;          /* LightingMasterOn.bin的地址 */
    uint32_t LightingMasterOnsize;          /* LightingMasterOn.bin的大小 */
    uint32_t LoRaOnaddr;          /* LoRaOn.bin的地址 */
    uint32_t LoRaOnsize;          /* LoRaOn.bin的大小 */
    uint32_t SpotlightOnaddr;          /* SpotlightOn.bin的地址 */
    uint32_t SpotlightOnsize;          /* SpotlightOn.bin的大小 */
    uint32_t Temperatureaddr;          /* Temperature.bin的地址 */
    uint32_t Temperaturesize;          /* Temperature.bin的大小 */
    uint32_t Wifiaddr;          /* Wifi.bin的地址 */
    uint32_t Wifisize;          /* Wifi.bin的大小 */
    uint32_t AlarmOffaddr;          /* AlarmOff.bin的地址 */
    uint32_t AlarmOffsize;          /* AlarmOff.bin的大小 */
    uint32_t FanOffaddr;          /* FanOff.bin的地址 */
    uint32_t FanOffsize;          /* FanOff.bin的大小 */
    uint32_t LightingMasterOffaddr;          /* LightingMasterOff.bin的地址 */
    uint32_t LightingMasterOffsize;          /* LightingMasterOff.bin的大小 */
    uint32_t LoRaOffaddr;          /* LoRaOff.bin的地址 */
    uint32_t LoRaOffsize;          /* LoRaOff.bin的大小 */
    uint32_t MainPageaddr;          /* MainPage.bin的地址 */
    uint32_t MainPagesize;          /* MainPage.bin的大小 */
    uint32_t MainPagePressedaddr;          /* MainPagePressed.bin的地址 */
    uint32_t MainPagePressedsize;          /* MainPagePressed.bin的大小 */
    uint32_t SpotlightOffaddr;          /* SpotlightOff.bin的地址 */
    uint32_t SpotlightOffsize;          /* SpotlightOff.bin的大小 */

} _icon_info;

/* 字库信息结构体 */
extern _icon_info* iconftinfo;

uint8_t icons_update_icon(uint16_t x, uint16_t y, uint8_t size, uint8_t *src, uint16_t color);  /* 更新图片 */
uint8_t icons_init(void);       /* 初始化字库 */
void erase_icon_sector(void);
#endif
