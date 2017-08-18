#include "InOutManager.h"
#include "ETSumoStrategy.h"

// 板の端からの直線距離
#define ArenaForwardDistance 200

void ETSumoStrategy::Run()
{
	InOutManager* IOManager = InOutManager::GetInstance();
	switch(CurrentState) {
		case ForwardArena:	
		// 黒線認識までライントレース

		break;
		default :
		break;
	}
}