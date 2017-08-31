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
//	Point p = SpManager->RobotPoint;
//	int currentAngle = SpManager->RobotAngle;
	int distance = SpManager->Distance;
	PIDDataManager* pidManager = PIDDataManager::GetInstance();
	bool leftEdge = true;
RETRY:

	switch(CurrentState) {
	// Rコース
	case R_A :
		if(distance > 2170 + 70) {	//スタートラインの手間70mmからスタート
			CurrentState = R_B;
			goto RETRY;
		}
		leftEdge = true;
		pidData = pidManager->GetPIDData(LineTraceStraight);
		InOut->LineTraceAction(pidData, CenterValue, leftEdge);
		break;
	case R_B:
		if(distance > 5590)	{ 
			CurrentState = R_C;
			goto RETRY;
		}
		leftEdge = true;
		pidData = pidManager->GetPIDData(pR_B);
		InOut->LineTraceAction(pidData, CenterValue, leftEdge);
		break;
	case R_C :
		if(distance > 6520) { 
			CurrentState = R_D;
			goto RETRY;		
		}		
		leftEdge = true;
		pidData = pidManager->GetPIDData(pR_C);	
		InOut->LineTraceAction(pidData, CenterValue, leftEdge);
		break;
	case R_D:
		if(distance > 7410){
			CurrentState = R_E;
			goto RETRY;
		}
		leftEdge = true;			
		pidData = pidManager->GetPIDData(pR_D);			
		InOut->LineTraceAction(pidData, CenterValue, leftEdge);
		break;
	case R_E:
		if(distance > 8900) {
			CurrentState = R_F;
			goto RETRY;
		}
		leftEdge = true;
		pidData = pidManager->GetPIDData(pR_E);
		InOut->LineTraceAction(pidData, CenterValue, leftEdge);
		break;
	case R_F:
		if(distance > 10390 + 150) {	//ゴール後150mm余分に進む
			InOut->Stop();
			// CurrentState = R_F;
			// goto RETRY;
			break;
		}
		leftEdge = true;
		pidData = pidManager->GetPIDData(LineTraceStraight);
		InOut->LineTraceAction(pidData, CenterValue, leftEdge);
		break;
// Lコース
	case L_A :
		if(distance > 2160 + 70) {	//スタートラインの手間70mmからスタート
			CurrentState = L_B;
			goto RETRY;
		}
		leftEdge = true;
		pidData = pidManager->GetPIDData(LineTraceStraight);
		InOut->LineTraceAction(pidData, CenterValue, leftEdge);
		break;
	case L_B:
		if(distance > 3890)	{ 
			CurrentState = L_C;
			goto RETRY;
		}
		leftEdge = true;
		pidData = pidManager->GetPIDData(pL_B);
		InOut->LineTraceAction(pidData, CenterValue, leftEdge);
		break;
	case L_C :
		if(distance > 5270) { 
			CurrentState = L_D;
			goto RETRY;		
		}		
		leftEdge = true;
		pidData = pidManager->GetPIDData(pL_C);	
		InOut->LineTraceAction(pidData, CenterValue, leftEdge);
		break;
	case L_D:
		if(distance > 6120){
			CurrentState = L_E;
			goto RETRY;
		}
		leftEdge = true;			
		pidData = pidManager->GetPIDData(pL_D);			
		InOut->LineTraceAction(pidData, CenterValue, leftEdge);
		break;
	case L_E:
		if(distance > 7120) {
			CurrentState = L_F;
			goto RETRY;
		}
		leftEdge = true;
		pidData = pidManager->GetPIDData(pL_E);
		InOut->LineTraceAction(pidData, CenterValue, leftEdge);
		break;
	case L_F:
		if(distance > 7620) {
			CurrentState = L_G;
			goto RETRY;
		}
		leftEdge = true;
		pidData = pidManager->GetPIDData(pL_F);
		InOut->LineTraceAction(pidData, CenterValue, leftEdge);
		break;
	case L_G:
		if(distance > 8620) {
			CurrentState = L_H;
			goto RETRY;
		}
		leftEdge = true;
		pidData = pidManager->GetPIDData(pL_G);
		InOut->LineTraceAction(pidData, CenterValue, leftEdge);
		break;
	case L_H:
		if(distance > 10010 + 150) {	//ゴール後150mm余分に進む
			InOut->Stop();
			// CurrentState = L_H;
			// goto RETRY;
			break;
		}
		leftEdge = true;
		pidData = pidManager->GetPIDData(LineTraceStraight);
		InOut->LineTraceAction(pidData, CenterValue, leftEdge);
		break;
	default:
		break;
	}	

}

