#ifndef __LSENS_H
#define __LSENS_H

#include "stm32f4xx.h"
#include "core_cm4.h"
#include "stm32f4xx_hal.h"

/******************************************************************************************/
/* ������������ӦADC3���������ź�ͨ�� ���� */

#define LSENS_ADC3_CHX_GPIO_PORT            GPIOF
#define LSENS_ADC3_CHX_GPIO_PIN             GPIO_PIN_7
#define LSENS_ADC3_CHX_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOF_CLK_ENABLE(); }while(0)     /* PF��ʱ��ʹ�� */

#define LSENS_ADC3_CHX                      ADC_CHANNEL_5                                   /* ͨ��Y,  0 <= Y <= 18 */ 

/******************************************************************************************/

void lsens_init(void);          /* ��ʼ������������ */
uint8_t lsens_get_val(void);    /* ��ȡ������������ֵ */
#endif
