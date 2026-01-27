#pragma once
#include<DirectXMath.h>
#include <System/RenderContext.h>
#include <vector>

struct Particle
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 velocity;
    float life;        // remaining time
    float maxLife;
    float size;
    DirectX::XMFLOAT4 color;
};

class ParticleSystem
{
public:

    ParticleSystem();
	void initParticle(Particle& p, const DirectX::XMFLOAT3& pos);
    void Spawn(const DirectX::XMFLOAT3& pos);
	void Update(float elapsedTime);
	void Draw(const RenderContext& rc);

private:

	std::vector<Particle> particles;
    std::vector<Particle*> activeParticles;  // alive
    std::vector<Particle*> freeParticles;
};