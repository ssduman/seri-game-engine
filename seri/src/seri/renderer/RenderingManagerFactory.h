#pragma once

#include "seri/renderer/IRenderingManager.h"

#include <memory>

namespace seri
{
	class RenderingManagerFactory
	{
	public:
		static std::unique_ptr<IRenderingManager>& Instance()
		{
			return _renderingManagerInstance;
		}

	private:
		static std::unique_ptr<IRenderingManager> _renderingManagerInstance;

	};
}
