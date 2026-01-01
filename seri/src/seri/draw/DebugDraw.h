#pragma once

#include "seri/util/Util.h"
#include "seri/texture/Color.h"
#include "seri/graphic/Material.h"
#include "seri/camera/CameraBase.h"
#include "seri/shader/ShaderLibrary.h"
#include "seri/rendering/RenderingManager.h"

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

		static void Line3D(const glm::vec3& beg, const glm::vec3& end, const Color& color, float duration = -1.0f);

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

		static inline std::vector<DebugLine> _lines;
		static inline std::shared_ptr<seri::Material> _material;
		static inline std::shared_ptr<seri::ShaderBase> _shader;
		static inline std::shared_ptr<seri::VertexBufferBase> _vertexBuffer;
		static inline std::shared_ptr<seri::VertexArrayBase> _vertexArray;

		static inline const uint64_t kMaxDrawCount = 1 << 12;

	};
}
