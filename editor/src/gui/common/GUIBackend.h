#pragma once

#include "gui/common/GUIRendererBase.h"

namespace seri::editor
{
	enum class GUIRendererBackend
	{
		imgui,
		seri,
	};

	struct GUIBackend
	{
		static void Init(GUIRendererBackend rendererBackend);

		static void Shutdown();

		static void NewFrame();

		static void Render();

		static void ProcessEvent(const void* event);

	private:
		inline static std::unique_ptr<GUIRendererBase> _renderer{ nullptr };

	};
}
