#include "CmdAndSensor.h"

#include "gpio.h"
#include "protocol.h"

int SensorValue[3] = {25, 56, 11101};

//1、控制功能

//1.1、风扇控制
void vFanCmd(int Cmd){

	// 需要控制zigbee节点，后面进行开发
	FanControl((uint8_t)(Cmd));

}


//1.2、报警控制
void vAlarmCmd(int Cmd){

	if(Cmd){
		HAL_GPIO_WritePin(BUZ_GPIO_Port, BUZ_Pin, GPIO_PIN_SET);
	}else{
  HAL_GPIO_WritePin(BUZ_GPIO_Port, BUZ_Pin, GPIO_PIN_RESET);	
	}




}

//1.3、灯光控制
void vLedCmd(int Cmd){

	if(Cmd){
		HAL_GPIO_WritePin(GPIOF, D6_Pin|D7_Pin|D8_Pin|D9_Pin, GPIO_PIN_RESET);
	}else{
		HAL_GPIO_WritePin(GPIOF, D6_Pin|D7_Pin|D8_Pin|D9_Pin, GPIO_PIN_SET);
	}


}

//2、采集功能

//2.1、光照采集
int iGetLightValue(void){

	return SensorValue[0];
}

//2.2、温度采集
int iGetTempValue(void){

		return SensorValue[1];
}

//2.3、湿度采集
int iGetHumValue(void){

		return SensorValue[2];
}


