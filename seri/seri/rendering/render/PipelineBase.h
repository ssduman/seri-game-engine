#pragma once

#include "seri/util/Util.h"
#include "seri/shader/ShaderBase.h"
#include "seri/graphic/Material.h"
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
		shadow,
		skybox,
		opaque,
		transparent,
		debug,
		ui,
	};

	struct RenderState
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
		PassType type{ PassType::opaque };

		bool clearDepth{ true };
		float clearDepthValue{ 1.0f };

		bool clearStencil{ true };
		int clearStencilValue{ 0 };

		bool clearColor{ true };
		glm::vec4 clearColorValue{ 0.2f, 0.2f, 0.2f, 1.0f };

		bool useViewport{ false };
		int vx{ 0 };
		int vy{ 0 };
		int vw{ 0 };
		int vh{ 0 };

		std::shared_ptr<CameraBase> camera{ nullptr };

		std::shared_ptr<FramebufferBase> rt{ nullptr };
	};

	struct DrawParams
	{
		uint32_t count{ 0 };
		uint32_t instanceCount{ 0 };
		DrawMode mode{ DrawMode::elements };
		Topology topology{ Topology::triangle };
		DataType dataType{ DataType::uint_type };
		const void* indices{ nullptr };
	};

	struct RenderCommand
	{
		std::string name;
		bool noop{ false };

		RenderState state;
		DrawParams draw;

		std::shared_ptr<CameraBase> camera{ nullptr };
		std::shared_ptr<Material> material{ nullptr };
		std::shared_ptr<VertexArrayBase> vao{ nullptr };
		std::shared_ptr<FramebufferBase> rt{ nullptr };

		glm::mat4 model{ 1.0f };
	};

	struct RenderItem
	{
		PassType type;
		std::string name;

		RenderState state;
		DrawParams draw;

		std::shared_ptr<Material> material;
		std::shared_ptr<VertexArrayBase> vao;

		glm::mat4 model{ 1.0f };
	};

	struct RenderPass
	{
		PassDesc desc;
		std::vector<RenderItem> items{};
	};

	class FrameGraph
	{
	public:
		void AddPass(RenderPass pass)
		{
			passes.emplace_back(pass);
		}

		RenderPass& GetPass(PassType type)
		{
			return passes[static_cast<int>(type)];
		}

		void AddItem(RenderItem item)
		{
			auto& pass = GetPass(item.type);
			pass.items.emplace_back(item);
		}

		void Clear()
		{
			passes.clear();
		}

		std::vector<RenderPass> passes{};

	};
}
