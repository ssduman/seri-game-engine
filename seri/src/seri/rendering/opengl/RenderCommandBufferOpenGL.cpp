#include "Seripch.h"

#include "seri/rendering/opengl/RenderCommandBufferOpenGL.h"

namespace seri
{
	void RenderCommandBufferOpenGL::Draw(DrawParams draw, const std::shared_ptr<VertexArrayBase>& vao)
	{
		_stats.drawCalls += 1;

		vao->Bind();

		switch (draw.mode)
		{
			case DrawMode::arrays:
				{
					_stats.triangles += draw.count / 3;

					glDrawArrays(
						GetTopology(draw.topology),
						0,
						draw.count
					);
				}
				break;
			case DrawMode::elements:
				{
					uint32_t indexCount = vao->GetIndexBuffer()->GetCount();

					_stats.triangles += indexCount / 3;

					glDrawElements(
						GetTopology(draw.topology),
						indexCount,
						GetDataType(draw.dataType),
						draw.indices
					);
				}
				break;
			case DrawMode::elements_instanced:
				{
					uint32_t indexCount = vao->GetIndexBuffer()->GetCount();

					_stats.triangles += (indexCount / 3) * draw.instanceCount;

					glDrawElementsInstanced(
						GetTopology(draw.topology),
						indexCount,
						GetDataType(draw.dataType),
						draw.indices,
						draw.instanceCount
					);
				}
				break;
		}
	}

	GLenum RenderCommandBufferOpenGL::GetTopology(Topology topology)
	{
		switch (topology)
		{
			case Topology::point: return GL_POINTS;
			case Topology::line: return GL_LINES;
			case Topology::line_strip: return GL_LINE_STRIP;
			case Topology::triangle: return GL_TRIANGLES;
			case Topology::triangle_strip: return GL_TRIANGLE_STRIP;
			case Topology::triangle_fan: return GL_TRIANGLE_FAN;
		}
		return GL_TRIANGLES;
	}

	GLenum RenderCommandBufferOpenGL::GetDataType(DataType dataType)
	{
		switch (dataType)
		{
			case DataType::byte_type: return GL_BYTE;
			case DataType::ubyte_type: return GL_UNSIGNED_BYTE;
			case DataType::short_type: return GL_SHORT;
			case DataType::ushort_type: return GL_UNSIGNED_SHORT;
			case DataType::int_type: return GL_INT;
			case DataType::uint_type: return GL_UNSIGNED_INT;
			case DataType::float_type: return GL_FLOAT;
			case DataType::double_type: return GL_DOUBLE;
		}
		return GL_FLOAT;
	}

}
