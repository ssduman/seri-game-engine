#pragma once

#include "gui/GUIContext.h"

namespace seri::editor
{
	class ScenePanel
	{
	public:
		void Draw(GUIContext& ctx);

	private:
		enum class GizmoSpace
		{
			local,
			world,
		};

		enum class GizmoOperation
		{
			translate,
			rotate,
			scale,
		};

		void ControlMove(const ImVec2& imageMin, const ImVec2& imageMax);

		void ShowGizmoToolbar(const ImVec2& imageMin);

		void ShowGizmo(const ImVec2& imageMin, const ImVec2& imageSize);

		void ShowEntityGizmo(GUIContext& ctx, const ImVec2& imageMin, const ImVec2& imageSize);

		GizmoSpace _gizmoSpace{ GizmoSpace::local };
		GizmoOperation _gizmoOperation{ GizmoOperation::translate };

	};
}
