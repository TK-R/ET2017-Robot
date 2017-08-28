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
	RStraight = 0,
	RFirstCurve,
	RSecondCurve,
	RThirdCurve,
	RFourthCurve,
	RLastStraight,
	L1,
	L2,
	L3,
	L4,
	L5,
	L6,
	L7
};

class LineTraceStrategy: public AbstractStrategy
{	
private:
	// 微分制御用
	int PrevDiff = 0;


	// 現在走行中のコース箇所
	// CurrentCoursePosition Position = RStraight;
	CurrentStateEnum CurrentState = L1;

public:
	int CenterValue = 0;

	void Run();
	using AbstractStrategy::AbstractStrategy;
};





#endif