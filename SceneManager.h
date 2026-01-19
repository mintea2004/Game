#pragma once

#include "Scene.h"

class SceneManager
{
private:
	SceneManager() {}
	~SceneManager() {}
public:

	static SceneManager& Instance()
	{
		static SceneManager instance;
		return instance;
	}

	void Update(float elapsedTime);

	void Render();

	void DrawGUI();

	void ChangeScene(Scene* newScene);

	void Clear();

private:

	Scene* currentScene = nullptr;
	Scene* nextScene = nullptr;
};