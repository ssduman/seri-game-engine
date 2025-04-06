#pragma once

#include "../event/UserEvent.h"

#include <seri/core/Seri.h>

class Camera : public ICamera
{
public:
	Camera(CameraProperties cameraProperties) : ICamera(cameraProperties)
	{
		//LOGGER(info, "camera init succeeded");
	}

	~Camera() override
	{
		//LOGGER(info, "camera delete succeeded");
	}

	void init() override
	{
		updateEulerAngles();
		updateView();
		updateProjection();
	}

	void update() override
	{
		auto deltaTime = WindowManagerFactory::instance()->getDeltaTime();
		auto movementSpeed = _cameraProperties.speed * deltaTime;

		if (_moveRequest & _wRequest)
		{
			_cameraProperties.position += _cameraProperties.front * movementSpeed;
		}
		if (_moveRequest & _sRequest)
		{
			_cameraProperties.position -= _cameraProperties.front * movementSpeed;
		}
		if (_moveRequest & _aRequest)
		{
			_cameraProperties.position -= _cameraProperties.right * movementSpeed;
		}
		if (_moveRequest & _dRequest)
		{
			_cameraProperties.position += _cameraProperties.right * movementSpeed;
		}

		updateView();
	}

	void onKeyEvent(const KeyEventData& data) override
	{
		if (!isPlayable())
		{
			return;
		}

		if (Input::isPressed(data, KeyCode::w))
		{
			_moveRequest |= _wRequest;
		}
		else if (Input::isReleased(data, KeyCode::w))
		{
			_moveRequest ^= _wRequest;
		}

		if (Input::isPressed(data, KeyCode::s))
		{
			_moveRequest |= _sRequest;
		}
		else if (Input::isReleased(data, KeyCode::s))
		{
			_moveRequest ^= _sRequest;
		}

		if (Input::isPressed(data, KeyCode::a))
		{
			_moveRequest |= _aRequest;
		}
		else if (Input::isReleased(data, KeyCode::a))
		{
			_moveRequest ^= _aRequest;
		}

		if (Input::isPressed(data, KeyCode::d))
		{
			_moveRequest |= _dRequest;
		}
		else if (Input::isReleased(data, KeyCode::d))
		{
			_moveRequest ^= _dRequest;
		}
	}

	void onUserEvent(IEventData& data) override
	{
		auto& d = dynamic_cast<IUserEventData&>(data);
		if (d.userEventType == UserEventType::game_state)
		{
			auto& userData = dynamic_cast<UserGameStateEventData&>(d);
			_gameState = userData.gameState;
		}
	}

	bool isPlayable() override
	{
		return _gameState == GameState::game;
	}

private:
	int _wRequest{ 0x0001 };
	int _sRequest{ 0x0010 };
	int _aRequest{ 0x0100 };
	int _dRequest{ 0x1000 };
	int _moveRequest{ 0x0000 };

	GameState _gameState{ GameState::idle };

};
