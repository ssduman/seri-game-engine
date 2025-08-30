#pragma once

#include "seri/rendering/RenderingManagerBase.h"

#include <memory>

namespace seri
{
	class RenderingManagerFactory
	{
	public:
		static std::unique_ptr<RenderingManagerBase>& Instance()
		{
			return _renderingManagerInstance;
		}

	private:
		static std::unique_ptr<RenderingManagerBase> _renderingManagerInstance;

	};
}
