#include"EnemyManager.h"
#include"Collision.h"
#include"Player.h"

#include<imgui.h>


void EnemyManager::Update(float elapsedTime)
{

	for (Enemy* enemy : enemies)
	{
		enemy->Update(elapsedTime);
	}

	for (Enemy* enemy : removes)
	{
		std::vector<Enemy*>::iterator it = std::find(enemies.begin(), enemies.end(), enemy);
		if (it != enemies.end())
		{
			enemies.erase(it);
		}delete enemy;
	}
	removes.clear();

	CollisionEnemyVsEnemies();

}

void EnemyManager::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	for (Enemy* enemy : enemies)
	{
		enemy->Render(rc, renderer);
	}
}

void EnemyManager::Register(Enemy* enemy)
{
	enemies.emplace_back(enemy);
}

void EnemyManager::Clear()
{
	for (Enemy* enemy : enemies)
	{
		delete enemy;
		enemy = nullptr;
	}
	enemies.clear();
}

void EnemyManager::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	for (Enemy* enemy : enemies)
	{
		enemy->RenderDebugPrimitive(rc, renderer);
		//renderer->RenderSphere(rc, enemy->GetPosition(),enemy->GetRadius(), DirectX::XMFLOAT4(1, 1, 1, 1));
	}

}

void EnemyManager::CollisionEnemyVsEnemies()
{
	size_t enemyCount = enemies.size();

	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemyA = enemies.at(i);

		for (int j = i + 1; j < enemyCount; ++j)
		{
			Enemy* enemyB = enemies.at(j);
			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectCylinderVsCylinder(
				enemyA->GetPosition(),
				enemyA->GetRadius(),
				enemyA->GetHeight(),
				enemyB->GetPosition(),
				enemyB->GetRadius(),
				enemyB->GetHeight(),
				outPosition,
				false))
			{
				enemyB->SetPosition(outPosition);
			}
		}
	}
}


void EnemyManager::Remove(Enemy* enemy)
{
	removes.insert(enemy);
}


void EnemyManager::DrawDebugGUI()
{
	ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();
	ImGui::SetNextWindowPos(ImVec2(pos.x + 1000, pos.y + 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Enemies", nullptr, ImGuiWindowFlags_None))
	{
		int i = 0;
		for (Enemy* enemy : enemies)
		{
			std::string title = "Enemy" + std::to_string(i++);
			if (ImGui::TreeNode(title.c_str()))
			{
				if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
				{
					DirectX::XMFLOAT3 pos = enemy->GetPosition();
					ImGui::InputFloat3("Position", &pos.x);
					enemy->SetPosition(pos);
					DirectX::XMFLOAT3 a;
					a.x = DirectX::XMConvertToDegrees(enemy->GetAngle().x);
					a.y = DirectX::XMConvertToDegrees(enemy->GetAngle().y);
					a.z = DirectX::XMConvertToDegrees(enemy->GetAngle().z);
					ImGui::InputFloat3("Angle", &a.x);
					a.x = DirectX::XMConvertToRadians(a.x);
					a.y = DirectX::XMConvertToRadians(a.y);
					a.z = DirectX::XMConvertToRadians(a.z);
					enemy->SetAngle(a);
					DirectX::XMFLOAT3 scale = enemy->GetScale();
					ImGui::InputFloat3("Scale", &scale.x);
					enemy->SetScale(scale);
				}
				ImGui::TreePop();
			}
		}
	}
	ImGui::End();
}
