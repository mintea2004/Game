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
    const float baseX = 0.01f;
    const float baseY = 0.01f;
    const float baseZ = 0.01f;

    // 見える潰れ（薄すぎない）
    const float targetY = baseY * 0.35f;  // 35%の高さ
    const float targetXZ = baseX * 1.35f;  // 135%に横広げ

    // 安全（0にしない）
    const float minS = baseX * 0.05f;      // 0.0005

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
        float t = elapsedTime * 8.0f;
        scale.y = Lerp(scale.y, minS, t);
        scale.x = Lerp(scale.x, minS, t);
        scale.z = Lerp(scale.z, minS, t);
       

    }
    else
    {
        Destroy();
    }

    // safety clamp
    if (scale.x < minS) scale.x = minS;
    if (scale.y < minS) scale.y = minS;
    if (scale.z < minS) scale.z = minS;
}
