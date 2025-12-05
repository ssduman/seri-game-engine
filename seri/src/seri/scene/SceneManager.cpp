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
		SceneManager::GetInstance()._activeScene->Serialize("assets/scenes/main_0.yaml");
	}

	void SceneManager::Update()
	{
		SceneManager::GetInstance()._activeScene->Update();
	}

	entt::registry& SceneManager::GetRegistry()
	{
		return SceneManager::GetInstance().registry;
	}

	entt::entity SceneManager::CreateEntity()
	{
		return GetRegistry().create();
	}

	void SceneManager::DestroyEntity(entt::entity e)
	{
		GetRegistry().destroy(e);
	}

	std::shared_ptr<Scene> SceneManager::GetActiveScene()
	{
		return GetInstance()._activeScene;
	}

}
