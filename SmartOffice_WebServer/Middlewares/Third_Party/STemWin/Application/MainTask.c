#include "dialog.h"//包含window对话框 头文件
#include "include_dlg.h"
void MainTask(void)

{

		WM_SetCreateFlags(WM_CF_MEMDEV);
    GUI_Init();                     //初始化emWin
    GUI_UC_SetEncodeUTF8();         //使能utf-8方案
    MainCreate();                 //创建窗体,父窗体是桌面背景
    while(1)
    {GUI_Delay(20);}       //调用GUI_Delay函数延时20MS(最终目的是调用GUI_Exec()函数)

}
