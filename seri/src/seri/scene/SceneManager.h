#pragma once

#include "seri/core/Seri.h"
#include "seri/util/Util.h"
#include "seri/scene/Scene.h"
#include "seri/texture/Skybox.h"
#include "seri/component/Components.h"

#include <entt/entt.hpp>
#include <yaml-cpp/yaml.h>

#include <memory>
#include <vector>

namespace seri
{
	namespace scene
	{
		class SceneManager
		{
		public:
			SceneManager(SceneManager const&) = delete;

			void operator=(SceneManager const&) = delete;

			SceneManager() = default;

			static SceneManager& GetInstance()
			{
				static SceneManager instance;
				return instance;
			}

			static void Init()
			{
				GetInstance()._activeScene = std::make_shared<seri::scene::Scene>();
				GetInstance()._activeScene->Init();

				GetInstance()._activeScene->Deserialize("assets/scenes/main.yaml");
			}

			static void Update()
			{
				GetInstance()._activeScene->Update();
			}

			static void CreateEntity()
			{
				entt::entity e = GetInstance().registry.create();
			}

			static void DestroyEntity(entt::entity e)
			{
				GetInstance().registry.destroy(e);
			}

		private:
			entt::registry registry;

			std::shared_ptr<seri::scene::Scene> _activeScene;
			std::vector<std::shared_ptr<seri::scene::Scene>> _scenes;

		};
	}
}
