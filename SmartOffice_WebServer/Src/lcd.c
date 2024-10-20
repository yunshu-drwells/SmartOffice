#include "lcd.h"
#include "Touch.h"
#include "stdlib.h"
#include "FreeRTOS.h"
#include "task.h"


#define MAX_HZ_POSX 480
#define MAX_HZ_POSY 272

#define LCD_RAM    *(__IO uint16_t *) (0x6C000002)    	//��ַ�Ĵ���
#define LCD_REG    *(__IO uint16_t *) (0x6C000000)	 	//ָ��Ĵ���

#define rw_data_prepare()               write_cmd(34)

void write_cmd(unsigned short cmd);
unsigned short read_data(void);
unsigned short DeviceCode;

static void delay(int cnt)
{
	volatile unsigned int dl;
	while(cnt--)
	{
		for(dl=0; dl<500; dl++);
	}
}

/*
 *	
 */
void write_cmd(unsigned short cmd)
{	
	LCD_REG = cmd;	
}

unsigned short read_data(void)
{
	unsigned short temp;	
	temp = LCD_RAM;
	temp = LCD_RAM;
	return temp;
}

void write_data(unsigned short data_code )
{	
	LCD_RAM = data_code;
}

void write_reg(unsigned char reg_addr,unsigned short reg_val)
{
	write_cmd(reg_addr);
	write_data(reg_val);
}

unsigned short read_reg(unsigned char reg_addr)
{
	unsigned short val=0;
	write_cmd(reg_addr);
	val = read_data();
	return (val);
}


void lcd_SetCursor(unsigned int x,unsigned int y)
{
	write_reg(0x004e,x);    /* 0-239 */
	write_reg(0x004f,y);    /* 0-319 */
}
/* ��ȡָ����ַ��GRAM */
unsigned short lcd_read_gram(unsigned int x,unsigned int y)
{
	unsigned short temp;
	taskENTER_CRITICAL();
	lcd_SetCursor(x,y);
	rw_data_prepare();
	/* dummy read */
	temp = read_data();
	temp = read_data();
	taskEXIT_CRITICAL();
	return temp;
}

void lcd_clear(unsigned short Color)
{
    unsigned int count;

	write_cmd(0x002a);	
	write_data(0);	    
	write_data(0);
	write_data(HDP>>8);	    
	write_data(HDP&0x00ff);
    write_cmd(0x002b);	
	write_data(0);	    
	write_data(0);
	write_data(VDP>>8);	    
	write_data(VDP&0x00ff);
	write_cmd(0x002c);	
	write_cmd(0x002c);


	for(count=0;count<130560;count++){
		
		write_data(Color);
	}
}



void lcd_init(void)
{

	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_9,GPIO_PIN_SET);
	
	delay(100);
	DeviceCode = read_reg(0x0000);

  write_cmd(0x002b);	
	write_data(0);

	HAL_Delay(50); 										// delay 50 ms 
	write_cmd(0x00E2);					//PLL multiplier, set PLL clock to 120M
	write_data(0x001D);					//N=0x36 for 6.5M, 0x23 for 10M crystal
//	write_data(0x0024);		
	write_data(0x0002);
	write_data(0x0004);
	
	write_cmd(0x00E0);					//PLL enable
	write_data(0x0001);

	HAL_Delay(1); 
	write_cmd(0x00E0);
	write_data(0x0003);
	HAL_Delay(5); 
//	Delay(10);
	write_cmd(0x0001);  					//software reset
	HAL_Delay(5); 
//	Delay(10);
	write_cmd(0x00E6);					//PLL setting for PCLK, depends on resolution
//	write_data(0x0001);
//	write_data(0x0033);
//	write_data(0x0032);
	write_data(0x0009);
	write_data(0x00D9);
	write_data(0x0016);

	write_cmd(0x00B0);								//LCD SPECIFICATION
	write_data(0x0020);
	write_data(0x0000);
	write_data((HDP>>8)&0X00FF);			//Set HDP
	write_data(HDP&0X00FF);
    write_data((VDP>>8)&0X00FF);			//Set VDP
	write_data(VDP&0X00FF);
    write_data(0x0000);

	write_cmd(0x00B4);								//HSYNC
	write_data((HT>>8)&0X00FF); 			//Set HT
	write_data(HT&0X00FF);
	write_data((HPS>>8)&0X00FF);			//Set HPS
	write_data(HPS&0X00FF);
	write_data(HPW);									//Set HPW
	write_data((LPS>>8)&0X00FF); 			//Set HPS
	write_data(LPS&0X00FF);
	write_data(0x0000);

	write_cmd(0x00B6);								//VSYNC
	write_data((VT>>8)&0X00FF);		   	//Set VT
	write_data(VT&0X00FF);
	write_data((VPS>>8)&0X00FF); 			//Set VPS
	write_data(VPS&0X00FF);
	write_data(VPW);						//Set VPW
	write_data((FPS>>8)&0X00FF);			//Set FPS
	write_data(FPS&0X00FF);

	//=============================================

	//=============================================
	write_cmd(0x00BA);
	write_data(0x0005);//0x000F);    //GPIO[3:0] out 1

	write_cmd(0x00B8);
	write_data(0x0007);    //GPIO3=input, GPIO[2:0]=output
	write_data(0x0001);    //GPIO0 normal

	write_cmd(0x0036); //rotation
	write_data(0x0000);

	HAL_Delay(50);
	write_cmd(0x00BE); //set PWM for B/L
	write_data(0x0006);
	write_data(0x0080);
	
	write_data(0x0001);
	write_data(0x00f0);
	write_data(0x0000);
	write_data(0x0000);

	write_cmd(0x00d0);//���ö�̬����������� 
	write_data(0x000d);
   
	write_cmd(0x00F0); //pixel data interface
	write_data(0x0003); //03:16λ   02:24λ

	write_cmd(0x0029); //display on

	//lcd_data_bus_test();
//	lcd_clear(Red);	

}
/*********************************************************************************************************
** Functoin name:       LCD_SetCursor
** Descriptions:        ��������
** input paraments:     Xpos��Ypos ����
** output paraments:    ��    
** Returned values:     ��
*********************************************************************************************************/

void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
	write_cmd(0x002A);	
	write_data(Xpos>>8);	    
	write_data(Xpos&0x00ff);
	write_data(479>>8);	    
	write_data(479&0x00ff);
    write_cmd(0x002b);	
	write_data(Ypos>>8);	    
	write_data(Ypos&0x00ff);
	write_data(271>>8);	    
	write_data(271&0x00ff);
}
/*********************************************************************************************************
** Functoin name:       LCD_DrawPoint
** Descriptions:        ����
** input paraments:     x.y:���������
						color����ɫ
** output paraments:    ��    
** Returned values:     ��
*********************************************************************************************************/

void LCD_DrawPoint(uint16_t xsta, uint16_t ysta, uint16_t color)
{
	taskENTER_CRITICAL();
	LCD_SetCursor(xsta, ysta);  		/*���ù��λ��  */
	write_data_Prepare();          	/*��ʼд��GRAM */
	write_data(color); 
	taskEXIT_CRITICAL();

}

///*********************************************************************************************************
//** Functoin name:       write_data_Prepare
//** Descriptions:        д���ݿ�ʼ
//** input paraments:     ��
//** output paraments:    ��    
//** Returned values:     ��
//*********************************************************************************************************/

void write_data_Prepare(void)
{
	write_cmd(0x002C);
}
/*********************************************************************************************************
** Functoin name:       LCD_WindowMax
** Descriptions:        ���ô���
** input paraments:     ���ڵ�λ��
** output paraments:    ��    
** Returned values:     ��
*********************************************************************************************************/
void LCD_WindowMax (unsigned int xsta,unsigned int ysta,unsigned int xend,unsigned int yend) 
{
	write_cmd(0X002A);
	write_data(xsta>>8);
	write_data(xsta&0X00FF);
	write_data(xend>>8);
	write_data(xend&0X00FF);
		
	write_cmd(0X002B);	
	write_data(ysta>>8);	
	write_data(ysta&0X00FF);
	write_data(yend>>8);	
	write_data(yend&0X00FF);			
}
/*********************************************************************************************************
** Functoin name:       LCD_Fill
** Descriptions:       ��䴰��
** input paraments:     ���ڵ�λ��
						colour����ɫ
** output paraments:    ��    
** Returned values:     ��
*********************************************************************************************************/
void LCD_Fill(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t colour)
{                    
    uint32_t n;

	/*���ô���	*/	
	LCD_WindowMax (xsta, ysta, xend, yend); 
	write_data_Prepare();         /*��ʼд��GRAM*/	 	   	   
	n=(uint32_t)(yend-ysta+1)*(xend-xsta+1);    
	while(n--){write_data(colour);} /*��ʾ��������ɫ*/	 
//	for(i = xsta ;i<xend;i++)
//		for(j = ysta ;j<yend;j++)
//				LCD_DrawPoint(i,j,colour);	
} 

