#pragma once

#include "C4Particles.h"

#define MAX_LIGHT_PARTICLES 64

namespace C4
{
	enum
	{
		kParticleSystemLight = 'lght'
	};

	class LightParticleSystem : public PointParticleSystem
	{

	private:
		Point3D start;
		Point3D end;

		ParticlePool<> particlePool;
		Particle particleArray[MAX_LIGHT_PARTICLES];

	public:
		LightParticleSystem(Point3D start, Point3D end);
		LightParticleSystem();
		~LightParticleSystem();

		void Preprocess(void) override;
		void AnimateParticles(void) override;

		void SetStart(Point3D start);
		void SetEnd(Point3D end);
	};
}
