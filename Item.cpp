#include"Item.h"

Item::Item(const DirectX::XMFLOAT3& pos)
{
    model = new Model("Data/Model/Slime/Slime.mdl");

    position = pos;

    radius = 0.5f;

    baseY = position.y;

    angle.z += DirectX::XMConvertToRadians(45);
}

Item::~Item()
{
    delete model;
}

void Item::Update(float elapsedTime)
{
    floatTimer += elapsedTime * floatSpeed;
    visualOffsetY = sinf(floatTimer) * floatHeight;

    // simple example: spin
    angle.y += elapsedTime;
    UpdateTransform();
    
}

void Item::Render(const RenderContext& rc, ModelRenderer* renderer)
{
    renderer->Render(rc, transform, model, ShaderId::Lambert, { 1.0,10.0,1.0,1 });
}

void Item::UpdateTransform()
{
    using namespace DirectX;

    float renderY = baseY + visualOffsetY;
  
    XMMATRIX S = XMMatrixScaling(scale.x, scale.y, scale.z);
    XMMATRIX R = XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    XMMATRIX T = XMMatrixTranslation(position.x, renderY, position.z);

    XMStoreFloat4x4(&transform, S * R * T);
}