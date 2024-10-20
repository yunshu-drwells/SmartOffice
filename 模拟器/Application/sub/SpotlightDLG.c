/*********************************************************************
*                                                                    *
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
*                                                                    *
**********************************************************************
*                                                                    *
* C-file generated by:                                               *
*                                                                    *
*        GUI_Builder for emWin version 5.44                          *
*        Compiled Nov 10 2017, 08:53:57                              *
*        (c) 2017 Segger Microcontroller GmbH & Co. KG               *
*                                                                    *
**********************************************************************
*                                                                    *
*        Internet: www.segger.com  Support: support@segger.com       *
*                                                                    *
**********************************************************************
*/

// USER START (Optionally insert additional includes)
// USER END

#include "DIALOG.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_0        (GUI_ID_USER + 0x00)
#define ID_TEXT_0        (GUI_ID_USER + 0x01)
#define ID_BUTTON_0        (GUI_ID_USER + 0x03)
#define ID_BUTTON_1        (GUI_ID_USER + 0x05)


// USER START (Optionally insert additional defines)
extern GUI_CONST_STORAGE GUI_BITMAP bmSpotlightOn;
extern GUI_CONST_STORAGE GUI_BITMAP bmSpotlightOff;
extern GUI_CONST_STORAGE GUI_BITMAP bmMainPage;
extern GUI_CONST_STORAGE GUI_BITMAP bmMainPagePressed;
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "Spotlight", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_0, 0, 0, 800, 32, 0, 0x64, 0 },
  { BUTTON_CreateIndirect, "", ID_BUTTON_0, 500, 200, 150, 150, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "", ID_BUTTON_1, 150, 200, 150, 150, 0, 0x0, 0 },
  // USER START (Optionally insert additional widgets)
  // USER END
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

// USER START (Optionally insert additional static code)
// USER END

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
  // USER START (Optionally insert additional variables)
  // USER END

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    //
    // Initialization of 'Spotlight'
    //
    hItem = pMsg->hWin;
    WINDOW_SetBkColor(hItem, GUI_MAKE_COLOR(0x00973F04));
    //
    // Initialization of 'Text'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
    TEXT_SetFont(hItem, GUI_FONT_32_ASCII);
    TEXT_SetText(hItem, "Spotlight");
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    // USER START (Optionally insert additional code for further widget initialization)
    // 根据空间ID,获取空间句柄
	hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
    //
	// Initialization of 'Button_Spotlight'
	//
	BUTTON_SetBitmap(hItem, BUTTON_BI_UNPRESSED, &bmSpotlightOff);
	BUTTON_SetBitmap(hItem, BUTTON_BI_PRESSED, &bmSpotlightOn);

    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
    //
	// Initialization of 'Button_MainPage'
	//
	BUTTON_SetBitmap(hItem, BUTTON_BI_UNPRESSED, &bmMainPage);
	BUTTON_SetBitmap(hItem, BUTTON_BI_PRESSED, &bmMainPagePressed);
    // USER END
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_BUTTON_0: // Notifications sent by ''
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_1: // Notifications sent by ''
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        //返回主页
        GUI_EndDialog(pMsg->hWin, 0);  //结束对话框
        CreateWindowMain(); // 创建WindowMain界面，调用其它界面的Create方法
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    // USER START (Optionally insert additional code for further Ids)
    // USER END
    }
    break;
  // USER START (Optionally insert additional message handling)
  // USER END
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       CreateSpotlight
*/
WM_HWIN CreateSpotlight(void);
WM_HWIN CreateSpotlight(void) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
