#include <map>
#include "SerialData.h"
#include "SerialReceiveTask.h"
#include "PIDDataManager.h"

PIDDataManager* PIDDataManager::GetInstance()
{
	static PIDDataManager manager;
	return &manager;
}

PIDData PIDDataManager::GetPIDData(int state) 
{
	CurrentPID = PIDMap[state];
	return PIDMap[state];
}

bool PIDDataManager::SetPIDData(PIDData data)
{
	PIDMap[data.State] = data;
	return true;

}