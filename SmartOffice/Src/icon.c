#include "icon.h"
#include "delay.h"
#include "norflash.h"


/* 图库存放起始地址
 * 从第10MB地址开始存放图库
 * 前面10MB大小被文件系统占用
 * 10MB后紧跟18个**.bin,总大小762KB, 190个扇区,被图库占用了,不能动!
 * 12MB后紧跟3个字库+UNIGBK.BIN+asc**.bin,总大小3.11M, 791+5个扇区,被字库占用了,不能动!
 * 以及15.09M以后, 用户可以自由使用. 建议用最后的100K字节比较好
 * 
 */
#define ICONINFOADDR        10 * 1024 * 1024

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
