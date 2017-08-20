#ifndef __PID_DATA_MANAGER__
#define __PID_DATA_MANAGER__

#include <map>
#include "SerialData.h"

enum StateNo {
	LineTracePIDState = 1,
	BlockMovePIDState = 10,
	ETSumoPIDState = 20
};


class PIDDataManager{
private:
	std::map<int, PIDData> PIDMap;	
	PIDDataManager()
	{
		// PIDDataの初期値を登録
		PIDData line = {30, 0.4, 0, 0.1, LineTracePIDState};
		SetPIDData(line);

		PIDData block = {30, 0.4, 0, 0.1, BlockMovePIDState};
		SetPIDData(block);

		PIDData sumo = {30, 0.4, 0, 0.1, ETSumoPIDState};
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