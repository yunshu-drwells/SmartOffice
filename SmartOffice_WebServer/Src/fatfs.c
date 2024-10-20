/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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

#include "stdio.h"

#include "string.h"

uint8_t retSD;    /* Return value for SD */
char SDPath[4];   /* SD logical drive path */
FATFS SDFatFS;    /* File system object for SD logical drive */
FIL SDFile;       /* File object for SD */

/* USER CODE BEGIN Variables */

char u8chr[] = "hello";

uint32_t u32Wbytes;

char SensorBuff[100];

/* USER CODE END Variables */    

void MX_FATFS_Init(void) 
{
  /*## FatFS: Link the SD driver ###########################*/
  retSD = FATFS_LinkDriver(&SD_Driver, SDPath);

  /* USER CODE BEGIN Init */
  
  if(f_mount(&SDFatFS,SDPath,1) == FR_OK)
  {
//    if(f_open(&SDFile,(const char*)"fatfs.txt",FA_CREATE_ALWAYS|FA_WRITE) == FR_OK)
//    {
//      if(f_write(&SDFile,u8chr,sizeof(u8chr),&u32Wbytes) == FR_OK)
//      {
//        f_close(&SDFile);
//      
//      }
    
//    }
  
  }
//	//创建文件
//	if(f_open(&SDFile,(const char*)"Sensor.csv",FA_CREATE_ALWAYS|FA_WRITE) == FR_OK){
//	
//		//格式化文件流
//		//创建表头
//		sprintf(SensorBuff, "序号,温度,湿度,光照\r\n");
//		f_write(&SDFile,SensorBuff,strlen(SensorBuff),&u32Wbytes);
//		
//		//循环写入表项
//		for(int i; i < 10; i++){
//			sprintf(SensorBuff, "%d,%d,%d,%d\r\n",i + 1, i + 20, i + 30, i + 40);
//			f_write(&SDFile,SensorBuff,strlen(SensorBuff),&u32Wbytes);
//			//刷新到文件中
//	  f_sync(&SDFile);
//		
//		}
//		//关闭文件，缓存写入文件内
//		f_close(&SDFile);
//	
//	
//	
//	
//	}
	
	
	
  
  
  /* additional user code for init */     
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
