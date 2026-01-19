#pragma once

#include"System/Model.h"
#include"Enemy.h"

class EnemySlime :public Enemy
{
public:

	EnemySlime();
	~EnemySlime() override;

	void Update(float elapsedTime) override;

	void Render(const RenderContext& rc, ModelRenderer* renderer) override;

	void ApplyHop(float elapsedTime);

private:

	Model* model = nullptr;


	float baseY = 0.0f;
	float hopTimer = 0.0f;
	float hopHeight = 0.5f;     // how high it hops
	float hopSpeed = 6.0f;    // how fast it hops

protected:

	void OnDead() override;
	void OnDamaged() override;
};