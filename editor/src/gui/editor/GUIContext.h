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
		animation_state_machine,
	};

	struct GUIContext
	{
		uint64_t selectedEntityId{ 0 };
		bool revealSelectedEntity{ false };
		InspectorType inspectorType{ InspectorType::none };
		seri::asset::AssetTreeNode selectedAsset{};
		std::filesystem::path currentAssetFolder{};

		uint64_t asmAssetId{ 0 };
		int asmNodeIndex{ -1 };
		int asmTransitionIndex{ -1 };

		bool showGizmos{ true };

		bool showHierarchy{ true };
		bool showScene{ true };
		bool showGame{ true };
		bool showInspector{ true };
		bool showConsole{ true };
		bool showProject{ true };
		bool showASM{ false };
		bool focusASM{ false };
		bool resetLayout{ false };
	};
}
