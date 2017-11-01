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
		PIDData lineLineTraceStraight = {100, 0.2142857, 0, 0.4615385, LineTraceStraight, 0};
		SetPIDData(lineLineTraceStraight);
		
		//Rコース
		PIDData linepR_B = {100, 0.2068965, 0, 0.4597702, pR_B, 9};
		SetPIDData(linepR_B);

		PIDData linepR_C = {89.01099, 0.2068965, 0, 0.3448276, pR_C, -4};
		SetPIDData(linepR_C);

		PIDData linepR_D = {82.3039, 0.2413793, 0, 0.6666667, pR_D, -25};
		SetPIDData(linepR_D);

		PIDData linepR_E = {89.25098, 0.2218643, 0, 0.1538462, pR_E, 15};
		SetPIDData(linepR_E);

		//Lコース
		PIDData linepL_B = {80.45978, 0.1896552, 0, 0.2068966, pL_B, 22};
		SetPIDData(linepL_B);		

		PIDData linepL_C = {80.45977, 0.2241379, 0, 0.5057471, pL_C, -6};
		SetPIDData(linepL_C);

		PIDData linepL_D = {78.16092, 0.1724138, 0, 0.4137931, pL_D, -11};
		SetPIDData(linepL_D);
		
		PIDData linepL_E = {83.90804, 0.1896552, 0, 0.3908046, pL_E, -3};
		SetPIDData(linepL_E);

		PIDData linepL_F = {70.11494, 0.2068965, 0, 0.2758621, pL_F, -30};
		SetPIDData(linepL_F);

		PIDData linepL_G = {71.26437, 0.1551724, 0, 0.2758621, pL_G, 30};
		SetPIDData(linepL_G);

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