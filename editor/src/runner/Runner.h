#pragma once

#include <seri/core/Seri.h>
#include <seri/layer/PlayerLayer.h>
#include <seri/platform/Platform.h>
#include <seri/project/ProjectManager.h>
#include <seri/profiling/Profiler.h>

#include "gui/common/GUIBackend.h"
#include "layer/EditorLayer.h"
#include "layer/SandboxLayer.h"
#include "layer/LauncherLayer.h"

namespace seri::editor
{
	class Runner : public seri::IRunner
	{
	public:
		Runner() = default;
		~Runner() override = default;

		void operator()(int argc, char* argv[])
		{
			seri::LoggerConfig loggerConfig;
			loggerConfig.level = seri::LogLevel::info;
			seri::Logger::Init(loggerConfig);

			std::filesystem::path playerProjectPath = FindPlayerProject();
			if (!playerProjectPath.empty())
			{
				RunPlayer(playerProjectPath);
				ShutdownPlatform();
				return;
			}

			InitPlatform(
				{
					.windowTitle = kWindowTitle,
					.isFullscreen = kIsFullscreen,
					.windowWidth = kLauncherWidth,
					.windowHeight = kLauncherHeight,
					.isOpenGLDebugContext = kIsRendererDebugContext
				}
			);

			std::filesystem::path projectPath;

			RunLauncher(projectPath);
			RunEditor(projectPath);

			ShutdownPlatform();
		}

	private:
		std::filesystem::path FindPlayerProject()
		{
			std::filesystem::path projectPath = seri::platform::GetExecutablePath().replace_extension(seri::project::ProjectManager::kProjectExtension);

			std::error_code ec;
			return std::filesystem::is_regular_file(projectPath, ec) ? projectPath : std::filesystem::path{};
		}

		void InitPlatform(const seri::WindowProperties& windowProperties)
		{
			seri::InputManager::Init();

			seri::WindowManager::Instance()->Init(windowProperties);

			seri::WindowManager::Instance()->AddProcessEventDelegate(
				[](const void* event)
				{
					seri::editor::GUIBackend::ProcessEvent(event);
				}
			);

			seri::RenderingManager::Instance()->Init(seri::WindowManager::Instance(), seri::RenderingProperties{});
		}

		void ShutdownPlatform()
		{
			seri::asset::AssetManager::ClearCache();
			seri::RenderingManager::Instance().reset();
			seri::WindowManager::Instance().reset();
		}

		void RunFrame(seri::LayerManager& layerManager)
		{
			layerManager.OnPreUpdate();
			layerManager.OnUpdate();
			layerManager.OnRender();
			layerManager.OnPostUpdate();

			SERI_PROFILER_FRAME_END_MARK;
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

		void RunPlayer(const std::filesystem::path& projectPath)
		{
			std::filesystem::current_path(projectPath.parent_path());

			InitPlatform(
				{
					.windowTitle = kWindowTitle,
					.isFullscreen = true,
					.isOpenGLDebugContext = kIsRendererDebugContext
				}
			);

			std::string error;
			if (!seri::project::ProjectManager::OpenProject(projectPath, error))
			{
				LIB_LOGGER(error, player) << "could not open project " << projectPath.string() << ": " << error;
				return;
			}

			seri::WindowManager::SetWindowTitle(seri::project::ProjectManager::GetName().c_str());
			seri::RenderingManager::SetViewport(0, 0, seri::WindowManager::GetWidth(), seri::WindowManager::GetHeight());

			seri::LayerManager playerLayerManager{};
			playerLayerManager.AddLayer(std::make_shared<seri::CoreLayer>());
			playerLayerManager.AddLayer(std::make_shared<seri::PlayerLayer>());

			LIB_LOGGER(info, player) << "seri game engine - player loop starting";

			while (!seri::WindowManager::GetWindowShouldClose())
			{
				RunFrame(playerLayerManager);
			}

			LIB_LOGGER(info, player) << "seri game engine - player loop stopped";
		}

		inline static const char* kWindowTitle = "Seri Game Engine";
		inline static constexpr bool kIsFullscreen = false;
		inline static constexpr bool kIsRendererDebugContext = SERI_OPENGL_DEBUG_CONTEXT;
		inline static constexpr int kLauncherWidth = 720;
		inline static constexpr int kLauncherHeight = 420;
		inline static constexpr int kEditorWidth = 1600;
		inline static constexpr int kEditorHeight = 900;

	};
}
