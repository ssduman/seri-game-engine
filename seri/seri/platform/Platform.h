#pragma once

#include <filesystem>
#include <functional>

namespace seri::platform
{
	std::filesystem::path GetExecutablePath();

	const char* GetSharedLibraryExtension();

	void BeginHighResolutionTimer();

	void EndHighResolutionTimer();

	bool EnableConsoleColor();

	void EnableCustomTitleBar(void* nativeWindow, const std::function<bool(int x, int y)>& hitTest);
}
