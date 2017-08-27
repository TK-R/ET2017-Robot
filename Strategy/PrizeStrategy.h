#ifndef __PRIZE_STRATEGY__
#define __PRIZE_STRATEGY__

#include "HSLColor.h"
#include "ColorDecision.h"
#include "Strategy.h"


// ET相撲におけるステートの列挙
enum PrizeStrategyStateEnum
{
	TurnOnLine = 0,		// 懸賞前の黒線の向きに旋回
	ForwardLineTrace,	// 懸賞前に移動
	TurnPrizeForward,	// 懸賞の向きに旋回
	ForwardPrize,		// 懸賞まで直進
	TurnPrizePlace,		// 懸賞置き場の方向に旋回
	ForwardPrizePlace,	// 懸賞置き場まで移動
	TurnLastLine,		// 最後の直線の方向に旋回
	ForwardLastLine,	// 最後の直線に向けて移動
	TurnOnLastLine,		// 最後の直線の上で旋回 
};

class PrizeStrategy: public AbstractStrategy
{
private:

	// 現在のステート情報
	PrizeStrategyStateEnum CurrentState = TurnOnLine;
	
public:
	void Run();
	using AbstractStrategy::AbstractStrategy;
};



#endif