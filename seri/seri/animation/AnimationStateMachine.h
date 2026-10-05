#pragma once

#include "seri/asset/AssetBase.h"
#include "seri/animation/AnimationUtil.h"

#include <string>
#include <vector>
#include <string_view>

namespace seri::animation
{
	class AnimationStateMachine : public seri::asset::AssetBase
	{
	public:
		static constexpr std::string_view kAnyState = "Any";

		AnimationStateMachine()
		{
			type = seri::asset::AssetType::animation_state_machine;
		}

		std::string entryState{ "" };
		std::vector<AnimatorParameter> parameters{};
		std::vector<AnimatorState> states{};
		std::vector<AnimatorTransition> transitions{};

		int FindState(const std::string& name) const
		{
			for (size_t i = 0; i < states.size(); i++)
			{
				if (states[i].name == name)
				{
					return static_cast<int>(i);
				}
			}

			return -1;
		}

		const AnimatorParameter* FindParameter(const std::string& name) const
		{
			for (const auto& parameter : parameters)
			{
				if (parameter.name == name)
				{
					return &parameter;
				}
			}

			return nullptr;
		}
	};
}
