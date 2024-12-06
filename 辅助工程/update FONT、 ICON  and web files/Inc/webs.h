#ifndef __WEBS_H
#define __WEBS_H

#include "stm32f4xx.h"
#include "core_cm4.h"
#include "stm32f4xx_hal.h"


extern uint32_t WEBINFOADDR;


typedef __PACKED_STRUCT
{
    uint8_t webstok;
	//html
    uint32_t index_html_addr;          //index.html
    uint32_t index_html_size;
	//mp3       
    uint32_t alarm_mp3_addr;           //alarm.mp3
    uint32_t alarm_mp3_size;
	//js           
	uint32_t abootstrap_min_js_addr;           //abootstrap.min.js
    uint32_t abootstrap_min_js_size;
	uint32_t jquery_js_addr;           //jquery-1.11.0.min.js
    uint32_t jquery_js_size;
	//image
	uint32_t _01Temperature_jpg_addr;           //01Temperature.jpg
    uint32_t _01Temperature_jpg_size;
	uint32_t _02Humidity_jpg_addr;           //02Humidity.jpg
    uint32_t _02Humidity_jpg_size;
	uint32_t _03Brightnesss_jpg_addr;           //03Brightnesss.jpg
    uint32_t _03Brightnesss_jpg_size;
	uint32_t _04LightingMasterOn_jpg_addr;           //04LightingMasterOn.jpg
    uint32_t _04LightingMasterOn_jpg_size;
	uint32_t _05SpotlightOn_jpg_addr;           //05SpotlightOn.jpg
    uint32_t _05SpotlightOn_jpg_size;
	uint32_t _06FanOn_jpg_addr;           //06FanOn.jpg
    uint32_t _06FanOn_jpg_size;
	uint32_t _07AlarmOn_jpg_addr;           //07AlarmOn.jpg
    uint32_t _07AlarmOn_jpg_size;
	uint32_t _08LoRaOn_jpg_addr;           //08LoRaOn.jpg
    uint32_t _08LoRaOn_jpg_size;
	uint32_t an_off_png_addr;           //an-off.png
    uint32_t an_off_png_size;
	uint32_t an_on_png_addr;           //an-on.png
    uint32_t an_on_png_size;
	uint32_t bg_fan_off_png_addr;           //bg-fan-off.png
    uint32_t bg_fan_off_png_size;
	uint32_t bg_fan_on_png_addr;           //bg-fan-on.png
    uint32_t bg_fan_on_png_size;
	uint32_t fan_off_png_addr;           //fan-off.png
    uint32_t fan_off_png_size;
	uint32_t fan_on_png_addr;           //fan-on.png
    uint32_t fan_on_png_size;
    uint32_t favicon_ico_addr;         //favicon.ico
    uint32_t favicon_ico_size;
	uint32_t left_title_png_addr;           //left-title.png
    uint32_t left_title_png_size;
	uint32_t null_png_addr;           //null.png
    uint32_t null_png_size;
	uint32_t sgbj_off_png_addr;           //sgbj-off.png
    uint32_t sgbj_off_png_size;
	uint32_t sgbj_on_gif_addr;           //sgbj-on.gif
    uint32_t sgbj_on_gif_size;
	uint32_t spotlight_png_addr;           //spotlight.png
    uint32_t spotlight_png_size;
	/*
	uint32_t sys_bg_jpg_addr;           //sys-bg.jpg
    uint32_t sys_bg_jpg_size;
	uint32_t sys_bg_off_jpg_addr;           //sys-bg-off.jpg
    uint32_t sys_bg_off_jpg_size;
	*/
	uint32_t sys_sp_jpg_addr;           //sys-sp.jpg
    uint32_t sys_sp_jpg_size;
	uint32_t sys_bg_jpg_addr;           //sys-bg.jpg
    uint32_t sys_bg_jpg_size;
	uint32_t sys_bg_sp_jpg_addr;           //sys-bg-sp.jpg
    uint32_t sys_bg_sp_jpg_size;
	uint32_t sys_bg_sp_off_jpg_addr;           //sys-bg-sp-off.jpg
    uint32_t sys_bg_sp_off_jpg_size;
	//css
	uint32_t bootstrap_css_addr;           //bootstrap.css
    uint32_t bootstrap_css_size;
} _webs_info;


extern _webs_info* webstinfo;

uint8_t webs_init(void);
uint8_t webs_update_files(uint16_t x, uint16_t y, uint8_t size, uint8_t *src, uint16_t color);
void erase_web_sector(void);
void show_webs_info(_webs_info* wi);
#endif
