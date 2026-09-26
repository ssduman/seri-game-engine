#include "Seripch.h"

#include "seri/system/PhysicsSystem.h"
#include "seri/system/ScriptSystem.h"
#include "seri/physics/PhysicsWorld.h"
#include "seri/scene/SceneManager.h"
#include "seri/draw/DebugDraw.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyLock.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>

namespace seri::system
{
	std::unique_ptr<physics::PhysicsWorld> PhysicsSystem::_world{};

	void PhysicsSystem::Init()
	{
		if (_inited)
		{
			return;
		}

		physics::PhysicsWorld::Init();

		scene::SceneManager::GetRegistry()
			.on_destroy<component::ColliderComponent>().connect<&PhysicsSystem::OnColliderDestroyed>();

		_inited = true;
	}

	void PhysicsSystem::Shutdown()
	{
		if (!_inited)
		{
			return;
		}

		Reset();

		scene::SceneManager::GetRegistry()
			.on_destroy<component::ColliderComponent>().disconnect<&PhysicsSystem::OnColliderDestroyed>();

		physics::PhysicsWorld::Shutdown();

		_inited = false;
	}

	void PhysicsSystem::Sync()
	{
		if (!_world)
		{
			_world = std::make_unique<physics::PhysicsWorld>();
			_world->GetSystem().SetGravity(physics::JoltUtil::ToJolt(_gravity));
		}

		auto& registry = scene::SceneManager::GetRegistry();
		JPH::BodyInterface& bodyInterface = _world->GetBodyInterface();

		auto view = registry.view<component::TransformComponent, component::ColliderComponent>();
		std::vector<entt::entity> entities(view.begin(), view.end());

		for (entt::entity entity : entities)
		{
			auto& transform = registry.get<component::TransformComponent>(entity);
			auto& collider = registry.get<component::ColliderComponent>(entity);
			auto* rigidbody = registry.try_get<component::RigidbodyComponent>(entity);

			auto it = _bodies.find(entity);

			if (!transform.isActiveInHierarchy)
			{
				if (it != _bodies.end())
				{
					RemoveBody(entity);
				}
				continue;
			}

			JPH::Vec3 linearVelocity = JPH::Vec3::sZero();
			JPH::Vec3 angularVelocity = JPH::Vec3::sZero();

			if (it != _bodies.end())
			{
				BodyRecord& record = it->second;
				JPH::BodyID bodyId{ record.bodyId };

				bool changed = record.collider != collider ||
					record.scale != transform.scale ||
					record.hasRigidbody != (rigidbody != nullptr) ||
					(rigidbody != nullptr && record.rigidbody != *rigidbody);

				if (!changed)
				{
					bool moved = record.position != transform.position || record.rotation != transform.rotation;
					if (moved && record.motionType != MotionType::kinematic)
					{
						glm::vec3 position;
						glm::quat rotation;
						glm::vec3 scale;
						GetWorldPose(entity, transform, position, rotation, scale);

						JPH::EActivation activation = record.motionType == MotionType::dynamic ? JPH::EActivation::Activate : JPH::EActivation::DontActivate;
						bodyInterface.SetPositionAndRotation(bodyId, physics::JoltUtil::ToJolt(position), physics::JoltUtil::ToJolt(rotation), activation);

						record.position = transform.position;
						record.rotation = transform.rotation;
					}
					continue;
				}

				linearVelocity = bodyInterface.GetLinearVelocity(bodyId);
				angularVelocity = bodyInterface.GetAngularVelocity(bodyId);

				RemoveBody(entity);
			}

			if (CreateBody(entity, transform, collider, rigidbody) && _bodies[entity].motionType == MotionType::dynamic)
			{
				JPH::BodyID bodyId{ _bodies[entity].bodyId };
				bodyInterface.SetLinearAndAngularVelocity(bodyId, linearVelocity, angularVelocity);
			}
		}
	}

	void PhysicsSystem::Update(float fixedDeltaTime)
	{
		SERI_PROFILER_ZONE_SCOPED;

		Sync();
		MoveKinematicBodies(fixedDeltaTime);

		_world->Step(fixedDeltaTime);

		WriteTransforms();
		DispatchContacts();
	}

	void PhysicsSystem::Reset()
	{
		_bodies.clear();
		_contacts.clear();
		_world.reset();
		_gravity = kDefaultGravity;
	}

	glm::vec3 PhysicsSystem::GetGravity()
	{
		return _gravity;
	}

	void PhysicsSystem::SetGravity(const glm::vec3& gravity)
	{
		_gravity = gravity;

		if (_world)
		{
			_world->GetSystem().SetGravity(physics::JoltUtil::ToJolt(_gravity));
		}
	}

	glm::vec3 PhysicsSystem::GetLinearVelocity(entt::entity entity)
	{
		uint32_t bodyId = 0;
		if (!TryGetBody(entity, bodyId))
		{
			return glm::vec3{ 0.0f };
		}

		return physics::JoltUtil::ToGlm(_world->GetBodyInterface().GetLinearVelocity(JPH::BodyID{ bodyId }));
	}

	void PhysicsSystem::SetLinearVelocity(entt::entity entity, const glm::vec3& velocity)
	{
		uint32_t bodyId = 0;
		if (TryGetBody(entity, bodyId))
		{
			_world->GetBodyInterface().SetLinearVelocity(JPH::BodyID{ bodyId }, physics::JoltUtil::ToJolt(velocity));
		}
	}

	glm::vec3 PhysicsSystem::GetAngularVelocity(entt::entity entity)
	{
		uint32_t bodyId = 0;
		if (!TryGetBody(entity, bodyId))
		{
			return glm::vec3{ 0.0f };
		}

		return physics::JoltUtil::ToGlm(_world->GetBodyInterface().GetAngularVelocity(JPH::BodyID{ bodyId }));
	}

	void PhysicsSystem::SetAngularVelocity(entt::entity entity, const glm::vec3& velocity)
	{
		uint32_t bodyId = 0;
		if (TryGetBody(entity, bodyId))
		{
			_world->GetBodyInterface().SetAngularVelocity(JPH::BodyID{ bodyId }, physics::JoltUtil::ToJolt(velocity));
		}
	}

	void PhysicsSystem::AddForce(entt::entity entity, const glm::vec3& force)
	{
		uint32_t bodyId = 0;
		if (TryGetBody(entity, bodyId, true))
		{
			_world->GetBodyInterface().AddForce(JPH::BodyID{ bodyId }, physics::JoltUtil::ToJolt(force));
		}
	}

	void PhysicsSystem::AddImpulse(entt::entity entity, const glm::vec3& impulse)
	{
		uint32_t bodyId = 0;
		if (TryGetBody(entity, bodyId, true))
		{
			_world->GetBodyInterface().AddImpulse(JPH::BodyID{ bodyId }, physics::JoltUtil::ToJolt(impulse));
		}
	}

	void PhysicsSystem::AddTorque(entt::entity entity, const glm::vec3& torque)
	{
		uint32_t bodyId = 0;
		if (TryGetBody(entity, bodyId, true))
		{
			_world->GetBodyInterface().AddTorque(JPH::BodyID{ bodyId }, physics::JoltUtil::ToJolt(torque));
		}
	}

	bool PhysicsSystem::Raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance, RaycastHit& hit)
	{
		if (!_world || glm::length(direction) <= 0.0f || maxDistance <= 0.0f)
		{
			return false;
		}

		JPH::RRayCast ray{ physics::JoltUtil::ToJolt(origin), physics::JoltUtil::ToJolt(glm::normalize(direction) * maxDistance) };
		JPH::RayCastResult result{};

		JPH::PhysicsSystem& system = _world->GetSystem();
		if (!system.GetNarrowPhaseQueryNoLock().CastRay(ray, result))
		{
			return false;
		}

		JPH::BodyLockRead lock(system.GetBodyLockInterfaceNoLock(), result.mBodyID);
		if (!lock.Succeeded())
		{
			return false;
		}

		const JPH::Body& body = lock.GetBody();
		JPH::RVec3 point = ray.GetPointOnRay(result.mFraction);

		hit.entity = seri::Entity{ static_cast<entt::entity>(body.GetUserData()), &scene::SceneManager::GetRegistry() };
		hit.point = physics::JoltUtil::ToGlm(point);
		hit.normal = physics::JoltUtil::ToGlm(body.GetWorldSpaceSurfaceNormal(result.mSubShapeID2, point));
		hit.distance = result.mFraction * maxDistance;

		return true;
	}

	void PhysicsSystem::DrawColliderGizmo(entt::entity entity)
	{
		auto& registry = scene::SceneManager::GetRegistry();

		auto* transform = registry.try_get<component::TransformComponent>(entity);
		auto* collider = registry.try_get<component::ColliderComponent>(entity);
		if (transform == nullptr || collider == nullptr)
		{
			return;
		}

		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 scale;
		Decompose(transform->worldMatrix, position, rotation, scale);

		ShapeSize size = GetShapeSize(*collider, scale);

		glm::vec4 color{ 0.4f, 1.0f, 0.4f, 1.0f };
		glm::mat4 pose = glm::translate(glm::mat4{ 1.0f }, position) * glm::mat4_cast(rotation);
		glm::vec3 center = glm::vec3(pose * glm::vec4(collider->center * scale, 1.0f));

		switch (collider->shape)
		{
			case physics::ColliderShape::box:
				debug::DebugDraw::DrawOBB(glm::translate(pose, collider->center * scale), size.halfExtents, color);
				break;
			case physics::ColliderShape::sphere:
				debug::DebugDraw::DrawWireSphere(center, size.radius, color);
				break;
			case physics::ColliderShape::capsule:
				{
					glm::vec3 up = rotation * glm::vec3{ 0.0f, size.halfHeight, 0.0f };
					glm::vec3 right = rotation * glm::vec3{ size.radius, 0.0f, 0.0f };
					glm::vec3 forward = rotation * glm::vec3{ 0.0f, 0.0f, size.radius };

					debug::DebugDraw::DrawWireSphere(center + up, size.radius, color);
					debug::DebugDraw::DrawWireSphere(center - up, size.radius, color);
					debug::DebugDraw::Line(center + up + right, center - up + right, color);
					debug::DebugDraw::Line(center + up - right, center - up - right, color);
					debug::DebugDraw::Line(center + up + forward, center - up + forward, color);
					debug::DebugDraw::Line(center + up - forward, center - up - forward, color);
				}
				break;
		}
	}

	void PhysicsSystem::OnColliderDestroyed(entt::registry& registry, entt::entity entity)
	{
		RemoveBody(entity);
	}

	bool PhysicsSystem::CreateBody(entt::entity entity, const component::TransformComponent& transform, const component::ColliderComponent& collider, const component::RigidbodyComponent* rigidbody)
	{
		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 scale;
		GetWorldPose(entity, transform, position, rotation, scale);

		ShapeSize size = GetShapeSize(collider, scale);

		JPH::ShapeRefC shape{};
		switch (collider.shape)
		{
			case physics::ColliderShape::box:
				shape = new JPH::BoxShape(physics::JoltUtil::ToJolt(size.halfExtents));
				break;
			case physics::ColliderShape::sphere:
				shape = new JPH::SphereShape(size.radius);
				break;
			case physics::ColliderShape::capsule:
				if (size.halfHeight > 0.0f)
				{
					shape = new JPH::CapsuleShape(size.halfHeight, size.radius);
				}
				else
				{
					shape = new JPH::SphereShape(size.radius);
				}
				break;
		}

		if (collider.center != glm::vec3{ 0.0f })
		{
			shape = new JPH::RotatedTranslatedShape(physics::JoltUtil::ToJolt(collider.center * scale), JPH::Quat::sIdentity(), shape);
		}

		MotionType motionType = MotionType::fixed;
		JPH::EAllowedDOFs allowedDOFs = JPH::EAllowedDOFs::All;

		if (rigidbody != nullptr)
		{
			const JPH::EAllowedDOFs locks[6] = {
				JPH::EAllowedDOFs::TranslationX, JPH::EAllowedDOFs::TranslationY, JPH::EAllowedDOFs::TranslationZ,
				JPH::EAllowedDOFs::RotationX, JPH::EAllowedDOFs::RotationY, JPH::EAllowedDOFs::RotationZ,
			};

			for (int i = 0; i < 3; i++)
			{
				if (rigidbody->lockPosition[i])
				{
					allowedDOFs &= ~locks[i];
				}
				if (rigidbody->lockRotation[i])
				{
					allowedDOFs &= ~locks[i + 3];
				}
			}

			bool kinematic = rigidbody->isKinematic || allowedDOFs == JPH::EAllowedDOFs::None;
			motionType = kinematic ? MotionType::kinematic : MotionType::dynamic;
		}

		JPH::EMotionType joltMotionType = JPH::EMotionType::Static;
		if (motionType == MotionType::kinematic)
		{
			joltMotionType = JPH::EMotionType::Kinematic;
		}
		else if (motionType == MotionType::dynamic)
		{
			joltMotionType = JPH::EMotionType::Dynamic;
		}

		JPH::ObjectLayer layer = motionType == MotionType::fixed ? physics::ObjectLayers::nonMoving : physics::ObjectLayers::moving;

		JPH::BodyCreationSettings settings{ shape.GetPtr(), physics::JoltUtil::ToJolt(position), physics::JoltUtil::ToJolt(rotation), joltMotionType, layer };
		settings.mUserData = static_cast<JPH::uint64>(entity);
		settings.mFriction = collider.friction;
		settings.mRestitution = collider.restitution;
		settings.mIsSensor = collider.isTrigger;
		settings.mCollideKinematicVsNonDynamic = collider.isTrigger && motionType == MotionType::kinematic;

		if (rigidbody != nullptr)
		{
			settings.mLinearDamping = rigidbody->linearDamping;
			settings.mAngularDamping = rigidbody->angularDamping;
			settings.mGravityFactor = rigidbody->gravityScale;
			settings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
			settings.mMassPropertiesOverride.mMass = std::max(rigidbody->mass, kMinExtent);

			if (motionType == MotionType::dynamic)
			{
				settings.mAllowedDOFs = allowedDOFs;
			}
		}

		JPH::EActivation activation = motionType == MotionType::fixed ? JPH::EActivation::DontActivate : JPH::EActivation::Activate;
		JPH::BodyID bodyId = _world->GetBodyInterface().CreateAndAddBody(settings, activation);
		if (bodyId.IsInvalid())
		{
			LIB_LOGGER(error, physics) << "could not create body, body limit reached";
			return false;
		}

		BodyRecord record{};
		record.bodyId = bodyId.GetIndexAndSequenceNumber();
		record.motionType = motionType;
		record.hasRigidbody = rigidbody != nullptr;
		record.rigidbody = rigidbody != nullptr ? *rigidbody : component::RigidbodyComponent{};
		record.collider = collider;
		record.scale = transform.scale;
		record.position = transform.position;
		record.rotation = transform.rotation;

		_bodies[entity] = record;

		return true;
	}

	void PhysicsSystem::RemoveBody(entt::entity entity)
	{
		auto it = _bodies.find(entity);
		if (it == _bodies.end() || !_world)
		{
			return;
		}

		uint32_t id = it->second.bodyId;
		std::erase_if(_contacts, [id](uint64_t key) { return static_cast<uint32_t>(key >> 32) == id || static_cast<uint32_t>(key) == id; });

		JPH::BodyID bodyId{ id };
		JPH::BodyInterface& bodyInterface = _world->GetBodyInterface();
		bodyInterface.RemoveBody(bodyId);
		bodyInterface.DestroyBody(bodyId);

		_bodies.erase(it);
	}

	void PhysicsSystem::MoveKinematicBodies(float fixedDeltaTime)
	{
		auto& registry = scene::SceneManager::GetRegistry();
		JPH::BodyInterface& bodyInterface = _world->GetBodyInterface();

		for (auto& [entity, record] : _bodies)
		{
			auto* transform = registry.try_get<component::TransformComponent>(entity);
			if (record.motionType != MotionType::kinematic || transform == nullptr)
			{
				continue;
			}

			glm::vec3 position;
			glm::quat rotation;
			glm::vec3 scale;
			GetWorldPose(entity, *transform, position, rotation, scale);

			bodyInterface.MoveKinematic(JPH::BodyID{ record.bodyId }, physics::JoltUtil::ToJolt(position), physics::JoltUtil::ToJolt(rotation), fixedDeltaTime);

			record.position = transform->position;
			record.rotation = transform->rotation;
		}
	}

	void PhysicsSystem::WriteTransforms()
	{
		auto& registry = scene::SceneManager::GetRegistry();
		JPH::BodyInterface& bodyInterface = _world->GetBodyInterface();

		for (auto& [entity, record] : _bodies)
		{
			JPH::BodyID bodyId{ record.bodyId };

			auto* transform = registry.try_get<component::TransformComponent>(entity);
			if (record.motionType != MotionType::dynamic || transform == nullptr || !bodyInterface.IsActive(bodyId))
			{
				continue;
			}

			JPH::RVec3 bodyPosition;
			JPH::Quat bodyRotation;
			bodyInterface.GetPositionAndRotation(bodyId, bodyPosition, bodyRotation);

			glm::vec3 position = physics::JoltUtil::ToGlm(bodyPosition);
			glm::quat rotation = physics::JoltUtil::ToGlm(bodyRotation);

			glm::mat4 parentWorld;
			if (TryGetParentWorldMatrix(entity, parentWorld))
			{
				glm::vec3 scale;
				glm::mat4 local = glm::inverse(parentWorld) * Util::GetTRS(position, rotation, glm::vec3{ 1.0f });
				Decompose(local, position, rotation, scale);
			}

			transform->position = position;
			transform->rotation = glm::degrees(glm::eulerAngles(rotation));

			record.position = transform->position;
			record.rotation = transform->rotation;
		}
	}

	void PhysicsSystem::DispatchContacts()
	{
		JPH::PhysicsSystem& system = _world->GetSystem();
		const JPH::BodyLockInterfaceNoLock& lockInterface = system.GetBodyLockInterfaceNoLock();

		std::vector<uint64_t> entered{};
		std::vector<uint64_t> exited{};

		for (const physics::ContactCollector::ContactEvent& contact : _world->ConsumeContacts())
		{
			uint64_t key = MakeContactKey(contact.body1.GetIndexAndSequenceNumber(), contact.body2.GetIndexAndSequenceNumber());
			if (_contacts.insert(key).second)
			{
				entered.push_back(key);
			}
		}

		for (auto it = _contacts.begin(); it != _contacts.end();)
		{
			JPH::BodyID bodyId1{ static_cast<uint32_t>(*it >> 32) };
			JPH::BodyID bodyId2{ static_cast<uint32_t>(*it) };

			JPH::BodyLockRead lock1(lockInterface, bodyId1);
			JPH::BodyLockRead lock2(lockInterface, bodyId2);
			if (!lock1.Succeeded() || !lock2.Succeeded())
			{
				it = _contacts.erase(it);
				continue;
			}

			const JPH::Body& body1 = lock1.GetBody();
			const JPH::Body& body2 = lock2.GetBody();

			bool sleeping = (body1.IsDynamic() && !body1.IsActive()) || (body2.IsDynamic() && !body2.IsActive());
			bool idle = !body1.IsActive() && !body2.IsActive();

			if (sleeping || idle || system.WereBodiesInContact(bodyId1, bodyId2))
			{
				++it;
				continue;
			}

			exited.push_back(*it);
			it = _contacts.erase(it);
		}

		for (uint64_t key : entered)
		{
			NotifyContact(key, true);
		}

		for (uint64_t key : exited)
		{
			NotifyContact(key, false);
		}
	}

	void PhysicsSystem::NotifyContact(uint64_t key, bool entered)
	{
		auto& registry = scene::SceneManager::GetRegistry();
		const JPH::BodyLockInterfaceNoLock& lockInterface = _world->GetSystem().GetBodyLockInterfaceNoLock();

		JPH::BodyLockRead lock1(lockInterface, JPH::BodyID{ static_cast<uint32_t>(key >> 32) });
		JPH::BodyLockRead lock2(lockInterface, JPH::BodyID{ static_cast<uint32_t>(key) });
		if (!lock1.Succeeded() || !lock2.Succeeded())
		{
			return;
		}

		seri::Entity entity1{ static_cast<entt::entity>(lock1.GetBody().GetUserData()), &registry };
		seri::Entity entity2{ static_cast<entt::entity>(lock2.GetBody().GetUserData()), &registry };
		bool trigger = lock1.GetBody().IsSensor() || lock2.GetBody().IsSensor();

		lock1.ReleaseLock();
		lock2.ReleaseLock();

		auto notify = [&](const seri::Entity& self, const seri::Entity& other)
			{
				if (!self.IsValid() || !other.IsValid())
				{
					return;
				}

				if (trigger)
				{
					ScriptSystem::Dispatch(self, entered ? "OnTriggerEnter" : "OnTriggerExit", [&](script::ScriptBase& script)
						{
							entered ? script.OnTriggerEnter(other) : script.OnTriggerExit(other);
						});
				}
				else
				{
					ScriptSystem::Dispatch(self, entered ? "OnCollisionEnter" : "OnCollisionExit", [&](script::ScriptBase& script)
						{
							entered ? script.OnCollisionEnter(other) : script.OnCollisionExit(other);
						});
				}
			};

		notify(entity1, entity2);
		notify(entity2, entity1);
	}

	uint64_t PhysicsSystem::MakeContactKey(uint32_t bodyId1, uint32_t bodyId2)
	{
		return (static_cast<uint64_t>(std::min(bodyId1, bodyId2)) << 32) | std::max(bodyId1, bodyId2);
	}

	bool PhysicsSystem::TryGetBody(entt::entity entity, uint32_t& bodyId, bool dynamicOnly)
	{
		if (!_world)
		{
			return false;
		}

		auto it = _bodies.find(entity);
		if (it == _bodies.end())
		{
			return false;
		}

		if (dynamicOnly && it->second.motionType != MotionType::dynamic)
		{
			return false;
		}

		bodyId = it->second.bodyId;

		return true;
	}

	bool PhysicsSystem::TryGetParentWorldMatrix(entt::entity entity, glm::mat4& parentWorld)
	{
		auto& registry = scene::SceneManager::GetRegistry();

		auto* id = registry.try_get<component::IDComponent>(entity);
		auto scene = scene::SceneManager::GetActiveScene();
		if (id == nullptr || id->parentId == 0 || !scene->HasEntity(id->parentId))
		{
			return false;
		}

		auto* parentTransform = registry.try_get<component::TransformComponent>(scene->GetEntityByID(id->parentId));
		if (parentTransform == nullptr)
		{
			return false;
		}

		parentWorld = parentTransform->worldMatrix;

		return true;
	}

	void PhysicsSystem::GetWorldPose(entt::entity entity, const component::TransformComponent& transform, glm::vec3& position, glm::quat& rotation, glm::vec3& scale)
	{
		glm::mat4 world = Util::GetTRS(transform.position, transform.rotation, transform.scale);

		glm::mat4 parentWorld;
		if (TryGetParentWorldMatrix(entity, parentWorld))
		{
			world = parentWorld * world;
		}

		Decompose(world, position, rotation, scale);
	}

	void PhysicsSystem::Decompose(const glm::mat4& matrix, glm::vec3& position, glm::quat& rotation, glm::vec3& scale)
	{
		glm::mat3 axes{ matrix };

		position = glm::vec3(matrix[3]);
		scale = glm::vec3{ glm::length(axes[0]), glm::length(axes[1]), glm::length(axes[2]) };

		if (glm::determinant(axes) < 0.0f)
		{
			scale.x = -scale.x;
		}

		for (int i = 0; i < 3; i++)
		{
			if (std::abs(scale[i]) > 0.0f)
			{
				axes[i] /= scale[i];
			}
		}

		rotation = glm::normalize(glm::quat_cast(axes));
	}

	PhysicsSystem::ShapeSize PhysicsSystem::GetShapeSize(const component::ColliderComponent& collider, const glm::vec3& scale)
	{
		glm::vec3 absScale = glm::abs(scale);

		ShapeSize size{};
		size.halfExtents = glm::max(collider.size * absScale * 0.5f, glm::vec3{ kMinExtent });

		switch (collider.shape)
		{
			case physics::ColliderShape::sphere:
				size.radius = std::max(collider.radius * std::max({ absScale.x, absScale.y, absScale.z }), kMinExtent);
				break;
			case physics::ColliderShape::capsule:
				size.radius = std::max(collider.radius * std::max(absScale.x, absScale.z), kMinExtent);
				size.halfHeight = std::max(collider.height * absScale.y * 0.5f - size.radius, 0.0f);
				break;
			default:
				break;
		}

		return size;
	}
}
