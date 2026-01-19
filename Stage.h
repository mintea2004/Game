#pragma once
#include"System/ModelRenderer.h"

class Stage
{
private:

	Model* model = nullptr;

public:

	Stage();
	~Stage();

	void Update(float elapsedTime);
	void Render(const RenderContext& rc, ModelRenderer* renderer);
};