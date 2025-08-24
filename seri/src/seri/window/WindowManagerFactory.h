#pragma once

#include "seri/window/IWindowManager.h"

#include <memory>

namespace seri
{
	class WindowManagerFactory
	{
	public:
		static std::unique_ptr<IWindowManager>& Instance()
		{
			return _windowManagerInstance;
		}

	private:
		static std::unique_ptr<IWindowManager> _windowManagerInstance;

	};
}
