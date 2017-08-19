#ifndef INPUT_DATA_MANAGER
#define INPUT_DATA_MANAGER
#include "ev3api.h"
#include <ColorSensor.h>
#include <Port.h>
#include <Motor.h>
#include <TouchSensor.h>
#include <SonarSensor.h>
#include <GyroSensor.h>

#include "HSLColor.h"
#include "ColorDecision.h"
#include "SerialReceiveTask.h"

using ev3api::Motor;
using ev3api::TouchSensor;
using ev3api::SonarSensor;
using ev3api::GyroSensor;
using ev3api::ColorSensor;

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

	// アーム上昇中ならTrue
	bool ArmUp = false;

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
	void LineTraceAction(bool LeftEdge);
	
	// アームを上昇させる、ただし、処理はすぐに戻る
	void UpARMMotor();

	// アームを下降させる（元の位置に戻す）
	void DownARMMotor();

	// センサ値を再読み込みする
	void ReadInputSensor();
	// モータ出力値を書き込む
	void WriteOutputMotor();
};


#endif 