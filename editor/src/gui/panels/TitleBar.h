#pragma once

#include "gui/GUIContext.h"

namespace seri::editor
{
	class TitleBar
	{
	public:
		~TitleBar();

		void Init();

		void Draw(GUIContext& ctx);

	private:
		void LoadIcon();

		void ShowMenus(GUIContext& ctx);

		void ShowControls();

		void ShowPlayControls();

		bool ShowButton(const char* id, float width, const ImVec4& hoveredColor, const ImVec4& activeColor);

		std::shared_ptr<seri::TextureBase> _icon;

		bool _draggable{ false };
		float _controlsMinX{ 0.0f };
		float _menusMinX{ 0.0f };
		float _menusMaxX{ 0.0f };
		float _playMinX{ 0.0f };
		float _playMaxX{ 0.0f };

		static constexpr float kTitleBarHeight = 36.0f;

	};
}
