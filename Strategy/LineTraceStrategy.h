#ifndef __LINE_TRACE__
#define __LINE_TRACE__

#include "Strategy.h"
#include <vector>

class StrategyManager;


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
    
    // 灰色検出用
	int GrayCount = 0;
	

public:
	// 現在走行中のコース箇所
	CurrentStateEnum CurrentState = L_A;
	
	int CenterValue = 0;

	void Run();
	using AbstractStrategy::AbstractStrategy;

	
	~LineTraceStrategy()
	{
		auto InOut = InOutManager::GetInstance();	
		InOut->ResetMotor();
	}
};





#endif