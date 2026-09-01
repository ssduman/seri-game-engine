#pragma once

#include "seri/component/Components.h"
#include "seri/script/ScriptBase.h"

namespace seri::script
{
	class RotatorScript : public ScriptBase
	{
	public:
		void OnUpdate(float deltaTime) override
		{
			auto* transform = entity.TryGet<component::TransformComponent>();
			if (!transform)
			{
				return;
			}

			transform->rotation.y += _speed * deltaTime;
		}

		std::vector<ScriptField> GetSerializedFields() override
		{
			return {
				{ "Speed", ScriptField::Type::floating, &_speed },
				{ "Label", ScriptField::Type::text, &_label },
			};
		}

	private:
		float _speed{ 45.0f };
		std::string _label{ "rotator" };

	};
}
