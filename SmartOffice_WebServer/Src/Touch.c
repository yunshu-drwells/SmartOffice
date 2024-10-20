#include "stm32f4xx.h"
#include "Touch.h"
#include "spi.h"

//校验参数（计算得出）
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

//SPI读数据 
//从7846/7843/XPT2046/UH7843/UH7846读取adc值	  

/*************************************************/
/* 功    能：读取X轴或Y轴的ADC值				 */
/* 入口参数：CMD:命令                            */ 
/*************************************************/
uint16_t XPT_Read_AD(uint8_t CMD)
{	  
	uint16_t NUMH,NUML;
	uint16_t Num = 0; 

	SPI_TOUCH_CS_LOW();     // CS=0 开始SPI通信	 

	WR_Cmd(CMD);
	HAL_Delay(1);            // 延时等待转换完成
	
	
	NUMH=WR_Cmd(0x00);
	NUML=WR_Cmd(0x00);
	Num=((NUMH)<<8)+NUML; 	
	Num>>=4;                // 只有高12位有效.
	
	SPI_TOUCH_CS_HIGH();    // CS=1 结束SPI通信	 
	
	//printf("num = %d\n", Num);
	return(Num);  
}


#define READ_TIMES 10    //读取次数
#define LOST_VAL 4	     //丢弃值
/*************************************************/
/* 功    能：读取X轴或Y轴的ADC值				 */
/* 入口参数：CMD_RDX:读取X的ADC值                */ 
/*           CMD_RDY:读取Y的ADC值                */
/* 说    明：与上一个函数相比，这个带有滤波，多读几个数据，去掉一些后，求平均*/
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
	for(i=0;i<READ_TIMES-1; i++)//排序
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//升序排列
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
/* 功    能：读取X轴和轴的ADC值                  */
/* 入口参数：&Pen_Point.X_ADC,&Pen_Point.Y_ADC   */
/* 出口参数：0：成功（返回的X,Y_ADC值有效）      */
/*           1: 失败（返回的X,Y_ADC值无效）      */ 
/*************************************************/
uint8_t Read_XPT(uint16_t *x,uint16_t *y)
{
	uint16_t xtemp,ytemp;			 	 		  
	xtemp=XPT_Read_XY(CMD_RDX);
	ytemp=XPT_Read_XY(CMD_RDY);	  												   
	//if(xtemp<100||ytemp<100)return 1;//读数失败
	*x=xtemp;
	*y=ytemp;
	return 0;//读数成功
}
/*************************************************/
/* 功能：连续两次读取ADC值						 */
/* 原理：把两次读取的值作比较，在误差范围内可取  */ 
/* 入口参数：&Pen_Point.X_ADC,&Pen_Point.Y_ADC   */
/* 出口参数：0：成功（返回的X,Y_ADC值有效）      */
/*           1: 失败（返回的X,Y_ADC值无效）      */ 
/*************************************************/
#define ERR_RANGE 50 //误差范围 
uint8_t Read_XPT2(uint16_t*x,uint16_t *y) 
{
	uint16_t x1,y1;
 	uint16_t x2,y2;
 	uint8_t res; 
	   
    res=Read_XPT(&x1,&y1);  // 第一次读取ADC值 
    if(res==1)return(1);	// 如果读数失败，返回1
	  HAL_Delay(3);
	
    res=Read_XPT(&x2,&y2);	// 第二次读取ADC值   
    if(res==1)return(1);   	// 如果读数失败，返回1
	
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))     //前后两次采样在+-50内
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
        return 0;	 // 正确读取，返回0
    }else return 1;	 // 前后不在+-50内，读数错误 
} 




//转换结果
//根据触摸屏的校准参数来决定转换后的结果,保存在X0,Y0中
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


