#pragma once

#include "seri/rendering/RenderingManager.h"
#include "seri/rendering/RenderCommandBufferBase.h"

#include <glad/gl.h>

namespace seri
{
	class RenderCommandBufferOpenGL : public RenderCommandBufferBase
	{
	public:
		void Begin(std::shared_ptr<CameraBase> camera) override
		{
		}

		void End() override
		{
		}

		void Submit(RenderCommand renderCommand) override
		{
			_commands.emplace_back(renderCommand);
		}

		void Execute() override
		{
			for (const auto& command : _commands)
			{
				RenderingManager::SetBlend(command.desc.blendEnabled, command.desc.blendFactorSrc, command.desc.blendFactorDst);
				RenderingManager::SetFrontFace(command.desc.frontFace);
				RenderingManager::SetCullFace(command.desc.cullFaceEnabled, command.desc.cullFace);
				RenderingManager::SetDepthFunc(command.desc.depthTestEnabled, command.desc.depthFunc);
				RenderingManager::SetDepthWrite(command.desc.depthWriteEnabled);
				RenderingManager::SetLineWidth(command.desc.lineWidth);
				RenderingManager::SetPointSize(command.desc.pointSize);

				if (command.noop)
				{
					continue;
				}

				command.material->SetMat4("u_model", command.trs);
				command.material->SetMat4("u_view", command.camera->GetView());
				command.material->SetMat4("u_projection", command.camera->GetProjection());
				command.material->Apply();
				command.vao->Bind();

				_stats.drawCalls += 1;

				switch (command.drawMode)
				{
					case DrawMode::arrays:
						{
							_stats.triangles += command.count / 3;

							glDrawArrays(
								GetTopology(command.topology),
								0,
								command.count
							);
						}
						break;
					case DrawMode::elements:
						{
							uint32_t indexCount = command.vao->GetIndexBuffer()->GetCount();

							_stats.triangles += indexCount / 3;

							glDrawElements(
								GetTopology(command.topology),
								indexCount,
								GetDataType(command.dataType),
								command.indices
							);
						}
						break;
					case DrawMode::elements_instanced:
						{
							uint32_t indexCount = command.vao->GetIndexBuffer()->GetCount();

							_stats.triangles += (indexCount / 3) * command.instanceCount;

							glDrawElementsInstanced(
								GetTopology(command.topology),
								indexCount,
								GetDataType(command.dataType),
								command.indices,
								command.instanceCount
							);
						}
						break;
					default:
						break;
				}
			}

			_commands.clear();
			_statsPrev = _stats;
			_stats.Reset();
		}

	private:
		GLenum GetTopology(Topology topology)
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

		GLenum GetDataType(DataType dataType)
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

	};
}
