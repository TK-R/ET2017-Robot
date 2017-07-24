#include <string.h>
#include "ev3api.h"
#include "SerialData.h"
#include "SerialSendTask.h"
#include "SelfPositionManager.h"

SelfPositionManager::SelfPositionManager(SelfPositionData p)
{
	PositionData = p;
}

void SelfPositionManager::UpdatePosition(uint8_t leftMotorCount, uint8_t rightMotorCount)
{
	
	// 自己位置情報と同様のサイズを持つ配列にコピー
	memcpy(buff_self_position, &PositionData, sizeof(SelfPositionData));
}

void SelfPositionManager::ResetPosition(SelfPositionData p)
{

}
