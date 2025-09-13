#pragma once

#include "seri/util/Util.h"
#include "seri/shader/ShaderBase.h"
#include "seri/rendering/BufferBase.h"
#include "seri/rendering/VertexArrayBase.h"

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

	struct PipelineDesc
	{
		bool depthTestEnabled{ true };
		bool depthWriteEnabled{ true };
		DepthFunc depthFunc{ DepthFunc::less };

		bool cullFaceEnabled{ true };
		CullFace cullFace{ CullFace::back };
		FrontFace frontFace{ FrontFace::ccw };

		bool blendEnabled{ true };
		BlendFactor blendFactorSrc{ BlendFactor::src_alpha };
		BlendFactor blendFactorDst{ BlendFactor::one_minus_src_alpha };

		float lineWidth{ 1.0f };
		float pointSize{ 1.0f };
	};
}
