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
	LastStraight,
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
		PIDData lineLineTraceStraight = {100, 0.08241761, 0, 0.4175824, LineTraceStraight, 0};
		SetPIDData(lineLineTraceStraight);

		PIDData lineLastStraight = {91.20879, 0.1648352, 0, 0.4835165, LastStraight, 0};
		SetPIDData(lineLastStraight);
		
		//Rコース
		PIDData linepR_B = {93.40659, 0.2068965, 0, 0.4597702, pR_B, 9};
		SetPIDData(linepR_B);

		PIDData linepR_C = {89.01099, 0.2068965, 0, 0.3448276, pR_C, -4};
		SetPIDData(linepR_C);

		PIDData linepR_D = {82.3039, 0.2413793, 0, 0.6666667, pR_D, -25};
		SetPIDData(linepR_D);

		PIDData linepR_E = {80.45978, 0.1888973, 0, 0.4395604, pR_E, 15};
		SetPIDData(linepR_E);

		//Lコース
		PIDData linepL_B = {87.05318, 0.1896552, 0, 0.4486548, pL_B, 22};
		SetPIDData(linepL_B);

		PIDData linepL_C = {79.36087, 0.2713149, 0, 0.7194644, pL_C, -10};
		SetPIDData(linepL_C);

		PIDData linepL_D = {79.25983, 0.2053808, 0, 0.5017052, pL_D, -11};
		SetPIDData(linepL_D);
		
		PIDData linepL_E = {80.61135, 0.2061387, 0, 0.4567387, pL_E, -3};
		SetPIDData(linepL_E);

		PIDData linepL_F = {76.70835, 0.2728306, 0, 0.8033346, pL_F, -30};
		SetPIDData(linepL_F);

		PIDData linepL_G = {80.05557, 0.204623, 0, 0.3417961, pL_G, 30};
		SetPIDData(linepL_G);

		// ブロック並べ中
		PIDData lineBlockMovePIDState = {16.48352, 0.8901099, 0, 0.5714286, BlockMovePIDState, 0};
		SetPIDData(lineBlockMovePIDState);

		
		// ブロック並べ中（高速）
		PIDData blockH = {28.4, 0.43, 0, 1.25, BlockMoveHighPIDState, 8};
		SetPIDData(blockH);

		// ET相撲中
		PIDData sumo = {16.1, 2.7, 0, 0.8, ETSumoPIDState, 0};
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