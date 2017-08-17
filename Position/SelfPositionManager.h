#ifndef SELF_POSITION_MANAGER_
#define SELF_POSITION_MANAGER_

#include "ev3api.h"
#include "SerialData.h"
#include "ParticleFilter.h"
#include "Point.h"
class SelfPositionManager
{
private:
	SelfPositionManager(){}
	ParticleFilter* Filter;
public:
	// ONなら自己位置推定を実施する
	bool ParticleFilterON = false;

	// 走行体の座標
	Point RobotPoint;
	
	// 走行体の角度
	double RobotAngle;

	// 総走行距離
	double Distance;
	static SelfPositionManager* GetInstance();

	void UpdatePosition(int8_t leftMotorCount, int8_t rightMotorCount);
	void ResetPosition(SelfPositionData p);

	// ある座標との距離を求める
	double GetDistance(Point* p);
};



#endif