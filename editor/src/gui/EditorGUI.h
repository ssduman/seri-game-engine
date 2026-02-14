#pragma once

#include <seri/core/Seri.h>

#include <ImGui/imgui.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>
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

		struct ScopedChild
		{
			ScopedChild(const char* id, ImVec2 size = ImVec2(0, 0), ImGuiChildFlags flags = ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY)
			{
				ImGui::BeginChild(id, size, flags);
			}

			~ScopedChild()
			{
				ImGui::EndChild();
			}
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
		void ShowEditorInspectorAssetMaterial();

		void ShowEditorProject();
		void ShowEditorProjectFolderTree(seri::asset::AssetTreeNode& node);
		void ShowEditorProjectAssetGrid();

		void ShowEditorComponentPickerPopup();
		bool ShowEditorAssetPickerPopup(seri::asset::AssetType type, bool& selected, uint64_t& selection);

		void ShowEditorImage(std::shared_ptr<seri::TextureBase>& texture, float size);
		bool ShowEditorImageButton(std::shared_ptr<seri::TextureBase>& texture, float size);

		void ShowEditorHierarchyAddMenu(const std::shared_ptr<seri::scene::Scene>& activeScene, uint64_t parentId);

		void DrawEditorLayout();

		bool DrawBool(const char* label, bool& value);
		bool DrawInt(const char* label, int& value, float speed = 1.0f, int min = 0, int max = 0);
		bool DrawFloat(const char* label, float& value, float speed = 0.1f, float min = 0.0f, float max = 0.0f, const char* format = "%.3f");
		bool DrawVec3(const char* label, glm::vec3& v, float speed);
		bool DrawColorVec3(const char* label, glm::vec3& color, float speed);
		bool DrawColorVec4(const char* label, glm::vec4& color, float speed);
		bool DrawLabel(const char* label, const char* value, bool isDisabled);
		bool DrawTextInput(const char* label, std::string& value, size_t bufferSize = 512);
		bool DrawAssetPicker(const char* label, uint64_t assetId, seri::asset::AssetType assetType, uint64_t& selection);

		uint64_t _selectedEntityId{ 0 };
		InspectorType _inspectorType{ InspectorType::none };

		GizmoSpace _gizmoSpace{ GizmoSpace::local };
		GizmoOperation _gizmoOperation{ GizmoOperation::translate };

		seri::asset::AssetTreeNode* _selectedFolder{ nullptr };
		seri::asset::AssetTreeNode _selectedAsset{};

	};
}
