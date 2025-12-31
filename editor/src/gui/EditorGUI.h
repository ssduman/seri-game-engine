#pragma once

#include <seri/core/Seri.h>

#include <ImGui/imgui.h>
#include <ImGui/backends/imgui_impl_opengl3.h>
#include <ImGui/misc/freetype/imgui_freetype.h>

#include <ImGuizmo/ImGuizmo.h>

#include <memory>
#include <unordered_map>

#if defined (SERI_USE_WINDOW_GLFW)
#include <GLFW/glfw3.h>
#include <ImGui/backends/imgui_impl_glfw.h>
#elif defined (SERI_USE_WINDOW_SDL3)
#include <SDL3/SDL.h>
#include <ImGui/backends/imgui_impl_sdl3.h>
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

		enum class GizmoSpace
		{
			local,
			world,
		};

		enum class GizmoOperation
		{
			translate,
			rotate,
			scale,
		};

		struct ComponentInfo
		{
			std::string name;
			std::function<void(uint64_t id)> AddComponent;
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
		void ControlEditorSceneMove(const ImVec2& imageMin, const ImVec2& imageMax);
		void ShowEditorSceneGizmoToolbar(const ImVec2& imageMin);
		void ShowEditorSceneGizmo(const ImVec2& imageMin, const ImVec2& finalSize);
		void ShowEditorSceneEntityGizmo(const ImVec2& imageMin, const ImVec2& finalSize);

		void ShowEditorHierarchy();
		void ShowEditorHierarchyImpl(const std::shared_ptr<seri::scene::Scene>& activeScene, seri::scene::SceneTreeNode& node, uint64_t& selectedId);

		void ShowEditorInspector();
		void ShowEditorInspectorScene();
		void ShowEditorInspectorEntity();
		void ShowEditorInspectorAsset();

		void ShowEditorProject();
		void ShowEditorProjectFolderTree(seri::asset::AssetTreeNode& node);
		void ShowEditorProjectAssetGrid();

		void ShowEditorComponentPickerPopup();
		bool ShowEditorAssetPickerPopup(seri::asset::AssetType type, bool& selected, uint64_t& selection);

		void ShowEditorImage(std::shared_ptr<seri::TextureBase>& texture, float size);
		bool ShowEditorImageButton(std::shared_ptr<seri::TextureBase>& texture, float size);

		void DrawEditorLayout();

		uint64_t _selectedEntityId{ 0 };
		InspectorType _inspectorType{ InspectorType::none };

		GizmoSpace _gizmoSpace{ GizmoSpace::local };
		GizmoOperation _gizmoOperation{ GizmoOperation::translate };

		seri::asset::AssetTreeNode* _selectedFolder{ nullptr };
		seri::asset::AssetTreeNode _selectedAsset{};

		std::vector<ComponentInfo> _componentInfos{};

	};
}
