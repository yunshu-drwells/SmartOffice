#ifndef __ICON_READ_H
#define __ICON_READ_H

#include "norflash.h"  //norflash_read
#include "icon.h"  //iconftinfo
#include "mymalloc.h"  //mymalloc
#include <string.h>  //memset
#include "usart.h"  //fputc
#include "GUI.h"  //GUI_BITMAP、GUI_DRAW_BMP565

unsigned char *_AlarmOn;
unsigned char *_BackLight;
unsigned char *_blue;
unsigned char *_Brightnesss;
unsigned char *_FanOn;
unsigned char *_Humidity;
unsigned char *_LightingMasterOn;
unsigned char *_LoRaOn;
unsigned char *_SpotlightOn;
unsigned char *_Temperature;
unsigned char *_Wifi;
unsigned char *_AlarmOff;
unsigned char *_FanOff;
unsigned char *_LightingMasterOff;
unsigned char *_LoRaOff;
unsigned char *_MainPage;
unsigned char *_MainPagePressed;
unsigned char *_SpotlightOff;

extern _icon_info* iconftinfo;  //main

// 位图结构体 
GUI_BITMAP bmAlarm;
GUI_BITMAP bmTurnOff;
GUI_BITMAP bmblue;
GUI_BITMAP bmBrightnesss;
GUI_BITMAP bmFanOn;
GUI_BITMAP bmHumidity;
GUI_BITMAP bmLightingMasterOn;
//LoRaOn
GUI_BITMAP bmSpotlightOn;
GUI_BITMAP bmTemperature;
//Wifi
GUI_BITMAP bmAlarmOff;
GUI_BITMAP bmFanOff;
GUI_BITMAP bmLightingMasterOff;  //重新启用
//LoRaOff
GUI_BITMAP bmMainPage;
GUI_BITMAP bmMainPagePressed;
GUI_BITMAP bmSpotlightOff;

void read_icons(){
	//加载AlarmOn.bin
	//在外扩SRAM中开辟空间
	_AlarmOn = (unsigned char *)mymalloc(2, iconftinfo->AlarmOnsize);
	while(NULL == _AlarmOn){
		_AlarmOn = (unsigned char *)mymalloc(2, iconftinfo->AlarmOnsize);
	}
	//对开辟的空间初始化
	memset(_AlarmOn, 0, iconftinfo->AlarmOnsize);
	//从NORFlash读取到开辟的空间中
	norflash_read((uint8_t *)_AlarmOn, iconftinfo->AlarmOnaddr, iconftinfo->AlarmOnsize);
	
	//加载BackLight
	_BackLight = (unsigned char *)mymalloc(2, iconftinfo->BackLightsize);
	while(NULL == _BackLight){
		_BackLight = (unsigned char *)mymalloc(2, iconftinfo->BackLightsize);
	}
	memset(_BackLight, 0, iconftinfo->BackLightsize);
	norflash_read((uint8_t *)_BackLight, iconftinfo->BackLightaddr, iconftinfo->BackLightsize);
	
	//加载blue
	_blue = (unsigned char *)mymalloc(2, iconftinfo->bluesize);
	while(NULL == _blue){
		_blue = (unsigned char *)mymalloc(2, iconftinfo->bluesize);
	}
	memset(_blue, 0, iconftinfo->bluesize);
	norflash_read((uint8_t *)_blue, iconftinfo->blueaddr, iconftinfo->bluesize);
	
	//加载Brightnesss
	_Brightnesss = (unsigned char *)mymalloc(2, iconftinfo->Brightnessssize);
	while(NULL == _Brightnesss){
		_Brightnesss = (unsigned char *)mymalloc(2, iconftinfo->Brightnessssize);
	}
	memset(_Brightnesss, 0, iconftinfo->Brightnessssize);
	norflash_read((uint8_t *)_Brightnesss, iconftinfo->Brightnesssaddr, iconftinfo->Brightnessssize);
	
	//加载FanOn
	_FanOn = (unsigned char *)mymalloc(2, iconftinfo->FanOnsize);
	while(NULL == _FanOn){
		_FanOn = (unsigned char *)mymalloc(2, iconftinfo->FanOnsize);
	}
	memset(_FanOn, 0, iconftinfo->FanOnsize);
	norflash_read((uint8_t *)_FanOn, iconftinfo->FanOnaddr, iconftinfo->FanOnsize);
	
	//加载Humidity
	_Humidity = (unsigned char *)mymalloc(2, iconftinfo->Humiditysize);
	while(NULL == _Humidity){
		_Humidity = (unsigned char *)mymalloc(2, iconftinfo->Humiditysize);
	}
	memset(_Humidity, 0, iconftinfo->Humiditysize);
	norflash_read((uint8_t *)_Humidity, iconftinfo->Humidityaddr, iconftinfo->Humiditysize);
	
	//加载LightingMasterOn
	_LightingMasterOn = (unsigned char *)mymalloc(2, iconftinfo->LightingMasterOnsize);
	while(NULL == _LightingMasterOn){
		_LightingMasterOn = (unsigned char *)mymalloc(2, iconftinfo->LightingMasterOnsize);
	}
	memset(_LightingMasterOn, 0, iconftinfo->LightingMasterOnsize);
	norflash_read((uint8_t *)_LightingMasterOn, iconftinfo->LightingMasterOnaddr, iconftinfo->LightingMasterOnsize);
	
	//加载LoRaOn
	_LoRaOn = (unsigned char *)mymalloc(2, iconftinfo->LoRaOnsize);
	while(NULL == _LoRaOn){
		_LoRaOn = (unsigned char *)mymalloc(2, iconftinfo->LoRaOnsize);
	}
	memset(_LoRaOn, 0, iconftinfo->LoRaOnsize);
	norflash_read((uint8_t *)_LoRaOn, iconftinfo->LoRaOnaddr, iconftinfo->LoRaOnsize);
	
	//加载SpotlightOn
	_SpotlightOn = (unsigned char *)mymalloc(2, iconftinfo->SpotlightOnsize);
	while(NULL == _SpotlightOn){
		_SpotlightOn = (unsigned char *)mymalloc(2, iconftinfo->SpotlightOnsize);
	}
	memset(_SpotlightOn, 0, iconftinfo->SpotlightOnsize);
	norflash_read((uint8_t *)_SpotlightOn, iconftinfo->SpotlightOnaddr, iconftinfo->SpotlightOnsize);
	
	//加载Temperature
	_Temperature = (unsigned char *)mymalloc(2, iconftinfo->Temperaturesize);
	while(NULL == _Temperature){
		_Temperature = (unsigned char *)mymalloc(2, iconftinfo->Temperaturesize);
	}
	memset(_Temperature, 0, iconftinfo->Temperaturesize);
	norflash_read((uint8_t *)_Temperature, iconftinfo->Temperatureaddr, iconftinfo->Temperaturesize);
	
	//加载Wifi
	_Wifi = (unsigned char *)mymalloc(2, iconftinfo->Wifisize);
	while(NULL == _Wifi){
		_Wifi = (unsigned char *)mymalloc(2, iconftinfo->Wifisize);
	}
	memset(_Wifi, 0, iconftinfo->Wifisize);
	norflash_read((uint8_t *)_Wifi, iconftinfo->Wifiaddr, iconftinfo->Wifisize);
	
	//加载AlarmOff
	_AlarmOff = (unsigned char *)mymalloc(2, iconftinfo->AlarmOffsize);
	while(NULL == _AlarmOff){
		_AlarmOff = (unsigned char *)mymalloc(2, iconftinfo->AlarmOffsize);
	}
	memset(_AlarmOff, 0, iconftinfo->AlarmOffsize);
	norflash_read((uint8_t *)_AlarmOff, iconftinfo->AlarmOffaddr, iconftinfo->AlarmOffsize);
	
	//加载FanOff
	_FanOff = (unsigned char *)mymalloc(2, iconftinfo->FanOffsize);
	while(NULL == _FanOff){
		_FanOff = (unsigned char *)mymalloc(2, iconftinfo->FanOffsize);
	}
	memset(_FanOff, 0, iconftinfo->FanOffsize);
	norflash_read((uint8_t *)_FanOff, iconftinfo->FanOffaddr, iconftinfo->FanOffsize);
	
	//加载LightingMasterOff
	_LightingMasterOff = (unsigned char *)mymalloc(2, iconftinfo->LightingMasterOffsize);
	while(NULL == _LightingMasterOff){
		_LightingMasterOff = (unsigned char *)mymalloc(2, iconftinfo->LightingMasterOffsize);
	}
	memset(_LightingMasterOff, 0, iconftinfo->LightingMasterOffsize);
	norflash_read((uint8_t *)_LightingMasterOff, iconftinfo->LightingMasterOffaddr, iconftinfo->LightingMasterOffsize);
	
	//加载LoRaOff
	_LoRaOff = (unsigned char *)mymalloc(2, iconftinfo->LoRaOffsize);
	while(NULL == _LoRaOff){
		_LoRaOff = (unsigned char *)mymalloc(2, iconftinfo->LoRaOffsize);
	}
	memset(_LoRaOff, 0, iconftinfo->LoRaOffsize);
	norflash_read((uint8_t *)_LoRaOff, iconftinfo->LoRaOffaddr, iconftinfo->LoRaOffsize);
	
	//加载MainPage
	_MainPage = (unsigned char *)mymalloc(2, iconftinfo->MainPagesize);
	while(NULL == _MainPage){
		_MainPage = (unsigned char *)mymalloc(2, iconftinfo->MainPagesize);
	}
	memset(_MainPage, 0, iconftinfo->MainPagesize);
	norflash_read((uint8_t *)_MainPage, iconftinfo->MainPageaddr, iconftinfo->MainPagesize);
	
	//加载MainPagePressed
	_MainPagePressed = (unsigned char *)mymalloc(2, iconftinfo->MainPagePressedsize);
	while(NULL == _MainPagePressed){
		_MainPagePressed = (unsigned char *)mymalloc(2, iconftinfo->MainPagePressedsize);
	}
	memset(_MainPagePressed, 0, iconftinfo->MainPagePressedsize);
	norflash_read((uint8_t *)_MainPagePressed, iconftinfo->MainPagePressedaddr, iconftinfo->MainPagePressedsize);
	
	//加载SpotlightOff
	_SpotlightOff = (unsigned char *)mymalloc(2, iconftinfo->SpotlightOffsize);
	while(NULL == _SpotlightOff){
		_SpotlightOff = (unsigned char *)mymalloc(2, iconftinfo->SpotlightOffsize);
	}
	memset(_SpotlightOff, 0, iconftinfo->SpotlightOffsize);
	norflash_read((uint8_t *)_SpotlightOff, iconftinfo->SpotlightOffaddr, iconftinfo->SpotlightOffsize);
	
}

void InitDynamicImage(void) {
    // 初始化位图结构体
    bmAlarm.XSize = 144;
    bmAlarm.YSize = 144;
    bmAlarm.BytesPerLine = 288; // 144像素 * 2字节/像素
    bmAlarm.BitsPerPixel = 16;
    bmAlarm.pData = _AlarmOn;
    bmAlarm.pPal = NULL;
    bmAlarm.pMethods = GUI_DRAW_BMP565;
	
	bmTurnOff.XSize = 150;
    bmTurnOff.YSize = 132;
    bmTurnOff.BytesPerLine = 288; // 144像素 * 2字节/像素
    bmTurnOff.BitsPerPixel = 16;
    bmTurnOff.pData = _BackLight;
    bmTurnOff.pPal = NULL;
    bmTurnOff.pMethods = GUI_DRAW_BMP565;
	
	bmblue.XSize = 144;
    bmblue.YSize = 144;
    bmblue.BytesPerLine = 288; // 144像素 * 2字节/像素
    bmblue.BitsPerPixel = 16;
    bmblue.pData = _blue;
    bmblue.pPal = NULL;
    bmblue.pMethods = GUI_DRAW_BMP565;
	
	bmBrightnesss.XSize = 150;
    bmBrightnesss.YSize = 150;
    bmBrightnesss.BytesPerLine = 288; // 144像素 * 2字节/像素
    bmBrightnesss.BitsPerPixel = 16;
    bmBrightnesss.pData = _Brightnesss;
    bmBrightnesss.pPal = NULL;
    bmBrightnesss.pMethods = GUI_DRAW_BMP565;
	
	bmFanOn.XSize = 150;
    bmFanOn.YSize = 150;
    bmFanOn.BytesPerLine = 288; // 144像素 * 2字节/像素
    bmFanOn.BitsPerPixel = 16;
    bmFanOn.pData = _FanOn;
    bmFanOn.pPal = NULL;
    bmFanOn.pMethods = GUI_DRAW_BMP565;
	
	bmHumidity.XSize = 144;
    bmHumidity.YSize = 144;
    bmHumidity.BytesPerLine = 288; // 144像素 * 2字节/像素
    bmHumidity.BitsPerPixel = 16;
    bmHumidity.pData = _Humidity;
    bmHumidity.pPal = NULL;
    bmHumidity.pMethods = GUI_DRAW_BMP565;
	
	bmLightingMasterOn.XSize = 150;
    bmLightingMasterOn.YSize = 150;
    bmLightingMasterOn.BytesPerLine = 288; // 144像素 * 2字节/像素
    bmLightingMasterOn.BitsPerPixel = 16;
    bmLightingMasterOn.pData = _LightingMasterOn;
    bmLightingMasterOn.pPal = NULL;
    bmLightingMasterOn.pMethods = GUI_DRAW_BMP565;
	
	bmSpotlightOn.XSize = 144;
    bmSpotlightOn.YSize = 144;
    bmSpotlightOn.BytesPerLine = 288; // 144像素 * 2字节/像素
    bmSpotlightOn.BitsPerPixel = 16;
    bmSpotlightOn.pData = _SpotlightOn;
    bmSpotlightOn.pPal = NULL;
    bmSpotlightOn.pMethods = GUI_DRAW_BMP565;
	
	bmTemperature.XSize = 144;
    bmTemperature.YSize = 144;
    bmTemperature.BytesPerLine = 288; // 144像素 * 2字节/像素
    bmTemperature.BitsPerPixel = 16;
    bmTemperature.pData = _Temperature;
    bmTemperature.pPal = NULL;
    bmTemperature.pMethods = GUI_DRAW_BMP565;
	
	bmAlarmOff.XSize = 150;
    bmAlarmOff.YSize = 150;
    bmAlarmOff.BytesPerLine = 288; // 144像素 * 2字节/像素
    bmAlarmOff.BitsPerPixel = 16;
    bmAlarmOff.pData = _AlarmOff;
    bmAlarmOff.pPal = NULL;
    bmAlarmOff.pMethods = GUI_DRAW_BMP565;
	
	bmFanOff.XSize = 150;
    bmFanOff.YSize = 150;
    bmFanOff.BytesPerLine = 288; // 144像素 * 2字节/像素
    bmFanOff.BitsPerPixel = 16;
    bmFanOff.pData = _FanOff;
    bmFanOff.pPal = NULL;
    bmFanOff.pMethods = GUI_DRAW_BMP565;

	bmLightingMasterOff.XSize = 150;
    bmLightingMasterOff.YSize = 150;
    bmLightingMasterOff.BytesPerLine = 288; // 144像素 * 2字节/像素
    bmLightingMasterOff.BitsPerPixel = 16;
    bmLightingMasterOff.pData = _LightingMasterOff;
    bmLightingMasterOff.pPal = NULL;
    bmLightingMasterOff.pMethods = GUI_DRAW_BMP565;
	
	bmMainPage.XSize = 150;
    bmMainPage.YSize = 150;
    bmMainPage.BytesPerLine = 288; // 144像素 * 2字节/像素
    bmMainPage.BitsPerPixel = 16;
    bmMainPage.pData = _MainPage;
    bmMainPage.pPal = NULL;
    bmMainPage.pMethods = GUI_DRAW_BMP565;
	
	bmMainPagePressed.XSize = 150;
    bmMainPagePressed.YSize = 150;
    bmMainPagePressed.BytesPerLine = 288; // 144像素 * 2字节/像素
    bmMainPagePressed.BitsPerPixel = 16;
    bmMainPagePressed.pData = _MainPagePressed;
    bmMainPagePressed.pPal = NULL;
    bmMainPagePressed.pMethods = GUI_DRAW_BMP565;
	
	bmSpotlightOff.XSize = 144;
    bmSpotlightOff.YSize = 144;
    bmSpotlightOff.BytesPerLine = 288; // 144像素 * 2字节/像素
    bmSpotlightOff.BitsPerPixel = 16;
    bmSpotlightOff.pData = _SpotlightOff;
    bmSpotlightOff.pPal = NULL;
    bmSpotlightOff.pMethods = GUI_DRAW_BMP565;
}

#endif
