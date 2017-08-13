
#ifndef __PARTICLE_FILTER__
#define  __PARTICLE_FILTER__

#include "Point.h"
#include "ev3api.h"
class Particle
{
private:
	// 尤度(0-100で、100が最も尤もらしい状態)
	int Likelihood;
	Point* RobotCenterPoint;
	double Angle;
public:
	double NormalDistribution(double sigma);
	void Update(int8_t leftMotorCount, int8_t rightMotorCount, double sigma);
	void Reset(Point* newPoint, double newAngle);
};

class ParticleFilter
{
private:
	Particle ParticleArray[30];


public:
	void Resampling(double x, double y);
	void UpdateParticle(double x, double y);
	Point* Localize();
	ParticleFilter(){}
};

#endif 