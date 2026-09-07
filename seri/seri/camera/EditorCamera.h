#pragma once

#include "seri/camera/CameraBase.h"

namespace seri
{
	class EditorCamera : public seri::CameraBase
	{
	public:
		EditorCamera(seri::CameraProperties cameraProperties) : CameraBase(cameraProperties)
		{
		}

		~EditorCamera() override = default;

		void Init() override
		{
			UpdateView();
			UpdateProjection();
		}

		void Update() override
		{
			UpdateView();
			UpdateProjection();
		}

		void Look(float deltaX, float deltaY)
		{
			glm::vec3 worldUp{ 0.0f, 1.0f, 0.0f };

			glm::quat yaw = glm::angleAxis(glm::radians(-deltaX * _sensitivity), worldUp);
			glm::quat rotation = glm::normalize(yaw * _cameraProperties.rotation);

			glm::vec3 right = glm::normalize(rotation * glm::vec3{ 1.0f, 0.0f, 0.0f });
			glm::quat pitch = glm::angleAxis(glm::radians(-deltaY * _sensitivity), right);
			glm::quat pitched = glm::normalize(pitch * rotation);

			glm::vec3 front = glm::normalize(pitched * glm::vec3{ 0.0f, 0.0f, -1.0f });
			if (glm::abs(glm::dot(front, worldUp)) < kPitchLimit)
			{
				rotation = pitched;
			}

			_cameraProperties.rotation = rotation;
		}

		void Move(const glm::vec3& localDirection, float deltaTime, bool fast)
		{
			glm::vec3 direction =
				GetRight() * localDirection.x +
				GetUp() * localDirection.y +
				GetFront() * localDirection.z;

			float length = glm::length(direction);
			if (length <= 0.0001f)
			{
				return;
			}

			float speed = fast ? _speed * 2.0f : _speed;

			_cameraProperties.position += (direction / length) * speed * deltaTime;
		}

		void Pan(float deltaX, float deltaY, float viewportHeight)
		{
			if (viewportHeight <= 0.0f)
			{
				return;
			}

			float halfFov = glm::radians(_cameraProperties.fov * 0.5f);
			float worldPerPixel = 2.0f * GetPivotDistance() * glm::tan(halfFov) / viewportHeight;

			_cameraProperties.position += GetRight() * (-deltaX * worldPerPixel) + GetUp() * (deltaY * worldPerPixel);
		}

		void Zoom(float delta)
		{
			_cameraProperties.position += GetFront() * (delta * _speed * kZoomScale);
		}

		float GetPivotDistance()
		{
			float height = _cameraProperties.position.y;
			float forwardY = GetFront().y;

			if (height > 0.0f && forwardY < -0.0001f)
			{
				return glm::clamp(height / -forwardY, kMinPivotDistance, kMaxPivotDistance);
			}

			return glm::clamp(glm::abs(height), kMinPivotDistance, kMaxPivotDistance);
		}

		float GetSpeed() const
		{
			return _speed;
		}

		void SetSpeed(float speed)
		{
			_speed = speed;
		}

		float GetSensitivity() const
		{
			return _sensitivity;
		}

		void SetSensitivity(float sensitivity)
		{
			_sensitivity = sensitivity;
		}

	private:
		float _speed{ 5.0f };
		float _sensitivity{ 0.1f };

		static constexpr float kPitchLimit = 0.999f;
		static constexpr float kMinPivotDistance = 1.0f;
		static constexpr float kMaxPivotDistance = 100.0f;
		static constexpr float kZoomScale = 0.1f;

	};
}
