#pragma once

#include "seri/camera/CameraBase.h"

namespace seri
{
	class RuntimeCamera : public seri::CameraBase
	{
	public:
		RuntimeCamera(seri::CameraProperties cameraProperties) : CameraBase(cameraProperties)
		{
		}

		~RuntimeCamera() override = default;

		void Init() override
		{
			Update();
		}

		void Update() override
		{
			_view = BuildViewMatrix();
			UpdateProjection();
		}

	private:

	};
}
