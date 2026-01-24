#include "Seripch.h"

#include "seri/rendering/opengl/RenderCommandBufferOpenGL.h"

namespace seri
{
	void RenderCommandBufferOpenGL::Init()
	{
	}

	void RenderCommandBufferOpenGL::Begin()
	{
		auto perspectiveCamera = Graphic::GetCameraPerspective();
		auto orthoCamera = Graphic::GetCameraOrtho();

		auto editorRT = RenderingManager::GetEditorRT();

		RenderPass passShadow;
		passShadow.desc.type = PassType::shadow;
		passShadow.desc.rt = nullptr;
		passShadow.desc.camera = nullptr;

		RenderPass passSkybox;
		passSkybox.desc.type = PassType::skybox;
		passSkybox.desc.rt = nullptr;
		passSkybox.desc.camera = perspectiveCamera;

		RenderPass passOpaque;
		passOpaque.desc.type = PassType::opaque;
		passOpaque.desc.rt = editorRT;
		passOpaque.desc.camera = perspectiveCamera;

		RenderPass passTransparent;
		passTransparent.desc.type = PassType::transparent;
		passTransparent.desc.rt = editorRT;
		passTransparent.desc.camera = perspectiveCamera;

		RenderPass passDebug;
		passDebug.desc.type = PassType::debug;
		passDebug.desc.rt = editorRT;
		passDebug.desc.camera = perspectiveCamera;

		RenderPass passUI;
		passUI.desc.type = PassType::ui;
		passUI.desc.rt = editorRT;
		passUI.desc.camera = orthoCamera;

		_frameGraph.Clear();
		_frameGraph.AddPass(passShadow);
		_frameGraph.AddPass(passSkybox);
		_frameGraph.AddPass(passOpaque);
		_frameGraph.AddPass(passTransparent);
		_frameGraph.AddPass(passDebug);
		_frameGraph.AddPass(passUI);
	}

	void RenderCommandBufferOpenGL::End()
	{
	}

	void RenderCommandBufferOpenGL::Submit(RenderCommand renderCommand)
	{
		_commands.emplace_back(renderCommand);
	}

	void RenderCommandBufferOpenGL::Execute()
	{
		for (const auto& command : _commands)
		{
			command.rt->Bind();

			RenderingManager::SetBlend(command.state.blendEnabled, command.state.blendFactorSrc, command.state.blendFactorDst);
			RenderingManager::SetFrontFace(command.state.frontFace);
			RenderingManager::SetCullFace(command.state.cullFaceEnabled, command.state.cullFace);
			RenderingManager::SetDepthFunc(command.state.depthTestEnabled, command.state.depthFunc);
			RenderingManager::SetDepthWrite(command.state.depthWriteEnabled);
			RenderingManager::SetStencilFunc(command.state.stencilTestEnabled, command.state.stencilFunc, command.state.stencilRef, command.state.stencilMaskAND);
			RenderingManager::SetStencilOp(command.state.stencilSfail, command.state.stencilDPfail, command.state.stencilDPpass);
			RenderingManager::SetStencilMask(command.state.stencilMask);
			RenderingManager::SetLineWidth(command.state.lineWidth);
			RenderingManager::SetPointSize(command.state.pointSize);

			if (command.noop)
			{
				continue;
			}

			glm::vec4 camPos = command.camera->GetPosition();
			glm::mat4 view = command.camera->GetView();
			glm::mat4 projection = command.camera->GetProjection();

			command.material->SetMat4(literals::kUniformModel, command.model);
			command.material->SetMat4(literals::kUniformView, view);
			command.material->SetMat4(literals::kUniformProjection, projection);
			command.material->SetFloat4(literals::kUniformCameraPos, camPos);
			command.material->Apply();
			command.vao->Bind();

			_stats.drawCalls += 1;

			switch (command.draw.mode)
			{
				case DrawMode::arrays:
					{
						_stats.triangles += command.draw.count / 3;

						glDrawArrays(
							GetTopology(command.draw.topology),
							0,
							command.draw.count
						);
					}
					break;
				case DrawMode::elements:
					{
						uint32_t indexCount = command.vao->GetIndexBuffer()->GetCount();

						_stats.triangles += indexCount / 3;

						glDrawElements(
							GetTopology(command.draw.topology),
							indexCount,
							GetDataType(command.draw.dataType),
							command.draw.indices
						);
					}
					break;
				case DrawMode::elements_instanced:
					{
						uint32_t indexCount = command.vao->GetIndexBuffer()->GetCount();

						_stats.triangles += (indexCount / 3) * command.draw.instanceCount;

						glDrawElementsInstanced(
							GetTopology(command.draw.topology),
							indexCount,
							GetDataType(command.draw.dataType),
							command.draw.indices,
							command.draw.instanceCount
						);
					}
					break;
				default:
					break;
			}
		}

		for (const auto& pass : _frameGraph.passes)
		{
			for (const auto& cmd : pass.items)
			{

			}
		}

		_commands.clear();
		_statsPrev = _stats;
		_stats.Reset();
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
