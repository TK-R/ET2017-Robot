
#ifndef __PARTICLE_FILTER__
#define  __PARTICLE_FILTER__

#include "Point.h"

class Particle
{


};

class ParticleFilter
{
private:
	Particle ParticleArray[30];


public:
	void Resampling(double x, double y, double theta);
	void UpdateParticle(double x, double y, double theta);
	Point* Localize();
};

#endif 