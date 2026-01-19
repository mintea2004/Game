#pragma once

#include<DirectXMath.h>
#include "System/ShapeRenderer.h"

class Character
{
public:
	Character() {}
	virtual ~Character() {}

	void UpdateTransform();

	const DirectX::XMFLOAT3& GetPosition() const { return position; }
	void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }

	const DirectX::XMFLOAT3& GetAngle() const { return angle; }
	void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }

	const DirectX::XMFLOAT3& GetScale() const { return scale; }
	void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }
	float GetRadius() const { return radius; }
	float GetSlamRadius() const { return slamRadius; }

	float GetVelocityY()const { return velocity.y; }

	virtual void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer);

	bool IsGround() const { return isGround; }
	bool IsAir() const { return !isGround; }
	bool IsSlamHitActive() const
	{
		return slamHitActive;
	}

	void AddImpulse(const DirectX::XMFLOAT3& impluse);

	float GetHeight() const { return height; }

	bool ApplyDamage(int damage, float invincibleTime);

protected:
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT3 scale = { 0.01f,0.01f,0.01f };
	DirectX::XMFLOAT4X4 transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	float radius = 0.5f;
	
	float gravity = -30.0f;
	DirectX::XMFLOAT3 velocity = { 0,0,0 };
	bool isGround = true;
	bool isMoving = false;
	float height = 2.0f;
	int health = 5;
	float invincibleTimer = 1.0f;
	float friction = 15.0f;
	float acceleration = 50.0f;
	float maxMoveSpeed = 5.0f;
	float moveVecX = 0.0f;
	float moveVecZ = 0.0f;
	float airControl = 0.3f;
	float visualOffset = 0.0f;

	//slam
	float slamDelay = 0.07f;
	float slamSpeed = 0.0f;
	float slamRadius = 2.0f;
	bool isSlamming = false;
	bool slamHitActive = false;
	//squash
	float slamSquashAmount = 3.0f;   // how strong the squash is
	float slamSquashSpeed = 1.9f;   // how fast it recovers
	
	void Move(float elapsedTime, float xv, float vz, float speed);
	void Turn(float elapsedTime, float vx, float vz, float speed);
	void Jump(float speed);
	void Slam(float elapsedTime);
	void UpdateVelocity(float elapsedTime);
	void UpdateInvincibleTimer(float elapsedTime);

	float Lerp(float a, float b, float t)
	{
		if (t < 0.0f) t = 0.0f;
		if (t > 1.0f) t = 1.0f;
		return a + (b - a) * t;
	}

	virtual void OnLanding() {}
	virtual void OnDamaged() {}
	virtual void OnDead() {}
	
private:

	void UpdateVerticalVelocity(float elapsedTime);
	void UpdateVerticalMove(float elapsedTime);
	void UpdateHorizontalVelocity(float elapsedTime);
	void UpdateHorizontalMove(float elapsedTime);

	
};
