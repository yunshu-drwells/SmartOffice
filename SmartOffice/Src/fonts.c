#include "fonts.h"
#include "ff.h"
#include "delay.h"
#include "norflash.h"

/* 字库存放起始地址
 * 从第12MB地址开始存放字库
 * 前面12MB大小被文件系统占用
 * 12MB后紧跟3个字库+UNIGBK.BIN+asc**.bin,总大小3.11M, 791+5个扇区,被字库占用了,不能动!
 * 15.09M以后, 用户可以自由使用. 建议用最后的100K字节比较好
 * 
 */
#define FONTINFOADDR        12 * 1024 * 1024

uint8_t fonts_init(void)
{
    uint8_t t = 0;

    while (t < 10)  /* 连续读取10次,都是错误,说明确实是有问题,得更新字库了 */
    {
        t++;
        norflash_read((uint8_t *)ftinfo, FONTINFOADDR, sizeof(_font_info));    /* 读出ftinfo结构体数据 */

        if (ftinfo->fontok == 0XAA)
        {
            break;
        }
        
        delay_ms(20);
    }

    if (ftinfo->fontok != 0XAA)
    {
        return 1;
    }
    
    return 0;
}
