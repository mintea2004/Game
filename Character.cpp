#include "Character.h"

void Character::UpdateTransform()
{
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y+visualOffset, position.z);
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);
}

void Character::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	renderer->RenderCylinder(rc, position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
}

void Character::Move(float elapsedTime, float vx, float vz, float speed)
{
	/*speed *= elapsedTime;
	position.x += vx * speed;
	position.z += vz * speed;*/

	moveVecX = vx;
	moveVecZ = vz;

	maxMoveSpeed = speed;
}

void Character::Turn(float elapsedTime, float vx, float vz, float speed)
{
	speed *= elapsedTime;
	float length = sqrtf(vx * vx + vz * vz);

	if (length < 0.001f)return;

	vx /= length;
	vz /= length;

	float frontX = sinf(angle.y);
	float frontZ = cosf(angle.y);

	float cross = (vz * frontX) - (vx * frontZ);
	float dot = (frontX * vx) + (frontZ * vz);

	float rot = 1.0f - dot;
	if (rot > speed) rot = speed;
	if (cross < 0.0f)
	{
		//angle.y += speed;
		angle.y += rot;
	}
	else
	{
		//angle.y -= speed;
		angle.y -= rot;
	}
}

void Character::Jump(float speed)
{
	velocity.y = speed;
	isGround = false;
}

void Character::Slam(float elapsedTime)
{
	
	if (slamDelay>0.0f)
	{
		slamDelay -= elapsedTime;
		velocity.y = 0.0f;
		return;
	}

	const float slamGravity	 = 6000.0f;
	const float maxSlamSpeed = 12000.0f;

	slamSpeed += slamGravity * elapsedTime;
	slamSpeed = min(slamSpeed, maxSlamSpeed);

	velocity.y = -slamSpeed;

	position.y += velocity.y * elapsedTime;

	if (position.y <= 0.0f)
	{
		position.y = 0.0f;
		velocity.y = 0.0f;
		visualOffset = 0.0f;
		OnLanding();
	}
}


void Character::UpdateVelocity(float elapsedTime)
{
	UpdateVerticalVelocity(elapsedTime);
	UpdateHorizontalVelocity(elapsedTime);
	UpdateVerticalMove(elapsedTime);
	UpdateHorizontalMove(elapsedTime);
	/*velocity.y += gravity * elapsedTime;
	position.y += velocity.y * elapsedTime;

	if (position.y < 0.0f)
	{
		position.y = 0.0f;
		velocity.y = 0.0f;



		if (!IsGround())
		{
			OnLanding();
			isGround = true;
		}

	}
	else
	{
		isGround = false;
	}*/
}

bool Character::ApplyDamage(int damage, float invincibleTime)
{
	if (damage <= 0)
	{
		return false;
	}

	if (hp <= 0)
	{
		return false;
	}

	if (invincibleTimer > 0.0f)
	{
		return false;
	}

	invincibleTimer = invincibleTime;


	hp = hp - damage;

	if (hp <= 0)
	{
		OnDead();
	}
	else
	{
		OnDamaged();
	}

	return true;
}

void Character::UpdateInvincibleTimer(float elapsedTime)
{
	if (invincibleTimer > 0.0f)
	{
		invincibleTimer -= elapsedTime;
	}
}

void Character::UpdateVerticalVelocity(float elapsedTime)
{
	velocity.y += gravity * elapsedTime;

	position.y += velocity.y * elapsedTime;
}

void Character::UpdateVerticalMove(float elapsedTime)
{
	if (position.y < 0.0f)
	{
		position.y = 0.0f;
		velocity.y = 0.0f;



		if (!IsGround())
		{
			OnLanding();
			isGround = true;
		}

	}
	else
	{
		isGround = false;
	}
}

void Character::UpdateHorizontalVelocity(float elapsedTime)
{
	float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);

	float friction = this->friction * elapsedTime;

	//XZŒ¸‘¬
	if (length > 0.0f)
	{
		if (!isGround)
		{
			friction *= airControl;
		}

		if (length > friction)
		{
			velocity.x -= (velocity.x / length) * friction;
			velocity.z -= (velocity.z / length) * friction;
		}
		else
		{
			velocity.x = 0.0f;
			velocity.z = 0.0f;
		}
	}

	//XZ‰Á‘¬
	if (length <= maxMoveSpeed)
	{
		float moveVecLength = sqrtf(moveVecX * moveVecX + moveVecZ * moveVecZ);
		if (moveVecLength > 0.0f)
		{
			float acceleration = this->acceleration * elapsedTime;
			if (!isGround)
			{
				friction *= airControl;
			}
			velocity.x += (moveVecX / moveVecLength) * acceleration;
			velocity.z += (moveVecZ / moveVecLength) * acceleration;

			float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
			if (length > maxMoveSpeed)
			{
				velocity.x = (velocity.x / length) * maxMoveSpeed;
				velocity.z = (velocity.z / length) * maxMoveSpeed;
			}
		}
	}
	moveVecX = 0.0f;
	moveVecZ = 0.0f;
}

void Character::UpdateHorizontalMove(float elapsedTime)
{
	position.x += velocity.x * elapsedTime;
	position.z += velocity.z * elapsedTime;
}

void Character::AddImpulse(const DirectX::XMFLOAT3& impulse)
{
	velocity.x += impulse.x;
	velocity.y += impulse.y;
	velocity.z += impulse.z;
}