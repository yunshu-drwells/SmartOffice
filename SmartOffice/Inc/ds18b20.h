#ifndef __DS18B20_H
#define __DS18B20_H

#include "stm32f4xx_hal.h"

//IO��������
#define DS18B20_IO_IN()  {GPIOG->MODER&=~(3<<(6*2));GPIOG->MODER|=0<<(6*2);}	//PG6����ģʽ
#define DS18B20_IO_OUT() {GPIOG->MODER&=~(3<<(6*2));GPIOG->MODER|=1<<(6*2);} 	//PG6���ģʽ
 
////IO��������											   
#define	DS18B20_OUT_LOW   HAL_GPIO_WritePin(GPIOG, GPIO_PIN_15,  GPIO_PIN_RESET)   //���ݶ˿�	PG6
#define	DS18B20_OUT_HIGH  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_15,  GPIO_PIN_SET)   //���ݶ˿�	PG6
#define	DS18B20_DQ_IN     HAL_GPIO_ReadPin(GPIOG,  GPIO_PIN_15)  //���ݶ˿�	PG6 



uint8_t DS18B20_Init(void);					//��ʼ��DS18B20
short DS18B20_Get_Temp(void);				//��ȡ�¶�
void DS18B20_Start(void);						//��ʼ�¶�ת��
void DS18B20_Write_Byte(uint8_t dat);//д��һ���ֽ�
uint8_t DS18B20_Read_Byte(void);		//����һ���ֽ�
uint8_t DS18B20_Read_Bit(void);			//����һ��λ
uint8_t DS18B20_Check(void);				//����Ƿ����DS18B20
void DS18B20_Reset(void);							//��λDS18B20 
#endif
