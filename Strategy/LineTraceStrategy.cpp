#include <cmath>
#include <vector>

#include "ev3api.h"
#include "InOutManager.h"
#include "Strategy.h"
#include "SelfPositionManager.h"
#include "SoundPlayTask.h"
#include "PIDDataManager.h"
#include "Point.h"

#define EDGE_LINE 120 // 黒線との境界線

void LineTraceStrategy::Run()
{
	auto InOut = InOutManager::GetInstance();
	PIDData pidData;
	SelfPositionManager* SpManager = SelfPositionManager::GetInstance();
	Point p = SpManager->RobotPoint;
	int currentAngle = SpManager->RobotAngle;
	int distance = SpManager->Distance;
	PIDDataManager* pidManager = PIDDataManager::GetInstance();
	bool leftEdge = true;
RETRY:

	switch(CurrentState) {
		// case RStraight :
		// 	// 最初の直線終了（Rコース）
		// 	if(p.Y > 2550) {
		// 		Position = RFirstCurve;
		// 		goto RETRY;
		// 	}
		// 	currentState = LineTraceStraight;
		// 	break;
		// case RFirstCurve:
		// 	// 最初のカーブ終了（Rコース）
		// 	if(p.Y < 2180){ 
		// 		Position = RSecondCurve;
		// 		goto RETRY;
		// 	}
		// 	currentState = LineTraceHighSpeedCurve;
		// 	break;
		// case RSecondCurve:
		// 	// 二番目のカーブ終了（Rコース）
		// 	if(p.Y < 1340){
		// 		Position = RThirdCurve;
		// 		goto RETRY;
		// 	}
		// 	currentState = LineTraceMiddleSpeedCurve;
		// 	break;
		// case RThirdCurve:
		// 	// 三番目のカーブ終了（Rコース）
		// 	if(p.Y > 1420) {
		// 		Position = RFourthCurve;
		// 		goto RETRY;
		// 	}
		// 	currentState = LineTraceSlowSpeedCurve;
		// 	break;
		// case RFourthCurve:
		// 	// 四番目のカーブ終了（Rコース）
		// 	if(p.X < 2900) {
		// 		Position = RLastStraight;
		// 		goto RETRY;
		// 	}
		// 	currentState = LineTraceMiddleSpeedCurve;
		// 	break;
		// case RLastStraight:
		// 	currentState = LineTraceStraight;
		// 	break;
		case L1 :
			// 最初の直線終了（Lコース）
			if(distance > 2230) {
				CurrentState = L2;
				goto RETRY;
			}
			leftEdge = true;
			pidData = pidManager->GetPIDData(LineTraceStraight);
			InOut->LineTraceAction(pidData, CenterValue, leftEdge);

			SpManager->ResetX(4790);
			break;
		case L2:
			// 最初のカーブ終了（Lコース）
			if(distance > 3700)
			{ 
				CurrentState = L25;
				goto RETRY;
			}
			leftEdge = true;
			pidData = pidManager->GetPIDData(LineTraceMiddleSpeedCurve);
			InOut->LineTraceAction(pidData, CenterValue, leftEdge);
			break;
		case L25 :
			if(distance > 9000) { 
				CurrentState = L3;
				goto RETRY;
			}		
			
			leftEdge = true;
			pidData = pidManager->GetPIDData(LineTraceHighSpeedCurve);			
			InOut->LineTraceAction(pidData, CenterValue, leftEdge);
			break;

		case L3:
			// 二番目のカーブ終了（Lコース）
			if(p.Y < 1350){
				CurrentState = L4;
				goto RETRY;
			}
			leftEdge = false;			
			pidData = pidManager->GetPIDData(LineTraceStraight);			
			InOut->LineTraceAction(pidData, CenterValue, leftEdge);
			break;
		case L4:
			// 三番目のカーブ終了（Lコース）
			if(p.X < 3810) {
				CurrentState = L5;
				goto RETRY;
			}
			leftEdge = false;
			pidData = pidManager->GetPIDData(LineTraceSlowSpeedCurve);
			InOut->LineTraceAction(pidData, CenterValue, leftEdge);
			break;
		case L5:
			// 四番目のカーブ終了（Lコース）
			if(p.X < 3230 || (p.X < 3500 && InOut->InputData.ReflectLight < EDGE_LINE)) {
				CurrentState = L6;
				goto RETRY;
			}
			InOut->Forward(pidManager->GetPIDData(LineTraceStraight).BasePower);
			break;
		case L6:
			// 五番目のカーブ終了（Lコース）
			if(p.X < 1170) {
				InOut->Stop();
				// CurrentState = L7;
				// goto RETRY;
				break;
			}
			leftEdge = true;
			pidData = pidManager->GetPIDData(LineTraceStraight);
			InOut->LineTraceAction(pidData, CenterValue, leftEdge);
			break;
		case L7:
			break;
		default:
			break;
	}	

}

