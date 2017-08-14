
#ifndef __PARTICLE_FILTER__
#define  __PARTICLE_FILTER__

#include "Point.h"
#include "ev3api.h"

// 散布する粒子数
#define PARTICLE_COUNT 30

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

	Particle(){}
};

class ParticleFilter
{
private:
	Particle* ParticleArray[PARTICLE_COUNT];

public:
	Point RobotPoint;
	double RobotAngle;
		 
	void Resampling(Point* newPoint, double newAngle, double sigma);
	void UpdateParticle(int8_t leftMotorCount, int8_t rightMotorCount);
	void Localize();
	
	// コンストラクタで粒子の生成
	ParticleFilter() 
	{
		for(int i = 0; i < PARTICLE_COUNT; i++) {
			ParticleArray[i] = new Particle();
		}
	}
};

#endif 