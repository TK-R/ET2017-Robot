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
	FirstOnArena = 0,		// 初回土俵入り
	FirstOverLine,			// 初回黒線到達
	FirstTurnLeftPlace,		// 初回左ブロック方向旋回中
	ForwardLeftPlace,		// 左ブロックまで直進
	DetectLeftBlock,		// 左ブロック色認識
	OSHIDASHILeft,			// 左ブロックを押し出し
	OSHIDASHILeftBack,		// 左ブロック押し出し後の後退処理
	YORIKIRILeft,			// 左ブロックを寄り切り
	TurnRightPlace,			// 右ブロック方向旋回
	ForwardRightPlace,		// 右ブロックまで直進	
	DetectRightBlock,		// 右ブロック色認識
	OSHIDASHIRight,			// 右ブロックを押し出し
	OSHIDASHIRightBack,		// 右ブロック押し出し後の後退処理
	YORIKIRIRight,			// 右ブロックを寄り切り
	TurnCenter,				// 中央方向へ旋回
	ForwardField,			// フィールド
	ForwardCenterFromRight,	// 中央方向へ前進
	TurnForward,			// 前方方向へ旋回
	ForwardRail				// 線路上を通貨
};

class ETSumoStrategy: public AbstractStrategy
{
private:
	// 色認識回数
	int ColorDetectCount = 0;
	HSLColorKind PrevColor = HSLBlack;

	// 現在のステート情報
	ETSumoStateEnum CurrentState = FirstOnArena;
	double CurrentDistance = 0;

	// 現在の土俵枚数
	int CurrentArena = 0;
	
	// ET相撲における土俵の定義 !! 当日変更箇所
	Arena ArenaArray[4] = {
		// Left, Right
		{ HSLBlue, HSLRed },
		{ HSLYellow, HSLGreen },
		{ HSLYellow, HSLGreen },
		{ HSLBlue, HSLRed }	
	};

	
public:
	void Run();
	using AbstractStrategy::AbstractStrategy;
};



#endif