#pragma once

#include <seri/core/Seri.h>

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <misc/freetype/imgui_freetype.h>

#include "event/UserEvent.h"

#include <memory>
#include <unordered_map>

#if defined (SERI_USE_WINDOW_GLFW)
#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#elif defined (SERI_USE_WINDOW_SDL3)
#include <SDL3/SDL.h>
#include <backends/imgui_impl_sdl3.h>
#endif

namespace seri::editor
{
	class EditorGUI : public seri::Object
	{
	public:
		EditorGUI();

		~EditorGUI() override;

		void Init() override;

		void Update() override;

		void Render() override;

		void ProcessEvent(const void* event);

	private:
		enum class InspectorType
		{
			none,
			scene,
			entity,
			asset,
		};

		void SetIO()
		{
			auto& io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			io.ConfigWindowsMoveFromTitleBarOnly = true;
		}

		void SetStyle()
		{
			ImGui::StyleColorsDark();
			auto& style = ImGui::GetStyle();

			style.GrabRounding = 6.0f;
			style.ChildRounding = 6.0f;
			style.FrameRounding = 6.0f;
			style.PopupRounding = 6.0f;
			style.WindowRounding = 6.0f;
			style.ScrollbarRounding = 6.0f;
		}

		void CheckShortcuts();

		void ShowEditorMainMenuBar();

		void ShowEditorSceneImage();
		void ControlEditorSceneMove(ImVec2& imageMin, ImVec2& imageMax);

		void ShowEditorHierarchy();
		void ShowEditorHierarchyImpl(const std::shared_ptr<seri::scene::Scene>& activeScene, seri::scene::SceneTreeNode& node, uint64_t& selectedId);

		void ShowEditorInspector();
		void ShowEditorInspectorScene();
		void ShowEditorInspectorEntity();
		void ShowEditorInspectorAsset();

		void ShowEditorProject();
		void ShowEditorProjectFolderTree(seri::asset::AssetTreeNode& node);
		void ShowEditorProjectAssetGrid();
		
		bool ShowEditorAssetPickerPopup(seri::asset::AssetType type, uint64_t& match);

		void ShowEditorImage(std::shared_ptr<seri::TextureBase>& texture, float size);
		bool ShowEditorImageButton(std::shared_ptr<seri::TextureBase>& texture, float size);

		void DrawEditorLayout();

		uint64_t _selectedEntityId{ 0 };
		InspectorType _inspectorType{ InspectorType::none };

		seri::asset::AssetTreeNode* _selectedFolder{ nullptr };
		seri::asset::AssetTreeNode _selectedAsset{};

	};
}
