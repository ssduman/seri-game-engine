#include "Seripch.h"

#include "seri/physics/JoltUtil.h"
#include "seri/physics/PhysicsWorld.h"
#include "seri/logging/Logger.h"

#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/PhysicsSettings.h>

#include <cstdarg>
#include <thread>

namespace seri::physics
{
	JPH::uint LayerInterface::GetNumBroadPhaseLayers() const
	{
		return BroadPhaseLayers::count;
	}

	JPH::BroadPhaseLayer LayerInterface::GetBroadPhaseLayer(JPH::ObjectLayer layer) const
	{
		return layer == ObjectLayers::nonMoving ? BroadPhaseLayers::nonMoving : BroadPhaseLayers::moving;
	}

	bool ObjectVsBroadPhaseFilter::ShouldCollide(JPH::ObjectLayer layer, JPH::BroadPhaseLayer broadPhaseLayer) const
	{
		return layer == ObjectLayers::moving || broadPhaseLayer == BroadPhaseLayers::moving;
	}

	bool ObjectPairFilter::ShouldCollide(JPH::ObjectLayer layer1, JPH::ObjectLayer layer2) const
	{
		return layer1 == ObjectLayers::moving || layer2 == ObjectLayers::moving;
	}

	void ContactCollector::OnContactAdded(const JPH::Body& body1, const JPH::Body& body2, const JPH::ContactManifold& manifold, JPH::ContactSettings& settings)
	{
		std::scoped_lock lock(_mutex);
		_events.push_back({ body1.GetID(), body2.GetID() });
	}

	std::vector<ContactCollector::ContactEvent> ContactCollector::Consume()
	{
		std::scoped_lock lock(_mutex);
		return std::exchange(_events, {});
	}

	PhysicsWorld::PhysicsWorld()
	{
		int threadCount = std::max(1, static_cast<int>(std::thread::hardware_concurrency()) - 1);

		_tempAllocator = std::make_unique<JPH::TempAllocatorImpl>(10 * 1024 * 1024);
		_jobSystem = std::make_unique<JPH::JobSystemThreadPool>(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, threadCount);

		_system = std::make_unique<JPH::PhysicsSystem>();
		_system->Init(65536, 0, 65536, 10240, _layerInterface, _objectVsBroadPhaseFilter, _objectPairFilter);
		_system->SetContactListener(&_contactCollector);
	}

	void PhysicsWorld::Init()
	{
		JPH::RegisterDefaultAllocator();

		JPH::Trace = &PhysicsWorld::Trace;
		JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = &PhysicsWorld::AssertFailed;);

		if (!JPH::VerifyJoltVersionID())
		{
			LIB_LOGGER(error, physics) << "jolt was compiled with a different configuration";
		}

		JPH::Factory::sInstance = new JPH::Factory();
		JPH::RegisterTypes();

		LIB_LOGGER(info, physics) << "jolt " << JPH_VERSION_MAJOR << "." << JPH_VERSION_MINOR << "." << JPH_VERSION_PATCH << " inited";
	}

	void PhysicsWorld::Shutdown()
	{
		JPH::UnregisterTypes();

		delete JPH::Factory::sInstance;
		JPH::Factory::sInstance = nullptr;
	}

	void PhysicsWorld::Step(float deltaTime)
	{
		if (!_optimized)
		{
			_system->OptimizeBroadPhase();
			_optimized = true;
		}

		JPH::EPhysicsUpdateError error = _system->Update(deltaTime, 1, _tempAllocator.get(), _jobSystem.get());
		if (error != JPH::EPhysicsUpdateError::None)
		{
			LIB_LOGGER(warning, physics) << "update error: " << static_cast<int>(error);
		}
	}

	JPH::PhysicsSystem& PhysicsWorld::GetSystem()
	{
		return *_system;
	}

	JPH::BodyInterface& PhysicsWorld::GetBodyInterface()
	{
		return _system->GetBodyInterfaceNoLock();
	}

	std::vector<ContactCollector::ContactEvent> PhysicsWorld::ConsumeContacts()
	{
		return _contactCollector.Consume();
	}

	void PhysicsWorld::Trace(const char* format, ...)
	{
		char buffer[1024];

		va_list args;
		va_start(args, format);
		vsnprintf(buffer, sizeof(buffer), format, args);
		va_end(args);

		LIB_LOGGER(warning, physics) << buffer;
	}

	bool PhysicsWorld::AssertFailed(const char* expression, const char* message, const char* file, JPH::uint line)
	{
		LIB_LOGGER(error, physics) << "assert failed: " << expression << " " << (message != nullptr ? message : "") << " at " << file << ":" << line;
		return false;
	}
}
