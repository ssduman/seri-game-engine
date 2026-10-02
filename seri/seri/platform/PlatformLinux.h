#pragma once

#include "seri/platform/Platform.h"

#include <unistd.h>

namespace seri::platform
{
	std::filesystem::path GetExecutablePath()
	{
		return std::filesystem::read_symlink("/proc/self/exe");
	}

	void BeginHighResolutionTimer()
	{
	}

	void EndHighResolutionTimer()
	{
	}

	bool EnableConsoleColor()
	{
		return isatty(STDERR_FILENO) != 0;
	}

	void EnableCustomTitleBar(void* nativeWindow, const std::function<bool(int x, int y)>& hitTest)
	{
	}
}
