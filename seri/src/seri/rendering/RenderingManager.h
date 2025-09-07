#pragma once

#include "seri/rendering/RenderingManagerBase.h"

#include <memory>

namespace seri
{
	class RenderingManager
	{
	public:
		static std::unique_ptr<RenderingManagerBase>& Instance()
		{
			return _renderingManager;
		}

		static void SetViewport(int x, int y, int width, int height)
		{
			_renderingManager->SetViewport(x, y, width, height);
		}

		static void Clear()
		{
			_renderingManager->Clear();
		}

		static void ClearColor(const glm::vec4& color)
		{
			_renderingManager->ClearColor(color.r, color.g, color.b, color.a);
		}

		static void ClearColor(float red = 0.2f, float green = 0.2f, float blue = 0.2f, float alpha = 1.0f)
		{
			_renderingManager->ClearColor(red, green, blue, alpha);
		}

		static void SetPointSize(float size)
		{
			_renderingManager->SetPointSize(size);
		}

		static void SetLineWidth(float width)
		{
			_renderingManager->SetLineWidth(width);
		}

		static DepthFuncType GetDepthFunc()
		{
			return _renderingManager->GetDepthFunc();
		}

		static DepthFuncType SetDepthFunc(DepthFuncType depthFuncType)
		{
			return _renderingManager->SetDepthFunc(depthFuncType);
		}

	private:
		static std::unique_ptr<RenderingManagerBase> _renderingManager;

	};
}
