#ifndef __PID_DATA_MANAGER__
#define __PID_DATA_MANAGER__

#include <map>
#include "SerialData.h"

enum StateNo {
	LineTraceStraight = 1,
	LineTraceHighSpeedCurve,
	LineTraceMiddleSpeedCurve,
	LineTraceSlowSpeedCurve,
	BlockMovePIDState = 10,
	ETSumoPIDState = 20,
	ETTrainSlow = 21,
	ETTrainHigh = 22,
};


class PIDDataManager{
private:
	std::map<int, PIDData> PIDMap;	
	PIDDataManager()
	{
		// PIDDataの初期値を登録
		// 直線
		PIDData lineStraight = {100, 0.48, 0.02, 0.90, LineTraceStraight};
		SetPIDData(lineStraight);

		// 高速カーブ
		PIDData lineHCurve = {37, 0.26, 0.03, 0.66, LineTraceHighSpeedCurve};	//未調整
		SetPIDData(lineHCurve);

		// 中速カーブ
		PIDData lineMCurve = {100, 0.53, 0.02, 0.90, LineTraceMiddleSpeedCurve};
		SetPIDData(lineMCurve);

		// 低速カーブ
		PIDData lineSCurve = {30, 0.51, 0, 0.96, LineTraceSlowSpeedCurve};	//未調整
		SetPIDData(lineSCurve);

		// ブロック並べ中
		PIDData block = {16, 0.4, 0, 0.7, BlockMovePIDState};
		SetPIDData(block);

		// ET相撲中
		PIDData sumo = {15, 1.2, 0, 1.0, ETSumoPIDState};
		SetPIDData(sumo);

		// ET相撲前の列車停止中
		PIDData trainS = {15, 1.2, 0, 1.0, ETTrainSlow};
		SetPIDData(trainS);
	
		// ET相撲前の列車停止中
		PIDData trainH = {28, 0.48, 0.02, 0.8, ETTrainHigh};
		SetPIDData(trainH);
	}

public:
	static PIDDataManager* GetInstance();

	// ステートNoを指定してキーを取得する
	PIDData GetPIDData(int state);

	// PIDDataをmapに登録する
	bool SetPIDData(PIDData data);
};

#endif