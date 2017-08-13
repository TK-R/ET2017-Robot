
#ifndef __PARTICLE_FILTER__
#define  __PARTICLE_FILTER__

#include "Point.h"
#include "ev3api.h"
class Particle
{
private:
	// 尤度(0-100で、100が最も尤もらしい状態)
	int Likelihood;
	// 粒子が持つ走行体の座標
	Point RobotPoint;
	// 粒子が持つ走行体の方向
	double Angle;

public:
	double NormalDistribution(double sigma);
	void Update(int8_t leftMotorCount, int8_t rightMotorCount);
	void Reset(Point* newPoint, double newAngle, double sigma);
};

class ParticleFilter
{
private:
	Particle ParticleArray[30];

public:
	Point RobotPoint;
	double RobotAngle;
		
	void Resampling(Point* newPoint, double Angle, double sigma);
	void UpdateParticle(int8_t leftMotorCount, int8_t rightMotorCount);
	void Localize();
	ParticleFilter(){}
};

#endif 