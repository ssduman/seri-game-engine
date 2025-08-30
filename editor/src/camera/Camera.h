#pragma once

#include "event/UserEvent.h"

#include <seri/core/Seri.h>

class Camera : public seri::ICamera
{
public:
	Camera(seri::CameraProperties cameraProperties) : ICamera(cameraProperties)
	{
		//LOGGER(info, "camera init succeeded");
	}

	~Camera() override
	{
		//LOGGER(info, "camera delete succeeded");
	}

	void Init() override
	{
		UpdateEulerAngles();
		UpdateView();
		UpdateProjection();
	}

	void Update() override
	{
		auto deltaTime = seri::WindowManagerFactory::Instance()->GetDeltaTime();
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

		UpdateView();
	}

	void OnKeyEvent(const seri::event::KeyEventData& data) override
	{
		if (!IsPlayable())
		{
			return;
		}

		if (seri::Input::IsPressed(data, seri::KeyCode::w))
		{
			_moveRequest |= _wRequest;
		}
		else if (seri::Input::IsReleased(data, seri::KeyCode::w))
		{
			_moveRequest ^= _wRequest;
		}

		if (seri::Input::IsPressed(data, seri::KeyCode::s))
		{
			_moveRequest |= _sRequest;
		}
		else if (seri::Input::IsReleased(data, seri::KeyCode::s))
		{
			_moveRequest ^= _sRequest;
		}

		if (seri::Input::IsPressed(data, seri::KeyCode::a))
		{
			_moveRequest |= _aRequest;
		}
		else if (seri::Input::IsReleased(data, seri::KeyCode::a))
		{
			_moveRequest ^= _aRequest;
		}

		if (seri::Input::IsPressed(data, seri::KeyCode::d))
		{
			_moveRequest |= _dRequest;
		}
		else if (seri::Input::IsReleased(data, seri::KeyCode::d))
		{
			_moveRequest ^= _dRequest;
		}
	}

	void OnUserEvent(seri::event::IEventData& data) override
	{
		auto& d = dynamic_cast<IUserEventData&>(data);
		if (d.userEventType == UserEventType::game_state)
		{
			auto& userData = dynamic_cast<UserGameStateEventData&>(d);
			_gameState = userData.gameState;
		}
	}

	bool IsPlayable() override
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
