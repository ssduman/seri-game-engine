#pragma once

#include "gui/GUIContext.h"

namespace seri::editor
{
	class InspectorPanel
	{
	public:
		void Draw(GUIContext& ctx);

	private:
		void DrawScene();

		void DrawEntity(GUIContext& ctx);

		void DrawAsset(GUIContext& ctx);

		void DrawAssetMaterial(GUIContext& ctx);

		void ShowComponentPickerPopup(GUIContext& ctx);

	};
}
