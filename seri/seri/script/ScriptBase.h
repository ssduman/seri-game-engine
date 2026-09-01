#pragma once

#include "seri/core/Entity.h"

#include <string>
#include <vector>

namespace seri::script
{
	struct ScriptField
	{
		enum class Type
		{
			boolean,
			integer,
			floating,
			vec3,
			color3,
			text,
		};

		std::string name{ "" };
		Type type{ Type::floating };
		void* ptr{ nullptr };
	};

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

		virtual std::vector<ScriptField> GetSerializedFields() { return {}; }

		seri::Entity entity{};

	};
}
