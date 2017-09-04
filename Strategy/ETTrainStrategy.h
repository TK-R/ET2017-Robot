#ifndef __ETTRAIN_STRATEGY__
#define __ETTRAIN_STRATEGY__

#include "HSLColor.h"
#include "ColorDecision.h"
#include "Strategy.h"


// ET相撲におけるステートの列挙
enum ETTrainStateEnum
{
	TurnToBack = 0,			// ゴール後、反対向きに旋回する
	BackToStation,			// 旋回後、駅の前まで移動する
	TurnToStation,			// 駅に尻尾を向ける形で旋回する
	TurnOffSwitch,			// スイッチを操作して、停止側にする
	TurnNextBack,			// もう一度背面を向く
	TurnFront,				// 進行方向まで旋回する
	LineTraceSlowToGrayArea, // 灰色領域までゆっくりライントレースする
	LineTraceToGrayArea,	// 灰色領域までライントレースする
	ForwardGrayArea,		// 灰色領域を直進する
	LineTraceToArena,		// 段差直前までライントレースする
	UpToArena				// 段差に上る
};

class ETTrainStrategy: public AbstractStrategy
{
private:

	// 現在のステート情報
	ETTrainStateEnum CurrentState = TurnToBack;
	int KillCount = 0;
	
public:
	void Initialize(){
		auto sp =  SelfPositionManager::GetInstance();
		sp->ResetX(1180);
		sp->ResetY(2190);
		sp->RobotAngle = 180;
		sp->Distance = 0;
	}

	void Run();
	using AbstractStrategy::AbstractStrategy;
};

#endif