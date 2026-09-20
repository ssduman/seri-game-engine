#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/ContactListener.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace seri::physics
{
	struct ObjectLayers
	{
		static constexpr JPH::ObjectLayer nonMoving = 0;
		static constexpr JPH::ObjectLayer moving = 1;
	};

	struct BroadPhaseLayers
	{
		static constexpr JPH::BroadPhaseLayer nonMoving{ 0 };
		static constexpr JPH::BroadPhaseLayer moving{ 1 };
		static constexpr JPH::uint count = 2;
	};

	struct JoltUtil
	{
		static JPH::Vec3 ToJolt(const glm::vec3& v)
		{
			return JPH::Vec3{ v.x, v.y, v.z };
		}

		static JPH::Quat ToJolt(const glm::quat& q)
		{
			return JPH::Quat{ q.x, q.y, q.z, q.w };
		}

		static glm::vec3 ToGlm(JPH::Vec3Arg v)
		{
			return glm::vec3{ v.GetX(), v.GetY(), v.GetZ() };
		}

		static glm::quat ToGlm(JPH::QuatArg q)
		{
			return glm::quat{ q.GetW(), q.GetX(), q.GetY(), q.GetZ() };
		}
	};
}
