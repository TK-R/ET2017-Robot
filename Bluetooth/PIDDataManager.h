#ifndef __PID_DATA_MANAGER__
#define __PID_DATA_MANAGER__

#include <map>
#include "SerialData.h"

enum StateNo {
	LineTraceStraight = 1,
	pR_B,
	pR_C,
	pR_D,
	pR_E,
	pL_B,
	pL_C,
	pL_D,
	pL_E,
	pL_F,
	pL_G,
	BlockMovePIDState = 30,
	BlockMoveHighPIDState,
	ETSumoPIDState,
	ETSumoHighPIDState,
	ETTrainSlow,
	ETTrainHigh,
	ForwardPID = 99,
};


class PIDDataManager{
private:
	std::map<int, PIDData> PIDMap;	
	PIDDataManager()
	{
		// PIDDataの初期値を登録
		// 直線
		PIDData lineStraight = {65, 0.14, 0, 0.56, 0, LineTraceStraight};
		SetPIDData(lineStraight);

		// 高速カーブ
		PIDData lineHCurve = {65, 0.14, 0.01, 0.22, 0, LineTraceHighSpeedCurve};	//未調整
		SetPIDData(lineHCurve);

		// 中速カーブ
		PIDData lineMCurve = {65, 0.24, 0.0, 0.22, 0, LineTraceMiddleSpeedCurve};
		SetPIDData(lineMCurve);

		// 低速カーブ
		PIDData lineSCurve = {65, 0.26, 0.01, 0.22, 0, LineTraceSlowSpeedCurve};	//未調整
		SetPIDData(lineSCurve);

		// ブロック並べ中
		PIDData block = {16, 0.4, 0, 0.7, 0, BlockMovePIDState};
		SetPIDData(block);

		// ブロック並べ中（高速）
		PIDData blockH = {16, 0.4, 0, 0.7, 0, ETSumoHighPIDState};
		SetPIDData(blockH);

		// ET相撲中
		PIDData sumo = {15, 1.2, 0, 1.0, 0, ETSumoPIDState};
		SetPIDData(sumo);

		// ET相撲中(高速）	
		PIDData sumoH = {15, 1.2, 0, 1.0, 0, ETSumoHighPIDState};
		SetPIDData(sumoH);

		// ET相撲前の列車停止中
		PIDData trainS = {15, 1.2, 0, 1.0, 0, ETTrainSlow};
		SetPIDData(trainS);
	
		// ET相撲前の列車停止中
		PIDData trainH = {25, 0.41, 0.0, 0.8, 0, ETTrainHigh};
		SetPIDData(trainH);

		// 直進中のPID制御
		PIDData forward = {100, 0.46, 0.88, 0.22, 0, ForwardPID};
		SetPIDData(forward);
	}

public:
	static PIDDataManager* GetInstance();

	// ステートNoを指定してキーを取得する
	PIDData GetPIDData(int state);

	// PIDDataをmapに登録する
	bool SetPIDData(PIDData data);
};

#endif