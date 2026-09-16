#pragma once

namespace seri::editor
{
	struct GUIBackend
	{
		static void Init();

		static void Shutdown();

		static void NewFrame();

		static void Render();

		static void ProcessEvent(const void* event);
	};
}
