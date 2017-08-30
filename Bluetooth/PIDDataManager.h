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
	BlockMoveHighPIDState,
	ETSumoPIDState = 20,
	ETSumoHighPIDState,
	ETTrainSlow = 30,
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
		PIDData lineStraight = {100, 0.35, 0, 0.33, LineTraceStraight, -6}; 	// 8.2V
		SetPIDData(lineStraight);

		// 高速カーブ
		PIDData lineHCurve = {65, 0.14, 0.01, 0.22, LineTraceHighSpeedCurve, 0};	//未調整
		SetPIDData(lineHCurve);

		// 中速カーブ
		PIDData lineMCurve = {65, 0.24, 0.0, 0.22, LineTraceMiddleSpeedCurve, 0};
		SetPIDData(lineMCurve);

		// 低速カーブ
		PIDData lineSCurve = {65, 0.26, 0.01, 0.22, LineTraceSlowSpeedCurve, 0};	//未調整
		SetPIDData(lineSCurve);

		// ブロック並べ中
		PIDData block = {16, 0.4, 0, 0.7, BlockMovePIDState, 0};
		SetPIDData(block);

		// ブロック並べ中（高速）
		PIDData blockH = {16, 0.4, 0, 0.7, ETSumoHighPIDState, 0};
		SetPIDData(blockH);

		// ET相撲中
		PIDData sumo = {15, 1.2, 0, 1.0, ETSumoPIDState, 0};
		SetPIDData(sumo);

		// ET相撲中(高速）	
		PIDData sumoH = {15, 1.2, 0, 1.0, ETSumoHighPIDState, 0};
		SetPIDData(sumoH);

		// ET相撲前の列車停止中
		PIDData trainS = {15, 1.2, 0, 1.0, ETTrainSlow, 0};
		SetPIDData(trainS);
	
		// ET相撲前の列車停止中
		PIDData trainH = {25, 0.41, 0.0, 0.8, ETTrainHigh, 0};
		SetPIDData(trainH);

		// 直進中のPID制御
		PIDData forward = {100, 0.46, 0.88, 0.22, ForwardPID, 0};
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