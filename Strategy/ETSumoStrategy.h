#ifndef __ETSUMO_STRATEGY__
#define __ETSUMO_STRATEGY__

#include "HSLColor.h"
#include "ColorDecision.h"
#include "Strategy.h"

// 土俵の色情報
struct Arena {
	HSLColorKind LeftPlaceColor;
	HSLColorKind RightPlaceColor;
};

// ET相撲におけるステートの列挙
enum ETSumoStateEnum
{
	ForwardArena = 0,	// 土俵直進中
	TurnLeftPlace,		// 左ブロック方向旋回
	ForwardLeftPlace,	// 左ブロックまで直進
	DetectLeftBlock,	// 左ブロック色認識
	OSHIDASHILeft,		// 左ブロックを押し出し
	YORIKIRILeft,		// 左ブロックを寄り切り
	TurnRightPlace,		// 右ブロック方向旋回
	ForwardRightPlace,	// 右ブロックまで直進	
	DetectRightBlock,	// 右ブロック色認識
	OSHIDASHIRight,		// 右ブロックを押し出し
	YORIKIRIRight,		// 右ブロックを寄り切り
	TurnCenter,			// 中央方向へ旋回
	ForwardCenter,		// 中央方向へ前進
	TurnForward,		// 前方方向へ旋回
};

class ETSumoStrategy: public AbstractStrategy
{
private:
	// 現在のステート情報
	ETSumoStateEnum CurrentState;
	double CurrentDistance = 0;
	Arena ArenaArray[4] = {
		// Left, Right
		{ HSLBlue, HSLRed },
		{ HSLBlue, HSLRed },
		{ HSLYellow, HSLGreen },
		{ HSLYellow, HSLGreen }
	};

public:
	void Run();
	using AbstractStrategy::AbstractStrategy;
};



#endif