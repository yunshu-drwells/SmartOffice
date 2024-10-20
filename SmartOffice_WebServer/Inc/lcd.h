#ifndef __LCD_H__
#define __LCD_H__	

#include "stm32f4xx_hal.h"
#define   HDP	479
#define   HT	531
#define   HPS	43
#define   LPS	8
#define   HPW	10
#define   VDP	271
#define   VT	288
#define   VPS	12
#define   FPS	4
#define    VPW	10
/***********************************************************************************
������ɫ�궨��
***********************************************************************************/
#define   Black        0x0000                    // ��ɫ�� 0, 0, 0              
#define   Navy         0x000F                    // ����ɫ�� 0, 0, 128          
#define   Dgreen       0x03E0                    // ����ɫ�� 0, 128, 0          
#define   Dcyan        0x03EF                    // ����ɫ�� 0, 128, 128        
#define   Marooon       0x7800                    // ���ɫ��128, 0, 0           
#define   Purple       0x780F                    // ��ɫ�� 128, 0, 128          
#define   Olive        0x7BE0                    // ����̣�128, 128, 0         
#define   Lgray        0xC618                    // �Ұ�ɫ��192, 192, 192        
#define   Dgray        0x7BEF                    // ���ɫ��128, 128, 128        
#define   Blue         0x001F                    // ��ɫ�� 0, 0, 255            
#define   Green        0x07E0                 	 // ��ɫ�� 0, 255, 0             
#define   Cyan         0x07FF                    // ��ɫ�� 0, 255, 255           
#define   Red          0xF800                    // ��ɫ�� 255, 0, 0             
#define   Magenta      0xF81F                    // Ʒ�죺 255, 0, 255          
#define   Yellow       0xFFE0                    // ��ɫ�� 255, 255, 0           
#define   White        0xFFFF                    // ��ɫ�� 255, 255, 255   

//��������
void lcd_clear(uint16_t Color);
void lcd_init(void);
void write_data_Prepare(void);
unsigned short lcd_read_gram(unsigned int x,unsigned int y);
void LCD_DrawPoint(uint16_t xsta, uint16_t ysta, uint16_t color);
void LCD_ShowString(uint16_t x0, uint16_t y0, uint8_t *pcStr, uint16_t PenColor, uint16_t BackColor);
void LCD_Fill(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t colour);
#endif
