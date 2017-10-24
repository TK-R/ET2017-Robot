#ifndef STRATEGY_H
#define STRATEGY_H

#include "ev3api.h"

class StrategyManager;

// 各種戦略クラスが継承する仮想戦略クラス
class AbstractStrategy
{
protected:
public:	
	StrategyManager* Manager;

	// 出力処理を実行するか
	bool OutputMotorValue = true;

	AbstractStrategy(StrategyManager* manager)
	{
		Manager = manager;
	}

	virtual void Run(){}
	virtual ~AbstractStrategy(){}
};

#endif