/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2017  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.44 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The  software has  been licensed  to STMicroelectronics International
N.V. a Dutch company with a Swiss branch and its headquarters in Plan-
les-Ouates, Geneva, 39 Chemin du Champ des Filles, Switzerland for the
purposes of creating libraries for ARM Cortex-M-based 32-bit microcon_
troller products commercialized by Licensee only, sublicensed and dis_
tributed under the terms and conditions of the End User License Agree_
ment supplied by STMicroelectronics International N.V.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : LCDConf_FlexColor_Template.c
Purpose     : Display controller configuration (single layer)
---------------------------END-OF-HEADER------------------------------
*/

/**
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics. 
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license SLA0044,
  * the "License"; You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *                      http://www.st.com/SLA0044
  *
  ******************************************************************************
  */

#include "GUI.h"
#include "GUIDRV_FlexColor.h"

#include "lcd.h"  //lcddev
#include "touch.h"  //tp_dev

/*********************************************************************
*
*       Layer configuration (to be modified)
*
**********************************************************************
*/

//
// Physical display size
//
//#define XSIZE_PHYS  240 // To be adapted to x-screen size
//#define YSIZE_PHYS  320 // To be adapted to y-screen size
#define XSIZE_PHYS  800 // To be adapted to x-screen size
#define YSIZE_PHYS  480 // To be adapted to y-screen size

#define COLOR_CONVERSION GUICC_8888
#define DISPLAY_DRIVER GUIDRV_WIN32

/*********************************************************************
*
*       Configuration checking
*
**********************************************************************
*/
#ifndef   VXSIZE_PHYS
  #define VXSIZE_PHYS XSIZE_PHYS
#endif
#ifndef   VYSIZE_PHYS
  #define VYSIZE_PHYS YSIZE_PHYS
#endif
#ifndef   XSIZE_PHYS
  #error Physical X size of display is not defined!
#endif
#ifndef   YSIZE_PHYS
  #error Physical Y size of display is not defined!
#endif
#ifndef   GUICC_565
  #error Color conversion not defined!
#endif
#ifndef   GUIDRV_FLEXCOLOR
  #error No display driver defined!
#endif

/*********************************************************************
*
*       Local functions
*
**********************************************************************
*/
/********************************************************************
*
*       LcdWriteReg
*
* Function description:
*   Sets display register
*/
	/*
static void LcdWriteReg(U16 Data) {
  // ... TBD by user
}
*/

/********************************************************************
*
*       LcdWriteData
*
* Function description:
*   Writes a value to a display register
*/
/*
static void LcdWriteData(U16 Data) {
  // ... TBD by user
}
*/

/********************************************************************
*
*       LcdWriteDataMultiple
*
* Function description:
*   Writes multiple values to a display register.
*/
/*
static void LcdWriteDataMultiple(U16 * pData, int NumItems) {
  while (NumItems--) {
    // ... TBD by user
  }
}
*/

/********************************************************************
*
*       LcdReadDataMultiple
*
* Function description:
*   Reads multiple values from a display register.
*/
/*
static void LcdReadDataMultiple(U16 * pData, int NumItems) {
  while (NumItems--) {
    // ... TBD by user
  }
}
*/

/*********************************************************************
*
*       Public functions
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_X_Config
*
* Function description:
*   Called during the initialization process in order to set up the
*   display driver configuration.
*
*/
void LCD_X_Config(void)
{
	GUI_DEVICE_CreateAndLink(&GUIDRV_Template_API, GUICC_M565, 0, 0); /* ������ʾ�������� */
	LCD_SetSizeEx(0, lcddev.width, lcddev.height);
	LCD_SetVSizeEx(0, lcddev.width, lcddev.height);

	
	/* �жϵ��������ǵ����� */
	if ((tp_dev.touchtype & 0x80) == 0)
	{
		/* ������Ļȫ��Ĭ��Ϊ2.8�� 320X240 */
		if (lcddev.dir == 0)        /* ���� */
		{
			GUI_TOUCH_SetOrientation(GUI_MIRROR_X);
			GUI_TOUCH_Calibrate(GUI_COORD_X, 0, lcddev.width, 155, 3903);
			GUI_TOUCH_Calibrate(GUI_COORD_Y, 0, lcddev.height, 188, 3935);
		}
		else                        /* ���� */
		{
			GUI_TOUCH_SetOrientation(GUI_SWAP_XY);
			GUI_TOUCH_Calibrate(GUI_COORD_X, 0, lcddev.width, 188, 3903);
			GUI_TOUCH_Calibrate(GUI_COORD_Y, 0, lcddev.height, 155, 3935);
		}
	}
	else                            /* ������ */
	{
		if (lcddev.id == 0X1963)    /* 1963Ϊ7���� 800*480 */
		{
			/* 6368 3816 */
			/* GUI_TOUCH_SetOrientation(GUI_SWAP_XY|GUI_MIRROR_Y); */
			GUI_TOUCH_Calibrate(GUI_COORD_X, 0, 800, 0, 799);
			GUI_TOUCH_Calibrate(GUI_COORD_Y, 0, 480, 0, 479);
		}
		else if (lcddev.id == 0X5310 || lcddev.id == 0X6804) /* 0X5510 0X6804Ϊ3.5�� 320x480 */
		{
			if (lcddev.dir == 0)    /* ���� */
			{
				GUI_TOUCH_SetOrientation(GUI_MIRROR_X);
			}
			else                    /* ����*/
			{
				GUI_TOUCH_SetOrientation(GUI_SWAP_XY);
			}
			
			GUI_TOUCH_Calibrate(GUI_COORD_X, 0, lcddev.width, 3931, 226);
			GUI_TOUCH_Calibrate(GUI_COORD_Y, 0, lcddev.height, 3812, 196);
		}
		else
		{
			GUI_TOUCH_Calibrate(GUI_COORD_X,0,lcddev.width,0,lcddev.width - 1);
			GUI_TOUCH_Calibrate(GUI_COORD_Y,0,lcddev.height,0,lcddev.height - 1);
		}
	}
}


/*********************************************************************
*
*       LCD_X_DisplayDriver
*
* Function description:
*   This function is called by the display driver for several purposes.
*   To support the according task the routine needs to be adapted to
*   the display controller. Please note that the commands marked with
*   'optional' are not cogently required and should only be adapted if
*   the display controller supports these features.
*
* Parameter:
*   LayerIndex - Index of layer to be configured
*   Cmd        - Please refer to the details in the switch statement below
*   pData      - Pointer to a LCD_X_DATA structure
*
* Return Value:
*   < -1 - Error
*     -1 - Command not handled
*      0 - Ok
*/
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData)
{
  GUI_USE_PARA(LayerIndex);
  GUI_USE_PARA(Cmd);
  GUI_USE_PARA(pData);
  return 0;
}

/*************************** End of file ****************************/

