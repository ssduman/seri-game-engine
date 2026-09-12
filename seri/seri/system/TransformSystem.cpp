#include "Seripch.h"
#include "seri/system/TransformSystem.h"

#include "seri/component/Components.h"

namespace seri::system
{
	void TransformSystem::Update()
	{
		const auto& registry = seri::scene::SceneManager::GetRegistry();
		const auto& tree = seri::scene::SceneManager::GetActiveScene()->GetSceneTreeRoot();

		auto view = registry.view<seri::component::IDComponent, seri::component::TransformComponent>();

		for (const auto& child : tree.children)
		{
			UpdateRecursive(child, glm::mat4{ 1.0f }, true);
		}
	}

	void TransformSystem::UpdateRecursive(const seri::scene::SceneTreeNode& node, const glm::mat4& parentWorld, bool parentActive)
	{
		auto& registry = seri::scene::SceneManager::GetRegistry();

		entt::entity entity = seri::scene::SceneManager::GetActiveScene()->GetEntityByID(node.id);

		auto& transformComp = registry.get<seri::component::TransformComponent>(entity);

		transformComp.localMatrix = Util::GetTRS(transformComp.position, transformComp.rotation, transformComp.scale);
		transformComp.worldMatrix = parentWorld * transformComp.localMatrix;
		transformComp.isActiveInHierarchy = parentActive && transformComp.isActive;

		for (const auto& child : node.children)
		{
			UpdateRecursive(child, transformComp.worldMatrix, transformComp.isActiveInHierarchy);
		}
	}
}
