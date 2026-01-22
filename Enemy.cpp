#include"Enemy.h"
#include"EnemyManager.h"
#include"Player.h"


void Enemy::ChasePlayer(float elapsedTime)
{
    Player* player = EnemyManager::Instance().GetPlayer();
    if (!player) return;

    // Don’t chase if dead / being slammed
    if (dying) return;

    DirectX::XMFLOAT3 p = player->GetPosition();

    float dx = p.x - position.x;
    float dz = p.z - position.z;

    float distSq = dx * dx + dz * dz;

    // stop when close enough
    const float stopDist = 1.0f;      // tune
    if (distSq < stopDist * stopDist)
    {
        isMoving = false;
        return;
    }
    isMoving = true;
    float dist = sqrtf(distSq);
    if (dist > 0.0001f)
    {
        dx /= dist;
        dz /= dist;
    }

    const float speed = 3.8f;         // tune (world units / sec)
    position.x += dx * speed * elapsedTime;
    position.z += dz * speed * elapsedTime;

    // Face the player (optional)
    angle.y = atan2f(dx, dz);
}

void Enemy::Dash(float elapsedTime)
{
    Player* player = EnemyManager::Instance().GetPlayer();
    if (!player) return;

    if (dying) return;


    float dx = player->GetPosition().x - position.x;
    float dz = player->GetPosition().z - position.z;
    float dy = player->GetPosition().y - position.y;
   

    float distSq = dx * dx + dz * dz+ dy*dy;

    dashTimer -= elapsedTime;

    switch (dashState)
    {
    case DashState::Cooldown:
        // wait for cooldown, and only start if close enough
        if (dashTimer <= 0.0f && distSq <= dashRange * dashRange)
        {
            dashState = DashState::Windup;
            dashTimer = windupTime;

            // lock target NOW (ram at where player was)
            dashdirect = player->GetPosition();
            targetLocked = true;

            // face target for telegraph
            float dx1 = dashdirect.x - position.x;
            float dz1 = dashdirect.z - position.z;
           
            angle.y = atan2f(dx1, dz1);
        }
        break;

    case DashState::Windup:
        // keep facing the locked target (optional)
    {
        float dx1 = dashdirect.x - position.x;
        float dz1 = dashdirect.z - position.z;
        
       
        angle.y = atan2f(dx1, dz1);
    }

    if (dashTimer <= 0.0f)
    {
        dashState = DashState::Dashing;
        dashTimer = dashTime;

        // compute dash direction once
        float dx1 = dashdirect.x - position.x;
        float dz1 = dashdirect.z - position.z;
        float dy1 = dashdirect.y - position.y;
        float len = sqrtf(dx1 * dx1 + dz1 * dz1 + dy1*dy1);
        if (len > 0.001f) { dx1 /= len; dz1 /= len; dy1 /= len; }
        else { dx1 = 0.0f; dz1 = 1.0f; dy1 = 0.0f; }

        const float maxUp = 0.35f;      // tune (0.0 = no vertical dash, 1.0 = full)
        if (dy1 > maxUp) dy1 = maxUp;
        if (dy1 < -maxUp) dy1 = -maxUp;

         len = sqrtf(dx1 * dx1 + dz1 * dz1 + dy1 * dy1);
        if (len > 0.001f) { dx1 /= len; dz1 /= len; dy1 /= len; }

        dashdirect = { dx1, dy1, dz1 };
        angle.y = atan2f(dashdirect.x, dashdirect.z);

        fallspeed = 0.0f;
    }
    break;

    case DashState::Dashing:
        // move straight (ram)
        position.x += dashdirect.x * dashSpeed * elapsedTime;
        position.z += dashdirect.z * dashSpeed * elapsedTime;
        position.y += dashdirect.y * dashSpeed * elapsedTime;

        // keep facing dash direction
        angle.y = atan2f(dashdirect.x, dashdirect.z);


       

        if (dashTimer <= 0.0f)
        {
            dashState = DashState::Recover;
            dashTimer = recoverTime;
            fallspeed = 0.0f;
        }
        break;

    case DashState::Recover:

        //fall
        float g = fabsf(gravity);   // always positive
        fallspeed -= g * elapsedTime; // makes fallspeed negative => falling down
        position.y += fallspeed * elapsedTime;
       

          // clamp to ground
          if (position.y <= 0.0f)
          {
              position.y = 0.0f;
              fallspeed = 0.0f;
              //angle.y = atan2f(dashdirect.x, dashdirect.z)+DirectX::XMConvertToRadians(45);
              
              angle.y = Lerp(angle.y, atan2f(dashdirect.x, dashdirect.z) + DirectX::XMConvertToRadians(45),6.0f*elapsedTime);
              // once on ground, finish recover timer
              //dashTimer -= elapsedTime;
              if (dashTimer <= 0.0f)
              {
                  dashState = DashState::Cooldown;
                  dashTimer = dashCooldown;
                  targetLocked = false;
              }
          }
         
        break;
    }
    

}

void Enemy::Impulse(float elapsedTime)
{
    if (!dying) return;

    if (deathImpulse) return;
    deathImpulse = true;

    Player* player = EnemyManager::Instance().GetPlayer();
    if (!player) return;

        //吹き飛ばす
        DirectX::XMFLOAT3 impulse;
        impulse.x = position.x - player->GetPosition().x;
        impulse.y = 0.2f; // 少し浮かせる（好み）
        impulse.z = position.z - player->GetPosition().z;

        // 正規化
        float len = sqrtf(impulse.x * impulse.x + impulse.z * impulse.z);
        if (len > 0.001f)
        {
            impulse.x /= len;
            impulse.z /= len;
        }
        else { impulse.x = 1.0f; impulse.z = 0.0f; }


        // 強さ
        const float knockbackPower = 8.0f;
        impulse.x *= knockbackPower;
        impulse.y *= knockbackPower;
        impulse.z *= knockbackPower;

        this->AddImpulse(impulse);

        Jump(30 * 0.5f);
}

void Enemy::Destroy()
{
	EnemyManager::Instance().Remove(this);
}

void Enemy::TakeDamage(int damage)
{
	if (hp <= 0) return; // prevent double trigger

	hp -= damage;

	if (hp<=0)
	{
		hp = 0;
		deathTimer = 0.0f;
		dying = true;
	
	}
}

void Enemy::gettingSlammed(float elapsedTime)
{
    if (!dying) return;

    deathTimer += elapsedTime;

    // 元の大きさを基準にする（0.01でもOK）
    // ※最初に一回だけ保存したいなら baseScale を持つのが理想だけど、
    // まずは簡単版でいく
    const float baseX =bs.x;
    const float baseY =bs.y;
    const float baseZ =bs.z;

    // 見える潰れ（薄すぎない）
    const float targetY = baseY * 0.35f;  // 35%の高さ
    const float targetXZ = baseX * 1.35f;  // 135%に横広げ

    // 安全（0にしない）

    // 🔥 ここが長さ調整ポイント
    const float squashTime = 0.15f;  // 潰れるまでの時間
    const float holdTime = 2.00f;  // ← 潰れたまま保持する時間（好きに増やす）
    const float popTime = 0.30f;  // 最後に少し小さくする時間（任意）

    // Phase 1: squash
    if (deathTimer < squashTime)
    {
        float t = elapsedTime * 20.0f;
        scale.y = Lerp(scale.y, targetY, t);
        scale.x = Lerp(scale.x, targetXZ, t);
        scale.z = Lerp(scale.z, targetXZ, t);
        

    }
    // Phase 2: HOLD（ここが長いほど「長く潰れてる」）
    else if (deathTimer < squashTime + holdTime)
    {
        scale.y = targetY;
        scale.x = targetXZ;
        scale.z = targetXZ;
      

    }
    // Phase 3: optional small shrink (NOT to zero!)
    else if (deathTimer < squashTime + holdTime + popTime)
    {
        Shrink(elapsedTime);
    }
    else
    {
        Destroy();
    }

}

void Enemy::Shrink(float elapsedTime)
{
    const float minS = bs.x * 0.05f;

    if (deathTimer > 2.0f)
    {
        float t = elapsedTime * 8.0f;
        scale.y = Lerp(scale.y, minS, t);
        scale.x = Lerp(scale.x, minS, t);
        scale.z = Lerp(scale.z, minS, t);
    }
    if (scale.x < minS) scale.x = minS;
    if (scale.y < minS) scale.y = minS;
    if (scale.z < minS) scale.z = minS;
}