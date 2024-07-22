#include "ds18b20.h"
#include "delay.h"
uint32_t usctick = 0;
uint32_t time_delay = 0;
extern TIM_HandleTypeDef htim6;

//��λDS18B20
void DS18B20_Reset(void)	   
{                 
	DS18B20_IO_OUT();   //����Ϊ���
	DS18B20_OUT_LOW ; 	  //����DQ
	delay_us(650);      //����650us
	DS18B20_OUT_HIGH ; 	//����DQ 
	delay_us(20);       //20US
}

//�ȴ�DS18B20�Ļ�Ӧ
//����1:δ��⵽DS18B20�Ĵ���
//����0:����
uint8_t DS18B20_Check(void) 	   
{   
	uint8_t retry=0;
	DS18B20_IO_IN();    //����Ϊ����
	
	 //�ȴ�DS18B20�������߻�Ӧ���������200usδ���ͣ�����Ϊδ��Ӧ
  while ((DS18B20_DQ_IN == 1) && (retry<200))   
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=200)return 1;        //DS18B20��ʱδ��������
	else retry=0;                  //DS18B20��������
	 
  while ( (DS18B20_DQ_IN == 0 ) && ( retry < 240) )    //�����������ߵ�ʱ���Ƿ���240us��
	{
		retry++;
		delay_us(1);
	};
	if(retry>=240)return 1;	      //����240us����   
	
	return 0;                     //��ȷ��Ӧ
}

//��DS18B20��ȡһ��λ
//����ֵ��1/0
uint8_t DS18B20_Read_Bit(void) 
{
	uint8_t data;
	DS18B20_IO_OUT();   //����Ϊ���
	DS18B20_OUT_LOW ; 	  //����DQ
	delay_us(3);
	DS18B20_OUT_HIGH ; 	//����DQ 
	DS18B20_IO_IN();    //����Ϊ����
	delay_us(12);
	
	if(DS18B20_DQ_IN)  data=1;
	else data=0;
	
	delay_us(50);  
	
	return data;
}

//��DS18B20��ȡһ���ֽ�
//����ֵ������������,�ȶ����ݵĵ�λ
uint8_t DS18B20_Read_Byte(void)   
{        
	uint8_t i,j,dat;
	dat=0;
	for (i=0;i<8;i++) 
	{
        j=DS18B20_Read_Bit();
        dat=(j<<i)|dat;
    }						    
	return dat;
}

//дһ���ֽڵ�DS18B20
//dat��Ҫд����ֽ�
void DS18B20_Write_Byte(uint8_t dat)     
 {             
    uint8_t j;
    uint8_t testb;
    DS18B20_IO_OUT();     //����Ϊ���
    for (j=1;j<=8;j++) 
	{
        testb=dat&0x01;
        dat=dat>>1;
        if(testb)       // д1
        {
            DS18B20_OUT_LOW ; 	  //����DQ
            delay_us(2);                            
            DS18B20_OUT_HIGH ; 	  //����DQ 
            delay_us(60);             
        }
        else            //д0
        {
            DS18B20_OUT_LOW ; 	  //����DQ
            delay_us(60);             
            DS18B20_OUT_HIGH ; 	  //����DQ 
            delay_us(2);                          
        }
    }
}
 

void DS18B20_Start(void)
{ 
		//��ʼ�¶�ת��
    DS18B20_Reset();	   
    DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);   // skip rom
    DS18B20_Write_Byte(0x44);   // convert
	
		//��ʼ��ȡ�¶�
		DS18B20_Reset();
    DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);   // skip rom
    DS18B20_Write_Byte(0xbe);   // convert
}

//��ʼ��DS18B20��IO�� DQ ͬʱ���DS�Ĵ���
//����1:������
//����0:����    	 
uint8_t DS18B20_Init(void)
{
	DS18B20_Reset();
	return DS18B20_Check();
}

//��ds18b20�õ��¶�ֵ
//���ȣ�0.1C
//����ֵ���¶�ֵ ��-550~1250�� 
short DS18B20_Get_Temp(void)
{
    uint8_t temp;
    uint8_t TL,TH;
    short tem;
	
    DS18B20_Start ();           //��ʼת����ȡ
	    
    TL=DS18B20_Read_Byte();     // LSB   
    TH=DS18B20_Read_Byte();     // MSB  	
    if(TH>7)										//�¶�Ϊ��  
    {
        TH=~TH;
        TL=~TL; 
        temp=0;      //�¶�Ϊ��  
    }else temp=1;    //�¶�Ϊ��	  
	  
    tem=TH; //��ø߰�λ
    tem<<=8;    
    tem+=TL;//��õװ�λ
    tem=(double)tem*0.625;//ת��  ��ò�������λ��11λ�¶�ֵ   
		
	if(temp)return tem; //�����¶�ֵ
	else return -tem;    
}
