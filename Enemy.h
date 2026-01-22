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

	//dash
	void Dash(float elapsedTime);


	void Impulse(float elapsedTime);

	void Shrink(float elapsedTime);

	void Destroy();

protected:

	//int hp = 1;
	float deathTimer;
	float squashY = 1.0f;

	bool dying = false;

	bool deathImpulse = false;

	//dash

	enum class DashState { Cooldown, Windup, Dashing, Recover, Dying };
	DashState dashState = DashState::Cooldown;

	bool targetLocked = false;
	DirectX::XMFLOAT3 dashdirect;
	float dashTimer = 0.0f;

	float dashCooldown = 2.0f;   // time between charges
	float windupTime = 0.4f;   // pause before dash (telegraph)
	float dashTime = 0.6f;   // how long it charges
	float recoverTime = 0.5f;   // pause after dash

	float dashSpeed = 12.0f;  // charge speed
	float dashRange = 12.0f;  // only charge if player within range
	float fallspeed;
	//shrink base scale
	DirectX::XMFLOAT3 bs;

};