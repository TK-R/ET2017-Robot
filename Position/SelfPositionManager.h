#ifndef SELF_POSITION_MANAGER_
#define SELF_POSITION_MANAGER_

#include "ev3api.h"
#include "SerialData.h"
#include "Point.h"
class SelfPositionManager
{
	private:
		SelfPositionManager(){}

	public:
		Point RobotPoint;
		double Angle, Distance;
		static SelfPositionManager* GetInstance();

		void UpdatePosition(int8_t leftMotorCount, int8_t rightMotorCount);
		void ResetPosition(SelfPositionData p);

		// ある座標との距離を求める
		double GetDistance(Point* p);
};



#endif