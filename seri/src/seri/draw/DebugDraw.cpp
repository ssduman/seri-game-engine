#include "Seripch.h"

#include "seri/draw/DebugDraw.h"

namespace seri::debug
{
	void DebugDraw::Init()
	{
		_shader = ShaderLibrary::Find("debug_0");

		_material = std::make_shared<seri::Material>();
		_material->shader = _shader;
		_material->SetMat4("u_view_proj", glm::mat4{ 1.0f });

		_vertexBuffer = seri::VertexBufferBase::Create(nullptr, kMaxDrawCount * sizeof(DebugVertex), BufferUsage::dynamic_draw);
		_vertexBuffer->AddElements({
				{ seri::LayoutLocation::loc_0, seri::ShaderDataType::float3_type },
				{ seri::LayoutLocation::loc_1, seri::ShaderDataType::float4_type },
			});

		_vertexArray = seri::VertexArrayBase::Create();
		_vertexArray->AddVertexBuffer(_vertexBuffer);
	}

	void DebugDraw::EndFrame()
	{
		float deltaTime = seri::TimeWrapper::GetDeltaTime();

		for (auto it = _lines.begin(); it != _lines.end();)
		{
			it->duration -= deltaTime;

			if (it->duration <= 0.0f)
			{
				it = _lines.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	void DebugDraw::Render(const std::shared_ptr<seri::CameraBase>& camera)
	{
		if (_lines.empty())
		{
			return;
		}

		std::vector<DebugVertex> vertices;
		vertices.reserve(_lines.size() * 2);
		for (auto& line : _lines)
		{
			vertices.push_back(line.beg);
			vertices.push_back(line.end);
		}

		_vertexBuffer->SetData(vertices.data(), vertices.size() * sizeof(DebugVertex));

		_material->SetMat4("u_view_proj", camera->GetViewProjection());

		seri::RenderCommand renderCommand{};
		renderCommand.name = "debug_line";
		renderCommand.camera = camera;
		renderCommand.material = _material;
		renderCommand.vao = _vertexArray;
		renderCommand.desc.depthTestEnabled = false;
		renderCommand.drawMode = seri::DrawMode::arrays;
		renderCommand.topology = seri::Topology::line;
		renderCommand.count = vertices.size();
		renderCommand.rt = seri::RenderingManager::GetEditorRT();
		renderCommand.trs = seri::Util::GetIdentityMatrix();

		seri::RenderingManager::Submit(renderCommand);
	}

	void DebugDraw::Line3D(const glm::vec3& beg, const glm::vec3& end, const Color& color, float duration)
	{
		DebugLine line;
		line.beg.position = beg;
		line.beg.color = color.GetColorRGBA();
		line.end.position = end;
		line.end.color = color.GetColorRGBA();
		line.duration = duration;

		_lines.push_back(line);
	}

}
