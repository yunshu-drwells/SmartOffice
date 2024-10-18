#include "GUI.h"
//#include "lcd.h"
#include "touch.h"  //tp_dev��tp_read_xy2


void GUI_TOUCH_X_ActivateX(void) 
{

}

void GUI_TOUCH_X_ActivateY(void)
{

}

//��ȡX����ADֵ
int  GUI_TOUCH_X_MeasureX(void) 
{
	int32_t xvalue;
	uint16_t x,y; 
	
	/* �жϵ��������ǵ����� */
	if ((tp_dev.touchtype & 0x80) == 0)
	{
			tp_dev.scan(0);
			
			if (tp_dev.sta & TP_PRES_DOWN)                              /* ������������ */
			{
					/* ���������ִ���û����ô˳�������⣬���touch.c�ļ��µ�TP_ERR_RANGE�궨�����ô�һ�� */
					tp_read_xy2(&x,&y); 
					return (4095 - x); 
					//return tp_read_xoy(0X90);                             /* CMD_RDX=0XD0  */
			}
	}
	else        /* ������ */
	{
			tp_dev.scan(0);
			xvalue = tp_dev.x[0];
			return xvalue;
	}
	return 0;
}

//��ȡY����ADֵ
int  GUI_TOUCH_X_MeasureY(void) 
{	
	int32_t yvalue;
	uint16_t x,y;
	
	/* �жϵ��������ǵ����� */
	if ((tp_dev.touchtype & 0x80) == 0)
	{
			/* ������ٰ��µ������������ */
			tp_dev.scan(0);
			
			if (tp_dev.sta & TP_PRES_DOWN)                              /* ������������ */
			{
					/* ���������ִ���û����ô˳�������⣬���touch.c�ļ��µ�TP_ERR_RANGE�궨�����ô�һ�� */
					tp_read_xy2(&x,&y); 
					return y; 
					//return tp_read_xoy(0XD0);                             /*CMD_RDX=0XD0  */
			}
	}
	else        /* ������ */
	{
			yvalue = tp_dev.y[0];
			return yvalue;
	}
	return 0;
}
