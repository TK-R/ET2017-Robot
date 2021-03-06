#ifndef __SERIAL_DATA__
#define  __SERIAL_DATA__

#include "ev3api.h"
#include <vector>

#pragma pack(push, 1) // アライメント1に設定

#define HEADER_HEAD_VALUE 0xFF
#define COMMAND_INPUT_SIGNAL_DATA 0x01
#define COMMAND_OUTPUT_SIGNAL_DATA 0x02
#define COMMAND_PID_DATA 0x10
#define COMMAND_BLOCK_MOVE_RULE_DATA 0x20
#define COMMAND_SELF_POSITION_DATA 0x30
#define COMMAND_HSL_COLOR_DATA 0x31
#define COMMAND_BLUETOOTH_CONTROL 0x40

#define HEADER_BYTE_SIZE 4
struct Header {
	uint8_t Head;
	uint16_t Size;
	uint8_t Command;
};

#define OUTPUT_SIGNAL_DATA_BYTE_SIZE 4
struct OutputSignalData {
	int8_t LeftMotorPower;		//左モータ-パワー[%]
	int8_t RightMotorPower;		//右モータ-パワー[%]
	int8_t ArmMotorPower;		//アームモータ-パワー[%]
	int8_t TailMotorPower;		//後モータ-パワー[%]
};

#define INPUT_SIGNAL_DATA_BYTE_SIZE 29
struct InputSignalData {
	int8_t LeftMotorAngle;		//前モータ-角位置[deg]
	int8_t RightMotorAngle;		//右モータ-角位置[deg]
	int8_t TailMotorAngle;		//左モータ-角位置[deg]
	int8_t ArmMotorAngle;		//後モータ-角位置[deg]
	int8_t TouchSensor;		//タッチセンサ
	uint16_t SonarDistance;	//超音波センサ（距離）[cm]
	uint16_t ColorRed;		//カラーセンサ（赤）
	uint16_t ColorGreen;		//カラーセンサ（緑）
	uint16_t ColorBlue;		//カラーセンサ（青）
	uint8_t AmbientLight;	//環境光
	uint8_t ReflectLight;	//反射光
	int16_t Angle;			//角位置……ジャイロセンサ角速度を代入
	int16_t AnglarSpeed;	//角速度…ジャイロセンサ角位置を代入
	int16_t SleepTime;		//周期処理にかかった時間
	float reserved2;		//予備2……0（固定）
	uint16_t BatteryCurrent;	//バッテリ電流[mA]
	uint16_t BatteryVoltage;	//バッテリ電圧[mV]
};

#define PID_DATA_BYTE_SIZE 21
struct PIDData {
	float BasePower;	// 計算の基準となる出力値
	float PGain;		// 比例ゲイン
	float IGain;		// 積分ゲイン
	float DGain; 		// 微分ゲイン
	int State; 			// 変更を適用するステートNo
	int8_t Steering;	// ステアリング値	
};

#define SELF_POSITION_BYTE_SIZE 14
struct SelfPositionData {
	uint PositionX; // X座標(mm/左上原点)
	uint PositionY; // Y座標(mm/左上原点)
	ushort Angle; 	// 角度(右向きを0度)
	uint Distance; 	// 総走行距離(mm)
};

#define HSL_COLOR_BYTE_SIZE 13
struct HSLColorData {
	float Hue;			// 色相
	float Saturation; 	// 彩度
	float Luminosity; 	// 輝度
	uint8_t HSLKind;	// 色種別
};

#define BLUETOOTH_CONTROL_BYTE_SIZE 1
struct BluetoothControlData {
	uint8_t SendON;		// 送信フラグ
};

#pragma pack(pop)

struct BlockMoveCommandData{
	uint8_t BlockColor;					// 運搬対象のブロック色
	uint8_t SourceBlockPosition;		// 運搬元ブロック置き場の番号
	uint8_t DestinationBlockPosition;	// 運搬先ブロック置き場の番号
	uint8_t ApproachWaypointCount;		// ブロック確保時の経路数
	uint8_t ApproachWayPoint[32];		// ブロック確保時の経路
	uint8_t BlockMoveWaypointCount;		// ブロック運搬時の経路数
	uint8_t BlockMoveWayPoint[32];		// ブロック運搬時の経路
};

struct BlockMoveRuleData{
	uint8_t CommandSize;
	std::vector<BlockMoveCommandData> Command;
};

#endif