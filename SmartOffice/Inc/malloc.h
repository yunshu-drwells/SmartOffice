/**
 ****************************************************************************************************
 * @file        malloc.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2021-11-04
 * @brief       内存管理 驱动
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台：正点原子 F407电机开发板
 * 在线视频：www.yuanzige.com
 * 技术论坛：http://www.openedv.com/forum.php
 * 公司网址：www.alientek.com
 * 购买地址：zhengdianyuanzi.tmall.com
 *
 * 修改说明
 * V1.0 20211104
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef __MALLOC_H
#define __MALLOC_H
//#include "./SYSTEM/sys/sys.h"
#include "stm32f4xx.h"
#include "core_cm4.h"
#include "stm32f4xx_hal.h"

#ifndef NULL
#define NULL 0
#endif

/* 定义三个内存池 */
#define     SRAMIN                  0                               /* 内部内存池 */
#define     SRAMCCM                 1                               /* CCM内存池(此部分SRAM仅仅CPU可以访问!!!) */
#define     SRAMBANK                2                               /* 定义支持的SRAM块数 */

/* 定义内存管理表类型,当外扩SDRAM的时候，必须使用uint32_t类型，否则定义成uint16_t，以节省内存占用 */
#define MT_TYPE     uint16_t

/* mem1内存参数设定.mem1完全处于内部SRAM里面 */
#define     MEM1_BLOCK_SIZE         32                              /* 内存块大小为32字节 */
#define     MEM1_MAX_SIZE           100 * 1024                      /* 最大管理内存 100K */
#define     MEM1_ALLOC_TABLE_SIZE   MEM1_MAX_SIZE/MEM1_BLOCK_SIZE   /* 内存表大小 */

/* mem2内存参数设定.mem2处于CCM,用于管理CCM(特别注意,这部分SRAM,仅CPU可以访问!!) */
#define     MEM2_BLOCK_SIZE         32                              /* 内存块大小为32字节 */
#define     MEM2_MAX_SIZE           60 * 1024                       /* 最大管理内存 60K */
#define     MEM2_ALLOC_TABLE_SIZE   MEM2_MAX_SIZE/MEM2_BLOCK_SIZE   /* 内存表大小 */



/* 内存管理控制器 */
struct _m_mallco_dev
{
    void (*init)(uint8_t);              /* 初始化 */
    uint16_t (*perused)(uint8_t);       /* 内存使用率 */
    uint8_t *membase[SRAMBANK];         /* 内存池 管理SRAMBANK个区域的内存 */
    MT_TYPE *memmap[SRAMBANK];          /* 内存管理状态表 */
    uint8_t  memrdy[SRAMBANK];          /* 内存管理是否就绪 */
};

extern struct _m_mallco_dev mallco_dev; /* 在mallco.c里面定义 */


/* 用户调用函数 */
void my_mem_init(uint8_t memx);                     /* 内存管理初始化函数(外/内部调用) */
uint16_t my_mem_perused(uint8_t memx) ;             /* 获得内存使用率(外/内部调用) */
void my_mem_set(void *s, uint8_t c, uint32_t count);/* 内存设置函数 */
void my_mem_copy(void *des, void *src, uint32_t n); /* 内存拷贝函数 */

void myfree(uint8_t memx, void *ptr);               /* 内存释放(外部调用) */
void *mymalloc(uint8_t memx, uint32_t size);        /* 内存分配(外部调用) */
void *myrealloc(uint8_t memx, void *ptr, uint32_t size);    /* 重新分配内存(外部调用) */

#endif
