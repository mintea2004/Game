#include"EnemySlime.h"

EnemySlime::EnemySlime()
{
	model = new Model("Data/Model/Slime/Slime.mdl");

	scale.x = scale.y = scale.z = 0.01f;

	radius = 0.5f;

	height = 1.0f;
}

EnemySlime::~EnemySlime()
{
	delete model;
}

void EnemySlime::Update(float elapsedTime)
{
	ChasePlayer(elapsedTime);

	if (dying)
	{
		gettingSlammed(elapsedTime);
		
		
		UpdateTransform();
		//transform._22 *= squashY;
		model->UpdateTransform();
		return; 
	}

	ApplyHop(elapsedTime);
	UpdateTransform();
	UpdateVelocity(elapsedTime);
	UpdateInvincibleTimer(elapsedTime);
	model->UpdateTransform();
}

void EnemySlime::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, transform, model, ShaderId::Lambert, {0.0,0.0,10.0,1});
}

void EnemySlime::OnDead()
{
	//gettingSlammed();
	//Destroy();
}

void EnemySlime::OnDamaged()
{

}

void EnemySlime::ApplyHop(float elapsedTime)
{

	if (isMoving)
	{
		hopTimer += elapsedTime * hopSpeed;

		// sine wave hop
		visualOffset = fabsf(sinf(hopTimer)) * hopHeight;
		scale.y = max(0.005f, 0.01f - visualOffset * 0.02f);
		scale.x = 0.01f + visualOffset * 0.01f;
		scale.z = 0.01f + visualOffset * 0.01f;
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