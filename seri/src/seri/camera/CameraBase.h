#pragma once

#include "seri/util/Util.h"
#include "seri/core/Object.h"

namespace seri
{
	struct CameraProperties
	{
		float fov{ 45.0f };
		float aspect{ 16.0f / 9.0f };
		float nearPlane{ 0.1f };
		float farPlane{ 1000.0f };

		bool isOrtho{ false };
		float width{ 1920.0f };
		float height{ 1080.0f };

		float speed{ 5.0f };
		float sensitivity{ 0.1f };

		glm::vec3 position{ 0.0f, 0.0f, 0.0f };
		glm::quat rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
	};

	class CameraBase
	{
	public:
		CameraBase(const CameraProperties& cameraProperties);
		virtual ~CameraBase() = default;

		virtual void Init() = 0;
		virtual void Update() = 0;

		void OnWindowResizeEvent(const event::WindowResizeEventData& data);

		const bool IsOrtho() const;

		glm::vec3 GetUp();
		glm::vec3 GetFront();
		glm::vec3 GetRight();

		glm::mat4 BuildViewMatrix();
		void SetFromViewMatrix(const glm::mat4& view);

		const glm::mat4& GetView();
		const glm::mat4& GetProjection();
		glm::mat4 GetViewProjection();

		CameraProperties& GetCameraProperties();

	protected:
		void UpdateEulerAngles();
		void UpdateView();
		void UpdateProjection();

		CameraProperties _cameraProperties;

		glm::mat4 _view{ 1.0f };
		glm::mat4 _projection{ 1.0f };

	};
}
