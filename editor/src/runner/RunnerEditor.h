#pragma once

#include <seri/core/Seri.h>
#include <seri/project/ProjectManager.h>

#include "gui/common/GUIBackend.h"
#include "layer/EditorLayer.h"
#include "layer/SandboxLayer.h"
#include "layer/LauncherLayer.h"

namespace seri::editor
{
	class RunnerEditor : public seri::IRunner
	{
	public:
		RunnerEditor() = default;

		~RunnerEditor() override = default;

		void operator()(int argc, char* argv[])
		{
			InitPlatform();

			std::filesystem::path projectPath;

			RunLauncher(projectPath);
			RunEditor(projectPath);
		}

	private:
		void InitPlatform()
		{
			seri::LoggerConfig loggerConfig;
			loggerConfig.level = seri::LogLevel::info;
			seri::Logger::Init(loggerConfig);

			seri::InputManager::Init();

			seri::WindowManager::Instance()->Init(
				{
					.windowTitle = kWindowTitle,
					.isFullscreen = kIsFullscreen,
					.windowWidth = kLauncherWidth,
					.windowHeight = kLauncherHeight
				}
			);

			seri::WindowManager::Instance()->AddProcessEventDelegate(
				[](const void* event)
				{
					seri::editor::GUIBackend::ProcessEvent(event);
				}
			);

			seri::RenderingManager::Instance()->Init(seri::WindowManager::Instance(), seri::RenderingProperties{});
		}

		void RunFrame(seri::LayerManager& layerManager)
		{
			layerManager.OnPreUpdate();
			layerManager.OnUpdate();
			layerManager.OnRender();
			layerManager.OnPostUpdate();
		}

		void RunLauncher(std::filesystem::path& projectPath)
		{
			auto launcherLayer = std::make_shared<seri::editor::LauncherLayer>();

			seri::LayerManager launcherLayerManager{};
			launcherLayerManager.AddLayer(launcherLayer);

			LIB_LOGGER(info, editor) << "seri game engine - launcher loop starting";

			while (!seri::WindowManager::GetWindowShouldClose() && launcherLayer->GetProjectPath().empty())
			{
				RunFrame(launcherLayerManager);
			}

			projectPath = launcherLayer->GetProjectPath();

			LIB_LOGGER(info, editor) << "seri game engine - launcher loop stopped";
		}

		void RunEditor(std::filesystem::path& projectPath)
		{
			if (projectPath.empty())
			{
				return;
			}

			seri::WindowManager::SetWindowTitle(fmt::format("{} - {}", kWindowTitle, seri::project::ProjectManager::GetName()).c_str());
			seri::WindowManager::SetWindowSize(kEditorWidth, kEditorHeight);
			seri::RenderingManager::SetViewport(0, 0, seri::WindowManager::GetWidth(), seri::WindowManager::GetHeight());

			seri::LayerManager editorLayerManager{};
			editorLayerManager.AddLayer(std::make_shared<seri::CoreLayer>());
			editorLayerManager.AddLayer(std::make_shared<seri::editor::SandboxLayer>());
			editorLayerManager.AddLayer(std::make_shared<seri::editor::EditorLayer>());

			LIB_LOGGER(info, editor) << "seri game engine - editor loop starting";

			while (!seri::WindowManager::GetWindowShouldClose())
			{
				RunFrame(editorLayerManager);
			}

			LIB_LOGGER(info, editor) << "seri game engine - editor loop stopped";
		}

		inline static const char* kWindowTitle = "Seri Game Engine";
		inline static constexpr bool kIsFullscreen = false;
		inline static constexpr int kLauncherWidth = 720;
		inline static constexpr int kLauncherHeight = 420;
		inline static constexpr int kEditorWidth = 1600;
		inline static constexpr int kEditorHeight = 900;

	};
}
