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

		void DrawAssetHeader(GUIContext& ctx);

		void DrawAssetMaterial(GUIContext& ctx);

		void DrawAssetTexture(GUIContext& ctx);

		void DrawAssetMesh(GUIContext& ctx);

		void DrawAssetShader(GUIContext& ctx);

		void ShowComponentPickerPopup(GUIContext& ctx);

	};
}
