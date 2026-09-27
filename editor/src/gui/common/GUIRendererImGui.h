#pragma once

#include "gui/common/GUIRendererBase.h"

namespace seri::editor
{
	class GUIRendererImGui : public GUIRendererBase
	{
	public:
		void Init() override;

		void Shutdown() override;

		void NewFrame() override;

		void RenderDrawData(ImDrawData* drawData) override;

	};
}
