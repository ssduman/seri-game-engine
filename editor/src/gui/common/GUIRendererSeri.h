#pragma once

#include "gui/common/GUIRendererBase.h"

namespace seri::editor
{
	class GUIRendererSeri : public GUIRendererBase
	{
	public:
		void Init() override;

		void Shutdown() override;

		void NewFrame() override;

		void RenderDrawData(ImDrawData* drawData) override;

	private:
		void SetupRenderState(ImDrawData* drawData, int fbWidth, int fbHeight);

		void UpdateTexture(ImTextureData* tex);

		void DestroyTexture(ImTextureData* tex);

		static void RenderWindow(ImGuiViewport* viewport, void*);

		std::shared_ptr<seri::ShaderBase> _shader{ nullptr };
		std::shared_ptr<seri::VertexBufferBase> _vertexBuffer{ nullptr };
		std::shared_ptr<seri::IndexBufferBase> _indexBuffer{ nullptr };
		std::unordered_map<ImTextureData*, std::shared_ptr<seri::TextureBase>> _textures{};

	};
}
