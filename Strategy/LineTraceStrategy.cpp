#include <cmath>
#include <vector>

#include "ev3api.h"
#include "InOutManager.h"
#include "Strategy.h"
#include "SelfPositionManager.h"
#include "SoundPlayTask.h"
#include "PIDDataManager.h"
#include "Point.h"

int LineTraceStrategy::GetCurrentState()
{
	Point p = SelfPositionManager::GetInstance()->RobotPoint;
	StateNo currentState = LineTraceStraight;
RETRY:

	switch(Position) {
		case RStraight :
			// 最初の直線終了（Rコース）
			if(p.Y > 2550) {
				Position = RFirstCurve;
				goto RETRY;
			}
			currentState = LineTraceStraight;
			break;
		case RFirstCurve:
			// 最初のカーブ終了（Rコース）
			if(p.Y < 2180){ 
				Position = RSecondCurve;
				goto RETRY;
			}
			currentState = LineTraceHighSpeedCurve;
			break;
		case RSecondCurve:
			// 二番目のカーブ終了（Rコース）
			if(p.Y < 1340){
				Position = RThirdCurve;
				goto RETRY;
			}
			currentState = LineTraceMiddleSpeedCurve;
			break;
		case RThirdCurve:
			// 三番目のカーブ終了（Rコース）
			if(p.Y > 1420) {
				Position = RFourthCurve;
				goto RETRY;
			}
			currentState = LineTraceSlowSpeedCurve;
			break;
		case RFourthCurve:
			// 四番目のカーブ終了（Rコース）
			if(p.X < 2900) {
				Position = RLastStraight;
				goto RETRY;
			}
			currentState = LineTraceMiddleSpeedCurve;
			break;
		case RLastStraight:
			currentState = LineTraceStraight;
			break;
		case LStraight :
			// 最初の直線終了（Lコース）
			if(p.Y > 2550) {
				Position = LFirstCurve;
				goto RETRY;
			}
			currentState = LineTraceStraight;
			break;
		case LFirstCurve:
			// 最初のカーブ終了（Lコース）
			if(p.Y < 2200){ 
				Position = LSecondCurve;
				goto RETRY;
			}
			currentState = LineTraceMiddleSpeedCurve;
			break;
		case LSecondCurve:
			// 二番目のカーブ終了（Lコース）
			if(p.X < 4000){
				Position = LThirdCurve;
				goto RETRY;
			}
			currentState = LineTraceHighSpeedCurve;
			break;
		case LThirdCurve:
			// 三番目のカーブ終了（Lコース）
			if(p.X < 3350) {
				Position = LFourthCurve;
				goto RETRY;
			}
			currentState = LineTraceHighSpeedCurve;
			break;
		case LFourthCurve:
			// 四番目のカーブ終了（Lコース）
			if(p.Y > 1150) {
				Position = LFifthCurve;
				goto RETRY;
			}
			currentState = LineTraceSlowSpeedCurve;
			break;
		case LFifthCurve:
			// 五番目のカーブ終了（Lコース）
			if(p.X < 2800) {
				Position = LLastStraight;
				goto RETRY;
			}
			currentState = LineTraceMiddleSpeedCurve;
			break;
		case LLastStraight:
			currentState = LineTraceStraight;
			break;
		default:
			currentState = LineTraceSlowSpeedCurve;
			break;
	}	

	return (int)currentState;
}

void LineTraceStrategy::Run()
{
	auto InOut = InOutManager::GetInstance();
	uint IntegralCount = 5;
	
	int state = GetCurrentState();
	auto pidData = PIDDataManager::GetInstance()->GetPIDData(state); 

	double pk = pidData.PGain, pi = pidData.IGain, pd = pidData.DGain, power = pidData.BasePower;
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

	// ステアリング値
	int steering = pk * diff + pi * intDiff + (diff - PrevDiff) * pd;

	// 微分偏差計算用の前回の偏差
	PrevDiff = diff;

	// 操舵角からモータ出力値を決定
	InOut->Forward(power, steering);

	/*
	if(SelfPositionManager::GetInstance()->RobotPoint.X < 1800)
	{
		InOut->Forward(0);
		InOut->WriteOutputMotor();
		PlaySound(LineTraceEnd);
		ext_tsk();

	}
*/
}

