/***************************************************************************
* File                  : protocol.h
* Function Name         : 
* Description           : 协议处理头文件
* Version               : v1.0
* Author                : zhengdao.liu
* Date                  : 2019/8/22
* Parameter             :
* Return                :
* Note                  :
***************************************************************************/
#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_
#define FAN_NODE

#include "main.h"

typedef uint8_t uint8;
typedef uint16_t uint16;
//zigbee应用协议头和尾定义
#define HEAD  0x99
#define TAIL  0x01
//由于不同的节点类型不一样，地址不一样，所以我们通过宏进行定义
#ifdef TEMP_NODE
#define NODE_ID     0x0000
#define NODE_TYPE   0x0
#define DATA_LEN    0x02
#define DATA_VALUE  0x2070
#endif

#ifdef FAN_NODE
#define NODE_ID     0x0001
#define NODE_TYPE   0x1
#define DATA_LEN    0x01
#define DATA_VALUE  0x01
#endif
//定义一种数据类型，用于协议组包和拆包
typedef struct{
  uint8   Head;			//头
  uint16  NodeId;   //节点ID
  uint8   NodeType;	//节点类型
  uint8   DataLen;	//数据长度
  uint8   Data;			//数据内容
  uint8   Crc;			//CRC校验码
  uint8   Tail;			//尾
}ProtocolDataType;
extern ProtocolDataType  ProtocolData;


extern uint8 Temp_Hum_Data[2];

extern uint8 fan;

#define PROTOCOL_LEN sizeof(ProtocolDataType)
//CRC校验算法
uint8 crc8(uint8 *data, uint8 length);
//检查协议合法性
uint8  ProtocolCheck(uint8 *Data,uint16 DataLen);
//协议解析
void ProtocolParse(uint8 *Data,uint16 DataLen);
//创建协议包
uint16  CreateData(uint8 *Data);
// 风扇节点控制
void FanControl(uint8_t Status);


#endif
