#include <cmath>
#include "ev3api.h"
#include "InOutManager.h"
#include "Strategy.h"
#include "SelfPositionManager.h"
void LineTraceStrategy::Run()
{
	auto InOut = InOutManager::GetInstance();
	double pk = CurrentPID.PGain, pd = CurrentPID.DGain, power = CurrentPID.BasePower;
	int center = 65;

	int light = InOut->InputData.ReflectLight;

	int diff =  (int)light - center;
	int steering = pk * diff + (diff - PrevDiff) * pd;
	

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
	}

}