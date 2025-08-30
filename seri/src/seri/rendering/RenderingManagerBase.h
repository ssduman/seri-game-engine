#pragma once

#include "seri/util/Util.h"
#include "seri/window/WindowManagerBase.h"

namespace seri
{
	struct RenderingProperties
	{
	};

	class RenderingManagerBase
	{
	public:
		RenderingManagerBase() = default;

		virtual ~RenderingManagerBase() = default;

		virtual void Init(std::unique_ptr<WindowManagerBase>& windowManager) = 0;

		virtual void SetViewport(int x, int y, int width, int height) = 0;

		virtual void Clear() = 0;

		virtual void ClearColor(float red = 0.2f, float green = 0.2f, float blue = 0.2f, float alpha = 1.0f) = 0;

		virtual void SetPointSize(float size) = 0;

		virtual void SetLineWidth(float width) = 0;

		void SetRenderingProperties(RenderingProperties renderingProperties)
		{
			if (_initialized)
			{
				throw std::runtime_error("[rendering] already initialized");
			}

			_renderingProperties = std::move(renderingProperties);
		}

	protected:
		RenderingProperties _renderingProperties;

		bool _initialized{ false };

	};
}
