#include "dialog.h"//����window�Ի��� ͷ�ļ�

extern GUI_CONST_STORAGE GUI_FONT GUI_Fontfont;

void MainTask(void)

{

	GUI_Init();                     //��ʼ��emWin/ucGUI
    GUI_UC_SetEncodeUTF8();         //ʹ��utf-8����

    //GUI_SetFont(&GUI_Fontfont);  // ��������
    //GUI_DispString("����");
    //GUI_SetFont(GUI_FONT_32_ASCII);  // ��������
    //GUI_DispString("233");

	//CreateWindow();                 //��������,�����������汳��
	CreateWindowMain();
	//CreateTemperature();  //����Temperature�Ӵ���
	//CreateHumidity();  //����Humidity�Ӵ���
	//CreateBrightnesss();  //����Brightnesss�Ӵ���
	//CreateAlarm();  //����Alarm�Ӵ���
	//CreateFan();  //����Fan�Ӵ���
	//CreateLightingMaster();  //����LightingMaster�Ӵ���
	//CreateSpotlight();  //����Spotlight�Ӵ���
    //CreateTurnOff();  //����TurnOff�Ӵ���
	while(1) {GUI_Delay(20);}       //����GUI_Delay������ʱ20MS(����Ŀ���ǵ���GUI_Exec()����)

}
