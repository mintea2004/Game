#pragma once

#include "System/ModelRenderer.h"
#include "Character.h"

class Enemy : public Character
{
public:

	Enemy()
	{

	}

	~Enemy() override
	{

	}

	virtual void Update(float elapsedTime) = 0;

	virtual void Render(const RenderContext& rc, ModelRenderer* renderer) = 0;

	//Damage System
	void TakeDamage(int damage);
	bool IsDead() const { return hp <= 0; }
	

	//slam
	void gettingSlammed(float elapsedTime);

	//chase
	void ChasePlayer(float elapsedTime);

	void Destroy();

protected:

	int hp = 1;
	float deathTimer;
	float squashY = 1.0f;

	bool dying = false;

};