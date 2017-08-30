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
		PIDData lineStraight = {100, 0.35, 0, 0.33, LineTraceStraight, -6}; 	// 8.2V
		SetPIDData(lineStraight);

		//Rコース
		PIDData lineR_B = {100, 0.35, 0, 0.33, pR_B, -6};
		SetPIDData(lineR_B);

		PIDData lineR_C = {100, 0.35, 0, 0.33, pR_C, -6};
		SetPIDData(lineR_C);

		PIDData lineR_D = {100, 0.35, 0, 0.33, pR_D, -6};
		SetPIDData(lineR_D);

		PIDData lineR_E = {100, 0.35, 0, 0.33, pR_E, -6};
		SetPIDData(lineR_E);

		//Lコース
		PIDData lineL_B = {100, 0.35, 0, 0.33, pL_B, -6};
		SetPIDData(lineL_B);

		PIDData lineL_C = {100, 0.35, 0, 0.33, pL_C, -6};
		SetPIDData(lineL_C);

		PIDData lineL_D = {100, 0.35, 0, 0.33, pL_D, -6};
		SetPIDData(lineL_D);

		PIDData lineL_E = {100, 0.35, 0, 0.33, pL_E, -6};
		SetPIDData(lineL_E);

		PIDData lineL_F = {100, 0.35, 0, 0.33, pL_F, -6};
		SetPIDData(lineL_F);

		PIDData lineL_G = {100, 0.35, 0, 0.33, pL_G, -6};
		SetPIDData(lineL_G);


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