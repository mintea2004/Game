#pragma once

#include"System//ModelRenderer.h"
#include"Character.h"
#define MAX_DASHCOOLDOWN 600.0f

enum class State
{
	Ground,
	Air,
	Dash,
	Slam,
	SlamCooldown
};

class Player : public Character
{
public:

	Player();
	~Player() override;

	void Update(float elapsedTime);

	void Render(const RenderContext& rc, ModelRenderer* renderer);

	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer);

	void DrawDebugGUI();

private:

	Model* model = nullptr;

	State state;
	
	//Model* sword = nullptr;
	DirectX::XMFLOAT3 GetMoveVec() const;

	float moveSpeed = 5.0f;

	void InputMove(float elapsedTime);
	void InputJump(float elapsedTime);
	
	void ApplyHop(float elapsedTime);

	void MoveSeiGen();

	//slam
	void StartSlam();
	//void SlamRecovery(float elapsedTime);
	void SlamPlayerVsEnemies();

	bool IsSlamming() { return state == State::Slam; }
	int GetSlamGauge()
	{
		return slamGauge;
	};

	//dash
	void StartDash();

	//dash
	bool  isDashing = false;
	float dashTimer = 0.0f;
	float dashDuration = 0.08f;
	float dashSpeed = 13.0f;
	float dashCooldown = 0.0f;

	// Dash lean
	float dashLeanAngle = 0.0f;          // current z-rotation
	float dashLeanTarget = 0.0f;         // where we want to lean
	float dashLeanMax = DirectX::XMConvertToRadians(45.0f);
	float dashLeanSpeed = 102.0f;
	float dashLeanHoldTimer = 0.0f;
	float dashLeanHoldDuration = 0.15f; // how long lean stays

	float dashLeanInSpeed = 16.0f;   // lean-in speed
	float dashLeanOutSpeed = 6.0f;   // recovery speed (slower!)

	// Double tap detection
	float forwardTapTimer = 0.0f;
	int   forwardTapCount = 0;
	float doubleTapTime = 0.25f;
	bool dashInputHeld = false;

	//prevent spam
	bool  dashUsedInAir = false;

	float turnSpeed = DirectX::XMConvertToRadians(720);
	void CollisionPlayerVsEnemies();

	//taking item
	void CollisionPlayerVsItem();
	
	float jumpSpeed = 16.0f;
	float walkJumpSpeed = 4.0f;
	int jumpCount = 1;
	int jumpLimit = 1;
	
	DirectX::XMFLOAT3 moveVec{};
	bool isMoving = false;

	//slam
	float slamInputBuffer = 0.0f;
	const float slamBufferTime = 0.25f;
	float slamAfterTimer = 0.6f;
	bool slamRecovery = false;
	float OffsetY = 3.0f;
	int slamGauge = 0;
	int MaxGauge = 1;

	//squash
	float slamSquashAmount = 0.35f;   // how strong the squash is
	float slamSquashSpeed = 1.9f;   // how fast it recovers

	float baseY = 0.0f;
	float hopTimer = 0.0f;
	float hopHeight = 0.5f;     // how high it hops
	float hopSpeed = 6.0f;    // how fast it hops
	bool grounded;


protected:

	void OnLanding()override;
	
};