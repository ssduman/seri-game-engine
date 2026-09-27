#pragma once

#include <imgui.h>

namespace seri::editor
{
	class GUIRendererBase
	{
	public:
		virtual ~GUIRendererBase() = default;

		virtual void Init() = 0;

		virtual void Shutdown() = 0;

		virtual void NewFrame() = 0;

		virtual void RenderDrawData(ImDrawData* drawData) = 0;

	};
}
