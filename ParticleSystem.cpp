#include"ParticleSystem.h"

ParticleSystem::ParticleSystem()
{
	particles.resize(500);

	for (auto& p : particles)
	{
		p.life = 0.0f;          // mark as dead
		freeParticles.push_back(&p);
	}
}

void ParticleSystem::initParticle(Particle& p,const DirectX::XMFLOAT3& pos)
{
		p.position = pos;

		p.velocity =
		{
			(rand() / (float)RAND_MAX - 0.5f) * 5.0f,
			(rand() / (float)RAND_MAX) * 5.0f,
			(rand() / (float)RAND_MAX - 0.5f) * 5.0f
		};

		p.life = p.maxLife = 1.0f;
		p.size = 0.2f;
		p.color = { 1.0f,0.8f,0.2f,1.0f };
	
}

void ParticleSystem::Spawn(const DirectX::XMFLOAT3& pos)
{
	int spawnCount = 30;

	auto it = freeParticles.begin();

	while (it != freeParticles.end() && spawnCount > 0)
	{
		Particle* p = *it;

		initParticle(*p, pos);

		activeParticles.emplace_back(p);

		it = freeParticles.erase(it); // remove ONE used particle
		--spawnCount;
	}
}

void ParticleSystem::Update(float elapsedTime)
{
	for (auto it = activeParticles.begin(); it != activeParticles.end(); )
	{
		Particle* p = *it;

		p->life -= elapsedTime;
		if (p->life <= 0.0f)
		{
			freeParticles.push_back(p);
			it = activeParticles.erase(it);
			continue;
		}

		p->position.x += p->velocity.x * elapsedTime;
		p->position.y += p->velocity.y * elapsedTime;
		p->position.z += p->velocity.z * elapsedTime;

		p->color.w = p->life / p->maxLife;
		++it;
	}
}
