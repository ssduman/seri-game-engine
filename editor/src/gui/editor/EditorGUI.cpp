#include "Editorpch.h"

#include "gui/editor/EditorGUI.h"
#include "gui/common/GUIStyle.h"
#include "gui/common/GUIBackend.h"

namespace seri::editor
{
	EditorGUI::EditorGUI()
	{
		LIB_LOGGER(info, gui) << "created, imgui version: " << IMGUI_VERSION;
	}

	EditorGUI::~EditorGUI()
	{
		GUIBackend::Shutdown();

		LIB_LOGGER(info, gui) << "destroyed";
	}

	void EditorGUI::Init()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		uint64_t fontId = seri::asset::AssetManager::FindAssetIdByName(seri::literals::kDefaultFontName);

		SetIO();
		SetFonts(seri::asset::AssetManager::GetAssetMetadata(fontId).source);
		SetStyle();

		_titleBar.Init();

		GUIBackend::Init();
	}

	void EditorGUI::Update()
	{
		SERI_PROFILER_ZONE_SCOPED;

		GUIBackend::NewFrame();

		ImGuizmo::BeginFrame();
		ImGuizmo::Enable(true);

		_consolePanel.Update();
		_gamePanel.Update(_context);

		DrawLayout();

		CheckShortcuts();
	}

	void EditorGUI::Render()
	{
		SERI_PROFILER_ZONE_SCOPED;

		GUIBackend::Render();
	}

	void EditorGUI::CheckShortcuts()
	{
		auto& io = ImGui::GetIO();

		if (io.WantTextInput)
		{
			return;
		}

		if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S, false))
		{
			Save();
		}

		if (ImGui::IsKeyPressed(ImGuiKey_Escape, false) &&
			!ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopupId | ImGuiPopupFlags_AnyPopupLevel))
		{
			if (seri::scene::SceneManager::GetState() != seri::scene::SceneState::edit)
			{
				seri::scene::SceneManager::SetState(seri::scene::SceneState::edit);
			}
		}
	}

	void EditorGUI::BuildDefaultDockLayout(ImGuiID dockspaceId)
	{
		ImGui::DockBuilderRemoveNode(dockspaceId);
		ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dockspaceId, ImGui::GetMainViewport()->WorkSize);

		ImGuiID dockCenter = dockspaceId;
		ImGuiID dockLeft = ImGui::DockBuilderSplitNode(dockCenter, ImGuiDir_Left, 0.15f, nullptr, &dockCenter);
		ImGuiID dockBottom = ImGui::DockBuilderSplitNode(dockCenter, ImGuiDir_Down, 0.35f, nullptr, &dockCenter);
		ImGuiID dockRight = ImGui::DockBuilderSplitNode(dockCenter, ImGuiDir_Right, 0.30f, nullptr, &dockCenter);
		ImGuiID dockBottomRight = ImGui::DockBuilderSplitNode(dockBottom, ImGuiDir_Right, 0.30f, nullptr, &dockBottom);

		ImGui::DockBuilderDockWindow("Hierarchy", dockLeft);
		ImGui::DockBuilderDockWindow("Game", dockCenter);
		ImGui::DockBuilderDockWindow("Scene", dockCenter);
		ImGui::DockBuilderDockWindow("Inspector", dockRight);
		ImGui::DockBuilderDockWindow("Project", dockBottom);
		ImGui::DockBuilderDockWindow("Console", dockBottomRight);

		ImGui::DockBuilderFinish(dockspaceId);

		LIB_LOGGER(info, gui) << "default dock layout built";
	}

	void EditorGUI::DrawLayout()
	{
		ImGuiWindowFlags window_flags =
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus |
			ImGuiWindowFlags_NoBackground
			;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin("Editor DockSpace", nullptr, window_flags);

		ImGui::PopStyleVar(3);

		ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");

		if (_context.resetLayout || ImGui::DockBuilderGetNode(dockspace_id) == nullptr)
		{
			BuildDefaultDockLayout(dockspace_id);
			_context.resetLayout = false;
		}

		const float tabPadding = 4.0f;
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(tabPadding, ImGui::GetStyle().FramePadding.y));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(4.0f, ImGui::GetStyle().ItemInnerSpacing.y));
		// negative border cancels the FramePadding.x offset ImGui adds before the first dock tab, so tabs start at the node edge
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, -tabPadding);
		ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);
		ImGui::PopStyleVar(3);

		ImGui::End();

		_titleBar.Draw(_context);

		if (_context.showInspector)
		{
			if (ImGui::Begin("Inspector", &_context.showInspector))
			{
				_inspectorPanel.Draw(_context);
			}
			ImGui::End();
		}

		if (_context.showHierarchy)
		{
			if (ImGui::Begin("Hierarchy", &_context.showHierarchy))
			{
				_hierarchyPanel.Draw(_context);
			}
			ImGui::End();
		}

		if (_context.showScene)
		{
			if (ImGui::Begin("Scene", &_context.showScene))
			{
				_scenePanel.Draw(_context);
			}
			ImGui::End();
		}

		if (_context.showGame)
		{
			if (_gamePanel.IsMaximized())
			{
				ImGuiWindowFlags flags =
					ImGuiWindowFlags_NoDocking |
					ImGuiWindowFlags_NoResize |
					ImGuiWindowFlags_NoMove |
					ImGuiWindowFlags_NoCollapse |
					ImGuiWindowFlags_NoSavedSettings
					;

				ImGui::SetNextWindowPos(viewport->WorkPos);
				ImGui::SetNextWindowSize(viewport->WorkSize);
				ImGui::SetNextWindowViewport(viewport->ID);

				if (ImGui::Begin("Game##maximized", &_context.showGame, flags))
				{
					_gamePanel.Draw(_context);
				}
				ImGui::End();
			}
			else
			{
				if (ImGui::Begin("Game", &_context.showGame))
				{
					_gamePanel.Draw(_context);
				}
				ImGui::End();
			}
		}

		if (_context.showProject)
		{
			if (ImGui::Begin("Project", &_context.showProject))
			{
				_projectPanel.Draw(_context);
			}
			ImGui::End();
		}

		if (_context.showConsole)
		{
			if (ImGui::Begin("Console", &_context.showConsole))
			{
				_consolePanel.Draw();
			}
			ImGui::End();
		}
	}
}
