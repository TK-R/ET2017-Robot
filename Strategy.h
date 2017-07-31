#ifndef STRATEGY_H
#define STRATEGY_H

#include "ev3api.h"

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

class LineTraceStrategy: public AbstractStrategy
{	
private:
	int PrevDiff = 0;
public:
	void Run();
	using AbstractStrategy::AbstractStrategy;
};





#endif