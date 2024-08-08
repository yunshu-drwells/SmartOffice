#include "fonts.h"
#include "ff.h"
#include "delay.h"
#include "norflash.h"

 
/* ���������ֿ������Ϣ����ַ����С�� */
_font_info ftinfo;

/**
 * @brief       ��ʼ������
 * @param       ��
 * @retval      0, �ֿ����; ����, �ֿⶪʧ;
 */
uint8_t fonts_init(void)
{
    uint8_t t = 0;

    while (t < 10)  /* ������ȡ10��,���Ǵ���,˵��ȷʵ��������,�ø����ֿ��� */
    {
        t++;
        norflash_read((uint8_t *)&ftinfo, FONTINFOADDR, sizeof(ftinfo));    /* ����ftinfo�ṹ������ */

        if (ftinfo.fontok == 0XAA)
        {
            break;
        }
        
        delay_ms(20);
    }

    if (ftinfo.fontok != 0XAA)
    {
        return 1;
    }
    
    return 0;
}
