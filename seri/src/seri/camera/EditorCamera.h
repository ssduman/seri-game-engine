#pragma once

#include <seri/core/Seri.h>
#include <seri/event/EventData.h>
#include <seri/core/TimeWrapper.h>
#include <seri/camera/CameraBase.h>
#include <seri/input/Input.h>
#include <seri/input/KeyCode.h>
#include <seri/input/InputManager.h>

namespace seri
{
	class EditorCamera : public seri::CameraBase
	{
	public:
		EditorCamera(seri::CameraProperties cameraProperties) : CameraBase(cameraProperties)
		{
			//LOGGER(info, "camera init succeeded");
		}

		~EditorCamera() override
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
			auto deltaTime = seri::TimeWrapper::GetDeltaTime();
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

		bool IsPlayable() override
		{
			return false;
		}

	private:
		int _wRequest{ 0x0001 };
		int _sRequest{ 0x0010 };
		int _aRequest{ 0x0100 };
		int _dRequest{ 0x1000 };
		int _moveRequest{ 0x0000 };

	};
}
