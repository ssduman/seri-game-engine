#pragma once

#include "seri/util/Util.h"
#include "seri/random/Random.h"
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
		struct GraphNode
		{
			seri::IDComponent idComponent{};
			std::vector<GraphNode> children{};
		};

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

			std::string GetName() const
			{
				return _idComponent.name;
			}

			GraphNode& GetSceneGraphRoot()
			{
				return _sceneGraphRoot;
			}

			entt::entity GetEntityById(uint64_t id)
			{
				if (_entityMap.contains(id))
				{
					return _entityMap[id];
				}
				LOGGER(error, fmt::format("[scene] entity with id {} not found", id));
				return {};
			}

		private:
			void FindAndAddAsChild(GraphNode& node, uint64_t parentId, seri::IDComponent childIdComponent);

			std::shared_ptr<seri::Skybox> _skybox;

			seri::IDComponent _idComponent;
			seri::SceneComponent _sceneComponent;

			std::unordered_map<uint64_t, entt::entity> _entityMap{};

			GraphNode _sceneGraphRoot{};

		};
	}
}
