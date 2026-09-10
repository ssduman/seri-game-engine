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

		void DrawAssetFont(GUIContext& ctx);

		void ShowComponentPickerPopup(GUIContext& ctx);

		std::string GetMaterialSlotLabel(const std::shared_ptr<seri::Model>& model, size_t slot);

	};
}
