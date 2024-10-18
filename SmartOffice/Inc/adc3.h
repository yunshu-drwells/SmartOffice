#ifndef __ADC3_H
#define __ADC3_H

#include "stm32f4xx.h"
#include "core_cm4.h"
#include "stm32f4xx_hal.h"


/******************************************************************************************/
/* ADC ���� */

#define ADC_ADCX                            ADC3
#define ADC3_CHY_CLK_ENABLE()               do{ __HAL_RCC_ADC3_CLK_ENABLE(); }while(0)      /* ADC1 ʱ��ʹ�� */

/******************************************************************************************/

void adc3_init(void);                           /* ADC3��ʼ�� */
void adc3_channel_set(ADC_HandleTypeDef *adc_handle, uint32_t ch,uint32_t rank, uint32_t stime);    /* ADC3ͨ������ */
uint32_t adc3_get_result(uint32_t ch);          /* ���ĳ��ͨ��ֵ */
uint32_t adc3_get_result_average(uint32_t ch, uint8_t times);                                       /* �õ�ĳ��ͨ����������������ƽ��ֵ */

#endif
