#ifndef INPUT_DATA_MANAGER
#define INPUT_DATA_MANAGER
#include "ev3api.h"
#include <ColorSensor.h>
#include <Port.h>
#include <Motor.h>
#include <TouchSensor.h>
#include <SonarSensor.h>
#include <GyroSensor.h>

#include "SerialData.h"
#include "HSLColor.h"
#include "ColorDecision.h"
#include "SerialReceiveTask.h"

using ev3api::Motor;
using ev3api::TouchSensor;
using ev3api::SonarSensor;
using ev3api::GyroSensor;
using ev3api::ColorSensor;

enum ArmStateEnum
{
	ArmStateUp,
	ArmStateDown,
	ArmStateBottom
};

enum TailStateEnum
{
	TailStateUp,
	TailStateDown,
	TailStateBottom
};

class InOutManager
{
private:
	// モータAPIクラスへの参照
	Motor* LeftMotor;
	Motor* RightMotor;
	Motor* ArmMotor;
	Motor* TailMotor;
	
	// センサAPIクラスへの参照
	TouchSensor* Touch;
	SonarSensor* Sonar;
	ColorSensor* Color;
	GyroSensor *Gyro;

	// ライントレース実施時に使用する値
	int PrevDiff = 0;

	InOutManager();

	// アーム状態
	ArmStateEnum ArmState = ArmStateDown;

	// 尻尾状態
	TailStateEnum TailState = TailStateUp;

public:
	InputSignalData InputData;
	OutputSignalData OutputData;
	HSLColorData HSLData;

	// HSL色情報
	HSLColor HSLValue;
	// HSL色種別
	HSLColorKind HSLKind;
	// 予測する対象の種別
	DetectTargetType HSLTargetType = FieldColor;

	// 制御にかかった時間
	uint16_t SleepTime = 0;
	
	static InOutManager* GetInstance();

	// 前進するように左右モータの値を更新する
	void Forward(int power);
	// 出力値と操舵角から左右モータの値を更新する（0：直進 100：右方向へ -100：左方向へ）
	void Forward(int power, int steering);
	// 後退するように左右モータの値を更新する
	void Back(int power);

	// 移動用モータを停止する
	void Stop();
	// 目標角度になるように旋回する
	void Turn(int currentAngle, int targetAngle, int power);
	// 時計回りに回転するように左右モータの値を更新する
	void TurnCW(int power);
	// 反時計回りに回転するように左右モータ値を更新する
	void TurnCCW(int power);
	
	// ライントレースを実施するように左右モータ値を更新する
	void LineTraceAction(PIDData data, int center, bool LeftEdge);

	// 後退しながらライントレースを実施するように左右モータ値を更新する
	void BackLineTraceAction(PIDData data, int center, bool LeftEdge);


	// アームを上昇させる
	void UpARMMotor();
	// アームを下降させる
	void DownARMMotor();
	// アームを懸賞箇所まで下げる
	void ARMMotorAtBottom();

	// 尻尾を上げる
	void UpTailMotor();
	// 尻尾を下げる
	void DownTailMotor();
	// 尻尾を一番下まで下げる 
	void BottomTailMotor();

	// センサ値を再読み込みする
	void ReadInputSensor();
	// モータ出力値を書き込む
	void WriteOutputMotor();
};

#endif 