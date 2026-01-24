#pragma once

#include "seri/util/Util.h"
#include "seri/camera/CameraBase.h"
#include "seri/rendering/render/PipelineBase.h"
#include "seri/rendering/render/RenderingStats.h"
#include "seri/rendering/common/RenderingManagerBase.h"
#include "seri/rendering/common/RenderCommandBufferBase.h"

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

		static void SetBlend(bool enabled, BlendFactor srcFactor, BlendFactor dstFactor)
		{
			_renderingManager->SetBlend(enabled, srcFactor, dstFactor);
		}

		static void SetDepthFunc(bool enabled, DepthFunc depthFunc)
		{
			_renderingManager->SetDepthFunc(enabled, depthFunc);
		}

		static void SetDepthWrite(bool enabled)
		{
			_renderingManager->SetDepthWrite(enabled);
		}

		static void SetStencilFunc(bool enabled, StencilFunc stencilFunc, int32_t ref, uint32_t mask)
		{
			_renderingManager->SetStencilFunc(enabled, stencilFunc, ref, mask);
		}

		static void SetStencilOp(StencilOp sfail, StencilOp dpfail, StencilOp dppass)
		{
			_renderingManager->SetStencilOp(sfail, dpfail, dppass);
		}

		static void SetStencilMask(uint32_t mask)
		{
			_renderingManager->SetStencilMask(mask);
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
			if (renderCommand.rt == nullptr)
			{
				renderCommand.rt = _renderingManager->mainRT;
			}

			_renderCommandBuffer->Submit(renderCommand);
		}

		static void Execute()
		{
			RenderCommand renderCommand;
			renderCommand.name = "noop";
			renderCommand.noop = true;
			Submit(renderCommand);

			_renderCommandBuffer->Execute();
		}

		static RenderingStats GetRenderingStats()
		{
			return _renderCommandBuffer->GetStats();
		}

		static std::shared_ptr<FramebufferBase> GetMainRT()
		{
			return _renderingManager->mainRT;
		}

		static std::shared_ptr<FramebufferBase> GetEditorRT()
		{
			return _renderingManager->editorRT;
		}

		static std::shared_ptr<UniformBufferBase> GetLightUBO()
		{
			return _renderingManager->lightUBO;
		}

		static std::shared_ptr<UniformBufferBase> GetCameraUBO()
		{
			return _renderingManager->cameraUBO;
		}

	private:
		static std::unique_ptr<RenderingManagerBase> _renderingManager;
		static std::unique_ptr<RenderCommandBufferBase> _renderCommandBuffer;

	};
}
