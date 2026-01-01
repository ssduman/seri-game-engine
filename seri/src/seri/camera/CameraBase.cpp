#include "Seripch.h"

#include "seri/camera/CameraBase.h"

namespace seri
{
	CameraBase::CameraBase(const CameraProperties& cameraProperties) : _cameraProperties(cameraProperties)
	{
	}

	void CameraBase::UpdateView()
	{
		if (_cameraProperties.isOrtho)
		{
			_view = glm::translate(glm::mat4{ 1.0f }, glm::vec3(0.0f, 0.0f, -1.0f));
		}
		else
		{
			glm::vec3 up = GetUp();
			glm::vec3 front = GetFront();

			_view = glm::lookAt(
				_cameraProperties.position,
				_cameraProperties.position + front,
				up
			);
		}
	}

	void CameraBase::UpdateProjection()
	{
		if (_cameraProperties.isOrtho)
		{
			float halfW = _cameraProperties.width * 0.5f;
			float halfH = _cameraProperties.height * 0.5f;

			_projection = glm::ortho(
				-halfW, halfW,
				-halfH, halfH,
				_cameraProperties.nearPlane,
				_cameraProperties.farPlane
			);
		}
		else
		{
			_projection = glm::perspective(
				glm::radians(_cameraProperties.fov),
				_cameraProperties.aspect,
				_cameraProperties.nearPlane,
				_cameraProperties.farPlane
			);
		}
	}

	void CameraBase::UpdateEulerAngles()
	{
		//constexpr glm::vec3 worldUp{ 0.0f, 1.0f, 0.0f };

		//float pitch = glm::radians(_cameraProperties.rotation.x);
		//float yaw = glm::radians(_cameraProperties.rotation.y);

		//glm::vec3 front;
		//front.x = cos(yaw) * cos(pitch);
		//front.y = sin(yaw);
		//front.z = cos(yaw) * sin(pitch);

		//_cameraProperties.front = glm::normalize(front);
		//_cameraProperties.right = glm::normalize(glm::cross(_cameraProperties.front, worldUp));
		//_cameraProperties.up = glm::normalize(glm::cross(_cameraProperties.right, _cameraProperties.front));
	}

	void CameraBase::OnWindowResizeEvent(const event::WindowResizeEventData& data)
	{
		if (data.width <= 0.0f || data.height <= 0.0f)
		{
			return;
		}

		_cameraProperties.width = data.width;
		_cameraProperties.height = data.height;
		_cameraProperties.aspect = data.width / data.height;

		UpdateProjection();
	}

	const bool CameraBase::IsOrtho() const
	{
		return _cameraProperties.isOrtho;
	}

	glm::vec3 CameraBase::GetUp()
	{
		return glm::normalize(_cameraProperties.rotation * glm::vec3(0, 1, 0));
	}

	glm::vec3 CameraBase::GetFront()
	{
		return glm::normalize(_cameraProperties.rotation * glm::vec3(0, 0, -1));
	}

	glm::vec3 CameraBase::GetRight()
	{
		return glm::normalize(_cameraProperties.rotation * glm::vec3(1, 0, 0));
	}

	glm::mat4 CameraBase::BuildViewMatrix()
	{
		glm::mat4 rot = glm::mat4_cast(glm::conjugate(_cameraProperties.rotation));
		glm::mat4 trans = glm::translate(glm::mat4(1.0f), -_cameraProperties.position);
		return rot * trans;
	}

	void CameraBase::SetFromViewMatrix(const glm::mat4& view)
	{
		glm::mat4 world = glm::inverse(view);

		_cameraProperties.position = glm::vec3(world[3]);
		_cameraProperties.rotation = glm::normalize(glm::quat_cast(glm::mat3(world)));
	}

	const glm::mat4& CameraBase::GetView()
	{
		return _view;
	}

	const glm::mat4& CameraBase::GetProjection()
	{
		return _projection;
	}

	glm::mat4 CameraBase::GetViewProjection()
	{
		return _projection * _view;
	}

	CameraProperties& CameraBase::GetCameraProperties()
	{
		return _cameraProperties;
	}

}
