#ifndef INPUT_DATA_MANAGER
#define INPUT_DATA_MANAGER
#include "ev3api.h"
#include <ColorSensor.h>
#include <Port.h>
#include <Motor.h>
#include <TouchSensor.h>
#include <SonarSensor.h>
#include <GyroSensor.h>


#include "SerialReceiveTask.h"

using ev3api::Motor;
using ev3api::TouchSensor;
using ev3api::SonarSensor;
using ev3api::GyroSensor;
using ev3api::ColorSensor;

class InOutManager
{
private:

	Motor* LeftMotor;
	Motor* RightMotor;
	Motor* ArmMotor;
	Motor* TailMotor;
	
	TouchSensor* Touch;
	SonarSensor* Sonar;
	ColorSensor* Color;
	GyroSensor *Gyro;

	InOutManager();


public:
	InputSignalData InputData;
	OutputSignalData OutputData;

	static InOutManager* GetInstance();

	// 前進するように左右モータの値を更新する
	void Forward(int power);
	// 後退するように左右モータの値を更新する
	void Back(int power);
	// 目標角度になるように旋回する
	void Turn(int currentAngle, int targetAngle, int power);
	// 時計回りに回転するように左右モータの値を更新する
	void TurnCW(int power);
	// 反時計回りに回転するように左右モータ値を更新する
	void TurnCCW(int power);
	void ReadInputSensor();
	void WriteOutputMotor();
};


#endif 