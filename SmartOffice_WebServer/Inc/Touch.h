#ifndef _TOUCH_H_
#define _TOUCH_H_

#include "stm32f4xx.h"

/* ADS7843/7846/UH7843/7846/XPT2046/TSC2046 ָ� */
#define CMD_RDY 0X90  //0B10010000���ò�ַ�ʽ��Y����
#define CMD_RDX	0XD0  //0B11010000���ò�ַ�ʽ��X���� 


/* ����״̬	*/ 
#define Key_Down 0x01
#define Key_Up   0x00 

/* �ʸ˽ṹ�� */
typedef struct 
{
	uint16_t X0;//ԭʼ����
	uint16_t Y0;
	uint16_t ADC_X; //X����ĵ�ѹֵ
	uint16_t ADC_Y;	//Y�����ϵĵ�ѹֵ					   	    
	uint8_t  Key_Sta;//�ʵ�״̬			  
	//������У׼����
	float xfac;
	short xoff;
	
	float yfac;
	short yoff;
}Pen_Holder;


#define SPI_TOUCH_CS_LOW()       HAL_GPIO_WritePin(GPIOG, GPIO_PIN_15, GPIO_PIN_RESET)
#define SPI_TOUCH_CS_HIGH()      HAL_GPIO_WritePin(GPIOG, GPIO_PIN_15, GPIO_PIN_SET)

uint16_t XPT_Read_XY(uint8_t xy);
uint8_t Convert_Pos(void);


#endif

