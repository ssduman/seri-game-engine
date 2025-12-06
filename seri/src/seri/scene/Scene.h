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
			uint64_t id{ 0 };
			uint64_t parentId{ 0 };
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
				return entt::null;
			}

			bool IsDirty()
			{
				return _isDirty;
			}

			void SetAsDirty()
			{
				_isDirty = true;
			}

			void Save();

			void Serialize(const std::string& file);
			void Deserialize(const std::string& file);

			void GetAllEntityIDs(std::vector<uint64_t>& ids);

			void DeleteEntity(uint64_t id);
			void AddEntityAsChild(uint64_t id, uint64_t parentId, const std::string& name);

		private:
			void GetAllEntityIDs(GraphNode& node, std::vector<uint64_t>& ids);

			void DeleteEntity(GraphNode& node, uint64_t id);
			void AddEntityAsChild(GraphNode& node, uint64_t id, uint64_t parentId, const std::string& name);

			std::string _filePath{ "" };
			bool _isDirty{ false };

			std::shared_ptr<seri::Skybox> _skybox;

			seri::IDComponent _idComponent;
			seri::SceneComponent _sceneComponent;

			std::unordered_map<uint64_t, entt::entity> _entityMap{};

			GraphNode _sceneGraphRoot{};

		};
	}
}
