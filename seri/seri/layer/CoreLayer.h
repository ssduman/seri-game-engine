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
#include "seri/rendering/render/RenderingManager.h"
#include "seri/texture/Skybox.h"
#include "seri/camera/EditorCamera.h"
#include "seri/event/EventManager.h"
#include "seri/event/EventDispatcher.h"
#include "seri/scripting/ScriptingManager.h"
#include "seri/system/LightSystem.h"
#include "seri/system/TransformSystem.h"
#include "seri/system/MeshRendererSystem.h"
#include "seri/script/ScriptSystem.h"
#include "seri/draw/DebugDraw.h"
#include "seri/logging/Logger.h"

namespace seri
{
	class CoreLayer : public LayerBase
	{
	public:
		CoreLayer() : LayerBase("CoreLayer")
		{
			srand(static_cast<unsigned int>(time(0)));

			LoggerConfig loggerConfig;
			loggerConfig.level = LogLevel::info;
			Logger::Init(loggerConfig);

			WindowManager::Instance()->Init(
				{
					.windowTitle = "Seri Game Engine - Editor",
					.isFullscreen = false,
					.windowWidth = 1280,
					.windowHeight = 720
				}
			);

			RenderingManager::Instance()->Init(WindowManager::Instance(), RenderingProperties{});

			ShaderLibrary::Init("assets/shaders/");
			asset::AssetManager::Init();
			TimeWrapper::Init();
			Application::Init();
			Graphic::Init();
			InputManager::Init();
			font::FontManager::Init("assets/fonts/");
			sound::SoundManager::Init("assets/sounds/");
			scene::SceneManager::Init();
			asset::AssetManager::StartAssetWatcher();
			scripting::ScriptingManager::Init();
			script::ScriptSystem::Init();
			debug::DebugDraw::Init();

			event::EventManager::Subscribe<event::WindowResizeEventData>(
				[](const event::WindowResizeEventData& data) -> bool
				{
					RenderingManager::SetViewport(0, 0, data.width, data.height);

					RenderingManager::GetEditorRT()->Bind();
					RenderingManager::GetEditorRT()->Resize(data.width, data.height);
					RenderingManager::GetEditorRT()->Unbind();

					Graphic::GetCameraOrtho()->OnWindowResizeEvent(data);
					Graphic::GetCameraPerspective()->OnWindowResizeEvent(data);

					return false;
				}
			);

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
		}

		~CoreLayer() override
		{
		}

		void OnPreUpdate() override
		{
			Application::SetFrameBegin();

			TimeWrapper::UpdateTime(WindowManager::GetTime());

			RenderingManager::Begin();

			RenderingManager::GetEditorRT()->Bind();
			RenderingManager::ClearColor();
			RenderingManager::Clear();

			RenderingManager::GetMainRT()->Bind();
			RenderingManager::ClearColor();
			RenderingManager::Clear();

			Graphic::GetCameraOrtho()->Update();
			Graphic::GetCameraPerspective()->Update();
		}

		void OnUpdate() override
		{
			float deltaTime = TimeWrapper::GetDeltaTime();

			asset::AssetManager::Update();
			scene::SceneManager::Update();

			script::ScriptSystem::Update(deltaTime);
			script::ScriptSystem::LateUpdate(deltaTime);

			system::TransformSystem::Update();
			system::LightSystem::Update();
			system::MeshRendererSystem::Update();

			scripting::ScriptingManager::Update();

			debug::DebugDraw::Render(Graphic::GetCameraPerspective());
			debug::DebugDraw::EndFrame();
		}

		void OnRender() override
		{
			RenderingManager::Execute();

			RenderingManager::End();
		}

		void OnPostUpdate() override
		{
			InputManager::Reset();

			WindowManager::PollEvents();
			WindowManager::SwapBuffers();

			Application::SetFrameEnd();

			Application::WaitForTargetFrameRate();
		}

	};
}
