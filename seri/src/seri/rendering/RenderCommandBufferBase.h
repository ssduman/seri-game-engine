#pragma once

#include "seri/util/Util.h"
#include "seri/graphic/Material.h"
#include "seri/camera/CameraBase.h"
#include "seri/rendering/PipelineBase.h"
#include "seri/rendering/RenderingStats.h"
#include "seri/rendering/RenderingManagerBase.h"

namespace seri
{
	struct RenderCommand
	{
	public:
		std::string name;
		bool noop{ false };

		PipelineDesc desc;

		uint32_t count{ 0 };
		uint32_t instanceCount{ 0 };
		DrawMode drawMode{ DrawMode::elements };
		Topology topology{ Topology::triangle };
		DataType dataType{ DataType::uint_type };
		const void* indices{ nullptr };

		std::shared_ptr<CameraBase> camera{ nullptr };
		std::shared_ptr<Material> material{ nullptr };
		std::shared_ptr<VertexArrayBase> vao{ nullptr };

		glm::mat4 trs{ 1.0f };
	};

	class RenderCommandBufferBase
	{
	public:
		virtual void Begin(std::shared_ptr<CameraBase> camera) = 0;

		virtual void End() = 0;

		virtual void Submit(RenderCommand renderCommand) = 0;

		virtual void Execute() = 0;

		RenderingStats GetStats()
		{
			return _statsPrev;
		}

	protected:
		std::vector<RenderCommand> _commands;

		RenderingStats _stats;
		RenderingStats _statsPrev;

	};
}
