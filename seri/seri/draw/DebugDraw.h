#pragma once

#include "seri/util/Util.h"
#include "seri/texture/Color.h"
#include "seri/graphic/Material.h"
#include "seri/camera/CameraBase.h"
#include "seri/shader/ShaderLibrary.h"
#include "seri/rendering/render/RenderingManager.h"

#include <limits>
#include <memory>
#include <vector>

namespace seri::debug
{
	class DebugDraw
	{
	public:
		static void Init();

		static void EndFrame();

		static void Render(const std::shared_ptr<seri::CameraBase>& camera);
		static void RenderUI(const std::shared_ptr<seri::CameraBase>& camera);

		static void Line(const glm::vec3& beg, const glm::vec3& end, const Color& color, float duration = -1.0f);
		static void DrawOBB(const glm::mat4& transform, const glm::vec3& halfExtents, const glm::vec4& color, float duration = -1.0f);
		static void DrawAABB(const glm::vec3& min, const glm::vec3& max, const glm::vec4& color, float duration = -1.0f);
		static void DrawSphere(const glm::vec3& center, float radius, const glm::vec4& color, int segments = 24, float duration = -1.0f);
		static void DrawRay(const glm::vec3& origin, const glm::vec3& dir, float length, const glm::vec4& color, float duration = -1.0f);
		static void DrawFrustum(const glm::mat4& viewProj, const glm::vec4& color, float duration = -1.0f);
		static void DrawCircle(const glm::vec3& center, const glm::vec3& normal, float radius, const glm::vec4& color, int segments = 24, float duration = -1.0f);
		static void DrawWireSphere(const glm::vec3& center, float radius, const glm::vec4& color, int segments = 24, float duration = -1.0f);
		static void DrawCone(const glm::vec3& apex, const glm::vec3& direction, float halfAngle, float range, const glm::vec4& color, int segments = 24, float duration = -1.0f);
		static void DrawArrow(const glm::vec3& beg, const glm::vec3& end, const glm::vec4& color, float headSize = 0.2f, float duration = -1.0f);

		static void LineUI(const glm::vec3& beg, const glm::vec3& end, const Color& color, float duration = -1.0f);

		private:
		struct DebugVertex
		{
			glm::vec3 position;
			glm::vec4 color;
		};

		struct DebugLine
		{
			DebugVertex beg;
			DebugVertex end;
			float duration;
		};

		static inline std::shared_ptr<seri::Material> _material;
		static inline std::shared_ptr<seri::ShaderBase> _shader;

		static inline std::vector<DebugLine> _lines;
		static inline std::shared_ptr<seri::VertexBufferBase> _vertexBuffer;
		static inline std::shared_ptr<seri::VertexArrayBase> _vertexArray;

		static inline std::vector<DebugLine> _linesUI;
		static inline std::shared_ptr<seri::VertexBufferBase> _vertexBufferUI;
		static inline std::shared_ptr<seri::VertexArrayBase> _vertexArrayUI;

		static inline const uint64_t kMaxDrawCount = 1 << 12;
		static inline const int kUIGizmoSortOrder = std::numeric_limits<int>::max();

		static void BuildBasis(const glm::vec3& axis, glm::vec3& right, glm::vec3& up);

	};
}
