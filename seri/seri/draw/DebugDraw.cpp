#include "Seripch.h"

#include "seri/draw/DebugDraw.h"

namespace seri::debug
{
	void DebugDraw::Init()
	{
		_shader = ShaderLibrary::Find("debug_0");

		_material = std::make_shared<seri::Material>();
		_material->SetShader(_shader);

		_vertexBuffer = seri::VertexBufferBase::Create(nullptr, kMaxDrawCount * sizeof(DebugVertex), BufferUsage::dynamic_draw);
		_vertexBuffer->AddElements({
				{ seri::LayoutLocation::loc_0, seri::ShaderDataType::float3_type },
				{ seri::LayoutLocation::loc_1, seri::ShaderDataType::float4_type },
			});

		_vertexArray = seri::VertexArrayBase::Create();
		_vertexArray->AddVertexBuffer(_vertexBuffer);

		_vertexBufferUI = seri::VertexBufferBase::Create(nullptr, kMaxDrawCount * sizeof(DebugVertex), BufferUsage::dynamic_draw);
		_vertexBufferUI->AddElements({
				{ seri::LayoutLocation::loc_0, seri::ShaderDataType::float3_type },
				{ seri::LayoutLocation::loc_1, seri::ShaderDataType::float4_type },
			});

		_vertexArrayUI = seri::VertexArrayBase::Create();
		_vertexArrayUI->AddVertexBuffer(_vertexBufferUI);
	}

	void DebugDraw::EndFrame()
	{
		float deltaTime = seri::TimeWrapper::GetDeltaTime();

		for (auto* lines : { &_lines, &_linesUI })
		{
			for (auto it = lines->begin(); it != lines->end();)
			{
				it->duration -= deltaTime;

				if (it->duration <= 0.0f)
				{
					it = lines->erase(it);
				}
				else
				{
					++it;
				}
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

		_material->SetMat4(literals::kUniformViewProjection, camera->GetViewProjection());

		seri::RenderItem renderItem{};
		renderItem.type = seri::PassType::debug;
		renderItem.name = "debug_line";
		renderItem.material = _material;
		renderItem.vao = _vertexArray;
		renderItem.state.depthTestEnabled = false;
		renderItem.draw.mode = seri::DrawMode::arrays;
		renderItem.draw.topology = seri::Topology::line;
		renderItem.draw.count = vertices.size();
		renderItem.model = seri::Util::GetIdentityMatrix();

		seri::RenderingManager::Submit(std::move(renderItem));
	}

	void DebugDraw::RenderUI(const std::shared_ptr<seri::CameraBase>& camera)
	{
		if (_linesUI.empty() || !camera)
		{
			return;
		}

		std::vector<DebugVertex> vertices;
		vertices.reserve(_linesUI.size() * 2);
		for (auto& line : _linesUI)
		{
			vertices.push_back(line.beg);
			vertices.push_back(line.end);
		}

		_vertexBufferUI->SetData(vertices.data(), vertices.size() * sizeof(DebugVertex));

		_material->SetMat4(literals::kUniformViewProjection, camera->GetViewProjection());

		seri::RenderItem renderItem{};
		renderItem.type = seri::PassType::ui;
		renderItem.name = "debug_line_ui";
		renderItem.material = _material;
		renderItem.vao = _vertexArrayUI;
		renderItem.state.depthTestEnabled = false;
		renderItem.draw.mode = seri::DrawMode::arrays;
		renderItem.draw.topology = seri::Topology::line;
		renderItem.draw.count = vertices.size();
		renderItem.model = seri::Util::GetIdentityMatrix();
		renderItem.sortOrder = kUIGizmoSortOrder;

		seri::RenderingManager::Submit(std::move(renderItem));
	}

	void DebugDraw::LineUI(const glm::vec3& beg, const glm::vec3& end, const Color& color, float duration)
	{
		DebugLine line;
		line.beg.position = beg;
		line.beg.color = color.GetColorRGBA();
		line.end.position = end;
		line.end.color = color.GetColorRGBA();
		line.duration = duration;

		_linesUI.push_back(line);
	}

	void DebugDraw::Line(const glm::vec3& beg, const glm::vec3& end, const Color& color, float duration)
	{
		DebugLine line;
		line.beg.position = beg;
		line.beg.color = color.GetColorRGBA();
		line.end.position = end;
		line.end.color = color.GetColorRGBA();
		line.duration = duration;

		_lines.push_back(line);
	}

	void DebugDraw::DrawOBB(const glm::mat4& transform, const glm::vec3& halfExtents, const glm::vec4& color, float duration)
	{
		glm::vec3 vertices[8] = {
			{ -halfExtents.x, -halfExtents.y, -halfExtents.z },
			{  halfExtents.x, -halfExtents.y, -halfExtents.z },
			{  halfExtents.x,  halfExtents.y, -halfExtents.z },
			{ -halfExtents.x,  halfExtents.y, -halfExtents.z },
			{ -halfExtents.x, -halfExtents.y,  halfExtents.z },
			{  halfExtents.x, -halfExtents.y,  halfExtents.z },
			{  halfExtents.x,  halfExtents.y,  halfExtents.z },
			{ -halfExtents.x,  halfExtents.y,  halfExtents.z },
		};

		for (auto& vertex : vertices)
		{
			vertex = glm::vec3(transform * glm::vec4(vertex, 1.0f));
		}

		Line(vertices[0], vertices[1], color, duration);
		Line(vertices[1], vertices[2], color, duration);
		Line(vertices[2], vertices[3], color, duration);
		Line(vertices[3], vertices[0], color, duration);
		Line(vertices[4], vertices[5], color, duration);
		Line(vertices[5], vertices[6], color, duration);
		Line(vertices[6], vertices[7], color, duration);
		Line(vertices[7], vertices[4], color, duration);
		Line(vertices[0], vertices[4], color, duration);
		Line(vertices[1], vertices[5], color, duration);
		Line(vertices[2], vertices[6], color, duration);
		Line(vertices[3], vertices[7], color, duration);
	}

	void DebugDraw::DrawAABB(const glm::vec3& min, const glm::vec3& max, const glm::vec4& color, float duration)
	{
		Line({ min.x, min.y, min.z }, { max.x, min.y, min.z }, color, duration);
		Line({ max.x, min.y, min.z }, { max.x, max.y, min.z }, color, duration);
		Line({ max.x, max.y, min.z }, { min.x, max.y, min.z }, color, duration);
		Line({ min.x, max.y, min.z }, { min.x, min.y, min.z }, color, duration);
		Line({ min.x, min.y, max.z }, { max.x, min.y, max.z }, color, duration);
		Line({ max.x, min.y, max.z }, { max.x, max.y, max.z }, color, duration);
		Line({ max.x, max.y, max.z }, { min.x, max.y, max.z }, color, duration);
		Line({ min.x, max.y, max.z }, { min.x, min.y, max.z }, color, duration);
		Line({ min.x, min.y, min.z }, { min.x, min.y, max.z }, color, duration);
		Line({ max.x, min.y, min.z }, { max.x, min.y, max.z }, color, duration);
		Line({ max.x, max.y, min.z }, { max.x, max.y, max.z }, color, duration);
		Line({ min.x, max.y, min.z }, { min.x, max.y, max.z }, color, duration);
	}

	void DebugDraw::DrawSphere(const glm::vec3& center, float radius, const glm::vec4& color, int segments, float duration)
	{
		const int latSegments = segments / 2;
		const int lonSegments = segments;
		for (int i = 0; i <= latSegments; ++i)
		{
			float theta1 = i * glm::pi<float>() / latSegments;
			float theta2 = (i + 1) * glm::pi<float>() / latSegments;
			for (int j = 0; j <= lonSegments; ++j)
			{
				float phi1 = j * 2.0f * glm::pi<float>() / lonSegments;
				float phi2 = (j + 1) * 2.0f * glm::pi<float>() / lonSegments;
				glm::vec3 p1 = center + radius * glm::vec3(sin(theta1) * cos(phi1), cos(theta1), sin(theta1) * sin(phi1));
				glm::vec3 p2 = center + radius * glm::vec3(sin(theta2) * cos(phi1), cos(theta2), sin(theta2) * sin(phi1));
				glm::vec3 p3 = center + radius * glm::vec3(sin(theta2) * cos(phi2), cos(theta2), sin(theta2) * sin(phi2));
				glm::vec3 p4 = center + radius * glm::vec3(sin(theta1) * cos(phi2), cos(theta1), sin(theta1) * sin(phi2));
				Line(p1, p2, color, duration);
				Line(p2, p3, color, duration);
				Line(p3, p4, color, duration);
				Line(p4, p1, color, duration);
			}
		}
	}

	void DebugDraw::DrawRay(const glm::vec3& origin, const glm::vec3& dir, float length, const glm::vec4& color, float duration)
	{
		glm::vec3 end = origin + glm::normalize(dir) * length;
		Line(origin, end, color, duration);
	}

	void DebugDraw::DrawFrustum(const glm::mat4& viewProj, const glm::vec4& color, float duration)
	{
		glm::mat4 invVP = glm::inverse(viewProj);
		glm::vec3 ndcCorners[8] = {
			{ -1, -1, -1 },
			{  1, -1, -1 },
			{  1,  1, -1 },
			{ -1,  1, -1 },
			{ -1, -1,  1 },
			{  1, -1,  1 },
			{  1,  1,  1 },
			{ -1,  1,  1 },
		};
		glm::vec3 frustumCorners[8];
		for (int i = 0; i < 8; ++i)
		{
			glm::vec4 corner = invVP * glm::vec4(ndcCorners[i], 1.0f);
			frustumCorners[i] = glm::vec3(corner) / corner.w;
		}
		Line(frustumCorners[0], frustumCorners[1], color, duration);
		Line(frustumCorners[1], frustumCorners[2], color, duration);
		Line(frustumCorners[2], frustumCorners[3], color, duration);
		Line(frustumCorners[3], frustumCorners[0], color, duration);
		Line(frustumCorners[4], frustumCorners[5], color, duration);
		Line(frustumCorners[5], frustumCorners[6], color, duration);
		Line(frustumCorners[6], frustumCorners[7], color, duration);
		Line(frustumCorners[7], frustumCorners[4], color, duration);
		Line(frustumCorners[0], frustumCorners[4], color, duration);
		Line(frustumCorners[1], frustumCorners[5], color, duration);
		Line(frustumCorners[2], frustumCorners[6], color, duration);
		Line(frustumCorners[3], frustumCorners[7], color, duration);
	}

	void DebugDraw::DrawCircle(const glm::vec3& center, const glm::vec3& normal, float radius, const glm::vec4& color, int segments, float duration)
	{
		glm::vec3 axis = glm::normalize(normal);
		glm::vec3 right;
		glm::vec3 up;
		BuildBasis(axis, right, up);

		glm::vec3 prev = center + right * radius;
		for (int i = 1; i <= segments; ++i)
		{
			float angle = i * 2.0f * glm::pi<float>() / segments;
			glm::vec3 next = center + (right * std::cos(angle) + up * std::sin(angle)) * radius;
			Line(prev, next, color, duration);
			prev = next;
		}
	}

	void DebugDraw::DrawWireSphere(const glm::vec3& center, float radius, const glm::vec4& color, int segments, float duration)
	{
		DrawCircle(center, glm::vec3{ 1.0f, 0.0f, 0.0f }, radius, color, segments, duration);
		DrawCircle(center, glm::vec3{ 0.0f, 1.0f, 0.0f }, radius, color, segments, duration);
		DrawCircle(center, glm::vec3{ 0.0f, 0.0f, 1.0f }, radius, color, segments, duration);
	}

	void DebugDraw::DrawCone(const glm::vec3& apex, const glm::vec3& direction, float halfAngle, float range, const glm::vec4& color, int segments, float duration)
	{
		glm::vec3 forward = glm::normalize(direction);
		glm::vec3 right;
		glm::vec3 up;
		BuildBasis(forward, right, up);

		float radius = std::tan(glm::radians(glm::clamp(halfAngle, 0.0f, 89.0f))) * range;
		glm::vec3 center = apex + forward * range;

		DrawCircle(center, forward, radius, color, segments, duration);

		Line(apex, center + right * radius, color, duration);
		Line(apex, center - right * radius, color, duration);
		Line(apex, center + up * radius, color, duration);
		Line(apex, center - up * radius, color, duration);
	}

	void DebugDraw::DrawArrow(const glm::vec3& beg, const glm::vec3& end, const glm::vec4& color, float headSize, float duration)
	{
		Line(beg, end, color, duration);

		glm::vec3 delta = end - beg;
		if (glm::length(delta) < glm::epsilon<float>())
		{
			return;
		}

		glm::vec3 forward = glm::normalize(delta);
		glm::vec3 right;
		glm::vec3 up;
		BuildBasis(forward, right, up);

		glm::vec3 base = end - forward * headSize;
		Line(end, base + right * headSize * 0.5f, color, duration);
		Line(end, base - right * headSize * 0.5f, color, duration);
		Line(end, base + up * headSize * 0.5f, color, duration);
		Line(end, base - up * headSize * 0.5f, color, duration);
	}

	void DebugDraw::BuildBasis(const glm::vec3& axis, glm::vec3& right, glm::vec3& up)
	{
		glm::vec3 reference = std::abs(axis.y) > 0.999f ? glm::vec3{ 1.0f, 0.0f, 0.0f } : glm::vec3{ 0.0f, 1.0f, 0.0f };
		right = glm::normalize(glm::cross(reference, axis));
		up = glm::cross(axis, right);
	}

}
