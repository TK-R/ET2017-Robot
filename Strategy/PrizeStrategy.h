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
	TurnToPrize,		// 懸賞の向きに旋回
	ForwardPrize,		// 懸賞まで直進
	BackPrize,			// 懸賞置き場から後退
	TurnPrizePlaceLine,	// 懸賞置き場に向かう直線の方向に旋回
	ForwardPrizePlace,	// 懸賞置き場まで移動
	TurnPrizePlace,		// 懸賞置き場の方向に旋回
	LastForwardPrizePlace,	// 懸賞置き場の直進	
	BackLastLine,		// 最後の直線の方向まで後退
	TurnGurage,			// ガレージに向くまで旋回
	ForwardGurage,		// 直角カーブまで直進
	TurnCurve,			// 旋回動作
	LastGarageStraight		// 最終直進
};

class PrizeStrategy: public AbstractStrategy
{
private:

	// 現在のステート情報
	PrizeStrategyStateEnum CurrentState = TurnOnLine;
	int GarageGrayCount = 0;

public:
	void Run();
	using AbstractStrategy::AbstractStrategy;
};



#endif