#pragma once
#include <vector>
#include <memory>
#include <DirectXMath.h>
#include "Item.h"
#include <set>



class ItemManager 
{
public:
    static ItemManager& Instance();

    void Update(float elapsedTime);
    void Render(const RenderContext& rc, ModelRenderer* renderer);

    void Spawn(const DirectX::XMFLOAT3& pos);
    void Clear();

    std::vector<std::unique_ptr<Item>>& GetItems() { return items; }

    void ItemManager::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer);

    int GetItemCount() const { return static_cast<int>(items.size()); }
    Item* GetItem(int index) { return items.at(index).get(); }

private:
    std::vector<std::unique_ptr<Item>> items;

    std::set<std::unique_ptr<Item>> removes;
};
