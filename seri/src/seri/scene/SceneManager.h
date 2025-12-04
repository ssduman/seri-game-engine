#pragma once

#include "seri/util/Util.h"
#include "seri/scene/Scene.h"
#include "seri/component/Components.h"

#include <entt/entt.hpp>

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

			static SceneManager& GetInstance();

			static void Init();

			static void Update();

			static entt::entity CreateEntity();

			static void DestroyEntity(entt::entity e);

		private:
			entt::registry registry;

			std::shared_ptr<Scene> _activeScene;
			std::vector<std::shared_ptr<Scene>> _scenes;

		};
	}
}
