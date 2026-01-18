#pragma once

#include "seri/util/Util.h"
#include "seri/scene/Scene.h"
#include "seri/component/Components.h"

#include <entt/entt.hpp>
#include <yaml-cpp/yaml.h>

#include <memory>
#include <vector>

namespace seri::scene
{
	struct ComponentIO
	{
		std::string_view name;

		std::function<bool(entt::registry&, entt::entity)> Has;
		std::function<void(entt::registry&, entt::entity)> Add;
		std::function<void(entt::registry&, entt::entity)> Remove;

		std::function<void(entt::registry&, entt::entity, YAML::Node&)> Serialize;
		std::function<void(entt::registry&, entt::entity, const YAML::Node&)> Deserialize;
	};

	class SceneManager
	{
	public:
		SceneManager() = default;
		SceneManager(SceneManager const&) = delete;
		void operator=(SceneManager const&) = delete;

		static SceneManager& GetInstance();

		static void Init();
		static void Update();

		static entt::registry& GetRegistry();
		static entt::entity CreateEntity();
		static void DestroyEntity(entt::entity entity);

		static std::shared_ptr<Scene> GetActiveScene();
		static const std::vector<seri::scene::ComponentIO>& GetCompIO();

		static bool HasComponent(entt::entity entity, std::string_view compName);
		static void AddComponent(entt::entity entity, std::string_view compName);
		static void RemoveComponent(entt::entity entity, std::string_view compName);
		static void SerializeComponent(entt::entity entity, YAML::Node& node, std::string_view compName);
		static void DeserializeComponent(entt::entity entity, const YAML::Node& node, std::string_view compName);

	private:
		template<typename T>
		static seri::scene::ComponentIO MakeComponentIO()
		{
			return {
				.name = T::compName,
				
				.Has = [](entt::registry& registry, entt::entity entity)
				{
					return registry.any_of<T>(entity);
				},
				
				.Add = [](entt::registry& registry, entt::entity entity)
				{
					registry.emplace_or_replace<T>(entity);
				},

				.Remove = [](entt::registry& registry, entt::entity entity)
				{
					if (registry.any_of<T>(entity))
					{
						registry.remove<T>(entity);
					}
				},

				.Serialize = [](entt::registry& registry, entt::entity entity, YAML::Node& node)
				{
					if (auto* comp = registry.try_get<T>(entity))
					{
						node[std::string(T::compName)] = T::Serialize(*comp);
					}
				},

				.Deserialize = [](entt::registry& registry, entt::entity entity, const YAML::Node& node)
				{
					registry.emplace_or_replace<T>(entity, T::Deserialize(node));
				}
			};
		}

		template<typename T>
		static void RegisterComponent()
		{
			ComponentIO io = MakeComponentIO<T>();
			GetInstance()._componentIOs.emplace_back(io);
			GetInstance()._componentIOmaps.emplace(io.name, io);
		}

		entt::registry registry;

		std::shared_ptr<Scene> _activeScene;
		std::vector<std::shared_ptr<Scene>> _scenes;

		std::vector<seri::scene::ComponentIO> _componentIOs{};
		std::unordered_map<std::string_view, ComponentIO> _componentIOmaps{};

	};
}
