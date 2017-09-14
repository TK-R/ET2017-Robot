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
		PIDData lineStraight = {67.9, 0.12, 0, 0.92, LineTraceStraight, 0}; 	// 8.7V
		SetPIDData(lineStraight);

		//Rコース
		PIDData lineR_B = {61.7, 0.14, 0, 0.63, pR_B, 9};
		SetPIDData(lineR_B);

		PIDData lineR_C = {37, 0.25, 0, 1.22, pR_C, 0};
		SetPIDData(lineR_C);

		PIDData lineR_D = {37, 0.27, 0, 0.9, pR_D, -36};
		SetPIDData(lineR_D);

		PIDData lineR_E = {50.6, 0.18, 0, 0.96, pR_E, 6};
		SetPIDData(lineR_E);

		//Lコース
		PIDData lineL_B = {61.7, 0.14, 0, 0.63, pL_B, 21};
		SetPIDData(lineL_B);

		PIDData lineL_C = {45.7, 0.2, 0, 1.37, pL_C, -7}; //8.8V
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
		PIDData block = {16, 1, 0, 0.61, BlockMovePIDState, 0};
		SetPIDData(block);

		// ブロック並べ中（高速）
		PIDData blockH = {23.5, 0.65, 0, 1.74, BlockMoveHighPIDState, 0};
		SetPIDData(blockH);

		// ET相撲中
		PIDData sumo = {16.1, 2.1, 0, 1.5, ETSumoPIDState, 0};
		SetPIDData(sumo);

		// ET相撲中(高速）	
		PIDData sumoH = {33.4, 0.47, 7, 1.52, ETSumoHighPIDState, 0};
		SetPIDData(sumoH);

		// ET相撲前の列車停止中
		PIDData trainS = {19, 0.8, 0.00, 0.81, ETTrainSlow, 0};
		SetPIDData(trainS);
	
		// ET相撲前の列車停止中
		PIDData trainH = {34.6, 0.43, 0.0, 1.25, ETTrainHigh, 0};
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