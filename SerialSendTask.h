#ifndef __SERIAL_SEND_TASK_H__
#define __SERIAL_SEND_TASK_H__

#pragma pack(push, 1) // アライメント1に設定

#define HEADER_HEAD_VALUE 0xFF
#define COMMAND_OUTPUT_SIGNAL_DATA 0x01
#define COMMAND_INPUT_SIGNAL_DATA 0x02

#define HEADER_BYTE_SIZE 4
struct Header {
	uint8_t Head;
	uint16_t Size;
	uint8_t Command;
};

#define OUTPUT_SIGNAL_DATA_BYTE_SIZE 4
struct OutputSignalData {
	int8_t FrontMotorPower;		//前モータ-パワー[%]
	int8_t RightMotorPower;		//右モータ-パワー[%]
	int8_t LeftMotorPower;		//左モータ-パワー[%]
	int8_t BackMotorPower;		//後モータ-パワー[%]
};

#define INPUT_SIGNAL_DATA_BYTE_SIZE 30
struct InputSignalData {
	uint16_t FrontMotorAngle;		//前モータ-角位置[deg]
	uint16_t RightMotorAngle;		//右モータ-角位置[deg]
	uint16_t LeftMotorAngle;		//左モータ-角位置[deg]
	uint16_t BackMotorAngle;		//後モータ-角位置[deg]
	int8_t TouchSensor;		//タッチセンサ
	uint16_t SonarDistance;	//超音波センサ（距離）[cm]
	uint8_t ColorRed;		//カラーセンサ（赤）
	uint8_t ColorGreen;		//カラーセンサ（緑）
	uint8_t ColorBlue;		//カラーセンサ（青）
	uint8_t AmbientLight;	//環境光
	uint8_t ReflectLight;	//反射光
	uint16_t AccelX;		//加速度[x]……ジャイロセンサ角速度を代入
	uint16_t AccelY;		//加速度[y]……ジャイロセンサ角位置を代入
	uint16_t AccelZ;		//加速度[z]……0（固定）
	float Temperature;		//温度……0（固定）
	uint16_t BatteryCurrent;	//バッテリ電流[mA]
	uint16_t BatteryVoltage;	//バッテリ電圧[mV]
};

#pragma pack(pop)

#endif