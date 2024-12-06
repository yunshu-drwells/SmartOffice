#include "string.h"
#include "lcd.h"
#include "webs.h"  //webstinfo
#include "mymalloc.h"
#include "ff.h"
#include "usart.h"
#include "delay.h"
#include "norflash.h"
#include "fonts.h"  //find_file


/* 总共2.52MB,约占646,4K一扇区) */
#define WEBSECSIZE         646


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

char *const WEB_FILE_NAME[29] =
{
	//html
    "index.html",     /* index.html*/
	//mp3
    "alarm.mp3",      /* alarm.mp3*/
	//js
    "bootstrap.min.js",      /* bootstrap.min.js*/
    "jquery-1.11.0.min.js",   /* jquery-1.11.0.min.js*/
	//images
    "01Temperature.jpg",      /* 01Temperature.jpg*/
    "02Humidity.jpg",         /* 02Humidity.jpg*/
    "03Brightnesss.jpg",      /* 03Brightnesss.jpg*/
    "04LightingMasterOn.jpg", /* 04LightingMasterOn.jpg*/
    "05SpotlightOn.jpg",      /* 05SpotlightOn.jpg*/
    "06FanOn.jpg",      /* 06FanOn.jpg*/
    "07AlarmOn.jpg",    /* 07AlarmOn.jpg*/
    "08LoRaOn.jpg",     /* 08LoRaOn.jpg*/
    "an-off.png",       /* an-off.png*/
    "an-on.png",        /* an-on.png*/
    "bg-fan-off.png",   /* bg-fan-off.png*/
    "bg-fan-on.png",    /* bg-fan-on.png*/
    "fan-off.png",      /* fan-off.png*/
    "fan-on.png",       /* fan-on.png*/
	"favicon.ico",      /* favicon.ico*/
	"left-title.png",   /* left-title.png*/
	"null.png",         /* null.png*/
	"sgbj-off.png",     /* sgbj-off.png*/
	"sgbj-on.gif",      /* sgbj-on.gif*/
	"spotlight.png",    /* spotlight.png*/
	//"sys-bg.jpg",       /* sys-bg.jpg*/
	//"sys-bg-off.jpg",   /* sys-bg-off.jpg*/
	"sys-sp.jpg",       /* sys-sp.jpg*/
	"sys-bg.jpg",       /* sys-bg.jpg*/
	"sys-bg-sp.jpg",       /* sys-bg-sp.jpg*/
	"sys-bg-sp-off.jpg",       /* sys-bg-sp-off.jpg*/
	//css
	"bootstrap.css",    /* bootstrap.css*/

};

const char* index_path = "/SYSTEM/SmartOfficeWeb/";         //index.html路径
const char* mp3_path = "/SYSTEM/SmartOfficeWeb/music/";     //mp3路径
const char* js_path = "/SYSTEM/SmartOfficeWeb/js/";         //js路径
const char* images_path = "/SYSTEM/SmartOfficeWeb/images/"; //images路径
const char* css_path = "/SYSTEM/SmartOfficeWeb/css/";       //css路径

/**
 * @brief       更新某一个文件
 * @param       x, y    : 提示信息的显示地址
 * @param       size    : 提示信息字体大小
 * @param       fpath   : 字体路径
 * @param       fx      : 更新的内容索引
 * @param       color   : 字体颜色
 * @retval      0, 成功; 其他, 错误代码;
 */
static uint8_t webs_update_filex(uint16_t x, uint16_t y, uint8_t size, uint8_t *fpath, uint8_t fx, uint16_t color)
{
    uint32_t flashaddr = 0;
    FIL *fftemp;
    uint8_t *tempbuf;
    uint8_t res;
    uint16_t bread;
    uint32_t offx = 0;
    uint8_t rval = 0;
    fftemp = (FIL *)mymalloc(SRAMIN, sizeof(FIL));  /* 锟斤拷锟斤拷锟节达拷 */
		//printf("fonts_update_fontx mymalloc FIL * successed!\n");
    if (fftemp == NULL)rval = 1;

    tempbuf = mymalloc(SRAMIN, 4096);               /* 锟斤拷锟斤拷4096锟斤拷锟街节空硷拷 */
		//printf("fonts_update_fontx mymalloc tempbuf successed!\n");
	
    if (tempbuf == NULL)rval = 1;

    res = f_open(fftemp, (const TCHAR *)fpath, FA_READ);
	//printf("fonts_update_fontx f_open return :%d\n", res);
	//printf("fftemp->obj.objsize: %u\n", fftemp->obj.objsize);
    if (res)rval = 2;   /* 打开文件失败 */
    if (rval == 0)
    {
		switch (fx)
        {
            case 0: /*  index.html */
                webstinfo->index_html_addr = WEBINFOADDR + sizeof(_webs_info);    /* 信息头之后，紧跟index.html转换码表 */
                webstinfo->index_html_size = fftemp->obj.objsize;              /* index.html的大小 */
                flashaddr = webstinfo->index_html_addr;
                break;

            case 1: /* alarm.mp3 */
                webstinfo->alarm_mp3_addr = webstinfo->index_html_addr + webstinfo->index_html_size; /* index.html 之后，紧跟 alarm.mp3 */
                webstinfo->alarm_mp3_size = fftemp->obj.objsize;             /* alarm.mp3的大小 */
                flashaddr = webstinfo->alarm_mp3_addr;                         /* alarm.mp3起始地址 */
                break;
				
			case 2: /* abootstrap.min.js */
                webstinfo->abootstrap_min_js_addr = webstinfo->alarm_mp3_addr + webstinfo->alarm_mp3_size; /* alarm.mp3 之后，紧跟 abootstrap.min.js */
                webstinfo->abootstrap_min_js_size = fftemp->obj.objsize;             /* abootstrap.min.js的大小 */
                flashaddr = webstinfo->abootstrap_min_js_addr;                         /* abootstrap.min.js起始地址 */
                break;
				
			case 3: /* jquery-1.11.0.min.js */
                webstinfo->jquery_js_addr = webstinfo->abootstrap_min_js_addr + webstinfo->abootstrap_min_js_size; /*  abootstrap.min.js之后，紧跟 jquery-1.11.0.min.js */
                webstinfo->jquery_js_size = fftemp->obj.objsize;             /* jquery-1.11.0.min.js的大小 */
                flashaddr = webstinfo->jquery_js_addr;                         /* jquery-1.11.0.min.js起始地址 */
                break;
				
			case 4: /* 01Temperature.jpg */
                webstinfo->_01Temperature_jpg_addr = webstinfo->jquery_js_addr + webstinfo->jquery_js_size; /* jquery-1.11.0.min.js 之后，紧跟 01Temperature.jpg */
                webstinfo->_01Temperature_jpg_size = fftemp->obj.objsize;             /* 01Temperature.jpg的大小 */
                flashaddr = webstinfo->_01Temperature_jpg_addr;                         /* 01Temperature.jpg起始地址 */
                break;
				
			case 5: /* 02Humidity.jpg */
                webstinfo->_02Humidity_jpg_addr = webstinfo->_01Temperature_jpg_addr + webstinfo->_01Temperature_jpg_size; /* 01Temperature.jpg 之后，紧跟 02Humidity.jpg */
                webstinfo->_02Humidity_jpg_size = fftemp->obj.objsize;             /* 02Humidity.jpg的大小 */
                flashaddr = webstinfo->_02Humidity_jpg_addr;                         /* 02Humidity.jpg起始地址 */
                break;
				
			case 6: /* 03Brightnesss.jpg */
                webstinfo->_03Brightnesss_jpg_addr = webstinfo->_02Humidity_jpg_addr + webstinfo->_02Humidity_jpg_size; /* 02Humidity.jpg 之后，紧跟 03Brightnesss.jpg */
                webstinfo->_03Brightnesss_jpg_size = fftemp->obj.objsize;             /* 03Brightnesss.jpg的大小 */
                flashaddr = webstinfo->_03Brightnesss_jpg_addr;                         /* 03Brightnesss.jpg起始地址 */
                break;
				
			case 7: /* 04LightingMasterOn.jpg */
                webstinfo->_04LightingMasterOn_jpg_addr = webstinfo->_03Brightnesss_jpg_addr + webstinfo->_03Brightnesss_jpg_size; /* 03Brightnesss.jpg 之后，紧跟 04LightingMasterOn.jpg */
                webstinfo->_04LightingMasterOn_jpg_size = fftemp->obj.objsize;             /* 04LightingMasterOn.jpg的大小 */
                flashaddr = webstinfo->_04LightingMasterOn_jpg_addr;                         /* 04LightingMasterOn.jpg起始地址 */
                break;
				
			case 8: /* 05SpotlightOn.jpg */
                webstinfo->_05SpotlightOn_jpg_addr = webstinfo->_04LightingMasterOn_jpg_addr + webstinfo->_04LightingMasterOn_jpg_size; /* 04LightingMasterOn.jpg 之后，紧跟 05SpotlightOn.jpg */
                webstinfo->_05SpotlightOn_jpg_size = fftemp->obj.objsize;             /* 05SpotlightOn.jpg的大小 */
                flashaddr = webstinfo->_05SpotlightOn_jpg_addr;                         /* 05SpotlightOn.jpg起始地址 */
                break;
				
			case 9: /* 06FanOn.jpg */
                webstinfo->_06FanOn_jpg_addr = webstinfo->_05SpotlightOn_jpg_addr + webstinfo->_05SpotlightOn_jpg_size; /* 05SpotlightOn.jpg 之后，紧跟 06FanOn.jpg */
                webstinfo->_06FanOn_jpg_size = fftemp->obj.objsize;             /* 06FanOn.jpg的大小 */
                flashaddr = webstinfo->_06FanOn_jpg_addr;                         /* 06FanOn.jpg起始地址 */
                break;
				
			case 10: /* 07AlarmOn.jpg */
                webstinfo->_07AlarmOn_jpg_addr = webstinfo->_06FanOn_jpg_addr + webstinfo->_06FanOn_jpg_size; /* 06FanOn.jpg 之后，紧跟 07AlarmOn.jpg */
                webstinfo->_07AlarmOn_jpg_size = fftemp->obj.objsize;             /* 07AlarmOn.jpg的大小 */
                flashaddr = webstinfo->_07AlarmOn_jpg_addr;                         /* 07AlarmOn.jpg起始地址 */
                break;
				
			case 11: /* 08LoRaOn.jpg */
                webstinfo->_08LoRaOn_jpg_addr = webstinfo->_07AlarmOn_jpg_addr + webstinfo->_07AlarmOn_jpg_size; /* 07AlarmOn.jpg 之后，紧跟 08LoRaOn.jpg */
                webstinfo->_08LoRaOn_jpg_size = fftemp->obj.objsize;             /* 08LoRaOn.jpg的大小 */
                flashaddr = webstinfo->_08LoRaOn_jpg_addr;                         /* 08LoRaOn.jpg起始地址 */
                break;
				
			case 12: /* an-off.png */
                webstinfo->an_off_png_addr = webstinfo->_08LoRaOn_jpg_addr + webstinfo->_08LoRaOn_jpg_size; /* 08LoRaOn.jpg 之后，紧跟 an-off.png */
                webstinfo->an_off_png_size = fftemp->obj.objsize;             /* an-off.png的大小 */
                flashaddr = webstinfo->an_off_png_addr;                         /* an-off.png起始地址 */
                break;
				
			case 13: /* an-on.png */
                webstinfo->an_on_png_addr = webstinfo->an_off_png_addr + webstinfo->an_off_png_size; /* an-off.png 之后，紧跟  an-on.png */
                webstinfo->an_on_png_size = fftemp->obj.objsize;             /*  an-on.png的大小 */
                flashaddr = webstinfo->an_on_png_addr;                         /*  an-on.png起始地址 */
                break;
				
			case 14: /* bg-fan-off.png */
                webstinfo->bg_fan_off_png_addr = webstinfo->an_on_png_addr + webstinfo->an_on_png_size; /*  an-on.png 之后，紧跟 bg-fan-off.png */
                webstinfo->bg_fan_off_png_size = fftemp->obj.objsize;             /* bg-fan-off.png的大小 */
                flashaddr = webstinfo->bg_fan_off_png_addr;                         /* bg-fan-off.png起始地址 */
                break;
				
			case 15: /* bg-fan-on.png */
                webstinfo->bg_fan_on_png_addr = webstinfo->bg_fan_off_png_addr + webstinfo->bg_fan_off_png_size; /* bg-fan-off.png 之后，紧跟 bg-fan-on.png */
                webstinfo->bg_fan_on_png_size = fftemp->obj.objsize;             /* bg-fan-on.png的大小 */
                flashaddr = webstinfo->bg_fan_on_png_addr;                         /* bg-fan-on.png起始地址 */
                break;
				
			case 16: /* fan-off.png */
                webstinfo->fan_off_png_addr = webstinfo->bg_fan_on_png_addr + webstinfo->bg_fan_on_png_size; /* bg-fan-on.png 之后，紧跟 fan-off.png */
                webstinfo->fan_off_png_size = fftemp->obj.objsize;             /* fan-off.png的大小 */
                flashaddr = webstinfo->fan_off_png_addr;                         /* fan-off.png起始地址 */
                break;
				
			case 17: /* fan-on.png */
                webstinfo->fan_on_png_addr = webstinfo->fan_off_png_addr + webstinfo->fan_off_png_size; /* fan-off.png 之后，紧跟 fan-on.png */
                webstinfo->fan_on_png_size = fftemp->obj.objsize;             /* fan-on.png的大小 */
                flashaddr = webstinfo->fan_on_png_addr;                         /* fan-on.png起始地址 */
                break;
			case 18: /* favicon.ico */
                webstinfo->favicon_ico_addr = webstinfo->fan_on_png_addr + webstinfo->fan_on_png_size; /* fan-on.png 之后，紧跟 favicon.ico */
                webstinfo->favicon_ico_size = fftemp->obj.objsize;             /* favicon.ico的大小 */
                flashaddr = webstinfo->favicon_ico_addr;                         /* favicon.ico起始地址 */
                break;
			case 19: /* left-title.png */
                webstinfo->left_title_png_addr = webstinfo->favicon_ico_addr + webstinfo->favicon_ico_size; /* favicon.ico 之后，紧跟 left-title.png */
                webstinfo->left_title_png_size = fftemp->obj.objsize;             /* left-title.png的大小 */
                flashaddr = webstinfo->left_title_png_addr;                         /* left-title.png起始地址 */
                break;
			case 20: /* null.png */
                webstinfo->null_png_addr = webstinfo->left_title_png_addr + webstinfo->left_title_png_size; /* left-title.png 之后，紧跟 null.png */
                webstinfo->null_png_size = fftemp->obj.objsize;             /* null.png的大小 */
                flashaddr = webstinfo->null_png_addr;                         /* null.png起始地址 */
                break;
			case 21: /* sgbj-off.png */
                webstinfo->sgbj_off_png_addr = webstinfo->null_png_addr + webstinfo->null_png_size; /* null.png 之后，紧跟 sgbj-off.png */
                webstinfo->sgbj_off_png_size = fftemp->obj.objsize;             /* sgbj-off.png的大小 */
                flashaddr = webstinfo->sgbj_off_png_addr;                         /* sgbj-off.png起始地址 */
                break;
			case 22: /* sgbj-on.gif */
                webstinfo->sgbj_on_gif_addr = webstinfo->sgbj_off_png_addr + webstinfo->sgbj_off_png_size; /* sgbj-off.png 之后，紧跟 sgbj-on.gif */
                webstinfo->sgbj_on_gif_size = fftemp->obj.objsize;             /* sgbj-on.gif的大小 */
                flashaddr = webstinfo->sgbj_on_gif_addr;                         /* sgbj-on.gif起始地址 */
                break;
			case 23: /* spotlight.png */
                webstinfo->spotlight_png_addr = webstinfo->sgbj_on_gif_addr + webstinfo->sgbj_on_gif_size; /* sgbj-on.gif 之后，紧跟 spotlight.png */
                webstinfo->spotlight_png_size = fftemp->obj.objsize;             /* spotlight.png的大小 */
                flashaddr = webstinfo->spotlight_png_addr;                         /* spotlight.png起始地址 */
                break;
			case 24: /* sys-sp.jpg */
                webstinfo->sys_sp_jpg_addr = webstinfo->spotlight_png_addr + webstinfo->spotlight_png_size; /* sys-bg.jpg 之后，紧跟 sys-bg-off.jpg */
                webstinfo->sys_sp_jpg_size = fftemp->obj.objsize;             /* sys-bg-off.jpg的大小 */
                flashaddr = webstinfo->sys_sp_jpg_addr;                         /* sys-bg-off.jpg起始地址 */
                break;			
			case 25: /* sys-bg.jpg */
                webstinfo->sys_bg_jpg_addr = webstinfo->sys_sp_jpg_addr + webstinfo->sys_sp_jpg_size; /* spotlight.png 之后，紧跟 sys-bg.jpg */
                webstinfo->sys_bg_jpg_size = fftemp->obj.objsize;             /* sys-bg.jpg的大小 */
                flashaddr = webstinfo->sys_bg_jpg_addr;                         /* sys-bg.jpg起始地址 */
                break;

			case 26: /* sys-bg-off.jpg */
                webstinfo->sys_bg_sp_jpg_addr = webstinfo->sys_bg_jpg_addr + webstinfo->sys_bg_jpg_size; /* sys-bg.jpg 之后，紧跟 sys-bg-off.jpg */
                webstinfo->sys_bg_sp_jpg_size = fftemp->obj.objsize;             /* sys-bg-off.jpg的大小 */
                flashaddr = webstinfo->sys_bg_sp_jpg_addr;                         /* sys-bg-off.jpg起始地址 */
                break;
			case 27: /* sys-bg-off.jpg */
                webstinfo->sys_bg_sp_off_jpg_addr = webstinfo->sys_bg_sp_jpg_addr + webstinfo->sys_bg_sp_jpg_size; /* sys-bg.jpg 之后，紧跟 sys-bg-off.jpg */
                webstinfo->sys_bg_sp_off_jpg_size = fftemp->obj.objsize;             /* sys-bg-off.jpg的大小 */
                flashaddr = webstinfo->sys_bg_sp_off_jpg_addr;                         /* sys-bg-off.jpg起始地址 */
                break;
			case 28: /* bootstrap.css */
                webstinfo->bootstrap_css_addr = webstinfo->sys_bg_sp_off_jpg_addr + webstinfo->sys_bg_sp_off_jpg_size; /* sys-bg-off.jpg 之后，紧跟 bootstrap.css */
                webstinfo->bootstrap_css_size = fftemp->obj.objsize;             /* bootstrap.css的大小 */
                flashaddr = webstinfo->bootstrap_css_addr;                         /* bootstrap.css起始地址 */
                break;				
        }

        while (res == FR_OK)   /* 死循环执行 */
        {
            res = f_read(fftemp, tempbuf, 4096, (UINT *)&bread);    /* 读取数据 */
            if (res != FR_OK)break;     /* 执行错误 */

            norflash_write(tempbuf, offx + flashaddr, bread);       /* 从0开始写入bread个数据 */
            offx += bread;
            //fonts_progress_show(x, y, size, fftemp->obj.objsize, offx, color);    /* 进度显示 */

            if (bread != 4096)break;    /* 读完了. */
            delay_ms(20);
        }

        f_close(fftemp);
    }

    myfree(SRAMIN, fftemp);     /* 释放内存 */
    myfree(SRAMIN, tempbuf);    /* 释放内存 */
    return res;
}

/**
 * @brief       更新文件入口函数
 *   @note      所有字库一起更新(UNIGBK,GBK12,GBK16,GBK24)
 * @param       x, y    : 提示信息的显示地址
 * @param       size    : 提示信息字体大小
 * @param       src     : 字库来源磁盘
 *   @arg                 "0:", SD卡
 * @param       color   : 字体颜色
 * @retval      0, 成功; 其他, 错误代码;
 */
uint8_t webs_update_files(uint16_t x, uint16_t y, uint8_t size, uint8_t *src, uint16_t color)
{
    uint8_t *pname;
    uint32_t *buf;
    uint8_t res = 0;
    uint16_t i;
    FIL *fftemp;
    uint8_t rval = 0;
    res = 0xFF;
    webstinfo->webstok = 0xFF;
	pname = mymalloc(SRAMIN, 100);                         /* 申请100字节内存 */
    buf = mymalloc(SRAMIN, 4096);                           /* 申请4K字节内存 */
    fftemp = (FIL *)mymalloc(SRAMIN, sizeof(FIL));          /* 分配内存 */

    if (buf == NULL || pname == NULL || fftemp == NULL)
    {
        myfree(SRAMIN, fftemp);
        myfree(SRAMIN, pname);
        myfree(SRAMIN, buf);
        return 5;           /* 内存申请失败 */
    }

	//读方式打开27个文件，可以同时判断文件是否存在是否损坏
    for (i = 0; i < 29; i++)
    {
        delay_ms(100);
        strcpy((char *)pname, (char *)src);                 // copy src内容到pname
		if(0 == i){  //html
			strcat((char *)pname, (char *)index_path);    // 追加具体文件路径 
		}else if(1 == i){  //mp3
			strcat((char *)pname, (char *)mp3_path); 
		}else if(i <= 3){  //js
			strcat((char *)pname, (char *)js_path);
		}else if(i<28){  //images [4:27]
			strcat((char *)pname, (char *)images_path);
		}else{  //css
			strcat((char *)pname, (char *)css_path);
		}
		strcat((char *)pname, (char *)WEB_FILE_NAME[i]);    // 追加具体文件名

		printf("open pname: %s; ", pname);

        res = f_open(fftemp, (const TCHAR *)pname, FA_READ);// 尝试打开
		printf("f_open return :%d", res);
		if (res){
            rval |= 1 << 7; // 标记打开文件失败 
            break;          // 出错了,直接退出 
        }
		//关闭文件
		//while(!f_close(fftemp));
		res = f_close(fftemp);
		//printf("f_close return :%d\n", res);
    }
	
	myfree(SRAMIN, fftemp); /* 释放内存 */

    if (rval == 0)          /* 文件都存在 */
    {
       //依次更新
		for (i = 0; i < 29; i++)
		{
			delay_ms(100);
			strcpy((char *)pname, (char *)src);                 // copy src内容到pname
			if(0 == i){  //html
				strcat((char *)pname, (char *)index_path);    // 追加具体文件路径
			}else if(1 == i){  //mp3
				strcat((char *)pname, (char *)mp3_path); 
			}else if(i <= 3){  //js
				strcat((char *)pname, (char *)js_path);
			}else if(i<28){  //images [4:27]
				strcat((char *)pname, (char *)images_path);
			}else{  //css
				strcat((char *)pname, (char *)css_path);
			}
			strcat((char *)pname, (char *)WEB_FILE_NAME[i]);    // 追加具体文件名
			printf("updating pname: %s; ", pname);
			lcd_fill(20, y, 200 + 20, 90 + 16, WHITE);
			lcd_show_string(30, y, 450, 16, 16, "Updating: ", RED);
			lcd_show_string(30+70, y, 200, 16, 16, (char *)WEB_FILE_NAME[i], RED);
			lcd_show_string(30+275, y, 20, 16, 16, "; ", RED);
			
									
            res = webs_update_filex(x + 20 * size / 2, y, size, pname, i, color);  // 更新文件
					
            if (res)
            {
				myfree(SRAMIN, buf);
                myfree(SRAMIN, pname);
				printf("updating error\n");
                return 1 + i;
            }
        }

        /* 全部更新好了 */
        webstinfo->webstok = 0xAA;
        norflash_write((uint8_t *)webstinfo, WEBINFOADDR, sizeof(_webs_info));           /* 保存字库信息 */
    }

    myfree(SRAMIN, pname);  /* 释放内存 */
	myfree(SRAMIN, buf);
    return rval;            /* 无错误. */
}


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

void erase_web_sector(){
	uint32_t *buf = mymalloc(SRAMIN, 4096);                           /* 申请4K字节内存 */
	uint16_t i, j;
	for (i = 0; i < WEBSECSIZE; i++)           // 先擦除字库区域,提高写入速度 
	{					
			norflash_read((uint8_t *)buf, ((WEBINFOADDR / 4096) + i) * 4096, 4096);    // 读出整个扇区的内容 

			for (j = 0; j < 1024; j++)              // 校验这个扇区的数据
			{
					if (buf[j] != 0xFFFFFFFF) break;    // 发现数据不为`0xFFFFFFFF`，则需要擦除该扇区 
			}

			if (j != 1024)
			{
					norflash_erase_sector((WEBINFOADDR / 4096) + i); // 需要擦除的扇区 
					//printf("norflash_erase_sector:%d\n", (FONTINFOADDR / 4096) + i);
			}
	}
	myfree(SRAMIN, buf);
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
	/*
	printf("sys_bg_jpg_addr:%u, ", wi->sys_bg_jpg_addr);
	printf("sys_bg_jpg_size:%u\n", wi->sys_bg_jpg_size);
	printf("sys_bg_off_jpg_addr:%u, ", wi->sys_bg_off_jpg_addr);
	printf("sys_bg_off_jpg_size:%u\n", wi->sys_bg_off_jpg_size);
	*/
	printf("sys_sp_jpg_addr:%u, ", wi->sys_sp_jpg_addr);
	printf("sys_sp_jpg_size:%u\n", wi->sys_sp_jpg_size);	
	printf("sys_bg_jpg_addr:%u, ", wi->sys_bg_jpg_addr);
	printf("sys_bg_jpg_size:%u\n", wi->sys_bg_jpg_size);
	printf("sys_bg_sp_jpg_addr:%u, ", wi->sys_bg_sp_jpg_addr);
	printf("sys_bg_sp_jpg_size:%u\n", wi->sys_bg_sp_jpg_size);
	printf("sys_bg_sp_off_jpg_addr:%u, ", wi->sys_bg_sp_off_jpg_addr);
	printf("sys_bg_sp_off_jpg_size:%u\n", wi->sys_bg_sp_off_jpg_size);
}
