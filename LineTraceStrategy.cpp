#include <cmath>
#include <vector>

#include "ev3api.h"
#include "InOutManager.h"
#include "Strategy.h"
#include "SelfPositionManager.h"
#include "SoundPlayTask.h"
void LineTraceStrategy::Run()
{
	auto InOut = InOutManager::GetInstance();
	uint IntegralCount = 5;

	double pk = CurrentPID.PGain, pi = CurrentPID.IGain, pd = CurrentPID.DGain, power = CurrentPID.BasePower;

	int light = InOut->InputData.ReflectLight;

	// 今回の偏差
	int diff =  (int)light - CenterValue;
	
	// 積分処理
	IntegralDiff.push_back(diff);
	if(IntegralDiff.size() > IntegralCount) IntegralDiff.erase(IntegralDiff.begin());

	// 積分偏差
	double intDiff = 0.0;
	for(int d : IntegralDiff)
		intDiff  += d;
 	intDiff /= IntegralDiff.size();	

	// 	ステアリング値
	int steering = pk * diff + pi * intDiff + (diff - PrevDiff) * pd;
	
	if(steering > 100) steering = 100;
	if(steering < -100) steering = -100;

	if (steering > 0)
	{
		InOut->OutputData.LeftMotorPower = (int8_t)(power + steering < 100 ? power + steering : 100);
		InOut->OutputData.RightMotorPower = (int8_t)(power - steering > 0 ? power - steering : 0);
	}
	else
	{
		InOut->OutputData.LeftMotorPower = (int8_t)(power + steering > 0 ? power + steering : 0);
		InOut->OutputData.RightMotorPower = (int8_t)(power - steering < 100 ? power - steering : 100);
	}

	PrevDiff = diff;
	if(SelfPositionManager::GetInstance()->PositionX < 1800)
	{
		InOut->Forward(0);
		InOut->WriteOutputMotor();
		PlaySound(LineTraceEnd);
		ext_tsk();

	}

}