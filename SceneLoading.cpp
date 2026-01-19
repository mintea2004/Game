#include "SceneLoading.h"
#include"System/Graphics.h"
#include "System/Input.h"
#include "SceneManager.h"

void SceneLoading::Initialize()
{
	sprite = new Sprite("Data/Sprite/LoadingIcon.png");
	thread = new std::thread(LoadingThread, this);
	
}

void SceneLoading::Finalize()
{
	if (sprite != nullptr)
	{
		delete sprite;
		sprite = nullptr;
	}

	if (thread != nullptr)
	{
		if (thread->joinable())
			thread->join();

		delete thread;
		thread = nullptr;
	}
}

void SceneLoading::Update(float elapsedTime)
{
	constexpr float speed = 180;
	angle += speed * elapsedTime;

	if (IsReady())
		SceneManager::Instance().ChangeScene(nextScene);

}

void SceneLoading::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	RenderState* renderState = graphics.GetRenderState();
	RenderContext rc;

	rc.deviceContext = dc;
	rc.renderState = graphics.GetRenderState();
	//2d sprite
	{
		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());
		float spriteWidth = 256;
		float spriteHeight = 256;
		float positionX = screenWidth - spriteWidth;
		float positionY = screenHeight - spriteHeight;
		sprite->Render(rc,
			positionX, positionY, 0, spriteWidth, spriteHeight
			, angle,
			1, 1, 1, 1);
	}
}

void SceneLoading::DrawGUI()
{
}


void SceneLoading::LoadingThread(SceneLoading* scene)
{
	CoInitialize(nullptr);

	scene->nextScene->Initialize();
	scene->nextScene->SetReady();

	CoUninitialize();

	scene->SetReady();
}