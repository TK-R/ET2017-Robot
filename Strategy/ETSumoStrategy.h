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
	ForwardArena = 0,		// 土俵直進中
	ForwardOverLeftLine,	// 土俵左黒線横断中
	TurnLeftPlace,			// 左ブロック方向旋回
	ForwardLeftPlace,		// 左ブロックまで直進
	DetectLeftBlock,		// 左ブロック色認識
	OSHIDASHILeft,			// 左ブロックを押し出し
	YORIKIRILeft,			// 左ブロックを寄り切り
	TurnRightPlace,			// 右ブロック方向旋回
	ForwardCenterFromLeft,	// 左ブロックから中央ラインに向かって前進
	ForwardOverCenterLineToRight,	// 右ブロックへ向けて中央ラインを横断
	ForwardRightPlace,		// 右ブロックまで直進	
	DetectRightBlock,		// 右ブロック色認識
	OSHIDASHIRight,			// 右ブロックを押し出し
	YORIKIRIRight,			// 右ブロックを寄り切り
	TurnCenter,				// 中央方向へ旋回
	ForwardCenterFromRight,	// 中央方向へ前進
	ForwardOverCenterLineFromRight, // 右ブロックから土俵中央黒線を横断中
	TurnForward,			// 前方方向へ旋回
};

class ETSumoStrategy: public AbstractStrategy
{
private:
	// 色認識回数
	int ColorDetectCount = 0;
	HSLColorKind PrevColor = HSLBlack;

	// 現在のステート情報
	ETSumoStateEnum CurrentState = ForwardArena;
	double CurrentDistance = 0;

	// 現在の土俵枚数
	int CurrentArena = 0;

	// ET相撲における土俵の定義 !! 当日変更箇所
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