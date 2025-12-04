#include "Seripch.h"

#include "seri/scene/SceneManager.h"
#include "seri/scene/Scene.h"

#include <entt/entt.hpp>

#include <memory>
#include <vector>

namespace seri::scene
{
	SceneManager& SceneManager::GetInstance()
	{
		static SceneManager instance;
		return instance;
	}

	void SceneManager::Init()
	{
		SceneManager::GetInstance()._activeScene = std::make_shared<Scene>();
		SceneManager::GetInstance()._activeScene->Init();

		SceneManager::GetInstance()._activeScene->Deserialize("assets/scenes/main.yaml");
	}

	void SceneManager::Update()
	{
		SceneManager::GetInstance()._activeScene->Update();
	}

	entt::entity SceneManager::CreateEntity()
	{
		return SceneManager::GetInstance().registry.create();
	}

	void SceneManager::DestroyEntity(entt::entity e)
	{
		GetInstance().registry.destroy(e);
	}
}
