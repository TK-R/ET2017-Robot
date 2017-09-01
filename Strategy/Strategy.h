#ifndef STRATEGY_H
#define STRATEGY_H

#include "ev3api.h"
#include <vector>

class StrategyManager;

// 各種戦略クラスが継承する仮想戦略クラス
class AbstractStrategy
{
protected:
	StrategyManager* Manager;

public:
	AbstractStrategy(StrategyManager* manager)
	{
		Manager = manager;
	}

	virtual void Run(){}
	virtual ~AbstractStrategy(){}
};

// 現在どこを走行中か
enum CurrentStateEnum {
	// Rコース
	R_A = 0,
	R_B,
	R_C,
	R_D,
	R_E,
	R_F,
	R_GRAY, // 灰色検出まで
	// Lコース
	L_A,
	L_B,
	L_C,
	L_D,
	L_E,
	L_F,
	L_G,
	L_H,
	L_GRAY,	// 灰色検出まで
	L_I
};

class LineTraceStrategy: public AbstractStrategy
{	
private:
	// 微分制御用
	int PrevDiff = 0;

	// 現在走行中のコース箇所
	CurrentStateEnum CurrentState = R_A;
//	CurrentStateEnum CurrentState = L_A;

public:
	int CenterValue = 0;

	void Run();
	using AbstractStrategy::AbstractStrategy;
};





#endif