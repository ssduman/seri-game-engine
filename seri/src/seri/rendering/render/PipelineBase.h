#pragma once

#include "seri/util/Util.h"
#include "seri/shader/ShaderBase.h"
#include "seri/rendering/render/RenderingUtil.h"
#include "seri/rendering/common/BufferBase.h"
#include "seri/rendering/common/VertexArrayBase.h"
#include "seri/rendering/common/FramebufferBase.h"

namespace seri
{
	enum class DataType
	{
		byte_type,
		ubyte_type,
		short_type,
		ushort_type,
		int_type,
		uint_type,
		float_type,
		double_type,
	};

	enum class Topology
	{
		point,
		line,
		line_loop,
		line_strip,
		triangle,
		triangle_strip,
		triangle_fan,
	};

	enum class DepthFunc
	{
		never,
		always,
		less,
		equal,
		l_equal,
		g_equal,
		greater,
		not_equal,
	};

	enum class StencilFunc
	{
		never,
		always,
		less,
		equal,
		l_equal,
		g_equal,
		greater,
		not_equal,
	};

	enum class StencilOp
	{
		keep,
		zero,
		replace,
		incr,
		incr_wrap,
		decr,
		decr_wrap,
		invert,
	};

	enum class CullFace
	{
		front,
		back,
		front_and_back,
	};

	enum class FrontFace
	{
		cw,
		ccw,
	};

	enum class BlendFactor
	{
		zero,
		one,
		src_color,
		one_minus_src_color,
		dst_color,
		one_minus_dst_color,
		src_alpha,
		one_minus_src_alpha,
		dst_alpha,
		one_minus_dst_alpha,
		constant_color,
		one_minus_constant_color,
		constant_alpha,
		one_minus_constant_alpha,
		src_alpha_saturate,
	};

	enum class DrawMode
	{
		arrays,
		elements,
		elements_instanced,
	};

	enum class PassType
	{
		opaque,
		transparent,
	};

	struct PipelineDesc
	{
		bool depthTestEnabled{ true };
		bool depthWriteEnabled{ true };
		DepthFunc depthFunc{ DepthFunc::less };

		bool stencilTestEnabled{ false };
		StencilFunc stencilFunc{ StencilFunc::always };
		int32_t stencilRef{ 0 };
		uint32_t stencilMask{ 0xFF };
		uint32_t stencilMaskAND{ 0xFF };
		StencilOp stencilSfail{ StencilOp::keep };
		StencilOp stencilDPfail{ StencilOp::keep };
		StencilOp stencilDPpass{ StencilOp::keep };

		bool cullFaceEnabled{ false };
		CullFace cullFace{ CullFace::back };
		FrontFace frontFace{ FrontFace::ccw };

		bool blendEnabled{ true };
		BlendFactor blendFactorSrc{ BlendFactor::src_alpha };
		BlendFactor blendFactorDst{ BlendFactor::one_minus_src_alpha };

		float lineWidth{ 1.0f };
		float pointSize{ 1.0f };
	};

	struct PassDesc
	{
		PassType passType{ PassType::opaque };

		bool clearDepth{ false };
		float clearDepthValue{ 1.0f };

		bool clearStencil{ false };
		int clearStencilValue{ 0 };

		bool clearColor{ false };
		glm::vec4 clearColorValue{ 0.2f, 0.2f, 0.2f, 1.0f };

		bool useViewport{ false };
		int vx{ 0 };
		int vy{ 0 };
		int vw{ 0 };
		int vh{ 0 };

		std::shared_ptr<CameraBase> camera{ nullptr };

		std::shared_ptr<FramebufferBase> rt{ nullptr };
	};
}
