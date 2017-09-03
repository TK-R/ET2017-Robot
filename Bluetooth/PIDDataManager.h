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
		PIDData lineStraight = {100, 0.3, 0, 0.52, LineTraceStraight, -9}; 	// 8.7V
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
		PIDData lineL_B = {100, 0.28, 0, 0.52, pL_B, 25};
		SetPIDData(lineL_B);

		PIDData lineL_C = {100, 0.28, 0, 0.52, pL_C, -21}; //8.8V
		SetPIDData(lineL_C);

		PIDData lineL_D = {70.4, 0.14, 0, 0.41, pL_D, -22};  // 8.5v
		SetPIDData(lineL_D);

		PIDData lineL_E = {60, 0.15, 0, 0.46, pL_E, -15};
		SetPIDData(lineL_E);

		PIDData lineL_F = {50, 0.16, 0, 0.46, pL_F, -45};
		SetPIDData(lineL_F);

		PIDData lineL_G = {100, 0.28, 0, 0.52, pL_G, 25};
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