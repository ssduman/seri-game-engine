#pragma once

#include "gui/GUIContext.h"

namespace seri::editor
{
	class GamePanel
	{
	public:
		void Update(GUIContext& ctx);

		void Draw(GUIContext& ctx);

		bool IsMaximized() { return _maximized; }

	private:
		struct AspectEntry
		{
			const char* name;
			float ratio;
			int width;
			int height;
		};

		void ShowToolbar();

		void ShowView();

		void ShowNoCamera(const ImVec2& viewMin, const ImVec2& viewMax);

		void ShowStats(const ImVec2& viewMin, const ImVec2& viewMax);

		void ComputeSizes(const ImVec2& viewSize, ImVec2& renderSize, ImVec2& imageSize);

		ImVec2 FitToView(float ratio, const ImVec2& viewSize);

		int _aspectIndex{ 0 };
		bool _maximized{ false };
		bool _maximizeOnPlay{ false };
		bool _showStats{ false };
		bool _focusRequested{ false };
		seri::scene::SceneState _statePrev{ seri::scene::SceneState::edit };

		static constexpr int kAspectCount = 8;
		static constexpr float kMinResolution = 16.0f;
		static constexpr float kMaxResolution = 4096.0f;

		static const AspectEntry kAspects[kAspectCount];

	};
}
