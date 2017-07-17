#ifndef STRATEGY_H
#define STRATEGY_H

#include "ev3api.h"

// 各種戦略クラスが継承する仮想戦略クラス
class AbstractStrategy
{
public:
	virtual void Run(){}
};

class LineTraceStrategy: public AbstractStrategy
{	
private:
	int PrevDiff = 0;
public:
	void Run();
};


class StrategyManager
{
public:
	AbstractStrategy* CurrentStrategy;

	void Run();
	void SetStrategy(AbstractStrategy* nextStrategy);
};



#endif