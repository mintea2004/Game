#include"EnemySword.h"

EnemySword::EnemySword()
{
	model = new Model("Data/Model/Sword/Sword.mdl");

	scale.x = scale.y = scale.z = 3.0f;

	bs = scale;

	hp = 1;
}

EnemySword::~EnemySword()
{
	delete model;
}

void EnemySword::Update(float elapsedTime)
{
	
	if (dashState != DashState::Cooldown)
	{
		Dash(elapsedTime);
		UpdateTransform();
		model->UpdateTransform();
		return;
	}

	ChasePlayer(elapsedTime);
	Dash(elapsedTime);

	if (dying)
	{
		deathTimer += elapsedTime;
		Impulse(elapsedTime);
		
		UpdateVelocity(elapsedTime);
		
		if (velocity.y <= 0.0f)
		{
			Shrink(elapsedTime);
		}
	
		UpdateTransform();
		model->UpdateTransform();
		
		if (deathTimer > 2.5f) Destroy();
		return;
	}

	UpdateVelocity(elapsedTime);   // if you want normal gravity for sword when alive
	UpdateTransform();

	
	UpdateInvincibleTimer(elapsedTime);
	model->UpdateTransform();
}

void EnemySword::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	DirectX::XMFLOAT4X4 t = transform; // copy
	t._42 += visualYOffset;               // constant visual lift
	renderer->Render(rc, t, model, ShaderId::Lambert, { 1.0,1.0,1.0,1 });
}

void EnemySword::OnDead()
{
  
}

void EnemySword::OnDamaged()
{
}
