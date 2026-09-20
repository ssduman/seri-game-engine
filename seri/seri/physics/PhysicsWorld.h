#pragma once

#include "seri/physics/JoltUtil.h"

#include <Jolt/Jolt.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/ContactListener.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <memory>
#include <mutex>
#include <vector>

namespace seri::system
{
	class PhysicsSystem;
}

namespace seri::physics
{
	class LayerInterface : public JPH::BroadPhaseLayerInterface
	{
	public:
		JPH::uint GetNumBroadPhaseLayers() const override;
		JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer layer) const override;
	};

	class ObjectVsBroadPhaseFilter : public JPH::ObjectVsBroadPhaseLayerFilter
	{
	public:
		bool ShouldCollide(JPH::ObjectLayer layer, JPH::BroadPhaseLayer broadPhaseLayer) const override;
	};

	class ObjectPairFilter : public JPH::ObjectLayerPairFilter
	{
	public:
		bool ShouldCollide(JPH::ObjectLayer layer1, JPH::ObjectLayer layer2) const override;
	};

	class ContactCollector : public JPH::ContactListener
	{
	public:
		struct ContactEvent
		{
			JPH::BodyID body1{};
			JPH::BodyID body2{};
		};

		void OnContactAdded(const JPH::Body& body1, const JPH::Body& body2, const JPH::ContactManifold& manifold, JPH::ContactSettings& settings) override;

		std::vector<ContactEvent> Consume();

	private:
		std::mutex _mutex;
		std::vector<ContactEvent> _events{};

	};

	class PhysicsWorld
	{
	public:
		PhysicsWorld();
		~PhysicsWorld() = default;

	private:
		static void Init();
		static void Shutdown();

		void Step(float deltaTime);

		JPH::PhysicsSystem& GetSystem();
		JPH::BodyInterface& GetBodyInterface();

		std::vector<ContactCollector::ContactEvent> ConsumeContacts();

		static void Trace(const char* format, ...);
		static bool AssertFailed(const char* expression, const char* message, const char* file, JPH::uint line);

		friend class seri::system::PhysicsSystem;

		std::unique_ptr<JPH::TempAllocatorImpl> _tempAllocator;
		std::unique_ptr<JPH::JobSystemThreadPool> _jobSystem;
		LayerInterface _layerInterface{};
		ObjectVsBroadPhaseFilter _objectVsBroadPhaseFilter{};
		ObjectPairFilter _objectPairFilter{};
		ContactCollector _contactCollector{};
		std::unique_ptr<JPH::PhysicsSystem> _system;
		bool _optimized{ false };

	};
}
