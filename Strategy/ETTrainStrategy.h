#ifndef __ETTRAIN_STRATEGY__
#define __ETTRAIN_STRATEGY__

#include "HSLColor.h"
#include "ColorDecision.h"
#include "Strategy.h"


// ET相撲におけるステートの列挙
enum ETTrainStateEnum
{
	BackToStation = 0,				// ゴール後、駅の前まで移動する
	TurnToStation,				// 駅に尻尾を向ける形で旋回する
	TurnOffSwitch,			// スイッチを操作して、停止側にする
	TurnFront,				// 進行方向まで旋回する
	ForwardGrayArea,		// 灰色領域を直進する
	LineTraceToArena,			// 段差直前までライントレースする
	UpToArena				// 段差に上る
};

class ETTrainStrategy: public AbstractStrategy
{
private:

	// 現在のステート情報
	ETTrainStateEnum CurrentState = BackToStation;

	
public:
	void Run();
	using AbstractStrategy::AbstractStrategy;
};

#endif