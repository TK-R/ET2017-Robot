#include "Strategy.h"
#include "StrategyManager.h"

void StrategyManager::Run()
{
	if(CurrentStrategy != NULL)
		CurrentStrategy->Run();	
}

void StrategyManager::SetStrategy(AbstractStrategy* nextStrategy)
{
	
	if(CurrentStrategy != NULL)
		delete CurrentStrategy;
	
	CurrentStrategy = nextStrategy;
}