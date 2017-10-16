#ifndef __LINE_TRACE__
#define __LINE_TRACE__

#include "Strategy.h"
#include <vector>

class StrategyManager;

// ポートAに接続されたモータを回転させて、出力値をモニタに表示する戦略クラス（モータ評価用）
class MotorTestStrategy: public AbstractStrategy
{	
private:
	int Power = 0;
	int RateCount = 0; // モニタ更新周期管理変数
	bool SwitchON = false;

	// 1秒分の回転角度(20個)
	std::vector<int> AngleVector = {0};
	
public:

	void Run();
	using AbstractStrategy::AbstractStrategy;
};





#endif