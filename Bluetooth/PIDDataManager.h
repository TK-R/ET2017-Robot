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
		PIDData lineStraight = {66.7, 0.08, 0, 0.90, LineTraceStraight, 0}; 	// 8.7V
		SetPIDData(lineStraight);

		//Rコース
		PIDData lineR_B = {100, 0.30, 0, 0.52, pR_B, 9};
		SetPIDData(lineR_B);

		PIDData lineR_C = {43, 0.23, 0, 0.43, pR_C, -21};
		SetPIDData(lineR_C);

		PIDData lineR_D = {40, 0.16, 0, 0.47, pR_D, -42};
		SetPIDData(lineR_D);

		PIDData lineR_E = {36.4, 0.27, 0, 0.36, pR_E, 10};
		SetPIDData(lineR_E);

		//Lコース
		PIDData lineL_B = {61.7, 0.14, 0, 0.63, pL_B, 21};
		SetPIDData(lineL_B);

		PIDData lineL_C = {50.9, 0.19, 0, 0.99, pL_C, -10}; //8.8V
		SetPIDData(lineL_C);

		PIDData lineL_D = {50.6, 0.18, 0, 0.96, pL_D, -13};  // 8.5v
		SetPIDData(lineL_D);

		PIDData lineL_E = {50.6, 0.18, 0, 0.96, pL_E, -9};
		SetPIDData(lineL_E);

		PIDData lineL_F = {44.7, 0.24, 0, 0.99, pL_F, -27};
		SetPIDData(lineL_F);

		PIDData lineL_G = {44.7, 0.21, 0, 0.99, pL_G, 27};
		SetPIDData(lineL_G);


		// ブロック並べ中
		PIDData block = {21, 0.71, 0, 0.4, BlockMovePIDState, 0};
		SetPIDData(block);

		// ブロック並べ中（高速）
		PIDData blockH = {18.5, 0.39, 0, 0.38, ETSumoHighPIDState, 0};
		SetPIDData(blockH);

		// ET相撲中
		PIDData sumo = {15, 1.2, 0, 0, ETSumoPIDState, 0};
		SetPIDData(sumo);

		// ET相撲中(高速）	
		PIDData sumoH = {30, 0.39, 0, 0.5, ETSumoHighPIDState, 0};
		SetPIDData(sumoH);

		// ET相撲前の列車停止中
		PIDData trainS = {15.5, 1, 0.08, 0.91, ETTrainSlow, 0};
		SetPIDData(trainS);
	
		// ET相撲前の列車停止中
		PIDData trainH = {32, 0.38, 0.0, 0.5, ETTrainHigh, 0};
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