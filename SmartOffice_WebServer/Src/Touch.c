#include "stm32f4xx.h"
#include "Touch.h"
#include "spi.h"

//У�����������ó���
Pen_Holder Pen_Point ={
	.xfac = 0.259067,
	.xoff = -20,
	.yfac = 0.158228,
	.yoff = -38, 
};

extern SPI_HandleTypeDef hspi2;


static uint8_t WR_Cmd(uint8_t cmd)
{   
	uint8_t Tx_DATA[1] = {cmd};
	uint8_t Rx_DATA[1] = {0};
	HAL_SPI_TransmitReceive( &hspi2,Tx_DATA,Rx_DATA,1,0x1000);
	
	return Rx_DATA[0];
}	

//SPI������ 
//��7846/7843/XPT2046/UH7843/UH7846��ȡadcֵ	  

/*************************************************/
/* ��    �ܣ���ȡX���Y���ADCֵ				 */
/* ��ڲ�����CMD:����                            */ 
/*************************************************/
uint16_t XPT_Read_AD(uint8_t CMD)
{	  
	uint16_t NUMH,NUML;
	uint16_t Num = 0; 

	SPI_TOUCH_CS_LOW();     // CS=0 ��ʼSPIͨ��	 

	WR_Cmd(CMD);
	HAL_Delay(1);            // ��ʱ�ȴ�ת�����
	
	
	NUMH=WR_Cmd(0x00);
	NUML=WR_Cmd(0x00);
	Num=((NUMH)<<8)+NUML; 	
	Num>>=4;                // ֻ�и�12λ��Ч.
	
	SPI_TOUCH_CS_HIGH();    // CS=1 ����SPIͨ��	 
	
	//printf("num = %d\n", Num);
	return(Num);  
}


#define READ_TIMES 10    //��ȡ����
#define LOST_VAL 4	     //����ֵ
/*************************************************/
/* ��    �ܣ���ȡX���Y���ADCֵ				 */
/* ��ڲ�����CMD_RDX:��ȡX��ADCֵ                */ 
/*           CMD_RDY:��ȡY��ADCֵ                */
/* ˵    ��������һ��������ȣ���������˲�������������ݣ�ȥ��һЩ����ƽ��*/
/*************************************************/
uint16_t XPT_Read_XY(uint8_t xy)
{
	uint16_t i, j;
	uint16_t buf[READ_TIMES];
	uint16_t sum=0;
	uint16_t temp;
	for(i=0;i<READ_TIMES;i++)
	{				 
		buf[i]=XPT_Read_AD(xy);	    
	}				    
	for(i=0;i<READ_TIMES-1; i++)//����
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//��������
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	  
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp;   
} 
/*************************************************/
/* ��    �ܣ���ȡX������ADCֵ                  */
/* ��ڲ�����&Pen_Point.X_ADC,&Pen_Point.Y_ADC   */
/* ���ڲ�����0���ɹ������ص�X,Y_ADCֵ��Ч��      */
/*           1: ʧ�ܣ����ص�X,Y_ADCֵ��Ч��      */ 
/*************************************************/
uint8_t Read_XPT(uint16_t *x,uint16_t *y)
{
	uint16_t xtemp,ytemp;			 	 		  
	xtemp=XPT_Read_XY(CMD_RDX);
	ytemp=XPT_Read_XY(CMD_RDY);	  												   
	//if(xtemp<100||ytemp<100)return 1;//����ʧ��
	*x=xtemp;
	*y=ytemp;
	return 0;//�����ɹ�
}
/*************************************************/
/* ���ܣ��������ζ�ȡADCֵ						 */
/* ԭ�������ζ�ȡ��ֵ���Ƚϣ�����Χ�ڿ�ȡ  */ 
/* ��ڲ�����&Pen_Point.X_ADC,&Pen_Point.Y_ADC   */
/* ���ڲ�����0���ɹ������ص�X,Y_ADCֵ��Ч��      */
/*           1: ʧ�ܣ����ص�X,Y_ADCֵ��Ч��      */ 
/*************************************************/
#define ERR_RANGE 50 //��Χ 
uint8_t Read_XPT2(uint16_t*x,uint16_t *y) 
{
	uint16_t x1,y1;
 	uint16_t x2,y2;
 	uint8_t res; 
	   
    res=Read_XPT(&x1,&y1);  // ��һ�ζ�ȡADCֵ 
    if(res==1)return(1);	// �������ʧ�ܣ�����1
	  HAL_Delay(3);
	
    res=Read_XPT(&x2,&y2);	// �ڶ��ζ�ȡADCֵ   
    if(res==1)return(1);   	// �������ʧ�ܣ�����1
	
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))     //ǰ�����β�����+-50��
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
        return 0;	 // ��ȷ��ȡ������0
    }else return 1;	 // ǰ����+-50�ڣ��������� 
} 




//ת�����
//���ݴ�������У׼����������ת����Ľ��,������X0,Y0��
uint8_t Convert_Pos(void)
{		
	HAL_Delay(8);
	if(Read_XPT2(&Pen_Point.ADC_X,&Pen_Point.ADC_Y) ==0 )
	{	
		//	printf("adc_x= %d, adc_y = %d\n",Pen_Point.ADC_X,Pen_Point.ADC_Y);
			Pen_Point.X0=Pen_Point.xfac*Pen_Point.ADC_X+Pen_Point.xoff;
			Pen_Point.Y0=Pen_Point.yfac*Pen_Point.ADC_Y+Pen_Point.yoff;  

			return 1;
	}
	else return 0;
}


