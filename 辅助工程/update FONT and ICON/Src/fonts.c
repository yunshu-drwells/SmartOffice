#include "string.h"
#include "lcd.h"
#include "fonts.h"
#include "mymalloc.h"
#include "ff.h"
#include "usart.h"
#include "delay.h"
#include "norflash.h"
//#include "fattester.h"


/* 字库区域占用的总扇区数大小(3个字库+unigbk表+字库信息+asc**.bin=3238700+20480 字节,约占796个25QXX扇区,一个扇区4K字节) */
#define FONTSECSIZE         796


/* 字库存放起始地址
 * 从第12MB地址开始存放字库
 * 前面12MB大小被文件系统占用
 * 12MB后紧跟3个字库+UNIGBK.BIN+asc**.bin,总大小3.11M, 791+5个扇区,被字库占用了,不能动!
 * 15.09M以后, 用户可以自由使用. 建议用最后的100K字节比较好
 * 
 */
#define FONTINFOADDR        12 * 1024 * 1024

 
/* 用来保存字库基本信息，地址，大小等 */
//_font_info* ftinfo;

/* 字库存放在磁盘中的路径 */
char *const FONT_GBK_PATH[8] =
{
    "UNIGBK.BIN",      /* UNIGBK.BIN的存放位置 */
    "GBK12.FON",       /* GBK12的存放位置 */
    "GBK16.FON",       /* GBK16的存放位置 */
    "GBK24.FON",       /* GBK24的存放位置 */
	"asc12.bin",       /* asc12的存放位置 */
	"asc16.bin",       /* asc16的存放位置 */
	"asc24.bin",       /* asc24的存放位置 */
	"asc32.bin",       /* asc32的存放位置 */
};

const char* path = "/SYSTEM/FONT/";

/* 更新时的提示信息 */
char *const FONT_UPDATE_REMIND_TBL[8] =
{
    "Updating UNIGBK.BIN",          /* 提示正在更新UNIGBK.bin */
    "Updating GBK12.FON ",          /* 提示正在更新GBK12 */
    "Updating GBK16.FON ",          /* 提示正在更新GBK16 */
    "Updating GBK24.FON ",          /* 提示正在更新GBK24 */
	"Updating asc12.bin ",          /* 提示正在更新asc12 */	
	"Updating asc16.bin ",          /* 提示正在更新asc16 */
	"Updating asc24.bin ",          /* 提示正在更新asc24 */
	"Updating asc32.bin ",          /* 提示正在更新asc32 */
};

/**
 * @brief       显示当前字体更新进度
 * @param       x, y    : 坐标
 * @param       size    : 字体大小
 * @param       totsize : 整个文件大小
 * @param       pos     : 当前文件指针位置
 * @param       color   : 字体颜色
 * @retval      无
 */
static void fonts_progress_show(uint16_t x, uint16_t y, uint8_t size, uint32_t totsize, uint32_t pos, uint16_t color)
{
    float prog;
    uint8_t t = 0XFF;
    prog = (float)pos / totsize;
    prog *= 100;

    if (t != prog)
    {
        lcd_show_string(x + 3 * size / 2, y, 240, 320, size, "%", color);
        t = prog;

        if (t > 100)t = 100;

        lcd_show_num(x, y, t, 3, size, color);  /* 显示数值 */
    }
}
   
//extern uint8_t sd_mount_status;               /* USB 挂载状态 */

/**
 * @brief       更新某一个字库
 * @param       x, y    : 提示信息的显示地址
 * @param       size    : 提示信息字体大小
 * @param       fpath   : 字体路径
 * @param       fx      : 更新的内容
 *   @arg                 0, ungbk;
 *   @Arg                 1, gbk12;
 *   @arg                 2, gbk16;
 *   @arg                 3, gbk24;
 * @param       color   : 字体颜色
 * @retval      0, 成功; 其他, 错误代码;
 */
static uint8_t fonts_update_fontx(uint16_t x, uint16_t y, uint8_t size, uint8_t *fpath, uint8_t fx, uint16_t color)
{
		//printf("fonts_update_fontx start!\n");
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
				//printf("update *.BIN\n");
        switch (fx)
        {
            case 0: /* 更新 UNIGBK.BIN */
								//printf("updating UNIGBK.BIN\n");
                ftinfo->ugbkaddr = FONTINFOADDR + sizeof(ftinfo);    /* 信息头之后，紧跟UNIGBK转换码表 */
                ftinfo->ugbksize = fftemp->obj.objsize;              /* UNIGBK大小 */
                flashaddr = ftinfo->ugbkaddr;
                break;

            case 1: /* 更新 GBK12.BIN */
								//printf("updating GBK12.BIN \n");
                ftinfo->f12addr = ftinfo->ugbkaddr + ftinfo->ugbksize; /* UNIGBK之后，紧跟GBK12字库 */
                ftinfo->gbk12size = fftemp->obj.objsize;             /* GBK12字库大小 */
                flashaddr = ftinfo->f12addr;                         /* GBK12的起始地址 */
                break;

            case 2: /* 更新 GBK16.BIN */
								//printf("updating GBK16.BIN \n");
                ftinfo->f16addr = ftinfo->f12addr + ftinfo->gbk12size; /* GBK12之后，紧跟GBK16字库 */
                ftinfo->gbk16size = fftemp->obj.objsize;             /* GBK16字库大小 */
                flashaddr = ftinfo->f16addr;                         /* GBK16的起始地址 */
                break;

            case 3: /* 更新 GBK24.BIN */
								//printf("updating GBK24.BIN \n");
                ftinfo->f24addr = ftinfo->f16addr + ftinfo->gbk16size; /* GBK16之后，紧跟GBK24字库 */
                ftinfo->gbk24size = fftemp->obj.objsize;             /* GBK24字库大小 */
                flashaddr = ftinfo->f24addr;                         /* GBK24的起始地址 */
                break;
			case 4: /* 更新 asc12.bin */
								//printf("updating asc12.bin \n");
                ftinfo->asc12addr = ftinfo->f24addr + ftinfo->gbk24size; /* GBK24之后，紧跟asc12字库 */
                ftinfo->asc12size = fftemp->obj.objsize;             /* asc12字库大小 */
                flashaddr = ftinfo->asc12addr;                         /* asc12的起始地址 */
                break;
			case 5: /* 更新 asc16.bin */
								//printf("updating asc16.bin \n");
                ftinfo->asc16addr = ftinfo->asc12addr + ftinfo->asc12size; /* asc12之后，紧跟asc16字库 */
                ftinfo->asc16size = fftemp->obj.objsize;             /* asc16字库大小 */
                flashaddr = ftinfo->asc16addr;                         /* asc16的起始地址 */
                break;
			case 6: /* 更新 asc24.bin */
								//printf("updating asc16.bin \n");
                ftinfo->asc24addr = ftinfo->asc16addr + ftinfo->asc16size; /* asc16之后，紧跟asc24字库 */
                ftinfo->asc24size = fftemp->obj.objsize;             /* asc16字库大小 */
                flashaddr = ftinfo->asc24addr;                         /* asc16的起始地址 */
                break;
			case 7: /* 更新 asc32.bin */
								//printf("updating asc16.bin \n");
                ftinfo->asc32addr = ftinfo->asc24addr + ftinfo->asc24size; /* asc24之后，紧跟asc32字库 */
                ftinfo->asc32size = fftemp->obj.objsize;             /* asc32字库大小 */
                flashaddr = ftinfo->asc32addr;                         /* asc32的起始地址 */
                break;				
        }

        while (res == FR_OK)   /* 死循环执行 */
        {
            res = f_read(fftemp, tempbuf, 4096, (UINT *)&bread);    /* 读取数据 */
            if (res != FR_OK)break;     /* 执行错误 */

            norflash_write(tempbuf, offx + flashaddr, bread);       /* 从0开始写入bread个数据 */
            offx += bread;
            fonts_progress_show(x, y, size, fftemp->obj.objsize, offx, color);    /* 进度显示 */

            if (bread != 4096)break;    /* 读完了. */
            delay_ms(20);
        }

        f_close(fftemp);
    }

    myfree(SRAMIN, fftemp);     /* 释放内存 */
    myfree(SRAMIN, tempbuf);    /* 释放内存 */
    return res;
}

int find_file(const char *path, const char *filename) {
    FRESULT res;
    DIR dir;
    FILINFO fno;

    res = f_opendir(&dir, path); // 打开目录
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno); // 读取目录中的一个项目
            if (res != FR_OK || fno.fname[0] == 0) break; // 错误或到达目录末尾
            if (fno.fattrib & AM_DIR) {
                // 如果是目录，递归调用
                if (find_file(fno.fname, filename)) {
                    f_closedir(&dir);
                    return 1; // 找到文件
                }
            } else {
                // 如果是文件，检查文件名
                if (strcmp(fno.fname, filename) == 0) {
										//printf("find file: %s/%s\n", path, fno.fname);
                    f_closedir(&dir);
                    return 1; // 找到文件
                }
            }
        }
        f_closedir(&dir); // 关闭目录
    } else {
        printf("can not open: %s\n", path);
    }
    return 0; // 未找到文件
}

//#include "exfuns.h"  //fs[1]
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
uint8_t fonts_update_font(uint16_t x, uint16_t y, uint8_t size, uint8_t *src, uint16_t color)
{
		//printf("fonts_update_font start\n");
    uint8_t *pname;
    uint32_t *buf;
    uint8_t res = 0;
    uint16_t i;
    FIL *fftemp;
    uint8_t rval = 0;
    res = 0xFF;
    ftinfo->fontok = 0xFF;
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

		//读方式打开三个文件，可以同时判断文件是否存在是否损坏
    for (i = 0; i < 8; i++) // 先查找文件UNIGBK,GBK12,GBK16,GBK24 是否正常 
    {
        delay_ms(100);
        strcpy((char *)pname, (char *)src);                 // copy src内容到pname
		strcat((char *)pname, (char *)path);    // 追加具体文件路径 
        strcat((char *)pname, (char *)FONT_GBK_PATH[i]);    // 追加具体文件名
				printf("open pname: %s; ", pname);

        res = f_open(fftemp, (const TCHAR *)pname, FA_READ);// 尝试打开
				printf("f_open return :%d; ", res);
        if (res)
        {
            rval |= 1 << 7; // 标记打开文件失败 
            break;          // 出错了,直接退出 
        }
				//关闭文件
				//while(!f_close(fftemp));
				res = f_close(fftemp);
				printf("f_close return :%d\n", res);
    }
	
	myfree(SRAMIN, fftemp); /* 释放内存 */

    if (rval == 0)          /* 字库文件都存在. */
    {
        for (i = 0; i < 8; i++) /* 依次更新UNIGBK,GBK12,GBK16,GBK24 */
        {
            lcd_show_string(x, y, 240, 320, size, FONT_UPDATE_REMIND_TBL[i], color);
            strcpy((char *)pname, (char *)src);                     /* copy src内容到pname */
			strcat((char *)pname, (char *)path);    // 追加具体文件路径 
            strcat((char *)pname, (char *)FONT_GBK_PATH[i]);        // 追加具体文件名
			printf("updating pname: %s; ", pname);
			lcd_fill(20, 530, 200 + 20, 90 + 16, WHITE);
			lcd_show_string(30, 530, 200, 16, 16, "Updating pname: ", RED);
			lcd_show_string(30+150, 530, 200, 16, 16, (char *)pname, RED);
			lcd_show_string(30+355, 530, 20, 16, 16, "; ", RED);
									
            res = fonts_update_fontx(x + 20 * size / 2, y, size, pname, i, color);  // 更新字库 
			lcd_fill(20, 530, 200 + 20, 90 + 16, WHITE);
			lcd_show_string(30, 530, 200, 16, 16, "Updated pname: ", RED);
			lcd_show_string(30+150, 530, 200, 16, 16, (char *)pname, RED);
			lcd_show_string(30+355, 530, 20, 16, 16, "; ", RED);
					
            if (res)
            {
				myfree(SRAMIN, buf);
                myfree(SRAMIN, pname);
				printf("fonts_update_font return %d\n", 1 + i);
                return 1 + i;
            }
        }

        /* 全部更新好了 */
        ftinfo->fontok = 0xAA;
        norflash_write((uint8_t *)ftinfo, FONTINFOADDR, sizeof(_font_info));           /* 保存字库信息 */
    }

    myfree(SRAMIN, pname);  /* 释放内存 */
	myfree(SRAMIN, buf);
	printf("fonts_update_font return %d\n", rval);
    return rval;            /* 无错误. */
}

/**
 * @brief       初始化字体
 * @param       无
 * @retval      0, 字库完好; 其他, 字库丢失;
 */
uint8_t fonts_init(void)
{
    uint8_t t = 0;

    while (t < 10)  /* 连续读取10次,都是错误,说明确实是有问题,得更新字库了 */
    {
        t++;
        norflash_read((uint8_t *)ftinfo, FONTINFOADDR, sizeof(_font_info));    /* 读出ftinfo结构体数据 */
		//printf("ftinfo->fontok %x\n", ftinfo->fontok);
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

void erase_gbk_sector(){
	uint32_t *buf = mymalloc(SRAMIN, 4096);                           /* 申请4K字节内存 */
	uint16_t i, j;
	for (i = 0; i < FONTSECSIZE; i++)           // 先擦除字库区域,提高写入速度 
	{					
			norflash_read((uint8_t *)buf, ((FONTINFOADDR / 4096) + i) * 4096, 4096);    // 读出整个扇区的内容 

			for (j = 0; j < 1024; j++)              // 校验这个扇区的数据
			{
					if (buf[j] != 0xFFFFFFFF) break;    // 发现数据不为`0xFFFFFFFF`，则需要擦除该扇区 
			}

			if (j != 1024)
			{
					norflash_erase_sector((FONTINFOADDR / 4096) + i); // 需要擦除的扇区 
					//printf("norflash_erase_sector:%d\n", (FONTINFOADDR / 4096) + i);
			}
	}
	myfree(SRAMIN, buf);
}
