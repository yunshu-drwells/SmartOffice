/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#include "fatfs.h"

uint8_t retUSER;    /* Return value for USER */
char USERPath[4];   /* USER logical drive path */
FATFS USERFatFS;    /* File system object for USER logical drive */
FIL USERFile;       /* File object for USER */

/* USER CODE BEGIN Variables */
#include "usart.h"
#if 1
#define OPEN_LOG
#endif
uint8_t u8chr[] = "hello";
uint32_t u32Wbytes;

#define STRSIZE sizeof(u8chr)
uint8_t u8chr_read[STRSIZE] = {0};

#include "ff.h"
void get_fatfs_info() {
    FATFS *fs;
    DWORD fre_clust, fre_sect, tot_sect;
    FRESULT res;

    // ��ȡ�ļ�ϵͳ�Ŀ��д���
    res = f_getfree("0:", &fre_clust, &fs);
    if (res == FR_OK) {
        // �������������Ϳ���������
        tot_sect = (fs->n_fatent - 2) * fs->csize;
        fre_sect = fre_clust * fs->csize;

        // ��ӡ��������ʣ������
        printf("Total capacity: %lu KB\n", tot_sect / 2);
        printf("Free space: %lu KB\n", fre_sect / 2);
    } else {
        printf("Failed to get free clusters: %d\n", res);
    }
}
void format_flash(void) {
    uint8_t work[4096]; // ����������
    FRESULT res;

    // ��������
    retUSER = FATFS_LinkDriver(&USER_Driver, USERPath);

    // ��ʽ��Flash
    res = f_mkfs(USERPath, FM_FAT32, 4096, work, sizeof(work));
    if (res != FR_OK) {
        // �����ʽ������
        printf("Format failed: %d\n", res);
        return;
    }
    // �����ļ�ϵͳ
    res = f_mount(&USERFatFS, USERPath, 1);
    if (res != FR_OK) {
        // ������ش���
        printf("Mount failed: %d\n", res);
        return;
    }
    printf("Format and mount successful!\n");
}
/* USER CODE END Variables */    

void MX_FATFS_Init(void) 
{
  /*## FatFS: Link the USER driver ###########################*/
  retUSER = FATFS_LinkDriver(&USER_Driver, USERPath);

  /* USER CODE BEGIN Init */
  /* additional user code for init */  
#ifdef OPEN_LOG
	printf("FATFS_LinkDriver return: %d\n", retUSER); 
	printf("USERPath:%s\n", USERPath);
#endif
	
	/*
	// ��ʽ��Flash
	uint8_t work[512]; // ����������
	FRESULT res = f_mkfs(USERPath, FM_FAT32, 4096, (void*)work, sizeof(work));  //������������ÿ���ص�������4096/512=8
	printf("f_mkfs return %d\n", res);
	*/

	FRESULT res;
	
	res = f_mount(&USERFatFS,USERPath,1);
#ifdef OPEN_LOG
	printf("f_mount return %d\n", res);
#endif
	if(res == FR_NO_FILESYSTEM){  //13
#ifdef OPEN_LOG
		//��ʽ��
		printf("There is no valid FAT volume\n");
		printf("formating...!\n");
#endif
		/*
		// ��ʽ��Flash
		uint8_t work[4096]; // ����������
		FRESULT res = f_mkfs(USERPath, FM_ANY, 4096, (void*)work, sizeof(work));
		printf("f_mkfs return %d\n", res);
		*/
	}
  if(res == FR_OK){  //0
#ifdef OPEN_LOG		
		printf("f_mount right!\n");
#endif		
		get_fatfs_info();  // ��ȡ����ӡ�����ϸ��Ϣ
    if(f_open(&USERFile,(const char*)"fatfs.txt",FA_CREATE_ALWAYS|FA_WRITE) == FR_OK){
#ifdef OPEN_LOG			
			printf("f_open fatfs.txt right!\n");
#endif			
      if(f_write(&USERFile,u8chr,sizeof(u8chr),&u32Wbytes) == FR_OK){
#ifdef OPEN_LOG				
				printf("f_write fatfs.txt right!\n");
#endif				
				f_sync(&USERFile);  // ��� f_sync ȷ������ͬ��
				HAL_Delay(10);  // ���һ�����ݵ��ӳ�
        f_close(&USERFile);
#ifdef OPEN_LOG				
				printf("f_close fatfs.txt right!\n");
#endif				
      }
    }
		res = f_open(&USERFile,(const char*)"fatfs.txt",FA_READ);
#ifdef OPEN_LOG		
		printf("f_open return:%d\n", res);
#endif		
		if(res == FR_OK){
#ifdef OPEN_LOG			
			printf("f_open fatfs.txt right!\n");
#endif			
			if(f_read(&USERFile,u8chr_read,sizeof(u8chr),&u32Wbytes) == FR_OK){
#ifdef OPEN_LOG				
				printf("f_read fatfs.txt right!\n");
#endif				
				printf("read u8chr_read:%s\n", u8chr_read);
				f_close(&USERFile);
#ifdef OPEN_LOG				
				printf("f_close fatfs.txt right!\n");
#endif				
			}
		}
  }	
  /* USER CODE END Init */
}

/**
  * @brief  Gets Time from RTC 
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{
  /* USER CODE BEGIN get_fattime */
  return 0;
  /* USER CODE END get_fattime */  
}

/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
