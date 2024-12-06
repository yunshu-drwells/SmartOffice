#include "dialog.h"//包含window对话框 头文件
#include "include_dlg.h"
#include <stdlib.h>  //abs
#include "usart.h"  //fputc printf
#include "GUI.h"  //GUI_PID_STATE

#include "FreeRTOS.h"
#include "task.h"

WM_HWIN hWin1;
WM_HWIN hWin2;
int currentDialog = 2; // 1 for Dialog1, 2 for Dialog2

static int startX = 0;
static int startY = 0;
static int lastX = 0;
static int lastY = 0;
static int press = 1;
static int pressed = 0;

/*
static void ReadTouchScreen(void *pvParameters){
	GUI_PID_STATE State;
	//获取触摸笔状态值
	GUI_TOUCH_GetState(&State);
	if(State.Pressed){
		if(press){
		  startX = State.x;
		  startY = State.y;
		  //printf("start x: %d, start y: %d\n", startX, startY);
		  press = 0;
		}
		lastX = State.x;
		lastY = State.y;
		//printf("end x: %d, end y: %d\n", lastX, lastY);
		pressed = 1;
	}else{
		if(pressed){
			press = 1;
			//printf("end x: %d, end y: %d\n", lastX, lastY);
			// 计算滑动距离
			int deltaX = lastX - startX;
			int deltaY = lastY - startY;
			//printf("deltaX: %d, deltaY: %d\n", deltaX, deltaY);
		
			//printf("abs(deltaX): %d., abs(deltaY): %d\n", abs(deltaX), abs(deltaY));
			if (abs(deltaX) > abs(deltaY)) { // 水平滑动
				//printf("Horizontal\n");
				if (deltaX > 50) { // 触摸从右向左，窗口向左滑动
					if (currentDialog == 2) {
						printf("left flip\n");
						WM_HideWindow(hWin2);
						WM_ShowWindow(hWin1);
						currentDialog = 1;
					}
				}else if (deltaX < -50) { // 触摸从左向右，窗口向右滑动
					if (currentDialog == 1) {
						printf("right flip\n");
						WM_HideWindow(hWin1);
						WM_ShowWindow(hWin2);
						currentDialog = 2;
					}
				}
			}
	  }
	  pressed = 0;
	}
}
*/

static TaskHandle_t xReadTouchScreenTaskHandle;
static void ReadTouchScreenTask(void *pvParameters){
	GUI_PID_STATE State;
	while(1){
		//获取触摸笔状态值
		GUI_TOUCH_GetState(&State);
		if(State.Pressed){
			if(press){
			  startX = State.x;
			  startY = State.y;
			  //printf("start x: %d, start y: %d\n", startX, startY);
			  press = 0;
			}
			lastX = State.x;
			lastY = State.y;
			//printf("end x: %d, end y: %d\n", lastX, lastY);
			pressed = 1;
		}else{
			if(pressed){
				press = 1;
				//printf("end x: %d, end y: %d\n", lastX, lastY);
				// 计算滑动距离
				int deltaX = lastX - startX;
				int deltaY = lastY - startY;
				//printf("deltaX: %d, deltaY: %d\n", deltaX, deltaY);
			
				//printf("abs(deltaX): %d., abs(deltaY): %d\n", abs(deltaX), abs(deltaY));
				if (abs(deltaX) > abs(deltaY)) { // 水平滑动
					//printf("Horizontal\n");
					if (deltaX > 50) { // 触摸从左向右，窗口向右滑动
						if (currentDialog == 2) {
							printf("window flip right\n");
							WM_HideWindow(hWin2);
							WM_ShowWindow(hWin1);
							currentDialog = 1;
						}
					}else if (deltaX < -50) { // 触摸从右向左，窗口向左滑动
						if (currentDialog == 1) {
							printf("window flip left\n");
							WM_HideWindow(hWin1);
							WM_ShowWindow(hWin2);
							currentDialog = 2;
						}
					}
				}
		  }
		  pressed = 0;
		}
		// 延时20ms
        vTaskDelay(pdMS_TO_TICKS(20));
	}
}


void MainTask(void)
{
	GUI_Init();                     //初始化emWin/ucGUI
	GUI_UC_SetEncodeUTF8();         //使能utf-8方案

    //GUI_SetFont(&GUI_Fontfont);  // 设置字体
    //GUI_DispString("主灯");
    //GUI_SetFont(GUI_FONT_32_ASCII);  // 设置字体
    //GUI_DispString("233");

	//CreateWindow();                 //创建窗体,父窗体是桌面背景
	//CreateWindow0Main();
	
	//CreateTemperature();  //测试Temperature子窗口
	//CreateHumidity();  //测试Humidity子窗口
	//CreateBrightnesss();  //测试Brightnesss子窗口
	//CreateAlarm();  //测试Alarm子窗口
	//CreateFan();  //测试Fan子窗口
	//CreateLightingMaster();  //测试LightingMaster子窗口
	//CreateSpotlight();  //测试Spotlight子窗口
	
	//CreateWindow1Main();  //测试主页2
	
	//WM_CreateWindowAsChild(0, 0, 800, 480, WM_HBKWIN, ID_WINDOW_0, Window0_Callback, 0);
    //WM_CreateWindowAsChild(800, 0, 800, 480, WM_HBKWIN, ID_WINDOW_1, &Window1_Callback, 0);
	
	//CreateWindowMain();
	
	//检测任务提前开启
	// 创建获取屏幕滑动状态的任务
    xTaskCreate(ReadTouchScreenTask, "getPositationTask", 256, 0, tskIDLE_PRIORITY + 1, &xReadTouchScreenTaskHandle);	

	hWin1 = CreateWindow0Main();	
    hWin2 = CreateWindow1Main();
	WM_ShowWindow(hWin1);  //显示主页1
    WM_HideWindow(hWin2);	//隐藏主页2
	//WM_ShowWindow(hWin2);  //显示主页2
    //WM_HideWindow(hWin1);	//隐藏主页1
	//开启一个任务用于获取屏幕滑动状态
	// 创建获取屏幕滑动状态的任务
    //xTaskCreate(ReadTouchScreenTask, "getPositationTask", 256, 0, tskIDLE_PRIORITY + 1, &xReadTouchScreenTaskHandle);

	
	//CreateNetworkWindow();  //测试网络界面
	
	while(1) {
        GUI_Delay(20); //调用GUI_Delay函数延时20MS(最终目的是调用GUI_Exec()函数)
        // 读取屏幕滑动状态并改变界面
        //ReadTouchScreen(0);
	}

}
