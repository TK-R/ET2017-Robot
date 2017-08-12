#ifndef _STRATEGY_MANAGER_
#define _STRATEGY_MANAGER_

#include "Strategy.h"

class StrategyManager
{
public:
	AbstractStrategy* CurrentStrategy;
	void Run();
	void SetStrategy(AbstractStrategy* nextStrategy);
};


#endif