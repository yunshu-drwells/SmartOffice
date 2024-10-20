#include "string.h"
//#include "lcd.h"
#include "fonts.h"
#include "mymalloc.h"
#include "ff.h"
#include "usart.h"
#include "delay.h"
#include "norflash.h"
//#include "fattester.h"


/* 字库区域占用的总扇区数大小(3个字库+unigbk表+字库信息=3238700 字节,约占791个25QXX扇区,一个扇区4K字节) */
#define FONTSECSIZE         791


/* 字库存放起始地址
 * 从第12MB地址开始存放字库
 * 前面12MB大小被文件系统占用
 * 12MB后紧跟3个字库+UNIGBK.BIN,总大小3.09M, 791个扇区,被字库占用了,不能动!
 * 15.10M以后, 用户可以自由使用. 建议用最后的100K字节比较好
 */
#define FONTINFOADDR        12 * 1024 * 1024

 
/* 用来保存字库基本信息，地址，大小等 */
_font_info ftinfo;

/* 字库存放在磁盘中的路径 */
char *const FONT_GBK_PATH[4] =
{
    "/SYSTEM/FONT/UNIGBK.BIN",      /* UNIGBK.BIN的存放位置 */
    "/SYSTEM/FONT/GBK12.FON",       /* GBK12的存放位置 */
    "/SYSTEM/FONT/GBK16.FON",       /* GBK16的存放位置 */
    "/SYSTEM/FONT/GBK24.FON",       /* GBK24的存放位置 */
};

/* 更新时的提示信息 */
char *const FONT_UPDATE_REMIND_TBL[4] =
{
    "Updating UNIGBK.BIN",          /* 提示正在更新UNIGBK.bin */
    "Updating GBK12.FON ",          /* 提示正在更新GBK12 */
    "Updating GBK16.FON ",          /* 提示正在更新GBK16 */
    "Updating GBK24.FON ",          /* 提示正在更新GBK24 */
};

uint8_t fonts_init(void)
{
    uint8_t t = 0;

    while (t < 10)  /* 连续读取10次,都是错误,说明确实是有问题,得更新字库了 */
    {
        t++;
        norflash_read((uint8_t *)&ftinfo, FONTINFOADDR, sizeof(ftinfo));    /* 读出ftinfo结构体数据 */

        if (ftinfo.fontok == 0XAA)
        {
            break;
        }
        
        delay_ms(20);
    }

    if (ftinfo.fontok != 0XAA)
    {
        return 1;
    }
    
    return 0;
}
