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
	ETSumoPIDState = 20
};


class PIDDataManager{
private:
	std::map<int, PIDData> PIDMap;	
	PIDDataManager()
	{
		// PIDDataの初期値を登録
		// 直線
		PIDData lineStraight = {60, 0.06, 0.06, 0.53, LineTraceStraight};
		SetPIDData(lineStraight);

		// 高速カーブ
		PIDData lineHCurve = {45, 0.09, 0.05, 0.53, LineTraceHighSpeedCurve};
		SetPIDData(lineHCurve);

		// 中速カーブ
		PIDData lineMCurve = {30.2, 0.26, 0.02, 0.78, LineTraceMiddleSpeedCurve};
		SetPIDData(lineMCurve);

		// 低速カーブ
		PIDData lineSCurve = {30, 0.76, 0, 0.84, LineTraceSlowSpeedCurve};
		SetPIDData(lineSCurve);

		// ブロック並べ中
		PIDData block = {16, 0.4, 0, 0.7, BlockMovePIDState};
		SetPIDData(block);

		// ET相撲中
		PIDData sumo = {15, 1.2, 0, 1.0, ETSumoPIDState};
		SetPIDData(sumo);
	}

public:
	static PIDDataManager* GetInstance();

	// ステートNoを指定してキーを取得する
	PIDData GetPIDData(int state);

	// PIDDataをmapに登録する
	bool SetPIDData(PIDData data);
};

#endif