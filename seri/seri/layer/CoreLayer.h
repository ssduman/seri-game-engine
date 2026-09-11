#pragma once

#include "seri/core/Seri.h"
#include "seri/core/TimeWrapper.h"
#include "seri/core/Application.h"
#include "seri/graphic/Graphic.h"
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
#include "seri/system/CameraSystem.h"
#include "seri/system/TransformSystem.h"
#include "seri/system/MeshRendererSystem.h"
#include "seri/system/AudioSystem.h"
#include "seri/system/SpriteRendererSystem.h"
#include "seri/system/TextRendererSystem.h"
#include "seri/system/AnimatorSystem.h"
#include "seri/system/SkinnedMeshRendererSystem.h"
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
					.windowWidth = 1600,
					.windowHeight = 900
				}
			);

			RenderingManager::Instance()->Init(WindowManager::Instance(), RenderingProperties{});

			ShaderLibrary::Init("assets/shaders/");
			asset::AssetManager::Init();
			TimeWrapper::Init();
			Application::Init();
			Graphic::Init();
			InputManager::Init();
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

					auto editorRT = RenderingManager::GetEditorRT();
					editorRT->Resize(data.width, data.height);

					event::WindowResizeEventData editorRTData{
						static_cast<int>(editorRT->GetWidth()),
						static_cast<int>(editorRT->GetHeight())
					};

					Graphic::GetCameraUI()->OnWindowResizeEvent(editorRTData);
					Graphic::GetEditorCamera()->OnWindowResizeEvent(editorRTData);

					return false;
				}
			);

			CameraProperties cameraPropertiesUI;
			cameraPropertiesUI.width = static_cast<float>(RenderingManager::GetEditorRT()->GetWidth());
			cameraPropertiesUI.height = static_cast<float>(RenderingManager::GetEditorRT()->GetHeight());
			cameraPropertiesUI.isOrtho = true;
			cameraPropertiesUI.aspect = RenderingManager::GetEditorRT()->GetAspectRatio();
			auto cameraUI = std::make_shared<EditorCamera>(cameraPropertiesUI);
			cameraUI->Init();

			CameraProperties cameraPropertiesEditor;
			cameraPropertiesEditor.width = static_cast<float>(RenderingManager::GetEditorRT()->GetWidth());
			cameraPropertiesEditor.height = static_cast<float>(RenderingManager::GetEditorRT()->GetHeight());
			cameraPropertiesEditor.isOrtho = false;
			cameraPropertiesEditor.aspect = RenderingManager::GetEditorRT()->GetAspectRatio();
			cameraPropertiesEditor.position = { 0.0f, 4.0f, 6.0f };
			cameraPropertiesEditor.rotation = Util::ToQuaternion({ -30.0f, 0.0f, 0.0f });
			auto cameraEditor = std::make_shared<EditorCamera>(cameraPropertiesEditor);
			cameraEditor->Init();

			Graphic::SetCameraUI(cameraUI);
			Graphic::SetEditorCamera(cameraEditor);

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

			RenderingManager::GetGameRT()->Bind();
			RenderingManager::ClearColor();
			RenderingManager::Clear();

			RenderingManager::GetMainRT()->Bind();
			RenderingManager::ClearColor();
			RenderingManager::Clear();

			Graphic::GetCameraUI()->Update();
			Graphic::GetEditorCamera()->Update();
		}

		void OnUpdate() override
		{
			float deltaTime = TimeWrapper::GetDeltaTime();

			asset::AssetManager::Update();
			scene::SceneManager::Update();

			script::ScriptSystem::Sync();

			bool isPlaying = scene::SceneManager::GetState() == scene::SceneState::play;

			if (isPlaying)
			{
				script::ScriptSystem::Update(deltaTime);
				script::ScriptSystem::LateUpdate(deltaTime);
			}

			system::TransformSystem::Update();
			system::CameraSystem::Update();
			system::LightSystem::Update();

			if (isPlaying)
			{
				system::AnimatorSystem::Update();
			}

			system::MeshRendererSystem::Update();
			system::SkinnedMeshRendererSystem::Update();
			system::AudioSystem::Update();
			system::SpriteRendererSystem::Update();
			system::TextRendererSystem::Update();

			scripting::ScriptingManager::Update();

			debug::DebugDraw::Render(Graphic::GetActiveCamera());
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
