#pragma once

#include<DirectXMath.h>
#include "System/ShapeRenderer.h"
#include <System/ModelRenderer.h>

class Item
{
public:

	Item(const DirectX::XMFLOAT3& pos);
	~Item();

	void Update(float elapsedTime);
	void Render(const RenderContext& rc, ModelRenderer* renderer);

	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
	{
		renderer->RenderCylinder(rc, { position.x,position.y,position.z }, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
	}

	const DirectX::XMFLOAT3& GetPosition() const { return position; }
	float GetRadius() const { return radius; }

	bool IsDead() const { return dead; }
	void Kill() { dead = true; }

private:
	
	DirectX::XMFLOAT3 position{};
	DirectX::XMFLOAT3 scale{ 0.006f, 0.006f, 0.006f };
	DirectX::XMFLOAT3 angle{};
	float radius = 0.5f;

	//floating
	float baseY = 0.0f;        // set this once when spawned
	float visualOffsetY = 0.0f;
	float floatTimer = 0.0f;

	float floatHeight = 0.25f; 
	float floatSpeed = 3.0f;  

	float height = 2.0f;

	bool dead = false;

	Model* model = nullptr;
	DirectX::XMFLOAT4X4 transform{};

	void UpdateTransform();
};