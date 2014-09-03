#include "LightParticleSystem.h"

using namespace C4;

LightParticleSystem::LightParticleSystem(Point3D start, Point3D end) :
	PointParticleSystem(kParticleSystemLight, &particlePool),
	particlePool(MAX_LIGHT_PARTICLES, particleArray)
{
	this->start = start;
	this->end = end;
}

LightParticleSystem::LightParticleSystem() :
PointParticleSystem(kParticleSystemLight, &particlePool),
particlePool(MAX_LIGHT_PARTICLES, particleArray)
{
	start = Point3D(0.0f, 0.0f, 0.0f);
	end = Point3D(0.0f, 0.0f, 0.0f);
}

LightParticleSystem::~LightParticleSystem()
{
}

void LightParticleSystem::Preprocess(void)
{
	SetEffectListIndex(kEffectListLight);
	PointParticleSystem::Preprocess();

	if (!GetFirstParticle())
	{
		Vector3D interval = (end - start) / MAX_LIGHT_PARTICLES;
		for (int i = 0; i < MAX_LIGHT_PARTICLES; i++)
		{
			Particle* particle = particlePool.NewParticle();

			particle->emitTime = 0;
			particle->lifeTime = 5000;
			particle->radius = 0.2f;
			particle->color.Set(1.0F, 1.0F, 1.0F, 1.0F);
			particle->position = start + (interval * i);
			particle->velocity.Set(0.0f, 0.0f, 0.0f);

			AddParticle(particle);
		}
	}
}

void LightParticleSystem::AnimateParticles(void)
{
	Vector3D interval = (end - start) / MAX_LIGHT_PARTICLES;
	Particle* particle = GetFirstParticle();
	int i = 0;
	while (particle)
	{
		Particle* next = particle->nextParticle;
		particle->position = start + (interval * i);
		particle = next;
		i++;
	}
}

void LightParticleSystem::SetStart(Point3D start)
{
	this->start = start;
}

void LightParticleSystem::SetEnd(Point3D end)
{
	this->end = end;
}