#include "dialog.h"//����window�Ի��� ͷ�ļ�
#include "include_dlg.h"
void MainTask(void)

{

		WM_SetCreateFlags(WM_CF_MEMDEV);
    GUI_Init();                     //��ʼ��emWin
    GUI_UC_SetEncodeUTF8();         //ʹ��utf-8����
    MainCreate();                 //��������,�����������汳��
    while(1)
    {GUI_Delay(20);}       //����GUI_Delay������ʱ20MS(����Ŀ���ǵ���GUI_Exec()����)

}
