#include "LightParticleSystem.h"

using namespace C4;

LightParticleSystem::LightParticleSystem(Point3D start, Point3D endLeft, Point3D endRight) :
	PointParticleSystem(kParticleSystemLight, &particlePool),
	particlePool(MAX_LIGHT_PARTICLES, particleArray)
{
	this->start = start;
	this->endLeft = endLeft;
	this->endRight = endRight;
}

LightParticleSystem::LightParticleSystem() :
PointParticleSystem(kParticleSystemLight, &particlePool),
particlePool(MAX_LIGHT_PARTICLES, particleArray)
{
	start = Point3D(0.0f, 0.0f, 0.0f);
	endLeft = Point3D(0.0f, 0.0f, 0.0f);
	endRight = Point3D(0.0f, 0.0f, 0.0f);
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
		Vector3D intervalLeft = (endLeft - start) / MAX_LIGHT_PARTICLES;
		Vector3D intervalRight = (endRight - start) / MAX_LIGHT_PARTICLES;
		bool left = true;
		for (int i = 0; i < MAX_LIGHT_PARTICLES; i++)
		{
			Particle* particle = particlePool.NewParticle();

			particle->emitTime = 0;
			particle->lifeTime = 5000;
			particle->radius = 0.02f + (0.0005f * i);
			particle->color.Set(0.0F, 01.0F, 1.0F, 1.0F);
			if (left)
			{
				particle->position = start + (intervalLeft * i);
			}
			else
			{
				particle->position = start + (intervalRight * i);
			}
			particle->velocity.Set(0.0f, 0.0f, 0.0f);

			AddParticle(particle);
			
			left = !left;
		}
	}
}

void LightParticleSystem::AnimateParticles(void)
{
	Vector3D intervalLeft = (endLeft - start) / MAX_LIGHT_PARTICLES;
	Vector3D intervalRight = (endRight - start) / MAX_LIGHT_PARTICLES;
	Particle* particle = GetFirstParticle();
	int i = 0;
	bool left = true;
	while (particle)
	{
		Particle* next = particle->nextParticle;
		if (left)
		{
			particle->position = start + (intervalLeft * i);
		}
		else
		{
			particle->position = start + (intervalRight * i);
		}
		particle = next;
		i++;
		left = !left;
	}
}

void LightParticleSystem::SetStart(Point3D start)
{
	this->start = start;
}

void LightParticleSystem::SetEndLeft(Point3D endLeft)
{
	this->endLeft = endLeft;
}

void LightParticleSystem::SetEndRight(Point3D endRight)
{
	this->endRight = endRight;
}