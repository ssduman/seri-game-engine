#pragma once

#include "seri/core/Seri.h"
#include "seri/core/TimeWrapper.h"
#include "seri/core/Application.h"
#include "seri/graphic/Graphic.h"
#include "seri/font/FontManager.h"
#include "seri/asset/AssetManager.h"
#include "seri/input/InputManager.h"
#include "seri/sound/SoundManager.h"
#include "seri/scene/SceneManager.h"
#include "seri/shader/ShaderLibrary.h"
#include "seri/rendering/RenderingManager.h"
#include "seri/behaviour/BehaviourManager.h"
#include "seri/texture/Skybox.h"
#include "seri/camera/EditorCamera.h"
#include "seri/event/EventCallback.h"
#include "seri/event/EventDispatcher.h"
#include "seri/scripting/ScriptingManager.h"

namespace seri
{
	class CoreLayer : public LayerBase
	{
	public:
		CoreLayer() : LayerBase("CoreLayer")
		{
			srand(static_cast<unsigned int>(time(0)));

			seri::WindowManager::Instance()->Init({ /*title*/ "Seri Game Engine - Editor", /*fullscreen*/ false, /*w*/ 1280, /*h*/ 720 });
			seri::RenderingManager::Instance()->Init(seri::WindowManager::Instance(), RenderingProperties{});

			seri::asset::AssetManager::Init();
			seri::TimeWrapper::Init();
			seri::Application::Init();
			seri::Graphic::Init();
			seri::InputManager::Init();
			seri::BehaviourManager::Init();
			seri::ShaderLibrary::Init("assets/shaders/");
			seri::font::FontManager::Init("assets/fonts/");
			seri::sound::SoundManager::Init("assets/sounds/");
			seri::scene::SceneManager::Init();
			seri::asset::AssetManager::StartAssetWatcher();
			seri::scripting::ScriptingManager::Init();

			seri::WindowManager::Instance()->AddEventCallback(seri::event::MakeEventCallback(
				[](const seri::event::IEventData& data)
				{
					seri::event::EventDispatcher{}(data);
				}
			));

			seri::WindowManager::Instance()->AddEventCallback(seri::event::MakeEventCallback(
				[](const seri::event::IEventData& data)
				{
					if (data.eventType == seri::event::EventType::window_resize)
					{
						auto& d = seri::event::EventDispatcher::CastEventData<seri::event::WindowResizeEventData&>(data);

						seri::RenderingManager::SetViewport(0, 0, d.width, d.height);

						seri::RenderingManager::GetEditorRT()->Bind();
						seri::RenderingManager::GetEditorRT()->Resize(d.width, d.height);
						seri::RenderingManager::GetEditorRT()->Unbind();

						seri::Graphic::GetCameraOrtho()->OnWindowResizeEvent(d);
						seri::Graphic::GetCameraPerspective()->OnWindowResizeEvent(d);
					}
				}
			));

			seri::CameraProperties cameraPropertiesOrtho;
			cameraPropertiesOrtho.width = seri::WindowManager::Instance()->GetWindowProperties().windowWidth;
			cameraPropertiesOrtho.height = seri::WindowManager::Instance()->GetWindowProperties().windowHeight;
			cameraPropertiesOrtho.isOrtho = true;
			cameraPropertiesOrtho.aspect = seri::WindowManager::GetAspectRatio();
			auto cameraOrtho = std::make_shared<seri::EditorCamera>(std::move(cameraPropertiesOrtho));
			cameraOrtho->Init();

			seri::CameraProperties cameraPropertiesPerspective;
			cameraPropertiesPerspective.width = seri::WindowManager::Instance()->GetWindowProperties().windowWidth;
			cameraPropertiesPerspective.height = seri::WindowManager::Instance()->GetWindowProperties().windowHeight;
			cameraPropertiesPerspective.isOrtho = false;
			cameraPropertiesPerspective.aspect = seri::WindowManager::GetAspectRatio();
			cameraPropertiesPerspective.position = { 0.0f, 4.0f, 6.0f };
			cameraPropertiesPerspective.rotation = { -90.0f, -30.0f, 0.0f };
			auto cameraPerspective = std::make_shared<seri::EditorCamera>(std::move(cameraPropertiesPerspective));
			cameraPerspective->Init();

			seri::Graphic::AddCamera(cameraOrtho);
			seri::Graphic::AddCamera(cameraPerspective);

			seri::Application::SetVSyncCount(1);
			seri::Application::SetTargetFrameRate(60);
		}

		~CoreLayer() override = default;

		void PreUpdate() override
		{
			seri::Application::SetFrameBegin();
		}

		void Update() override
		{
			seri::RenderingManager::GetEditorRT()->Bind();
			seri::RenderingManager::ClearColor();
			seri::RenderingManager::Clear();

			seri::RenderingManager::GetMainRT()->Bind();
			seri::RenderingManager::ClearColor();
			seri::RenderingManager::Clear();

			seri::TimeWrapper::UpdateTime(seri::WindowManager::GetTime());

			seri::Graphic::GetCameraOrtho()->Update();
			seri::Graphic::GetCameraPerspective()->Update();

			seri::asset::AssetManager::Update();
			seri::scene::SceneManager::Update();
			seri::scripting::ScriptingManager::Update();

			seri::BehaviourManager::UpdateBehaviours();

			seri::RenderingManager::Execute();
		}

		void PostUpdate() override
		{
			seri::InputManager::Reset();

			seri::RenderingManager::GetEditorRT()->Bind();
			seri::WindowManager::PollEvents();
			seri::WindowManager::SwapBuffers();

			seri::Application::SetFrameEnd();

			double waitTime = seri::Application::GetWaitTime();
			if (waitTime > 0.0)
			{
				//std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(waitTime));
			}
		}

	};
}
