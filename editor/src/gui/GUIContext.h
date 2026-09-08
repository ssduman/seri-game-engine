#pragma once

#include "gui/EditorCommon.h"

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

		bool showHierarchy{ true };
		bool showScene{ true };
		bool showGame{ true };
		bool showInspector{ true };
		bool showConsole{ true };
		bool showProject{ true };
		bool resetLayout{ false };
	};
}
