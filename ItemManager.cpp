#include "ItemManager.h"
#include "Item.h"
#include <algorithm>

ItemManager& ItemManager::Instance()
{
    static ItemManager instance;
    return instance;
}

void ItemManager::Spawn(const DirectX::XMFLOAT3& pos)
{
    items.push_back(std::make_unique<Item>(pos));
}

void ItemManager::Update(float elapsedTime)
{
    for (auto& it : items) it->Update(elapsedTime);

    // remove dead
    items.erase(
        std::remove_if(items.begin(), items.end(),
            [](const std::unique_ptr<Item>& item)
            {
                return item->IsDead();
            }),
        items.end());

}

void ItemManager::Render(const RenderContext& rc, ModelRenderer* renderer)
{
    for (auto& item : items)
    {
        item->Render(rc, renderer);
    }
}

void ItemManager::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
    for (auto& item : items)
    {
        item->RenderDebugPrimitive(rc, renderer);
        //renderer->RenderSphere(rc, enemy->GetPosition(),enemy->GetRadius(), DirectX::XMFLOAT4(1, 1, 1, 1));
    }
}

void ItemManager::Clear()
{
    items.clear();
}