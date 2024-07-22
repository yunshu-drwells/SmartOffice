/**
 ****************************************************************************************************
 * @file        GT9xxx.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2023-05-29
 * @brief       4.3����ݴ�����-GT9xxx ��������
 *   @note      GTϵ�е��ݴ�����ICͨ������,������֧��: GT9147/GT917S/GT968/GT1151/GT9271 �ȶ���
 *              ����IC, ��Щ����IC��ID��һ��, ��������������Ҫ���κ��޸ļ���ͨ��������ֱ������
 *
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨������ԭ�� F407���������
 * ������Ƶ��www.yuanzige.com
 * ������̳��http://www.openedv.com/forum.php
 * ��˾��ַ��www.alientek.com
 * �����ַ��zhengdianyuanzi.tmall.com
 *
 * �޸�˵��
 * V1.0 20211025
 * ��һ�η���
 * V1.1 20230529
 * 1, ������ST7796 3.5����/ILI9806 4.3����GT1151��֧��
 * 2, gt9xxx_init���������Ӵ���IC�ж����������ض�����IC�ͷ���1��ʾ��ʼ��ʧ��
 ****************************************************************************************************
 */

#ifndef __GT9XXX_H
#define __GT9XXX_H

#include "stm32f4xx.h"
#include "core_cm4.h"
#include "stm32f4xx_hal.h"
#include "main.h"  //���Ŷ���


/******************************************************************************************/
/* GT9XXX INT �� RST ���� ���� */
/*
#define GT9XXX_RST_GPIO_PORT            GPIOG
#define GT9XXX_RST_GPIO_PIN             GPIO_PIN_1
#define GT9XXX_RST_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOG_CLK_ENABLE(); }while(0)   // PG��ʱ��ʹ�� 

#define GT9XXX_INT_GPIO_PORT            GPIOH
#define GT9XXX_INT_GPIO_PIN             GPIO_PIN_7
#define GT9XXX_INT_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOH_CLK_ENABLE(); }while(0)   // PH��ʱ��ʹ�� 
*/

#define GT9XXX_RST_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOG_CLK_ENABLE(); }while(0)   // PG��ʱ��ʹ�� 

#define GT9XXX_INT_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOH_CLK_ENABLE(); }while(0)   // PH��ʱ��ʹ�� 

/******************************************************************************************/

/* ����ݴ��������ӵ�оƬ����(δ����IIC����) 
 * IO�������� 
 */
/*
#define GT9XXX_RST(x)     do{ x ? \
                              HAL_GPIO_WritePin(GT9XXX_RST_GPIO_PORT, GT9XXX_RST_GPIO_PIN, GPIO_PIN_SET) : \
                              HAL_GPIO_WritePin(GT9XXX_RST_GPIO_PORT, GT9XXX_RST_GPIO_PIN, GPIO_PIN_RESET); \
                          }while(0)       // ��λ���� 

#define GT9XXX_INT        HAL_GPIO_ReadPin(GT9XXX_INT_GPIO_PORT, GT9XXX_INT_GPIO_PIN)     // ��ȡ��������
*/
													
#define GT9XXX_RST(x)     do{ x ? \
                              HAL_GPIO_WritePin(GT9XXX_RST_GPIO_Port, GT9XXX_RST_Pin, GPIO_PIN_SET) : \
                              HAL_GPIO_WritePin(GT9XXX_RST_GPIO_Port, GT9XXX_RST_Pin, GPIO_PIN_RESET); \
                          }while(0)       // ��λ���� 

#define GT9XXX_INT        HAL_GPIO_ReadPin(GT9XXX_INT_GPIO_Port, GT9XXX_INT_Pin)     // ��ȡ�������� 												

/* IIC��д���� */
#define GT9XXX_CMD_WR       0x28        /* д���� */
#define GT9XXX_CMD_RD       0x29        /* ������ */

/* GT9XXX ���ּĴ�������  */
#define GT9XXX_CTRL_REG     0x8040      /* GT9XXX���ƼĴ��� */
#define GT9XXX_CFGS_REG     0x8047      /* GT9XXX������ʼ��ַ�Ĵ��� */
#define GT9XXX_CHECK_REG    0x80FF      /* GT9XXXУ��ͼĴ��� */
#define GT9XXX_PID_REG      0x8140      /* GT9XXX��ƷID�Ĵ��� */

#define GT9XXX_GSTID_REG    0x814E      /* GT9XXX��ǰ��⵽�Ĵ������ */
#define GT9XXX_TP1_REG      0x8150      /* ��һ�����������ݵ�ַ */
#define GT9XXX_TP2_REG      0x8158      /* �ڶ������������ݵ�ַ */
#define GT9XXX_TP3_REG      0x8160      /* ���������������ݵ�ַ */
#define GT9XXX_TP4_REG      0x8168      /* ���ĸ����������ݵ�ַ */
#define GT9XXX_TP5_REG      0x8170      /* ��������������ݵ�ַ */
#define GT9XXX_TP6_REG      0x8178      /* ���������������ݵ�ַ */
#define GT9XXX_TP7_REG      0x8180      /* ���߸����������ݵ�ַ */
#define GT9XXX_TP8_REG      0x8188      /* �ڰ˸����������ݵ�ַ */
#define GT9XXX_TP9_REG      0x8190      /* �ھŸ����������ݵ�ַ */
#define GT9XXX_TP10_REG     0x8198      /* ��ʮ�����������ݵ�ַ */
 
/******************************************************************************************/
/* �������� */

uint8_t gt9xxx_wr_reg(uint16_t reg,uint8_t *buf,uint8_t len);
void gt9xxx_rd_reg(uint16_t reg,uint8_t *buf,uint8_t len); 
uint8_t gt9xxx_init(void);
uint8_t gt9xxx_scan(uint8_t mode); 

#endif
