#pragma once

#include <core/Seri.h>
#include <event/EventData.h>
#include <core/TimeWrapper.h>
#include <camera/CameraBase.h>
#include <input/Input.h>
#include <input/KeyCode.h>
#include <input/InputManager.h>

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
