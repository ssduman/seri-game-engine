#include "Editorpch.h"

#include "gui/common/GUIRendererImGui.h"

#include <imgui_impl_opengl3.h>

namespace seri::editor
{
	void GUIRendererImGui::Init()
	{
		ImGui_ImplOpenGL3_Init("#version 460");

		LIB_LOGGER(info, gui_renderer) << "imgui renderer backend " << ImGui::GetIO().BackendRendererName << " inited";
	}

	void GUIRendererImGui::Shutdown()
	{
		ImGui_ImplOpenGL3_Shutdown();
	}

	void GUIRendererImGui::NewFrame()
	{
		ImGui_ImplOpenGL3_NewFrame();
	}

	void GUIRendererImGui::RenderDrawData(ImDrawData* drawData)
	{
		ImGui_ImplOpenGL3_RenderDrawData(drawData);
	}
}
