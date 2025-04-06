#pragma once

#include "seri/window/IWindowManager.h"

#include <memory>

namespace seri
{
	class WindowManagerFactory
	{
	public:
		static std::shared_ptr<IWindowManager> instance()
		{
			return _windowManagerInstance;
		}

	private:
		static std::shared_ptr<IWindowManager> _windowManagerInstance;

	};
}
