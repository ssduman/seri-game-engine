#pragma once

#include "gui/common/GUICommon.h"

namespace seri::editor
{
	enum class InspectorType
	{
		none,
		scene,
		entity,
		asset,
	};

	struct GUIContext
	{
		uint64_t selectedEntityId{ 0 };
		InspectorType inspectorType{ InspectorType::none };
		seri::asset::AssetTreeNode selectedAsset{};
		std::filesystem::path currentAssetFolder{};

		bool showHierarchy{ true };
		bool showScene{ true };
		bool showGame{ true };
		bool showInspector{ true };
		bool showConsole{ true };
		bool showProject{ true };
		bool resetLayout{ false };
	};
}
