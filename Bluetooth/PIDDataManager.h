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
		PIDData lineStraight = {100, 0.08620686, 0, 0.183908, LineTraceStraight, 0};
		SetPIDData(lineStraight);
				
		//Rコース
		PIDData lineR_B = {100, 0.1551724, 0, 0.3908046, pR_B, 7};
		SetPIDData(lineR_B);

		PIDData lineR_C = {80.45978, 0.2586207, 0, 0.2758621, pR_C, -2};
		SetPIDData(lineR_C);

		PIDData lineR_D = {80.45977, 0.2586207, 0, 0.6666667, pR_D, -24};
		SetPIDData(lineR_D);
		
		PIDData lineR_E = {80.45977, 0.2413793, 0, 0.2988506, pR_E, 8};
		SetPIDData(lineR_E);

		//Lコース
		PIDData lineL_B = {80.45978, 0.1896552, 0, 0.2068966, pL_B, 17};
		SetPIDData(lineL_B);
		
		PIDData lineL_C = {80.45977, 0.1896552, 0, 0.3908046, pL_C, -6};
		SetPIDData(lineL_C);

		PIDData lineL_D = {80.45977, 0.1724138, 0, 0.2988506, pL_D, -15};
		SetPIDData(lineL_D);

		PIDData lineL_E = {90.8046, 0.1896552, 0, 0.2528736, pL_E, -7};
		SetPIDData(lineL_E);

		PIDData lineL_F = {77.0115, 0.2068965, 0, 0.2758621, pL_F, -28};
		SetPIDData(lineL_F);

		PIDData lineL_G = {78.16092, 0.2068965, 0, 0.2988506, pL_G, 28};
		SetPIDData(lineL_G);

		// ブロック並べ中
		PIDData block = {16, 1, 0, 0.61, BlockMovePIDState, 0};
		SetPIDData(block);

		// ブロック並べ中（高速）
		PIDData blockH = {28.4, 0.43, 0, 1.25, BlockMoveHighPIDState, 8};
		SetPIDData(blockH);

		// ET相撲中
		PIDData sumo = {16.1, 2.7, 0, 1.0, ETSumoPIDState, 0};
		SetPIDData(sumo);

		// ET相撲中(高速）	
		PIDData sumoH = {30, 0.39, 0, 0.5, ETSumoHighPIDState, 0};
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