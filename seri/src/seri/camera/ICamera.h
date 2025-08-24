#pragma once

#include "seri/util/Util.h"
#include "seri/core/Object.h"
#include "seri/logging/Logger.h"
#include "seri/camera/CameraProperties.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

namespace seri
{
	class ICamera : public Object
	{
	public:
		ICamera(CameraProperties cameraProperties) : _cameraProperties{ cameraProperties } {}

		~ICamera() override = default;

		void Render() override {}

		void OnWindowResizeEvent(const event::WindowResizeEventData& data) override
		{
			auto width = static_cast<float>(data.width);
			auto height = static_cast<float>(data.height);

			if (width <= 0.1f || height <= 0.1f)
			{
				return;
			}

			_cameraProperties.width = width;
			_cameraProperties.height = height;
			_cameraProperties.aspect = width / height;

			UpdateProjection();
		}

		void OnMousePositionEvent(const event::MousePositionEventData& data) override
		{
			if (!IsPlayable())
			{
				return;
			}

			auto xpos = static_cast<float>(data.xpos);
			auto ypos = static_cast<float>(data.ypos);

			if (!_init)
			{
				_init = true;
				_xPosLast = xpos;
				_yPosLast = ypos;
			}

			const auto deltaX = xpos - _xPosLast;
			const auto deltaY = _yPosLast - ypos;

			_xPosLast = xpos;
			_yPosLast = ypos;

			float& _yaw = _cameraProperties.rotation.x;
			float& _pitch = _cameraProperties.rotation.y;

			_yaw += deltaX * _cameraProperties.sensitivity;
			_pitch += deltaY * _cameraProperties.sensitivity;

			if (_pitch > 89.0f)
			{
				_pitch = 89.0f;
			}
			if (_pitch < -89.0f)
			{
				_pitch = -89.0f;
			}

			while (_yaw < -180.0f)
			{
				_yaw += 360.0f;
			}
			while (_yaw > 180.0f)
			{
				_yaw -= 360.0f;
			}

			UpdateEulerAngles();
			UpdateView();
		}

		glm::vec3 GetPosition() override
		{
			return _cameraProperties.position;
		}

		void SetPosition(glm::vec3 position) override
		{
			_cameraProperties.position = position;
		}

		virtual bool IsPlayable() = 0;

		const glm::mat4& GetModel()
		{
			return _model;
		}

		const glm::mat4& GetView()
		{
			return _view;
		}

		const glm::mat4& GetProjection()
		{
			return _projection;
		}

		const bool IsOrtho() const
		{
			return _cameraProperties.isOrtho;
		}

		const CameraProperties& GetCameraProperties()
		{
			return _cameraProperties;
		}

		void SetCameraProperties(CameraProperties cameraProperties)
		{
			_cameraProperties = std::move(cameraProperties);
		}

	protected:
		virtual void UpdateView()
		{
			if (_cameraProperties.isOrtho)
			{
				_view = glm::translate(glm::mat4{ 1.0f }, glm::vec3(0.0f, 0.0f, -1.0f));
			}
			else
			{
				auto quat = glm::quat(
					glm::vec3{
						glm::radians(_cameraProperties.rotation.x),
						glm::radians(_cameraProperties.rotation.y),
						glm::radians(_cameraProperties.rotation.z)
					}
				);

				_view = glm::lookAt(
					_cameraProperties.position,
					_cameraProperties.position + _cameraProperties.front,
					_cameraProperties.up
				);
			}
		}

		virtual void UpdateProjection()
		{
			if (_cameraProperties.isOrtho)
			{
				_projection = glm::ortho(
					0.0f,
					_cameraProperties.width,
					_cameraProperties.height,
					0.0f,
					_cameraProperties.near,
					_cameraProperties.far
				);
			}
			else
			{
				_projection = glm::perspective(
					glm::radians(_cameraProperties.fov),
					_cameraProperties.aspect,
					_cameraProperties.near,
					_cameraProperties.far
				);
			}
		}

		virtual void UpdateEulerAngles()
		{
			glm::vec3 eulerAngle{};
			eulerAngle.x = cos(glm::radians(_cameraProperties.rotation.y)) * cos(glm::radians(_cameraProperties.rotation.x));
			eulerAngle.y = sin(glm::radians(_cameraProperties.rotation.y));
			eulerAngle.z = cos(glm::radians(_cameraProperties.rotation.y)) * sin(glm::radians(_cameraProperties.rotation.x));

			_cameraProperties.front = glm::normalize(eulerAngle);
			_cameraProperties.right = glm::normalize(glm::cross(_cameraProperties.front, _cameraProperties.up));
		}

		CameraProperties _cameraProperties;

		glm::mat4 _model{ 1.0f };
		glm::mat4 _view{};
		glm::mat4 _projection{};

		bool _init{ false };
		float _xPosLast{ -1.0f };
		float _yPosLast{ -1.0f };

	};
}
