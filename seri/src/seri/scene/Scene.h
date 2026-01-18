#pragma once

#include "seri/util/Util.h"
#include "seri/random/Random.h"
#include "seri/texture/Skybox.h"
#include "seri/texture/InfiniteGrid.h"
#include "seri/component/Components.h"

#include <entt/entt.hpp>

#include <string>
#include <memory>
#include <unordered_map>

namespace seri::scene
{
	struct SceneTreeNode
	{
		uint64_t id{ 0 };
		uint64_t parentId{ 0 };
		std::vector<SceneTreeNode> children{};
	};

	class Scene
	{
	public:
		class SceneManager;

		Scene() = default;

		~Scene() = default;

		void Init();

		void Update();

		seri::component::IDComponent GetIDComponent()
		{
			return _idComponent;
		}

		seri::component::SceneComponent GetSceneComponent()
		{
			return _sceneComponent;
		}

		uint64_t GetID()
		{
			return GetIDComponent().id;
		}

		std::string GetName()
		{
			return GetIDComponent().name;
		}

		SceneTreeNode& GetSceneTreeRoot()
		{
			return _sceneTreeRoot;
		}

		entt::entity GetEntityByID(uint64_t id)
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
		void GetAllEntityIDs(SceneTreeNode& node, std::vector<uint64_t>& ids);

		void DeleteEntity(SceneTreeNode& node, uint64_t id);
		void AddEntityAsChild(SceneTreeNode& node, uint64_t id, uint64_t parentId, const std::string& name);

		std::string _filePath{ "" };
		bool _isDirty{ false };

		std::shared_ptr<seri::Skybox> _skybox;
		std::shared_ptr<seri::InfiniteGrid> _infiniteGrid;

		seri::component::IDComponent _idComponent;
		seri::component::SceneComponent _sceneComponent;

		std::unordered_map<uint64_t, entt::entity> _entityMap{};

		SceneTreeNode _sceneTreeRoot{};

	};
}
