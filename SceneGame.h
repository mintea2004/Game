#pragma once

#include "stage.h"
#include "Player.h"
#include "CameraController.h"
#include "Scene.h"
//#include "EnemySlime.h"

// ゲームシーン
class SceneGame : public Scene
{
public:
	SceneGame() {};
	~SceneGame() override{};


	// 初期化
	void Initialize();

	// 終了化
	void Finalize();

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Render();

	// GUI描画
	void DrawGUI();

private:
	Stage* stage = nullptr;
	Player* player = nullptr;
	//EnemySlime* slime = nullptr;
	//Character* enemy = nullptr;
	CameraController* cameraController = nullptr;
};
