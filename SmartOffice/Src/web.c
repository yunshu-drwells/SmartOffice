#include "web.h"
#include "ff.h"
#include "delay.h"
#include "norflash.h"
#include "usart.h"

/*
 * 从第7MB地址开始存放web相关的文件
 * 前面6MB大小被文件系统占用
 * 6MB后紧跟web相关文件
 * 10MB后紧跟18个**.bin,总大小762KB, 190个扇区,被图库占用了,不能动!
 * 12MB后紧跟3个字库+UNIGBK.BIN+asc**.bin,总大小3.11M, 791+5个扇区,被字库占用了,不能动!
 * 以及15.09M以后, 用户可以自由使用. 建议用最后的100K字节比较好
 * 15.09M以后, 用户可以自由使用. 建议用最后的100K字节比较好
 * 
 */
#define WEBINFOADDR        7 * 1024 * 1024

uint8_t webs_init(void)
{
    uint8_t t = 0;

    while (t < 10)  /* 连续读取10次,都是错误,说明确实是有问题,得更新字库了 */
    {
        t++;
        norflash_read((uint8_t *)webstinfo, WEBINFOADDR, sizeof(_webs_info));    /* 读出ftinfo结构体数据 */

        if (webstinfo->webstok == 0XAA)
        {
            break;
        }
        
        delay_ms(20);
    }

    if (webstinfo->webstok != 0XAA)
    {
		//printf("return 1\n");
        return 1;
    }
    //printf("return 0\n");
    return 0;
}


void show_webs_info(_webs_info* wi){
	printf("index_html_addr:%u, ", wi->index_html_addr);
	printf("index_html_size:%u\n", wi->index_html_size);
	printf("alarm_mp3_addr:%u, ", wi->alarm_mp3_addr);
	printf("alarm_mp3_size:%u\n", wi->alarm_mp3_size);
	printf("abootstrap_min_js_addr:%u, ", wi->abootstrap_min_js_addr);
	printf("abootstrap_min_js_size:%u\n", wi->abootstrap_min_js_size);
	printf("jquery_js_addr:%u, ", wi->jquery_js_addr);
	printf("jquery_js_size:%u\n", wi->jquery_js_size);
	printf("_01Temperature_jpg_addr:%u, ", wi->_01Temperature_jpg_addr);
	printf("_01Temperature_jpg_size:%u\n", wi->_01Temperature_jpg_size);
	printf("_02Humidity_jpg_addr:%u, ", wi->_02Humidity_jpg_addr);
	printf("_02Humidity_jpg_size:%u\n", wi->_02Humidity_jpg_size);
	printf("_03Brightnesss_jpg_addr:%u, ", wi->_03Brightnesss_jpg_addr);
	printf("_03Brightnesss_jpg_size:%u\n", wi->_03Brightnesss_jpg_size);
	printf("_04LightingMasterOn_jpg_addr:%u, ", wi->_04LightingMasterOn_jpg_addr);
	printf("_04LightingMasterOn_jpg_size:%u\n", wi->_04LightingMasterOn_jpg_size);
	printf("_05SpotlightOn_jpg_addr:%u, ", wi->_05SpotlightOn_jpg_addr);
	printf("_05SpotlightOn_jpg_size:%u\n", wi->_05SpotlightOn_jpg_size);
	printf("_06FanOn_jpg_addr:%u, ", wi->_06FanOn_jpg_addr);
	printf("_06FanOn_jpg_size:%u\n", wi->_06FanOn_jpg_size);
	printf("_07AlarmOn_jpg_addr:%u, ", wi->_07AlarmOn_jpg_addr);
	printf("_07AlarmOn_jpg_size:%u\n", wi->_07AlarmOn_jpg_size);
	printf("_08LoRaOn_jpg_addr:%u, ", wi->_08LoRaOn_jpg_addr);
	printf("_08LoRaOn_jpg_size:%u\n", wi->_08LoRaOn_jpg_size);
	printf("an_off_png_addr:%u, ", wi->an_off_png_addr);
	printf("an_off_png_size:%u\n", wi->an_off_png_size);
	printf("an_on_png_addr:%u, ", wi->an_on_png_addr);
	printf("an_on_png_size:%u\n", wi->an_on_png_size);
	printf("bg_fan_off_png_addr:%u, ", wi->bg_fan_off_png_addr);
	printf("bg_fan_off_png_size:%u\n", wi->bg_fan_off_png_size);	
	printf("bg_fan_on_png_addr:%u, ", wi->bg_fan_on_png_addr);
	printf("bg_fan_on_png_size:%u\n", wi->bg_fan_on_png_size);
	printf("fan_off_png_addr:%u, ", wi->fan_off_png_addr);
	printf("fan_off_png_size:%u\n", wi->fan_off_png_size);
	printf("fan_on_png_addr:%u, ", wi->fan_on_png_addr);
	printf("fan_on_png_size:%u\n", wi->fan_on_png_size);
	printf("favicon_ico_addr:%u, ", wi->favicon_ico_addr);
	printf("favicon_ico_size:%u\n", wi->favicon_ico_size);
	printf("left_title_png_addr:%u, ", wi->left_title_png_addr);
	printf("left_title_png_size:%u\n", wi->left_title_png_size);
	printf("null_png_addr:%u, ", wi->null_png_addr);
	printf("null_png_size:%u\n", wi->null_png_size);
	printf("sgbj_off_png_addr:%u, ", wi->sgbj_off_png_addr);
	printf("sgbj_off_png_size:%u\n", wi->sgbj_off_png_size);
	printf("sgbj_on_gif_addr:%u, ", wi->sgbj_on_gif_addr);
	printf("sgbj_on_gif_size:%u\n", wi->sgbj_on_gif_size);
	printf("spotlight_png_addr:%u, ", wi->spotlight_png_addr);
	printf("spotlight_png_size:%u\n", wi->spotlight_png_size);
	printf("sys_bg_jpg_addr:%u, ", wi->sys_bg_jpg_addr);
	printf("sys_bg_jpg_size:%u\n", wi->sys_bg_jpg_size);
	printf("sys_bg_off_jpg_addr:%u, ", wi->sys_bg_off_jpg_addr);
	printf("sys_bg_off_jpg_size:%u\n", wi->sys_bg_off_jpg_size);	
}
