#include <cmath>
#include <vector>

#include "ev3api.h"
#include "InOutManager.h"
#include "Strategy.h"
#include "SelfPositionManager.h"
#include "SoundPlayTask.h"
#include "PIDDataManager.h"
#include "Point.h"

void LineTraceStrategy::Run()
{
	auto InOut = InOutManager::GetInstance();
	PIDData pidData;
	Point p = SelfPositionManager::GetInstance()->RobotPoint;
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
			if(p.Y > 2550) {
				CurrentState = L2;
				goto RETRY;
			}
			leftEdge = true;
			pidData = pidManager->GetPIDData(LineTraceStraight);
			break;
		case L2:
			// 最初のカーブ終了（Lコース）
			if(p.Y < 2200){ 
				CurrentState = L3;
				goto RETRY;
			}
			leftEdge = true;
			pidData = pidManager->GetPIDData(LineTraceMiddleSpeedCurve);
			break;
		case L3:
			// 二番目のカーブ終了（Lコース）
			if(p.X < 4000){
				CurrentState = L4;
				goto RETRY;
			}
			break;
		case L4:
			// 三番目のカーブ終了（Lコース）
			if(p.X < 3350) {
				CurrentState = L5;
				goto RETRY;
			}
			break;
		case L5:
			// 四番目のカーブ終了（Lコース）
			if(p.Y > 1150) {
				CurrentState = L6;
				goto RETRY;
			}
			break;
		case L6:
			// 五番目のカーブ終了（Lコース）
			if(p.X < 2800) {
				CurrentState = L7;
				goto RETRY;
			}
			break;
		case L7:
			break;
		default:
			break;
	}	

	InOut->LineTraceAction(pidData, CenterValue, true);
}

