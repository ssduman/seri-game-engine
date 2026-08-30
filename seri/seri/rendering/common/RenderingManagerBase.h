#pragma once

#include "seri/util/Util.h"
#include "seri/window/WindowManagerBase.h"
#include "seri/rendering/render/PipelineBase.h"
#include "seri/rendering/render/RenderingData.h"
#include "seri/rendering/common/FramebufferBase.h"

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

		virtual void Init(std::unique_ptr<WindowManagerBase>& windowManager, RenderingProperties renderingProperties) = 0;

		virtual void SetViewport(int x, int y, int width, int height) = 0;

		virtual void Clear() = 0;

		virtual void ClearColor(float red = 0.2f, float green = 0.2f, float blue = 0.2f, float alpha = 1.0f) = 0;

		virtual void SetPointSize(float size) = 0;

		virtual void SetLineWidth(float width) = 0;

		virtual void SetBlend(bool enabled, BlendFactor srcFactor, BlendFactor dstFactor) = 0;

		virtual void SetDepthFunc(bool enabled, DepthFunc depthFunc) = 0;

		virtual void SetDepthWrite(bool enabled) = 0;

		virtual void SetStencilFunc(bool enabled, StencilFunc stencilFunc, int32_t ref, uint32_t mask) = 0;

		virtual void SetStencilOp(StencilOp sfail, StencilOp dpfail, StencilOp dppass) = 0;

		virtual void SetStencilMask(uint32_t mask) = 0;

		virtual void SetCullFace(bool enabled, CullFace cullFace) = 0;

		virtual void SetFrontFace(FrontFace frontFace) = 0;

		std::shared_ptr<FramebufferBase> mainRT{ nullptr };
		std::shared_ptr<FramebufferBase> editorRT{ nullptr };
		std::shared_ptr<FramebufferBase> shadowRT{ nullptr };
		std::shared_ptr<FramebufferBase> spotShadowRTs[literals::kMaxSpotLightShadowCount];

		std::shared_ptr<UniformBufferBase> lightUBO{ nullptr };
		std::shared_ptr<UniformBufferBase> cameraUBO{ nullptr };
		std::shared_ptr<UniformBufferBase> shadowUBO{ nullptr };
		
		glm::mat4 dirShadowLightViewProj{ 1.0f };
		
		int spotShadowCount{ 0 };
		glm::mat4 spotShadowLightViewProjs[literals::kMaxSpotLightShadowCount];

	protected:
		RenderingProperties _renderingProperties;

		bool _initialized{ false };

	};
}
