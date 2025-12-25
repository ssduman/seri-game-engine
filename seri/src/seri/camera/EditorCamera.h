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
			UpdateEulerAngles();
			UpdateView();
			UpdateProjection();
		}

	private:

	};
}
