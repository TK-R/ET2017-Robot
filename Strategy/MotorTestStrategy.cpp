#include <cmath>
#include <vector>

#include "ev3api.h"
#include "InOutManager.h"
#include "Strategy.h"
#include "MotorTestStrategy.h"
#include "SelfPositionManager.h"
#include "SoundPlayTask.h"
#include "PIDDataManager.h"
#include "Point.h"
#include "StrategyManager.h"
#include "ETTrainStrategy.h"
#include "BlockMoveStrategy.h"

#define AVERAGE_COUNT 200

void MotorTestStrategy::Run()
{
	auto InOut = InOutManager::GetInstance();

	// スイッチの立ち上がりでパワー値を増加
	if(SwitchON == false) {
		if(InOut->InputData.TouchSensor == 1) {
			Power += 25;
			SwitchON = true;
		
			// パワーが100を超えたら、0に初期化
			if(Power > 100) {
				Power = 0;
			}
		}
	} else {
		if(InOut->InputData.TouchSensor == 0) {
			SwitchON == false;
		}
	}

	// 平均格納ベクタを更新
	AngleVector.push_back(InOut->InputData.LeftMotorAngle);
	if(AngleVector.size() > AVERAGE_COUNT) AngleVector.erase(AngleVector.begin());

	// 平均値を算出
	double average = 0.0;
	for(int d : AngleVector) average  += d;
	average /= AngleVector.size();	

	// 5ms * 20 = 100ms周期でモニタを更新
	if(RateCount < 20) {
		RateCount++;
	} else {
		RateCount = 0;
		static char buf[256];
		  
		sprintf(buf, "Power:%d:::", Power); 
		ev3_lcd_draw_string(buf, 0, 0);    
	
		sprintf(buf, "Ave:%f:::", average); 
		ev3_lcd_draw_string(buf, 12, 0);    
	}

	// モータの出力値を更新
	InOut->OutputData.LeftMotorPower = Power;
}

