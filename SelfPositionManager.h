#ifndef SELF_POSITION_MANAGER_
#define SELF_POSITION_MANAGER_

#include "ev3api.h"
#include "SerialData.h"

class SelfPositionManager
{
	private:
		SelfPositionManager(){}

	public:
		double PositionX, PositionY, Angle;
		static SelfPositionManager* GetInstance();

		void UpdatePosition(int8_t leftMotorCount, int8_t rightMotorCount);
		void ResetPosition(SelfPositionData p);
};



#endif