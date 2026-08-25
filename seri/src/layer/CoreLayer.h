#pragma once

#include "core/Seri.h"
#include "core/TimeWrapper.h"
#include "core/Application.h"
#include "graphic/Graphic.h"
#include "font/FontManager.h"
#include "asset/AssetManager.h"
#include "input/InputManager.h"
#include "sound/SoundManager.h"
#include "scene/SceneManager.h"
#include "shader/ShaderLibrary.h"
#include "rendering/render/RenderingManager.h"
#include "behaviour/BehaviourManager.h"
#include "texture/Skybox.h"
#include "camera/EditorCamera.h"
#include "event/EventCallback.h"
#include "event/EventDispatcher.h"
#include "scripting/ScriptingManager.h"
#include "system/LightSystem.h"
#include "system/TransformSystem.h"
#include "system/MeshRendererSystem.h"
#include "draw/DebugDraw.h"

namespace seri
{
	class CoreLayer : public LayerBase
	{
	public:
		CoreLayer() : LayerBase("CoreLayer")
		{
			srand(static_cast<unsigned int>(time(0)));

			WindowManager::Instance()->Init({ /*title*/ "Seri Game Engine - Editor", /*fullscreen*/ false, /*w*/ 1280, /*h*/ 720 });
			RenderingManager::Instance()->Init(WindowManager::Instance(), RenderingProperties{});

			ShaderLibrary::Init("assets/shaders/");
			asset::AssetManager::Init();
			TimeWrapper::Init();
			Application::Init();
			Graphic::Init();
			InputManager::Init();
			BehaviourManager::Init();
			font::FontManager::Init("assets/fonts/");
			sound::SoundManager::Init("assets/sounds/");
			scene::SceneManager::Init();
			asset::AssetManager::StartAssetWatcher();
			scripting::ScriptingManager::Init();
			debug::DebugDraw::Init();

			WindowManager::Instance()->AddEventCallback(event::MakeEventCallback(
				[](const event::IEventData& data)
				{
					event::EventDispatcher{}(data);
				}
			));

			WindowManager::Instance()->AddEventCallback(event::MakeEventCallback(
				[](const event::IEventData& data)
				{
					if (data.eventType == event::EventType::window_resize)
					{
						auto& d = event::EventDispatcher::CastEventData<event::WindowResizeEventData&>(data);

						RenderingManager::SetViewport(0, 0, d.width, d.height);

						RenderingManager::GetEditorRT()->Bind();
						RenderingManager::GetEditorRT()->Resize(d.width, d.height);
						RenderingManager::GetEditorRT()->Unbind();

						Graphic::GetCameraOrtho()->OnWindowResizeEvent(d);
						Graphic::GetCameraPerspective()->OnWindowResizeEvent(d);
					}
				}
			));

			CameraProperties cameraPropertiesOrtho;
			cameraPropertiesOrtho.width = WindowManager::Instance()->GetWindowProperties().windowWidth;
			cameraPropertiesOrtho.height = WindowManager::Instance()->GetWindowProperties().windowHeight;
			cameraPropertiesOrtho.isOrtho = true;
			cameraPropertiesOrtho.aspect = WindowManager::GetAspectRatio();
			auto cameraOrtho = std::make_shared<EditorCamera>(cameraPropertiesOrtho);
			cameraOrtho->Init();

			CameraProperties cameraPropertiesPerspective;
			cameraPropertiesPerspective.width = WindowManager::Instance()->GetWindowProperties().windowWidth;
			cameraPropertiesPerspective.height = WindowManager::Instance()->GetWindowProperties().windowHeight;
			cameraPropertiesPerspective.isOrtho = false;
			cameraPropertiesPerspective.aspect = WindowManager::GetAspectRatio();
			cameraPropertiesPerspective.position = { 0.0f, 4.0f, 6.0f };
			cameraPropertiesPerspective.rotation = Util::ToQuaternion({ -30.0f, 0.0f, 0.0f });
			auto cameraPerspective = std::make_shared<EditorCamera>(cameraPropertiesPerspective);
			cameraPerspective->Init();

			Graphic::AddCamera(cameraOrtho);
			Graphic::AddCamera(cameraPerspective);

			RenderingManager::Init();

			Application::SetVSyncCount(1);
			Application::SetTargetFrameRate(60);
		}

		~CoreLayer() override = default;

		void PreUpdate() override
		{
			Application::SetFrameBegin();
		}

		void Update() override
		{
			RenderingManager::Begin();

			RenderingManager::GetEditorRT()->Bind();
			RenderingManager::ClearColor();
			RenderingManager::Clear();

			RenderingManager::GetMainRT()->Bind();
			RenderingManager::ClearColor();
			RenderingManager::Clear();

			TimeWrapper::UpdateTime(WindowManager::GetTime());

			Graphic::GetCameraOrtho()->Update();
			Graphic::GetCameraPerspective()->Update();

			asset::AssetManager::Update();
			scene::SceneManager::Update();

			system::TransformSystem::Update();
			system::MeshRendererSystem::Update();
			system::LightSystem::Update();
			scripting::ScriptingManager::Update();

			BehaviourManager::UpdateBehaviours();

			debug::DebugDraw::Render(Graphic::GetCameraPerspective());
			debug::DebugDraw::EndFrame();

			RenderingManager::Execute();

			RenderingManager::End();
		}

		void PostUpdate() override
		{
			InputManager::Reset();

			RenderingManager::GetEditorRT()->Bind();
			WindowManager::PollEvents();
			WindowManager::SwapBuffers();

			Application::SetFrameEnd();

			double waitTime = Application::GetWaitTime();
			if (waitTime > 0.0)
			{
				//std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(waitTime));
			}
		}

	};
}
