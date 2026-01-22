#include"Player.h"
#include"EnemyManager.h"
#include"ItemManager.h"
#include"Collision.h"
#include"System/Input.h"
#include<imgui.h>
#include"Camera.h"





Player::Player()
{
	model = new Model("Data/Model/Slime/Slime.mdl");
	radius = 0.5f;
	height = 1.0f;
	hp = 10.0f;
	//sword = new Model("Data/Model/Sword/Sword.mdl");
}

Player::~Player()
{
	delete model;
	//delete sword;
}




void Player::Update(float elapsedTime)
{
	
	moveVec = GetMoveVec();

	const float DEADZONE = 0.05f;
	isMoving =
		fabsf(moveVec.x) > DEADZONE ||
		fabsf(moveVec.z) > DEADZONE;

	//jump
	InputJump(elapsedTime);

	//slam
	if (slamInputBuffer > 0.0f)
	{
		slamInputBuffer -= elapsedTime;
	}

	if (slamAfterBuffer>0.0f)
	{
		slamAfterBuffer -= elapsedTime;
	}
	SlamRecovery(elapsedTime);

	//dash
	if (isDashing)
	{
		dashTimer -= elapsedTime;
		if (dashTimer <= 0.0f)
		{
			isDashing = false;
		}
	}

	// tilt
	// --- DASH FORWARD LEAN UPDATE ---
	if (dashLeanHoldTimer > 0.0f)
	{
		dashLeanHoldTimer -= elapsedTime;

		// lean INTO dash
		dashLeanAngle = Lerp(
			dashLeanAngle,
			dashLeanTarget,
			elapsedTime * dashLeanInSpeed
		);
	}
	else
	{
		// recover AFTER hold
		dashLeanAngle = Lerp(
			dashLeanAngle,
			0.0f,
			elapsedTime * dashLeanOutSpeed
		);
	}

	angle.x = dashLeanAngle;



	grounded = isGround;
	//CollisionPlayerVsEnemies();

	InputMove(elapsedTime);
	
	if (!slamHitActive)
	{
		CollisionPlayerVsEnemies();
	}
	// start slam ONLY when falling
	if (slamGauge==MaxGauge&&!isGround &&
		!isSlamming &&
		slamInputBuffer > 0.0f &&
		velocity.y <= 0.0f)
	{
		StartSlam();
		slamInputBuffer = 0.0f; // consume
	}

	if (isSlamming&&velocity.y<=0.0f)
	{
		//slamGauge = 0;
		Slam(elapsedTime);
	}

	if (slamHitActive)
	{
		SlamPlayerVsEnemies();
	}

	if (!slamRecovery&&!isDashing) UpdateVelocity(elapsedTime);
	else if (isDashing)
	{
		// move only horizontally during dash
		position.x += velocity.x * elapsedTime;
		position.z += velocity.z * elapsedTime;
	}


	
	
	
	UpdateInvincibleTimer(elapsedTime);

	CollisionPlayerVsItem();

	ApplyHop(elapsedTime);
	UpdateTransform();

	model->UpdateTransform();
}

void Player::InputMove(float elapsedTime)
{
	if (slamRecovery)
	{
		return;
	}

	GamePad& gamePad = Input::Instance().GetGamePad();

	float ay = gamePad.GetAxisLY();
	float ax = gamePad.GetAxisLX();

	bool dashInputNow =
		fabsf(ax) > 0.8f ||
		fabsf(ay) > 0.8f;

	if (dashInputNow&&!dashInputHeld)
	{
		if (forwardTapTimer <= 0.0f)
		{
			forwardTapCount = 1;
			forwardTapTimer = doubleTapTime;
		}
		else
		{
			forwardTapCount++;
		}
	}

	dashInputHeld = dashInputNow;

	if (forwardTapTimer > 0.0f)
	{
		forwardTapTimer -= elapsedTime;
		if (forwardTapTimer <= 0.0f)
		{
			forwardTapCount = 0;
		}
	}

	if (!isGround &&
		!isDashing &&
		!dashUsedInAir &&
		forwardTapCount >= 2)
	{
		StartDash();
		forwardTapCount = 0;
		forwardTapTimer = 0.0f;
	}

	DirectX::XMFLOAT3 moveVec = GetMoveVec();

	Move(elapsedTime, moveVec.x, moveVec.z, moveSpeed);
	Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
}

DirectX::XMFLOAT3 Player::GetMoveVec()const
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();


	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf((cameraRight.x * cameraRight.x) + (cameraRight.z * cameraRight.z));

	if (cameraRightLength > 0.0f)
	{
		cameraRightX /= cameraRightLength;
		cameraRightZ /= cameraRightLength;
	}

	float cameraFrontX = cameraFront.x;
	float cameraFrontZ = cameraFront.z;
	float cameraFrontLength = sqrtf((cameraFront.x * cameraFront.x) + (cameraFront.z * cameraFront.z));

	if (cameraFrontLength > 0.0f)
	{
		cameraFrontX /= cameraFrontLength;
		cameraFrontZ /= cameraFrontLength;
	}

	DirectX::XMFLOAT3 vec;
	vec.x = (cameraRightX * ax) + (cameraFrontX * ay);
	vec.z = (cameraRightZ * ax) + (cameraFrontZ * ay);
	vec.y = 0.0f;

	return vec;
}

void Player::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, transform, model, ShaderId::Lambert);
}

void Player::DrawDebugGUI()
{

	ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();
	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
	{
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::InputInt("HP", &hp);
			ImGui::InputFloat3("Position", &position.x);
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::InputFloat3("Angle", &a.x);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);
			ImGui::InputFloat3("Scale", &scale.x);
			ImGui::InputInt("SlamGauge", &slamGauge);
			ImGui::InputFloat("Invi", &invincibleTimer);
			//ImGui::Text("isSlamming() = ", isSlamming());
			ImGui::Text("GetSlamRadius() = %.3f", GetSlamRadius());

		}
	}
	ImGui::End();
}

void Player::CollisionPlayerVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	int enemyCount = enemyManager.GetEnemyCount();

	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);
		DirectX::XMFLOAT3 outPosition;
		
		if (Collision::IntersectCylinderVsCylinder(position,
			radius,
			height,
			enemy->GetPosition(),
			enemy->GetRadius(),
			enemy->GetHeight(),
			outPosition,
			isSlamming))
		{
			enemy->SetPosition(outPosition);
			if(enemy->GetHp()>0&&!IsAir())
			{
			this->ApplyDamage(1, 1.0f);
			
				//吹き飛ばす
				DirectX::XMFLOAT3 impulse;
				impulse.x = position.x - enemy->GetPosition().x;
				impulse.y = 0.2f; // 少し浮かせる（好み）
				impulse.z = position.z - enemy->GetPosition().z;

				// 正規化
				float len = sqrtf(impulse.x * impulse.x + impulse.z * impulse.z);
				if (len > 0.001f)
				{
					impulse.x /= len;
					impulse.z /= len;
				}

				// 強さ
				const float knockbackPower = 8.0f;
				impulse.x *= knockbackPower;
				impulse.y *= knockbackPower;
				impulse.z *= knockbackPower;

				this->AddImpulse(impulse);

				Jump(jumpSpeed * 0.5f);
			}
		}
	}
}
void Player::CollisionPlayerVsItem()
{
	ItemManager& itemManager = ItemManager::Instance();

	int itemCount = itemManager.GetItemCount();

	DirectX::XMFLOAT3 outPos{};

	for (int i = 0; i < itemCount; ++i)
	{
		Item* item = itemManager.GetItem(i);
		

		if (Collision::IntersectCylinderVsCylinder(position,
			radius,
			height,
			item->GetPosition(),
			item->GetRadius(),
			2.0f,
			outPos,
			false))
		{
			if (slamGauge >= MaxGauge)
			{
				slamGauge = MaxGauge;
				break;
			}
			slamGauge++;
			item->Kill();

		}
	}

}
void Player::SlamPlayerVsEnemies()
{
	if (!isSlamming || !slamHitActive)
		return;

	EnemyManager& enemyManager = EnemyManager::Instance();

	int enemyCount = enemyManager.GetEnemyCount();

	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		if (!enemy) continue;
		if (enemy->GetHp() <= 0) continue;

		DirectX::XMFLOAT3 outPosition;

		// ① 下向き中のみ
		if (velocity.y > 0.0f)
			continue;

		if (Collision::IntersectCylinderVsCylinder(
			{ position.x,0.0f,position.z },
			GetSlamRadius(),
			GetHeight(),
			enemy->GetPosition(),
			enemy->GetRadius(),
			enemy->GetHeight(),
			outPosition,
			isSlamming))
		{
			// 💥 SLAM DAMAGE
			enemy->SlamCheck();
	        enemy->TakeDamage(1);
	
	
		}
	}
}

void Player::InputJump(float elapsedTime)
{
	if (slamRecovery)return;

	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_A)
	{
		if (jumpCount == jumpLimit)
		{
			Jump(jumpSpeed);
			jumpCount--;
		}
		else if(jumpCount<jumpLimit)
		{
			slamInputBuffer = slamBufferTime;
		}
		else if (gamePad.GetButtonDown()& GamePad::BTN_UP)
		{
			velocity.z += 10.0f;
		}
		
	}
}

void Player::OnLanding()
{
	jumpCount = 1;
	baseY = position.y;
	isGround = true;

	
	dashUsedInAir = false;
	isDashing = false;
	dashTimer = 0.0f;
	forwardTapCount = 0;
	forwardTapTimer = 0.0f;

	dashLeanAngle = 0.0f;
	dashLeanTarget = 0.0f;
	angle.z = 0.0f;

	if (isSlamming)
	{
		isSlamming = false;
		slamHitActive = false;
		slamRecovery = true;
	}
	slamAfterBuffer = slamAfterTimer;

	velocity = { 0.0f,0.0f,0.0f };

	if (slamRecovery)
	{
		Camera::Instance().AddShake(0.35f);
		scale.y *= (3.0f - slamSquashAmount);
		scale.x *= (1.5f + slamSquashAmount);
		scale.z *= (1.5f + slamSquashAmount);
	}

	hopTimer = 0.0f;
}

void Player::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	Character::RenderDebugPrimitive(rc, renderer);

	//projectileManager.RenderDebugPrimitive(rc, renderer);
}

void Player::ApplyHop(float elapsedTime)
{
	scale.x = Lerp(scale.x, 0.01f, elapsedTime * slamSquashSpeed);
	scale.y = Lerp(scale.y, 0.01f, elapsedTime * slamSquashSpeed);
	scale.z = Lerp(scale.z, 0.01f, elapsedTime * slamSquashSpeed);

	if (slamRecovery)
	{
		visualOffset = 0.0f;	
		return;
	}
	
	if (grounded&&isMoving)
	{
		hopTimer += elapsedTime * hopSpeed ;

		// sine wave hop
		visualOffset = fabsf(sinf(hopTimer)) * hopHeight;
		scale.y = max(0.005f, 0.01f - visualOffset * 0.02f);
		scale.x = 0.01f + visualOffset * 0.01f;
		scale.z = 0.01f + visualOffset * 0.01f;

		
	}
	else if (!grounded)
	{
		// do NOT reset hopTimerw
		// do NOT modify position.y (physics owns it)

		// softly relax scale while airborne
		scale.x = Lerp(scale.x, 0.01f, elapsedTime * 4.0f);
		scale.y = Lerp(scale.y, 0.01f, elapsedTime * 4.0f);
		scale.z = Lerp(scale.z, 0.01f, elapsedTime * 4.0f);
	}
	else
	{
		// reset hop when stopping
		//hopTimer = 0.0f;
		visualOffset = Lerp(visualOffset, baseY, elapsedTime * 8.0f);

		// ✨ smooth return to original shape
		scale.x = Lerp(scale.x, 0.01f, elapsedTime * 6.0f);
		scale.y = Lerp(scale.y, 0.01f, elapsedTime * 6.0f);
		scale.z = Lerp(scale.z, 0.01f, elapsedTime * 6.0f);
	}
}

void Player::StartSlam()
{
	if (isSlamming)
	{
		return;
	}
	if (slamGauge <= 0) return;

	slamGauge = 0;
	isSlamming = true;
	slamHitActive = true;
}

void Player::SlamRecovery(float elapsedTime)
{
	if (!slamRecovery)
	{
		return;
	}
	if (slamAfterBuffer <= 0.0f)
	{
		slamRecovery = false;
	}
}

void Player::StartDash()
{
	isDashing = true;
	dashUsedInAir = true;
	dashTimer = dashDuration;

	DirectX::XMFLOAT3 dir;

	if (fabsf(moveVec.x) > 0.01f || fabsf(moveVec.z) > 0.01f)
	{
		dir = moveVec; // dash where player is moving
	}
	else
	{
		dir.x = sinf(angle.y);
		dir.z = cosf(angle.y);
		dir.y = 0.0f;
	}

	// normalize
	float len = sqrtf(dir.x * dir.x + dir.z * dir.z);
	if (len > 0.0f)
	{
		dir.x /= len;
		dir.z /= len;
	}

	//INSTANT TURN TO DASH DIRECTION
	angle.y = atan2f(dir.x, dir.z);

	scale.x *= 1.2f;
	scale.z *= 1.2f;
	scale.y *= 0.8f;

	//tilt
	dashLeanTarget = dashLeanMax; 
	dashLeanHoldTimer = dashLeanHoldDuration;

	// HARD impulse
	velocity.x = dir.x * dashSpeed;
	velocity.z = dir.z * dashSpeed;
}

