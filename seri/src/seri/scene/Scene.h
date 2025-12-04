#pragma once

#include "seri/util/Util.h"
#include "seri/texture/Skybox.h"
#include "seri/component/Components.h"

#include <entt/entt.hpp>

#include <string>
#include <memory>
#include <unordered_map>

namespace seri
{
	namespace scene
	{
		class Scene
		{
		public:
			class SceneManager;

			Scene() = default;

			~Scene() = default;

			void Init()
			{
				_skybox = std::make_shared<seri::Skybox>();
			}

			void Update()
			{
				_skybox->Update();
			}

			void Deserialize(const std::string& file);

		private:
			std::shared_ptr<seri::Skybox> _skybox;

			seri::IDComponent _idComponent;
			seri::SceneComponent _sceneComponent;

			std::unordered_map<uint32_t, entt::entity> _entityMap{};

		};
	}
}
