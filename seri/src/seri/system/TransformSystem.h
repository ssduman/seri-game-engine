#pragma once

#include "seri/util/Util.h"
#include "seri/scene/SceneManager.h"
#include "seri/component/Components.h"

namespace seri::system
{
	class TransformSystem
	{
	public:
		static void Update()
		{
			const auto& registry = seri::scene::SceneManager::GetRegistry();
			const auto& tree = seri::scene::SceneManager::GetActiveScene()->GetSceneTreeRoot();

			auto view = registry.view<seri::component::IDComponent, seri::component::TransformComponent>();

			for (const auto& child : tree.children)
			{
				UpdateRecursive(child, glm::mat4{ 1.0f });
			}
		}

	private:
		static void UpdateRecursive(seri::scene::SceneTreeNode node, const glm::mat4& parentWorld)
		{
			auto& registry = seri::scene::SceneManager::GetRegistry();

			entt::entity entity = seri::scene::SceneManager::GetActiveScene()->GetEntityByID(node.id);

			auto& transformComp = registry.get<seri::component::TransformComponent>(entity);

			transformComp.localMatrix =
				glm::translate(glm::mat4{ 1.0f }, transformComp.position) *
				glm::mat4_cast(seri::Util::ToQuaternion(transformComp.rotation)) *
				glm::scale(glm::mat4{ 1.0f }, transformComp.scale);

			transformComp.worldMatrix = parentWorld * transformComp.localMatrix;

			for (const auto& child : node.children)
			{
				UpdateRecursive(child, transformComp.worldMatrix);
			}
		}
	};
}
