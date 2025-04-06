#pragma once

#include "GameState.h"

#include <seri/core/Seri.h>

enum class UserEventType
{
	game_state,
	unknown,
};

inline const char* toString(UserEventType userEventType)
{
	switch (userEventType)
	{
		case UserEventType::game_state: return "game_state";
		case UserEventType::unknown: return "unknown";
		default: return "";
	}
}

struct IUserEventData : public seri::event::IEventData
{
	IUserEventData(UserEventType type_) : IEventData(seri::event::EventType::user), userEventType(type_) {}

	~IUserEventData() override = default;

	UserEventType userEventType{ UserEventType::unknown };
};

struct UserGameStateEventData : public IUserEventData
{
	UserGameStateEventData(GameState gameState_) : IUserEventData(UserEventType::game_state), gameState(gameState_) {}

	~UserGameStateEventData() override = default;

	std::string toString() override
	{
		std::stringstream ss;
		ss << seri::event::toString(eventType) << "/" << ::toString(userEventType) << ": {"
			<< "'gameState': " << ::toString(gameState)
			<< "}";

		return ss.str();
	}

	GameState gameState;
};
