#include "Editorpch.h"

#include "gui/EditorStyle.h"

namespace seri::editor
{
	void SetIO()
	{
		auto& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigWindowsMoveFromTitleBarOnly = true;
	}

	void SetFonts()
	{
		auto& io = ImGui::GetIO();

		std::filesystem::path fontPath = seri::asset::AssetManager::GetAssetDirectory() / "fonts" / "Roboto-Regular.ttf";
		if (std::filesystem::exists(fontPath))
		{
			ImFontConfig config{};
			io.FontDefault = io.Fonts->AddFontFromFileTTF(fontPath.string().c_str(), 16.0f, &config);
		}
		else
		{
			LIB_LOGGER(warning, gui) << fmt::format("editor font not found: {}", fontPath.string());
		}
	}

	void SetStyle()
	{
		auto& style = ImGui::GetStyle();

		const ImVec4 bgDeep = RGBNormalized(18, 20, 24);
		const ImVec4 bgWindow = RGBNormalized(28, 31, 38);
		const ImVec4 bgPanel = RGBNormalized(34, 38, 46);
		const ImVec4 bgRaised = RGBNormalized(42, 47, 57);
		const ImVec4 bgHovered = RGBNormalized(52, 58, 70);
		const ImVec4 bgActive = RGBNormalized(62, 69, 83);
		const ImVec4 border = RGBNormalized(48, 53, 63);
		const ImVec4 text = RGBNormalized(228, 231, 236);
		const ImVec4 textDim = RGBNormalized(138, 145, 158);
		const ImVec4 accent = RGBNormalized(76, 141, 255);
		const ImVec4 accentHovered = RGBNormalized(107, 161, 255);
		const ImVec4 accentActive = RGBNormalized(60, 121, 224);
		const ImVec4 warning = RGBNormalized(240, 173, 78);

		style.WindowPadding = ImVec2(10.0f, 10.0f);
		style.FramePadding = ImVec2(8.0f, 5.0f);
		style.ItemSpacing = ImVec2(8.0f, 6.0f);
		style.ItemInnerSpacing = ImVec2(6.0f, 5.0f);
		style.CellPadding = ImVec2(8.0f, 4.0f);
		style.IndentSpacing = 22.0f;
		style.ScrollbarSize = 12.0f;
		style.GrabMinSize = 10.0f;

		style.WindowBorderSize = 1.0f;
		style.ChildBorderSize = 1.0f;
		style.PopupBorderSize = 1.0f;
		style.FrameBorderSize = 0.0f;
		style.TabBorderSize = 0.0f;
		style.TabBarBorderSize = 2.0f;
		style.TabBarOverlineSize = 2.0f;
		style.SeparatorTextBorderSize = 1.0f;
		style.DockingSeparatorSize = 2.0f;

		style.WindowRounding = 8.0f;
		style.ChildRounding = 6.0f;
		style.FrameRounding = 5.0f;
		style.PopupRounding = 6.0f;
		style.ScrollbarRounding = 9.0f;
		style.GrabRounding = 5.0f;
		style.TabRounding = 0.0f;
		style.ImageRounding = 4.0f;

		style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
		style.WindowMenuButtonPosition = ImGuiDir_None;
		style.ColorButtonPosition = ImGuiDir_Right;
		style.SeparatorTextAlign = ImVec2(0.0f, 0.5f);
		style.SeparatorTextPadding = ImVec2(16.0f, 4.0f);

		style.TreeLinesFlags = ImGuiTreeNodeFlags_DrawLinesToNodes;
		style.TreeLinesSize = 1.0f;

		style.AntiAliasedLines = true;
		style.AntiAliasedLinesUseTex = true;
		style.AntiAliasedFill = true;

		style.DisabledAlpha = 0.45f;

		ImVec4* colors = style.Colors;

		colors[ImGuiCol_Text] = text;
		colors[ImGuiCol_TextDisabled] = textDim;
		colors[ImGuiCol_TextSelectedBg] = ImVec4(accent.x, accent.y, accent.z, 0.35f);
		colors[ImGuiCol_TextLink] = accentHovered;

		colors[ImGuiCol_WindowBg] = bgWindow;
		colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		colors[ImGuiCol_PopupBg] = bgPanel;
		colors[ImGuiCol_Border] = border;
		colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

		colors[ImGuiCol_FrameBg] = bgPanel;
		colors[ImGuiCol_FrameBgHovered] = bgHovered;
		colors[ImGuiCol_FrameBgActive] = bgActive;

		colors[ImGuiCol_TitleBg] = bgDeep;
		colors[ImGuiCol_TitleBgActive] = bgPanel;
		colors[ImGuiCol_TitleBgCollapsed] = bgDeep;
		colors[ImGuiCol_MenuBarBg] = bgDeep;

		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		colors[ImGuiCol_ScrollbarGrab] = bgRaised;
		colors[ImGuiCol_ScrollbarGrabHovered] = bgHovered;
		colors[ImGuiCol_ScrollbarGrabActive] = bgActive;

		colors[ImGuiCol_CheckMark] = accent;
		colors[ImGuiCol_CheckboxSelectedBg] = ImVec4(accent.x, accent.y, accent.z, 0.18f);
		colors[ImGuiCol_SliderGrab] = accent;
		colors[ImGuiCol_SliderGrabActive] = accentHovered;
		colors[ImGuiCol_InputTextCursor] = text;

		colors[ImGuiCol_Button] = bgRaised;
		colors[ImGuiCol_ButtonHovered] = bgHovered;
		colors[ImGuiCol_ButtonActive] = bgActive;

		colors[ImGuiCol_Header] = ImVec4(accent.x, accent.y, accent.z, 0.28f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(accent.x, accent.y, accent.z, 0.40f);
		colors[ImGuiCol_HeaderActive] = ImVec4(accent.x, accent.y, accent.z, 0.55f);

		colors[ImGuiCol_Separator] = border;
		colors[ImGuiCol_SeparatorHovered] = accentActive;
		colors[ImGuiCol_SeparatorActive] = accent;

		colors[ImGuiCol_ResizeGrip] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(accent.x, accent.y, accent.z, 0.45f);
		colors[ImGuiCol_ResizeGripActive] = accent;

		colors[ImGuiCol_Tab] = bgDeep;
		colors[ImGuiCol_TabHovered] = bgHovered;
		colors[ImGuiCol_TabSelected] = bgWindow;
		colors[ImGuiCol_TabSelectedOverline] = accent;
		colors[ImGuiCol_TabDimmed] = bgDeep;
		colors[ImGuiCol_TabDimmedSelected] = bgPanel;
		colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(accent.x, accent.y, accent.z, 0.35f);

		colors[ImGuiCol_DockingPreview] = ImVec4(accent.x, accent.y, accent.z, 0.45f);
		colors[ImGuiCol_DockingEmptyBg] = bgDeep;

		colors[ImGuiCol_PlotLines] = accentHovered;
		colors[ImGuiCol_PlotLinesHovered] = accent;
		colors[ImGuiCol_PlotHistogram] = accent;
		colors[ImGuiCol_PlotHistogramHovered] = accentHovered;

		colors[ImGuiCol_TableHeaderBg] = bgPanel;
		colors[ImGuiCol_TableBorderStrong] = border;
		colors[ImGuiCol_TableBorderLight] = ImVec4(border.x, border.y, border.z, 0.5f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.025f);

		colors[ImGuiCol_TreeLines] = border;
		colors[ImGuiCol_UnsavedMarker] = warning;

		colors[ImGuiCol_DragDropTarget] = accent;
		colors[ImGuiCol_DragDropTargetBg] = ImVec4(accent.x, accent.y, accent.z, 0.20f);

		colors[ImGuiCol_NavCursor] = accent;
		colors[ImGuiCol_NavWindowingHighlight] = accentHovered;
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.55f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.55f);

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			colors[ImGuiCol_WindowBg].w = 1.0f;
		}
	}
}
