/***************************************************************************
* File                  : protocol.h
* Function Name         : 
* Description           : Э�鴦��ͷ�ļ�
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
//zigbeeӦ��Э��ͷ��β����
#define HEAD  0x99
#define TAIL  0x01
//���ڲ�ͬ�Ľڵ����Ͳ�һ������ַ��һ������������ͨ������ж���
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
//����һ���������ͣ�����Э������Ͳ��
typedef struct{
  uint8   Head;			//ͷ
  uint16  NodeId;   //�ڵ�ID
  uint8   NodeType;	//�ڵ�����
  uint8   DataLen;	//���ݳ���
  uint8   Data;			//��������
  uint8   Crc;			//CRCУ����
  uint8   Tail;			//β
}ProtocolDataType;
extern ProtocolDataType  ProtocolData;


extern uint8 Temp_Hum_Data[2];

extern uint8 fan;

#define PROTOCOL_LEN sizeof(ProtocolDataType)
//CRCУ���㷨
uint8 crc8(uint8 *data, uint8 length);
//���Э��Ϸ���
uint8  ProtocolCheck(uint8 *Data,uint16 DataLen);
//Э�����
void ProtocolParse(uint8 *Data,uint16 DataLen);
//����Э���
uint16  CreateData(uint8 *Data);
// ���Ƚڵ����
void FanControl(uint8_t Status);


#endif
