#pragma once

#include "seri/core/Entity.h"
#include "seri/physics/PhysicsUtil.h"
#include "seri/component/Components.h"

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace seri::physics
{
	class PhysicsWorld;
}

namespace seri::system
{
	struct RaycastHit
	{
		seri::Entity entity{};
		glm::vec3 point{ 0.0f };
		glm::vec3 normal{ 0.0f };
		float distance{ 0.0f };
	};

	class PhysicsSystem
	{
	public:
		static void Init();
		static void Shutdown();

		static void Sync();
		static void Update(float fixedDeltaTime);
		static void Reset();

		static glm::vec3 GetGravity();
		static void SetGravity(const glm::vec3& gravity);

		static glm::vec3 GetLinearVelocity(entt::entity entity);
		static void SetLinearVelocity(entt::entity entity, const glm::vec3& velocity);

		static glm::vec3 GetAngularVelocity(entt::entity entity);
		static void SetAngularVelocity(entt::entity entity, const glm::vec3& velocity);

		static void AddForce(entt::entity entity, const glm::vec3& force);
		static void AddImpulse(entt::entity entity, const glm::vec3& impulse);
		static void AddTorque(entt::entity entity, const glm::vec3& torque);

		static bool Raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance, RaycastHit& hit);

		static void DrawColliderGizmo(entt::entity entity);

	private:
		enum class MotionType
		{
			fixed,
			kinematic,
			dynamic,
		};

		struct BodyRecord
		{
			uint32_t bodyId{ 0 };
			MotionType motionType{ MotionType::fixed };
			bool hasRigidbody{ false };
			component::RigidbodyComponent rigidbody{};
			component::ColliderComponent collider{};
			glm::vec3 scale{ 1.0f };
			glm::vec3 position{ 0.0f };
			glm::vec3 rotation{ 0.0f };
		};

		struct ShapeSize
		{
			glm::vec3 halfExtents{ 0.5f };
			float radius{ 0.5f };
			float halfHeight{ 0.0f };
		};

		static void OnColliderDestroyed(entt::registry& registry, entt::entity entity);

		static bool CreateBody(entt::entity entity, const component::TransformComponent& transform, const component::ColliderComponent& collider, const component::RigidbodyComponent* rigidbody);
		static void RemoveBody(entt::entity entity);
		static void MoveKinematicBodies(float fixedDeltaTime);
		static void WriteTransforms();
		static void DispatchContacts();
		static void NotifyContact(uint64_t key, bool entered);
		static uint64_t MakeContactKey(uint32_t bodyId1, uint32_t bodyId2);

		static bool TryGetBody(entt::entity entity, uint32_t& bodyId, bool dynamicOnly = false);
		static bool TryGetParentWorldMatrix(entt::entity entity, glm::mat4& parentWorld);
		static void GetWorldPose(entt::entity entity, const component::TransformComponent& transform, glm::vec3& position, glm::quat& rotation, glm::vec3& scale);
		static void Decompose(const glm::mat4& matrix, glm::vec3& position, glm::quat& rotation, glm::vec3& scale);
		static ShapeSize GetShapeSize(const component::ColliderComponent& collider, const glm::vec3& scale);

		static inline const glm::vec3 kDefaultGravity{ 0.0f, -9.81f, 0.0f };
		static inline const float kMinExtent{ 0.001f };

		static std::unique_ptr<physics::PhysicsWorld> _world;
		static inline std::unordered_map<entt::entity, BodyRecord> _bodies{};
		static inline std::unordered_set<uint64_t> _contacts{};
		static inline glm::vec3 _gravity{ kDefaultGravity };
		static inline bool _inited{ false };

	};
}
