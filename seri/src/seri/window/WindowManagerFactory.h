#pragma once

#include "seri/window/WindowManagerBase.h"

#include <memory>

namespace seri
{
	class WindowManagerFactory
	{
	public:
		static std::unique_ptr<WindowManagerBase>& Instance()
		{
			return _windowManagerInstance;
		}

	private:
		static std::unique_ptr<WindowManagerBase> _windowManagerInstance;

	};
}
