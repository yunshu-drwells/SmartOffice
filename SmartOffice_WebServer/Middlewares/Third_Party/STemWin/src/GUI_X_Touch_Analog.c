#include "GUI.h"
#include "Touch.h"


void GUI_TOUCH_X_ActivateX(void) 
{

}

void GUI_TOUCH_X_ActivateY(void)
{

}

//��ȡX����ADֵ
int  GUI_TOUCH_X_MeasureX(void) 
{
	return XPT_Read_XY(CMD_RDX);
}

//��ȡY����ADֵ
int  GUI_TOUCH_X_MeasureY(void) 
{	
	return XPT_Read_XY(CMD_RDY);
}


