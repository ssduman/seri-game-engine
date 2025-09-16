#pragma once

#include "seri/rendering/PipelineBase.h"
#include "seri/rendering/RenderingStats.h"
#include "seri/rendering/RenderingManagerBase.h"
#include "seri/rendering/RenderCommandBufferBase.h"

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

		static DepthFunc GetDepthFunc()
		{
			return _renderingManager->GetDepthFunc();
		}

		static void SetDepthFunc(bool enabled, DepthFunc depthFunc)
		{
			_renderingManager->SetDepthFunc(enabled, depthFunc);
		}

		static void SetDepthWrite(bool enabled)
		{
			_renderingManager->SetDepthWrite(enabled);
		}

		static void SetBlend(bool enabled, BlendFactor srcFactor, BlendFactor dstFactor)
		{
			_renderingManager->SetBlend(enabled, srcFactor, dstFactor);
		}

		static void SetCullFace(bool enabled, CullFace cullFace)
		{
			_renderingManager->SetCullFace(enabled, cullFace);
		}

		static void SetFrontFace(FrontFace frontFace)
		{
			_renderingManager->SetFrontFace(frontFace);
		}

		static void Begin(std::shared_ptr<CameraBase> camera)
		{
			_renderCommandBuffer->Begin(camera);
		}

		static void End()
		{
			_renderCommandBuffer->End();
		}

		static void Submit(RenderCommand renderCommand)
		{
			_renderCommandBuffer->Submit(renderCommand);
		}

		static void Execute()
		{
			_renderCommandBuffer->Execute();
		}

		static RenderingStats GetRenderingStats()
		{
			return _renderCommandBuffer->GetStats();
		}

	private:
		static std::unique_ptr<RenderingManagerBase> _renderingManager;
		static std::unique_ptr<RenderCommandBufferBase> _renderCommandBuffer;

	};
}
