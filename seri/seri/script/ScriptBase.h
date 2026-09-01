#pragma once

#include "seri/core/Entity.h"

namespace seri::script
{
	class ScriptBase
	{
	public:
		virtual ~ScriptBase() = default;

		virtual void OnCreate() {}
		virtual void OnStart() {}
		virtual void OnUpdate(float deltaTime) {}
		virtual void OnLateUpdate(float deltaTime) {}
		virtual void OnEnable() {}
		virtual void OnDisable() {}
		virtual void OnDestroy() {}

		seri::Entity entity{};

	};
}
