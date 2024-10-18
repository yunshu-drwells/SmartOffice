#ifndef __CT_IIC_H
#define __CT_IIC_H

#include "stm32f4xx.h"
#include "core_cm4.h"
#include "stm32f4xx_hal.h"
#include "main.h"  //���Ŷ��� GT9XXX_SCL��GT9XXX_MOSI

/* IO���� */
#define CT_IIC_SCL(x)     do{ x ? \
                              HAL_GPIO_WritePin(GT9XXX_SCL_GPIO_Port, GT9XXX_SCL_Pin, GPIO_PIN_SET) : \
                              HAL_GPIO_WritePin(GT9XXX_SCL_GPIO_Port, GT9XXX_SCL_Pin, GPIO_PIN_RESET); \
                          }while(0)       /* SCL */

#define CT_IIC_SDA(x)     do{ x ? \
                              HAL_GPIO_WritePin(GT9XXX_MOSI_GPIO_Port, GT9XXX_MOSI_Pin, GPIO_PIN_SET) : \
                              HAL_GPIO_WritePin(GT9XXX_MOSI_GPIO_Port, GT9XXX_MOSI_Pin, GPIO_PIN_RESET); \
                          }while(0)       /* SDA */

#define CT_READ_SDA       HAL_GPIO_ReadPin(GT9XXX_MOSI_GPIO_Port, GT9XXX_MOSI_Pin) /* ��ȡSDA */


/* IIC���в������� */
void ct_iic_init(void);             /* ��ʼ��IIC��IO�� */
void ct_iic_stop(void);             /* ����IICֹͣ�ź� */
void ct_iic_start(void);            /* ����IIC��ʼ�ź� */

void ct_iic_ack(void);              /* IIC����ACK�ź� */
void ct_iic_nack(void);             /* IIC������ACK�ź� */
uint8_t ct_iic_wait_ack(void);      /* IIC�ȴ�ACK�ź� */

void ct_iic_send_byte(uint8_t txd);         /* IIC����һ���ֽ� */
uint8_t ct_iic_read_byte(unsigned char ack);/* IIC��ȡһ���ֽ� */

#endif
