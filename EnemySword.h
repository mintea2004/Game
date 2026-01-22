#pragma once
#include"System/Model.h"
#include"Enemy.h"

class EnemySword : public Enemy
{
public:

	EnemySword();
	~EnemySword() override;

	void Update(float elapsedTime)override;

	void Render(const RenderContext& rc, ModelRenderer* renderer) override;

private:

	Model* model = nullptr;
	float visualYOffset = 1.0f;


protected:

	void OnDead() override;
	void OnDamaged() override;
};