#pragma once

#include "seri/util/Util.h"
#include "seri/core/Object.h"
#include "seri/logging/Logger.h"
#include "seri/camera/CameraProperties.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

class ICamera : public Object
{
public:
	ICamera(CameraProperties cameraProperties) : _cameraProperties{ cameraProperties } {}

	~ICamera() override = default;

	void render() override {}

	void onWindowResizeEvent(const WindowResizeEventData& data) override
	{
		auto width = static_cast<float>(data.width);
		auto height = static_cast<float>(data.height);

		_cameraProperties.width = width;
		_cameraProperties.height = height;
		_cameraProperties.aspect = width / height;
		updateProjection();
	}

	void onMousePositionEvent(const MousePositionEventData& data) override
	{
		if (!isPlayable())
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

		updateEulerAngles();
		updateView();
	}

	virtual bool isPlayable() = 0;

	const glm::mat4& getModel()
	{
		return _model;
	}

	const glm::mat4& getView()
	{
		return _view;
	}

	const glm::mat4& getProjection()
	{
		return _projection;
	}

	const bool IsOrtho() const
	{
		return _cameraProperties.isOrtho;
	}

	const CameraProperties& getCameraProperties()
	{
		return _cameraProperties;
	}

	void setCameraProperties(CameraProperties cameraProperties)
	{
		_cameraProperties = std::move(cameraProperties);
	}

protected:
	virtual void updateView()
	{
		if (_cameraProperties.isOrtho)
		{
			_view = glm::translate(glm::mat4{ 1.0f }, glm::vec3(0.0f, 0.0f, -1.0f));
		}
		else
		{
			_view = glm::lookAt(
				_cameraProperties.position,
				_cameraProperties.position + _cameraProperties.front,
				_cameraProperties.up
			);
		}
	}

	virtual void updateProjection()
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

	virtual void updateEulerAngles()
	{
		glm::vec3 eulerAngle{};
		eulerAngle.x = cos(glm::radians(_pitch)) * cos(glm::radians(_yaw));
		eulerAngle.y = sin(glm::radians(_pitch));
		eulerAngle.z = cos(glm::radians(_pitch)) * sin(glm::radians(_yaw));

		_cameraProperties.front = glm::normalize(eulerAngle);
		_cameraProperties.right = glm::normalize(glm::cross(_cameraProperties.front, _cameraProperties.up));
	}

	CameraProperties _cameraProperties;

	glm::mat4 _model{ 1.0f };
	glm::mat4 _view{};
	glm::mat4 _projection{};

	bool _init{ false };
	float _roll{ 0.0f };
	float _pitch{ 0.0f };
	float _yaw{ 90.0f };
	float _xPosLast{ -1.0f };
	float _yPosLast{ -1.0f };

};
