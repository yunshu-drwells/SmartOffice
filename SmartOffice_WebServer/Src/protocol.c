/***************************************************************************
* File                  : protocol.c
* Function Name         : 
* Description           : 协议处理
* Version               : v1.0
* Author                : zhengdao.liu
* Date                  : 2019/8/22
* Parameter             :
* Return                :
* Note                  :
***************************************************************************/

#include "protocol.h"

#include "stdio.h"

#include "string.h"
//传感器数据内容
extern int SensorValue[3];
//引用uart1
extern UART_HandleTypeDef huart1;
//定义协议结构体数据
ProtocolDataType  ProtocolData = {
    HEAD,
    NODE_ID,
    NODE_TYPE,
    DATA_LEN,
    0x0,
    0x0,
    TAIL,
};


/******************************************************************************
 * Name:    CRC-8               x8+x2+x+1
 * Poly:    0x07
 * Init:    0x00
 * Refin:   False
 * Refout:  False
 * Xorout:  0x00
 * Note:
 *****************************************************************************/
uint8 crc8(uint8 *data, uint8 length)
{
    uint8 i;
    uint8 crc = 0;        // Initial value
    while(length--)
    {
        crc ^= *data++;        // crc ^= *data; data++;
        for ( i = 0; i < 8; i++ )
        {
            if ( crc & 0x80 )
                crc = (crc << 1) ^ 0x07;
            else
                crc <<= 1;
        }
    }
    return crc;
}

/***************************************************************************
* Function Name         : ProtocolCheck
* Description           : 检查协议是否合法
* Parameter             :Data：协议数据包 DataLen：数据包长度
* Return                :合法：1，非法：-1
* Note                  : NULL
***************************************************************************/
uint8  ProtocolCheck(uint8 *Data,uint16 DataLen){
  
  uint8 crc = 0;
//  1.检查协议头
  if(Data[0] != HEAD){
    printf("Protocol Head Error!\r\n");
    return 0;
  }
//  2.检查协议尾
  if(Data[DataLen-1] != TAIL){
    printf("Protocol Tail Error!\r\n");
    return 0;
  }
//  3.校验CRC
  
  crc = crc8(&Data[1],DataLen-3);
  if(crc != Data[DataLen-2]){
    printf("Protocol CRC Error!\r\n");
    return 0;
  }
  return 1;

}
/***************************************************************************
* Function Name         : 
* Description           : 协议处理
* Parameter             :
* Return                :
* Note                  :
***************************************************************************/
void ProtocolParse(uint8 *Data,uint16 DataLen){

  if(ProtocolCheck(Data,DataLen) == 0){
    return;
  
  }
  switch(Data[3]){
  case 0x0:
		SensorValue[0] = Data[5];
	  SensorValue[1] = Data[6];
    break;
  case 0x01:
		//应该是光照值，这里不应该赋值
//    SensorValue[2] = Data[5];
    break;
  default:
    break;
  
  }
 


}
/***************************************************************************
* Function Name         : 
* Description           : 协议处理
* Parameter             :
* Return                :
* Note                  :
***************************************************************************/
uint16  CreateData(uint8 *Data){

  //检查是否有错误
  if(Data !=NULL){
    Data[0] = ProtocolData.Head;
    Data[1] = ProtocolData.NodeId>>8;  
    Data[2] = ProtocolData.NodeId;  
    Data[3] = ProtocolData.NodeType;  
    Data[4] = ProtocolData.DataLen; 
		Data[5] = ProtocolData.Data; 
    Data[6] = crc8(&Data[1],5);   
    Data[7] = ProtocolData.Tail;
    return 8;  
  }
  else{
    printf("CreateData Error!\r\n");
    return 0;
  }

}

//风扇节点进行组包
void FanControl(uint8_t Status){
	
		uint8_t DataBuff[10];
		uint8_t DataLen = 0;
		//改变data值
		ProtocolData.Data = Status;
		//进行组包
		DataLen = CreateData(DataBuff);
	  //通过串口1发送到zigbee协调器上
		HAL_UART_Transmit(&huart1,DataBuff,DataLen,100);

}
