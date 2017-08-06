#include <cmath>
#include "ev3api.h"
#include "InOutManager.h"
#include "Strategy.h"
#include "SelfPositionManager.h"
#include "SoundPlayTask.h"

void DragRaceStrategy::Run()
{
	auto InOut = InOutManager::GetInstance();
	auto sp = SelfPositionManager::GetInstance();

	double pk = CurrentPID.PGain, pd = CurrentPID.DGain, power = CurrentPID.BasePower;
	int diff =  (int)sp->PositionY - 1000;
	int steering = pk * diff + (diff - PrevDiff) * pd;
s
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
	if(SelfPositionManager::GetInstance()->PositionX < 600) {
		InOut->Forward(0);
		InOut->WriteOutputMotor();
		PlaySound(LineTraceEnd);
		ext_tsk();

	}
}