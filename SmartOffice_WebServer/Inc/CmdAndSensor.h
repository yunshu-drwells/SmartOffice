#ifndef _CMD_AND_SENSOR_H
#define _CMD_AND_SENSOR_H

//1、控制功能

//1.1、风扇控制
void vFanCmd(int Cmd);


//1.2、报警控制
void vAlarmCmd(int Cmd);

//1.3、灯光控制
void vLedCmd(int Cmd);

//2、采集功能

//2.1、光照采集
int iGetLightValue(void);

//2.2、温度采集
int iGetTempValue(void);

//2.3、湿度采集
int iGetHumValue(void);
#endif


