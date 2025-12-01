#pragma once

#include <seri/core/Seri.h>

enum class GameState
{
	idle,
	menu,
	game,
	game_over,
};

enum class UserEventType
{
	game_state,
	unknown,
};

inline const char* ToString(GameState gameState)
{
	switch (gameState)
	{
		case GameState::idle: return "idle";
		case GameState::menu: return "menu";
		case GameState::game: return "game";
		case GameState::game_over: return "game_over";
		default: return "";
	}
}

inline const char* ToString(UserEventType userEventType)
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

	std::string ToString() override
	{
		return "IUserEventData";
	}

	UserEventType userEventType{ UserEventType::unknown };
};

struct UserGameStateEventData : public IUserEventData
{
	UserGameStateEventData(GameState gameState_) : IUserEventData(UserEventType::game_state), gameState(gameState_) {}

	~UserGameStateEventData() override = default;

	std::string ToString() override
	{
		std::stringstream ss;
		ss << seri::event::ToString(eventType) << "/" << ::ToString(userEventType) << ": "
			<< "{"
			<< "'gameState': " << ::ToString(gameState)
			<< "}";

		return ss.str();
	}

	GameState gameState;
};
