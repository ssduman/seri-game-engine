#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include <glm/glm.hpp>

namespace seri::animation
{
	enum class AnimatorMode
	{
		clip = 0,
		animation_state_machine = 1,
	};

	inline const char* AnimatorModeToString(AnimatorMode mode)
	{
		switch (mode)
		{
			case AnimatorMode::clip: return "Clip";
			case AnimatorMode::animation_state_machine: return "ASM";
			default: return "Unknown";
		}
	}

	enum class AnimatorParameterType
	{
		number = 0,
		boolean = 1,
		trigger = 2,
	};

	enum class AnimatorConditionOp
	{
		greater = 0,
		less = 1,
		equals = 2,
		not_equals = 3,
	};

	struct AnimatorParameter
	{
		std::string name{ "" };
		AnimatorParameterType type{ AnimatorParameterType::number };
		float value{ 0.0f };
	};

	struct AnimatorCondition
	{
		std::string parameter{ "" };
		AnimatorConditionOp op{ AnimatorConditionOp::equals };
		float value{ 1.0f };
	};

	struct AnimatorState
	{
		std::string name{ "" };
		uint64_t clipAssetId{ 0 };
		std::string clip{ "" };
		bool loop{ true };
		float speed{ 1.0f };
		glm::vec2 position{ 0.0f, 0.0f };
	};

	struct AnimatorTransition
	{
		std::string from{ "" };
		std::string to{ "" };
		float fade{ 0.25f };
		bool exitTime{ false };
		std::vector<AnimatorCondition> conditions{};
	};

	struct AnimatorPlayback
	{
		uint64_t clipAssetId{ 0 };
		int clipIndex{ -1 };
		bool loop{ true };
		bool finished{ false };
		float speed{ 1.0f };
		float time{ 0.0f };
	};

	inline const char* AnimatorParameterTypeToString(AnimatorParameterType type)
	{
		switch (type)
		{
			case AnimatorParameterType::number: return "float";
			case AnimatorParameterType::boolean: return "bool";
			case AnimatorParameterType::trigger: return "trigger";
			default: return "float";
		}
	}

	inline AnimatorParameterType AnimatorParameterTypeFromString(const std::string& value)
	{
		if (value == "bool")
		{
			return AnimatorParameterType::boolean;
		}
		if (value == "trigger")
		{
			return AnimatorParameterType::trigger;
		}
		return AnimatorParameterType::number;
	}

	inline const char* AnimatorConditionOpToString(AnimatorConditionOp op)
	{
		switch (op)
		{
			case AnimatorConditionOp::greater: return "greater";
			case AnimatorConditionOp::less: return "less";
			case AnimatorConditionOp::equals: return "equals";
			case AnimatorConditionOp::not_equals: return "not_equals";
			default: return "equals";
		}
	}

	inline AnimatorConditionOp AnimatorConditionOpFromString(const std::string& value)
	{
		if (value == "greater")
		{
			return AnimatorConditionOp::greater;
		}
		if (value == "less")
		{
			return AnimatorConditionOp::less;
		}
		if (value == "not_equals")
		{
			return AnimatorConditionOp::not_equals;
		}
		return AnimatorConditionOp::equals;
	}
}
