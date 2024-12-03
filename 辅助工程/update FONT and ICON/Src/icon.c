#include "string.h"
#include "lcd.h"
#include "icon.h"
#include "mymalloc.h"
#include "ff.h"
#include "usart.h"
#include "delay.h"
#include "norflash.h"
#include "fonts.h"  //find_file

/* 图库区域占用的总扇区数大小(18个**.bin=795KB,约占198个25QXX扇区,一个扇区4K字节) */
#define ICONSECSIZE         198

/* 图库存放起始地址
 * 从第10MB地址开始存放图库
 * 前面10MB大小被文件系统占用
 * 10MB后紧跟18个**.bin,总大小762KB, 190个扇区,被图库占用了,不能动!
 * 12MB后紧跟3个字库+UNIGBK.BIN+asc**.bin,总大小3.11M, 791+5个扇区,被字库占用了,不能动!
 * 以及15.09M以后, 用户可以自由使用. 建议用最后的100K字节比较好
 * 
 */
#define ICONINFOADDR        10 * 1024 * 1024

 
/* 用来保存字库基本信息，地址，大小等 */
//_font_info* ftinfo;

/* 图库存放在磁盘中的文件名 */
char *const ICON_BIN_PATH[18] =
{
    "AlarmOn.bin",      /* AlarmOn.bin存放位置 */
    "BackLight.bin",      /* BackLight.bin存放位置 */
    "blue.bin",      /* blue.bin存放位置 */
    "Brightnesss.bin",      /* Brightnesss.bin存放位置 */
    "FanOn.bin",      /* FanOn.bin存放位置 */
    "Humidity.bin",      /* Humidity.bin存放位置 */
    "LightingMasterOn.bin",      /* LightingMasterOn.bin存放位置 */
    "LoRaOn.bin",      /* LoRaOn.bin存放位置 */
    "SpotlightOn.bin",      /* SpotlightOn.bin存放位置 */
    "Temperature.bin",      /* Temperature.bin存放位置 */
    "Wifi.bin",      /* Wifi.bin存放位置 */

    "AlarmOff.bin",      /* AlarmOff.bin存放位置 */
    "FanOff.bin",      /* FanOff.bin存放位置 */
    "LightingMasterOff.bin",      /* LightingMasterOff.bin存放位置 */
    "LoRaOff.bin",      /* LoRaOff.bin存放位置 */
    "MainPage.bin",      /* MainPage.bin存放位置 */
    "MainPagePressed.bin",      /* MainPagePressed.bin存放位置 */
    "SpotlightOff.bin",      /* SpotlightOff.bin存放位置 */
};

const char* icon_main_path = "/SYSTEM/icon/main/";  //图库main路径
const char* icon_sub_path = "/SYSTEM/icon/sub/";  //图库sub路径


/**
 * @brief       更新某一个字库
 * @param       x, y    : 提示信息的显示地址
 * @param       size    : 提示信息字体大小
 * @param       fpath   : 字体路径
 * @param       fx      : 更新的内容索引
 * @param       color   : 字体颜色
 * @retval      0, 成功; 其他, 错误代码;
 */
static uint8_t icons_update_iconx(uint16_t x, uint16_t y, uint8_t size, uint8_t *fpath, uint8_t fx, uint16_t color)
{
    uint32_t flashaddr = 0;
    FIL *fftemp;
    uint8_t *tempbuf;
    uint8_t res;
    uint16_t bread;
    uint32_t offx = 0;
    uint8_t rval = 0;
    fftemp = (FIL *)mymalloc(SRAMIN, sizeof(FIL));  /* 分配内存 */
		//printf("fonts_update_fontx mymalloc FIL * successed!\n");
    if (fftemp == NULL)rval = 1;

    tempbuf = mymalloc(SRAMIN, 4096);               /* 分配4096个字节空间 */
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
            case 0: /* 更新 AlarmOn.bin */
                iconftinfo->AlarmOnaddr = ICONINFOADDR + sizeof(_icon_info);    /* 信息头之后，紧跟AlarmOn.bin转换码表 */
                iconftinfo->AlarmOnsize = fftemp->obj.objsize;              /* AlarmOn.bin大小 */
                flashaddr = iconftinfo->AlarmOnaddr;
                break;

            case 1: /* 更新 BackLight.bin */
                iconftinfo->BackLightaddr = iconftinfo->AlarmOnaddr + iconftinfo->AlarmOnsize; /* AlarmOn.bin之后，紧跟 BackLight.bin */
                iconftinfo->BackLightsize = fftemp->obj.objsize;             /* BackLight.bin大小 */
                flashaddr = iconftinfo->BackLightaddr;                         /* BackLight的起始地址 */
                break;
				
			case 2: /* 更新 blue.bin */
                iconftinfo->blueaddr = iconftinfo->BackLightaddr + iconftinfo->BackLightsize; /* BackLight.bin之后，紧跟 blue.bin */
                iconftinfo->bluesize = fftemp->obj.objsize;             /* blue.bin大小 */
                flashaddr = iconftinfo->blueaddr;                         /* blue的起始地址 */
                break;
				
			case 3: /* 更新 Brightnesss.bin */
                iconftinfo->Brightnesssaddr = iconftinfo->blueaddr + iconftinfo->bluesize; /* blue.bin之后，紧跟 Brightnesss.bin */
                iconftinfo->Brightnessssize = fftemp->obj.objsize;             /* Brightnesss.bin大小 */
                flashaddr = iconftinfo->Brightnesssaddr;                         /* Brightnesss的起始地址 */
                break;
				
			case 4: /* 更新 FanOn.bin */
                iconftinfo->FanOnaddr = iconftinfo->Brightnesssaddr + iconftinfo->Brightnessssize; /* Brightnesss.bin之后，紧跟 FanOn.bin */
                iconftinfo->FanOnsize = fftemp->obj.objsize;             /* FanOn.bin大小 */
                flashaddr = iconftinfo->FanOnaddr;                         /* FanOn的起始地址 */
                break;
				
			case 5: /* 更新 Humidity.bin */
                iconftinfo->Humidityaddr = iconftinfo->FanOnaddr + iconftinfo->FanOnsize; /* FanOn.bin之后，紧跟 Humidity.bin */
                iconftinfo->Humiditysize = fftemp->obj.objsize;             /* Humidity.bin大小 */
                flashaddr = iconftinfo->Humidityaddr;                         /* Humidity的起始地址 */
                break;
				
			case 6: /* 更新 LightingMasterOn.bin */
                iconftinfo->LightingMasterOnaddr = iconftinfo->Humidityaddr + iconftinfo->Humiditysize; /* Humidity.bin之后，紧跟 LightingMasterOn.bin */
                iconftinfo->LightingMasterOnsize = fftemp->obj.objsize;             /* BackLight.bin大小 */
                flashaddr = iconftinfo->LightingMasterOnaddr;                         /* LightingMasterOn的起始地址 */
                break;
				
			case 7: /* 更新 LoRaOn.bin */
                iconftinfo->LoRaOnaddr = iconftinfo->LightingMasterOnaddr + iconftinfo->LightingMasterOnsize; /* LightingMasterOn.bin之后，紧跟 LoRaOn.bin */
                iconftinfo->LoRaOnsize = fftemp->obj.objsize;             /* BackLight.bin大小 */
                flashaddr = iconftinfo->LoRaOnaddr;                         /* LoRaOn的起始地址 */
                break;
				
			case 8: /* 更新 SpotlightOn.bin */
                iconftinfo->SpotlightOnaddr = iconftinfo->LoRaOnaddr + iconftinfo->LoRaOnsize; /* LoRaOn.bin之后，紧跟 SpotlightOn.bin */
                iconftinfo->SpotlightOnsize = fftemp->obj.objsize;             /* SpotlightOn.bin大小 */
                flashaddr = iconftinfo->SpotlightOnaddr;                         /* SpotlightOn的起始地址 */
                break;
				
			case 9: /* 更新 Temperature.bin */
                iconftinfo->Temperatureaddr = iconftinfo->SpotlightOnaddr + iconftinfo->SpotlightOnsize; /* SpotlightOn.bin之后，紧跟 Temperature.bin */
                iconftinfo->Temperaturesize = fftemp->obj.objsize;             /* Temperature.bin大小 */
                flashaddr = iconftinfo->Temperatureaddr;                         /* Temperature的起始地址 */
                break;
				
			case 10: /* 更新 Wifi.bin */
                iconftinfo->Wifiaddr = iconftinfo->Temperatureaddr + iconftinfo->Temperaturesize; /* Temperature.bin之后，紧跟 Wifi.bin */
                iconftinfo->Wifisize = fftemp->obj.objsize;             /* Wifi.bin大小 */
                flashaddr = iconftinfo->Wifiaddr;                         /* Wifi的起始地址 */
                break;
				
			case 11: /* 更新 AlarmOff.bin */
                iconftinfo->AlarmOffaddr = iconftinfo->Wifiaddr + iconftinfo->Wifisize; /* Wifi.bin之后，紧跟 AlarmOff.bin */
                iconftinfo->AlarmOffsize = fftemp->obj.objsize;             /* AlarmOff.bin大小 */
                flashaddr = iconftinfo->AlarmOffaddr;                         /* AlarmOff的起始地址 */
                break;
				
			case 12: /* 更新 FanOff.bin */
                iconftinfo->FanOffaddr = iconftinfo->AlarmOffaddr + iconftinfo->AlarmOffsize; /* AlarmOff.bin之后，紧跟 FanOff.bin */
                iconftinfo->FanOffsize = fftemp->obj.objsize;             /* FanOff.bin大小 */
                flashaddr = iconftinfo->FanOffaddr;                         /* FanOff的起始地址 */
                break;
				
			case 13: /* 更新 LightingMasterOff.bin */
                iconftinfo->LightingMasterOffaddr = iconftinfo->FanOffaddr + iconftinfo->FanOffsize; /* FanOff.bin之后，紧跟 LightingMasterOff.bin */
                iconftinfo->LightingMasterOffsize = fftemp->obj.objsize;             /* LightingMasterOff.bin大小 */
                flashaddr = iconftinfo->LightingMasterOffaddr;                         /* LightingMasterOff的起始地址 */
                break;
				
			case 14: /* 更新 LoRaOff.bin */
                iconftinfo->LoRaOffaddr = iconftinfo->LightingMasterOffaddr + iconftinfo->LightingMasterOffsize; /* LightingMasterOff.bin之后，紧跟 LoRaOff.bin */
                iconftinfo->LoRaOffsize = fftemp->obj.objsize;             /* LoRaOff.bin大小 */
                flashaddr = iconftinfo->LoRaOffaddr;                         /* LoRaOff的起始地址 */
                break;
				
			case 15: /* 更新 MainPage.bin */
                iconftinfo->MainPageaddr = iconftinfo->LoRaOffaddr + iconftinfo->LoRaOffsize; /* LoRaOff.bin之后，紧跟 MainPage.bin */
                iconftinfo->MainPagesize = fftemp->obj.objsize;             /* MainPage.bin大小 */
                flashaddr = iconftinfo->MainPageaddr;                         /* MainPage的起始地址 */
                break;
				
			case 16: /* 更新 MainPagePressed.bin */
                iconftinfo->MainPagePressedaddr = iconftinfo->MainPageaddr + iconftinfo->MainPagesize; /* MainPage.bin之后，紧跟 MainPagePressed.bin */
                iconftinfo->MainPagePressedsize = fftemp->obj.objsize;             /* MainPagePressed.bin大小 */
                flashaddr = iconftinfo->MainPagePressedaddr;                         /* MainPagePressed的起始地址 */
                break;
				
			case 17: /* 更新 SpotlightOff.bin */
                iconftinfo->SpotlightOffaddr = iconftinfo->MainPagePressedaddr + iconftinfo->MainPagePressedsize; /* MainPagePressed.bin之后，紧跟 SpotlightOff.bin */
                iconftinfo->SpotlightOffsize = fftemp->obj.objsize;             /* SpotlightOff.bin大小 */
                flashaddr = iconftinfo->SpotlightOffaddr;                         /* SpotlightOff的起始地址 */
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
 * @brief       更新字体文件
 *   @note      所有字库一起更新(UNIGBK,GBK12,GBK16,GBK24)
 * @param       x, y    : 提示信息的显示地址
 * @param       size    : 提示信息字体大小
 * @param       src     : 字库来源磁盘
 *   @arg                 "0:", SD卡
 * @param       color   : 字体颜色
 * @retval      0, 成功; 其他, 错误代码;
 */
uint8_t icons_update_icon(uint16_t x, uint16_t y, uint8_t size, uint8_t *src, uint16_t color)
{
    uint8_t *pname;
    uint32_t *buf;
    uint8_t res = 0;
    uint16_t i;
    FIL *fftemp;
    uint8_t rval = 0;
    res = 0xFF;
    iconftinfo->icontok = 0xFF;
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

	//读方式打开18个文件，可以同时判断文件是否存在是否损坏
    for (i = 0; i < 18; i++) // 先查找文件**.bin
    {
        delay_ms(100);
        strcpy((char *)pname, (char *)src);                 // copy src内容到pname
		if(i > 10){
			strcat((char *)pname, (char *)icon_sub_path);    // 追加具体文件路径 
		}else{
			strcat((char *)pname, (char *)icon_main_path);    // 追加具体文件路径 
		}
        strcat((char *)pname, (char *)ICON_BIN_PATH[i]);    // 追加具体文件名

		printf("open pname: %s; ", pname);

        res = f_open(fftemp, (const TCHAR *)pname, FA_READ);// 尝试打开
		printf("f_open return :%d", res);
        if (res)
        {
            rval |= 1 << 7; // 标记打开文件失败 
            break;          // 出错了,直接退出 
        }
		//关闭文件
		//while(!f_close(fftemp));
		res = f_close(fftemp);
		//printf("f_close return :%d\n", res);
    }
	
		myfree(SRAMIN, fftemp); /* 释放内存 */

    if (rval == 0)          /* 图库文件都存在. */
    {
        for (i = 0; i < 18; i++) // 依次更新**.bin
        {
            strcpy((char *)pname, (char *)src);                     /* copy src内容到pname */
			if(i > 10){
				strcat((char *)pname, (char *)icon_sub_path);    // 追加具体文件路径 
			}else{
				strcat((char *)pname, (char *)icon_main_path);    // 追加具体文件路径 
			}
			strcat((char *)pname, (char *)ICON_BIN_PATH[i]);    // 追加具体文件名
			printf("updating pname: %s; ", pname);
			lcd_fill(20, 610, 200 + 20, 90 + 16, WHITE);
			lcd_show_string(30, 610, 200, 16, 16, "Updating pname: ", RED);
			lcd_show_string(30+150, 610, 200, 16, 16, (char *)pname, RED);
			lcd_show_string(30+355, 610, 20, 16, 16, "; ", RED);
			
									
            res = icons_update_iconx(x + 20 * size / 2, y, size, pname, i, color);  // 更新图库 
					
            if (res)
            {
				myfree(SRAMIN, buf);
                myfree(SRAMIN, pname);
                return 1 + i;
            }
        }

        /* 全部更新好了 */
        iconftinfo->icontok = 0xAA;
        norflash_write((uint8_t *)iconftinfo, ICONINFOADDR, sizeof(_icon_info));           /* 保存字库信息 */
    }

    myfree(SRAMIN, pname);  /* 释放内存 */
	myfree(SRAMIN, buf);
    return rval;            /* 无错误. */
}


uint8_t icons_init(void)
{
    uint8_t t = 0;

    while (t < 10)  /* 连续读取10次,都是错误,说明确实是有问题,得更新字库了 */
    {
        t++;
        norflash_read((uint8_t *)iconftinfo, ICONINFOADDR, sizeof(_icon_info));    /* 读出ftinfo结构体数据 */

        if (iconftinfo->icontok == 0XAA)
        {
            break;
        }
        
        delay_ms(20);
    }

    if (iconftinfo->icontok != 0XAA)
    {
        return 1;
    }
    
    return 0;
}

void erase_icon_sector(){
	uint32_t *buf = mymalloc(SRAMIN, 4096);                           /* 申请4K字节内存 */
	uint16_t i, j;
	for (i = 0; i < ICONSECSIZE; i++)           // 先擦除字库区域,提高写入速度 
	{					
			norflash_read((uint8_t *)buf, ((ICONINFOADDR / 4096) + i) * 4096, 4096);    // 读出整个扇区的内容 

			for (j = 0; j < 1024; j++)              // 校验这个扇区的数据
			{
					if (buf[j] != 0xFFFFFFFF) break;    // 发现数据不为`0xFFFFFFFF`，则需要擦除该扇区 
			}

			if (j != 1024)
			{
					norflash_erase_sector((ICONINFOADDR / 4096) + i); // 需要擦除的扇区 
					//printf("norflash_erase_sector:%d\n", (FONTINFOADDR / 4096) + i);
			}
	}
	myfree(SRAMIN, buf);
}
