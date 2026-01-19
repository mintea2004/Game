#pragma once

#pragma once

#include<vector>
#include"Enemy.h"
#include<set>

class Player;

class EnemyManager
{
private:

	EnemyManager()
	{

	}

	~EnemyManager()
	{

	}

public:

	static EnemyManager& Instance()
	{
		static EnemyManager instance;
		return instance;
	}

	void Update(float elapsedTime);

	void Render(const RenderContext& rc, ModelRenderer* renderer);

	void Register(Enemy* enemy);

	void Clear();

	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer);

	int GetEnemyCount() const { return static_cast<int>(enemies.size()); }

	Enemy* GetEnemy(int index) { return enemies.at(index); }

	void Remove(Enemy* enemy);

	void EnemyManager::DrawDebugGUI();

	//void CollisionEnemyVsPlayerSlam(Player* player);

	//playerî•ñ“n‚·
	void SetPlayer(Player* p) { player = p; }
	Player* GetPlayer() const { return player; }

private:

	void CollisionEnemyVsEnemies();

	Player* player = nullptr;

	std::vector<Enemy*> enemies;

	std::set<Enemy*> removes;
};