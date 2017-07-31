#include "Strategy.h"
#include "StrategyManager.h"

void StrategyManager::Run()
{
	if(CurrentStrategy != NULL)
		CurrentStrategy->Run();	
}

void StrategyManager::SetStrategy(AbstractStrategy* nextStrategy)
{
	// TODO DELETE!!
	//delete CurrentStrategy;
	CurrentStrategy = nextStrategy;
}