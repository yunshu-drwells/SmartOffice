#ifndef __GT9XXX_H
#define __GT9XXX_H

#include "stm32f4xx.h"
#include "core_cm4.h"
#include "stm32f4xx_hal.h"
#include "main.h"  //���Ŷ��� GT9XXX_RST��GT9XXX_INT

/* ����ݴ��������ӵ�оƬ����(δ����IIC����) 
 * IO�������� 
 */
#define GT9XXX_RST(x)     do{ x ? \
                              HAL_GPIO_WritePin(GT9XXX_RST_GPIO_Port, GT9XXX_RST_Pin, GPIO_PIN_SET) : \
                              HAL_GPIO_WritePin(GT9XXX_RST_GPIO_Port, GT9XXX_RST_Pin, GPIO_PIN_RESET); \
                          }while(0)       /* ��λ���� */

#define GT9XXX_INT        HAL_GPIO_ReadPin(GT9XXX_INT_GPIO_Port, GT9XXX_INT_Pin)     /* ��ȡ�������� */

/* IIC��д���� */
#define GT9XXX_CMD_WR       0X28        /* д���� */
#define GT9XXX_CMD_RD       0X29        /* ������ */

/* GT9XXX ���ּĴ�������  */
#define GT9XXX_CTRL_REG     0X8040      /* GT9XXX���ƼĴ��� */
#define GT9XXX_CFGS_REG     0X8047      /* GT9XXX������ʼ��ַ�Ĵ��� */
#define GT9XXX_CHECK_REG    0X80FF      /* GT9XXXУ��ͼĴ��� */
#define GT9XXX_PID_REG      0X8140      /* GT9XXX��ƷID�Ĵ��� */

#define GT9XXX_GSTID_REG    0X814E      /* GT9XXX��ǰ��⵽�Ĵ������ */
#define GT9XXX_TP1_REG      0X8150      /* ��һ�����������ݵ�ַ */
#define GT9XXX_TP2_REG      0X8158      /* �ڶ������������ݵ�ַ */
#define GT9XXX_TP3_REG      0X8160      /* ���������������ݵ�ַ */
#define GT9XXX_TP4_REG      0X8168      /* ���ĸ����������ݵ�ַ */
#define GT9XXX_TP5_REG      0X8170      /* ��������������ݵ�ַ */
#define GT9XXX_TP6_REG      0X8178      /* ���������������ݵ�ַ */
#define GT9XXX_TP7_REG      0X8180      /* ���߸����������ݵ�ַ */
#define GT9XXX_TP8_REG      0X8188      /* �ڰ˸����������ݵ�ַ */
#define GT9XXX_TP9_REG      0X8190      /* �ھŸ����������ݵ�ַ */
#define GT9XXX_TP10_REG     0X8198      /* ��ʮ�����������ݵ�ַ */
 
/******************************************************************************************/
/* �������� */

uint8_t gt9xxx_wr_reg(uint16_t reg,uint8_t *buf,uint8_t len);   /* ��gt9xxд������ */
void gt9xxx_rd_reg(uint16_t reg,uint8_t *buf,uint8_t len);      /* ��gt9xx��ȡ���� */
uint8_t gt9xxx_init(void);                                      /* ��ʼ��gt9xxx������ */
uint8_t gt9xxx_scan(uint8_t mode);                              /* ɨ�败���� */

#endif
