#include "GUI.h"
#include "Touch.h"


void GUI_TOUCH_X_ActivateX(void) 
{

}

void GUI_TOUCH_X_ActivateY(void)
{

}

//获取X坐标AD值
int  GUI_TOUCH_X_MeasureX(void) 
{
	return XPT_Read_XY(CMD_RDX);
}

//获取Y坐标AD值
int  GUI_TOUCH_X_MeasureY(void) 
{	
	return XPT_Read_XY(CMD_RDY);
}


