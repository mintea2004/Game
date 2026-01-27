#include "System/Graphics.h"
#include "SceneGame.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "EnemySlime.h"
#include "EnemySword.h"
#include "Item.h"
#include"ItemManager.h"
#include <cstdlib>
#include <ctime>


// 初期化
void SceneGame::Initialize()
{
	stage = new Stage();

	player = new Player();

	EnemyManager& enemyManager = EnemyManager::Instance();
	//player情報渡す
	enemyManager.SetPlayer(player);
	

	/*for (int i = 0; i < 2; i++)
	{
		EnemySlime* slime = new EnemySlime();
		slime->SetPosition(DirectX::XMFLOAT3(i * 2.0f, 0, 5));
		enemyManager.Register(slime);
	}*/

	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	cameraController = new CameraController();

	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 10, -10),
		DirectX::XMFLOAT3(0, 0, 0),
		DirectX::XMFLOAT3(0, 1, 0)
		/*camera.SetLookAt(
			DirectX::XMFLOAT3(0, player->position.y, -10),
			DirectX::XMFLOAT3(0, 0, 0),
			DirectX::XMFLOAT3(0, 0, 0.5f)*/
	);
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),
		graphics.GetScreenWidth() / graphics.GetScreenHeight(),
		0.1f,
		1000.0f
	);
	srand((unsigned)time(nullptr));


}

// 終了化
void SceneGame::Finalize()
{
	if (stage != nullptr)
	{
		delete stage;
		stage = nullptr;
		delete player;
		player = nullptr;
		/*delete slime;
		slime = nullptr;*/
		EnemyManager::Instance().Clear();
		ItemManager::Instance().Clear();
	}

	if (cameraController != nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
	}
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
	if (!player) return; // or Initialize() fallback

	stage->Update(elapsedTime);
	player->Update(elapsedTime);
	
	
	int j = rand() % 2;
	static float EneSpawnTimer = 0.0f;
	EneSpawnTimer -= elapsedTime;
	if(EneSpawnTimer<=0.0f&& rand() % 2 == 0&&EnemyManager::Instance().GetEnemyCount()<10)
	{
		EneSpawnTimer = 5.0f;

		auto RandRangeEne = [](float a, float b)
			{
				float t = (float)rand() / (float)RAND_MAX;
				return a + (b - a) * t;
			};

		float x = RandRangeEne(-10.0f, 10.0f);
		float z = RandRangeEne(player->GetPosition().z - 20.0f, player->GetPosition().z + 20.0f); // in front of camera
		
		if (j) { 
			EnemySlime* slime = new EnemySlime(); 
			slime->SetPosition(DirectX::XMFLOAT3(x, 0, z));

			EnemyManager::Instance().Register(slime);
		}
		else { 
			EnemySword* sword = new EnemySword(); 
			sword->SetPosition(DirectX::XMFLOAT3(x, 0, z));

			EnemyManager::Instance().Register(sword);
		}
		
	//	
	//	
	//}
	//if(EneSpawnTimer<=0.0f&& rand() % 2 == 0&&EnemyManager::Instance().GetEnemyCount()<1)
	//{
	//	EneSpawnTimer = 5.0f;

	//	auto RandRangeEne = [](float a, float b)
	//		{
	//			float t = (float)rand() / (float)RAND_MAX;
	//			return a + (b - a) * t;
	//		};

	//	float x = RandRangeEne(-10.0f, 10.0f);
	//	float z = RandRangeEne(player->GetPosition().z - 20.0f, player->GetPosition().z + 20.0f); // in front of camera
	//	
	//	if (j) { 
	//		EnemySlime* slime = new EnemySlime(); 
	//		slime->SetPosition(DirectX::XMFLOAT3(x, 0, z));

	//		EnemyManager::Instance().Register(slime);
	//	}
	//	else { 
	//		EnemySword* sword = new EnemySword(); 
	//		sword->SetPosition(DirectX::XMFLOAT3(x, 0, z));

	//		EnemyManager::Instance().Register(sword);
	//	}
	//	
		
		
	}

	EnemyManager::Instance().Update(elapsedTime);

	
	DirectX::XMFLOAT3 target = player->GetPosition();
	target.y += 0.5f;
	cameraController->SetTarget(target);
	cameraController->Update(elapsedTime);
	Camera::Instance().UpdateShake(elapsedTime);

	/*static float spawnTimer = 0.0f;
	spawnTimer -= elapsedTime;
	if (spawnTimer <= 0.0f)
	{
		spawnTimer = 2.0f;
		ItemManager::Instance().Spawn({ 0,0,10 });
	}*/
	// ✅ spawn every 2 seconds at random x/z
	static float spawnTimer = 0.0f;
	spawnTimer -= elapsedTime;
	if (spawnTimer <= 0.0f)
	{
		spawnTimer = 10.0f;

		auto RandRange = [](float a, float b)
			{
				float t = (float)rand() / (float)RAND_MAX;
				return a + (b - a) * t;
			};

		float x = RandRange(player->GetPosition().x - 10.0f, player->GetPosition().x + 10.0f);
		float z = RandRange(player->GetPosition().z - 20.0f, player->GetPosition().z + 20.0f); 
		float y;
		switch (rand() % 2)
		{
		case 0:
			y = 0.5f;
			break;
		case 1:
			y = 3.0f;
			break;
		}
		if (z > 40.0f)  z = 40.0f;
		if (z < -40.0f) z = -40.0f;
		if (x > 12.0f)	x = 12.0f;
		if (x < -12.0f) x = -12.0f;
		ItemManager::Instance().Spawn({ x, y, z });
	}

	// ✅ update items
	ItemManager::Instance().Update(elapsedTime);
}

// 描画処理
void SceneGame::Render()
{
	if (!stage || !player) return;

	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ShapeRenderer* shapeRenderer = graphics.GetShapeRenderer();
	ModelRenderer* modelRenderer = graphics.GetModelRenderer();

	// 描画準備
	RenderContext rc;

	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();
	rc.deviceContext = dc;
	rc.lightDirection = { 0.0f, -1.0f, 0.0f };	// ライト方向（下方向）
	rc.renderState = graphics.GetRenderState();

	// ビュー行列
	//{
	//	DirectX::XMFLOAT3 eye = { 0, 10, -10 };	// カメラの視点（位置）
	//	DirectX::XMFLOAT3 focus = { 0, 0, 0 };	// カメラの注視点（ターゲット）
	//	DirectX::XMFLOAT3 up = { 0, 1, 0 };		// カメラの上方向

	//	DirectX::XMVECTOR Eye	= DirectX::XMLoadFloat3(&eye);
	//	DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
	//	DirectX::XMVECTOR Up	= DirectX::XMLoadFloat3(&up);
	//	DirectX::XMMATRIX View	= DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
	//	DirectX::XMStoreFloat4x4(&rc.view, View);
	//}
	//// プロジェクション行列
	//{
	//	float fovY = DirectX::XMConvertToRadians(45);	// 視野角
	//	float aspectRatio = graphics.GetScreenWidth() / graphics.GetScreenHeight();	// 画面縦横比率
	//	float nearZ = 0.1f;	// カメラが映し出すの最近距離
	//	float farZ = 1000.0f;	// カメラが映し出すの最遠距離
	//	DirectX::XMMATRIX Projection = DirectX::XMMatrixPerspectiveFovLH(fovY, aspectRatio, nearZ, farZ);
	//	DirectX::XMStoreFloat4x4(&rc.projection, Projection);
	//}

	// 3Dモデル描画
	{
		stage->Render(rc, modelRenderer);
		player->Render(rc, modelRenderer);
		EnemyManager::Instance().Render(rc, modelRenderer);
		ItemManager::Instance().Render(rc, modelRenderer);
	}

	// 3Dデバッグ描画
	{
		player->RenderDebugPrimitive(rc, shapeRenderer);
		EnemyManager::Instance().RenderDebugPrimitive(rc, shapeRenderer);
		ItemManager::Instance().RenderDebugPrimitive(rc, shapeRenderer);
	}
	

	// 2Dスプライト描画
	{

	}
}

// GUI描画
void SceneGame::DrawGUI()
{
	if (player) {
		player->DrawDebugGUI();
		EnemyManager::Instance().DrawDebugGUI();
	}
}
