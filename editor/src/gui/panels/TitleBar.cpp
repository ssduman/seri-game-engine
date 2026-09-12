#include "Editorpch.h"

#include "gui/panels/TitleBar.h"
#include "gui/EditorWidgets.h"

namespace seri::editor
{
	TitleBar::~TitleBar()
	{
		seri::WindowManager::SetCustomTitleBar(nullptr);
	}

	void TitleBar::Init()
	{
		LoadIcon();

		seri::WindowManager::SetCustomTitleBar(
			[this](int x, int y)
			{
				if (!_draggable || y < 0 || y >= static_cast<int>(kTitleBarHeight))
				{
					return false;
				}

				bool overMenus = x >= static_cast<int>(_menusMinX) && x < static_cast<int>(_menusMaxX);
				bool overPlay = x >= static_cast<int>(_playMinX) && x < static_cast<int>(_playMaxX);
				bool overControls = x >= static_cast<int>(_controlsMinX);

				return !overMenus && !overPlay && !overControls;
			}
		);
	}

	void TitleBar::LoadIcon()
	{
		std::filesystem::path iconPath = seri::asset::AssetManager::GetAssetDirectory() / "icons" / "seri.png";
		if (!std::filesystem::exists(iconPath))
		{
			LIB_LOGGER(warning, gui) << fmt::format("editor icon not found: {}", iconPath.string());
			return;
		}

		seri::TextureDesc desc{};
		desc.minFilter = seri::TextureMinFilter::linear_mipmap_linear;

		_icon = seri::TextureBase::Create();
		_icon->Init(desc, iconPath.string());

		int width, height, components;
		if (void* pixels = seri::TextureBase::LoadTexture(iconPath.string(), width, height, components, false))
		{
			if (components == 4)
			{
				seri::WindowManager::SetWindowIcon(width, height, static_cast<const unsigned char*>(pixels));
			}
			else
			{
				LIB_LOGGER(warning, gui) << "editor icon has unsupported number of components: " << components;
			}

			seri::TextureBase::UnloadTexture(pixels);
		}
	}

	void TitleBar::Draw(GUIContext& ctx)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();

		ImGuiWindowFlags window_flags =
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_MenuBar
			;

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, (kTitleBarHeight - ImGui::GetFontSize()) * 0.5f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		bool open = ImGui::BeginViewportSideBar("##EditorTitleBar", viewport, ImGuiDir_Up, kTitleBarHeight, window_flags);
		bool menuBar = open && ImGui::BeginMenuBar();

		ImGui::PopStyleVar(2);

		_draggable = false;

		if (menuBar)
		{
			const float iconSize = 20.0f;
			ImVec2 iconMin(ImGui::GetCursorScreenPos().x, ImGui::GetWindowPos().y + (kTitleBarHeight - iconSize) * 0.5f);
			if (_icon)
			{
				auto tex = (ImTextureID)(intptr_t)_icon->GetHandle();
				ImGui::GetWindowDrawList()->AddImage(tex, iconMin, ImVec2(iconMin.x + iconSize, iconMin.y + iconSize), ImVec2(0, 1), ImVec2(1, 0));
			}
			ImGui::Dummy(ImVec2(iconSize, 0.0f));

			ImGui::TextUnformatted("Seri");

			ImGui::Dummy(ImVec2(6.0f, 0.0f));

			_menusMinX = ImGui::GetCursorPosX();
			ShowMenus(ctx);
			_menusMaxX = ImGui::GetCursorPosX();

			ShowPlayControls();

			ShowControls();

			_draggable = !ImGui::IsPopupOpen("", ImGuiPopupFlags_AnyPopup);

			ImGui::EndMenuBar();
		}

		ImGui::End();
	}

	void TitleBar::ShowMenus(GUIContext& ctx)
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Noop", "Noop"))
			{
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			bool canSave = seri::scene::SceneManager::GetState() == seri::scene::SceneState::edit;
			if (ImGui::MenuItem("Save", "CTRL+S", false, canSave))
			{
				Save();
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window"))
		{
			ImGui::MenuItem("Hierarchy", nullptr, &ctx.showHierarchy);
			ImGui::MenuItem("Scene", nullptr, &ctx.showScene);
			ImGui::MenuItem("Game", nullptr, &ctx.showGame);
			ImGui::MenuItem("Inspector", nullptr, &ctx.showInspector);
			ImGui::MenuItem("Project", nullptr, &ctx.showProject);
			ImGui::MenuItem("Console", nullptr, &ctx.showConsole);

			ImGui::Separator();

			if (ImGui::MenuItem("Reset Layout"))
			{
				ctx.resetLayout = true;
			}

			ImGui::EndMenu();
		}
	}

	void TitleBar::ShowControls()
	{
		const float buttonWidth = 46.0f;
		const ImVec4 hovered = ImVec4(1.0f, 1.0f, 1.0f, 0.10f);
		const ImVec4 active = ImVec4(1.0f, 1.0f, 1.0f, 0.06f);
		const ImVec4 closeHovered = RGBNormalized(196, 43, 28);
		const ImVec4 closeActive = RGBNormalized(160, 35, 23);
		const ImU32 glyphColor = ImGui::GetColorU32(ImGuiCol_Text);

		auto itemCenter = []()
			{
				ImVec2 min = ImGui::GetItemRectMin();
				ImVec2 max = ImGui::GetItemRectMax();
				return ImFloor(ImVec2((min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f));
			};

		_controlsMinX = ImGui::GetWindowWidth() - buttonWidth * 3.0f;

		ImGui::SetCursorPos(ImVec2(_controlsMinX, 0.0f));

		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

		ImDrawList* drawList = ImGui::GetWindowDrawList();

		if (ShowButton("##minimize", buttonWidth, hovered, active))
		{
			seri::WindowManager::IconifyWindow();
		}

		ImVec2 c = itemCenter();

		drawList->AddLine(ImVec2(c.x - 5.0f, c.y), ImVec2(c.x + 4.0f, c.y), glyphColor, 1.0f);

		bool maximized = seri::WindowManager::IsWindowMaximized();
		if (ShowButton("##maximize", buttonWidth, hovered, active))
		{
			if (maximized)
			{
				seri::WindowManager::RestoreWindow();
			}
			else
			{
				seri::WindowManager::MaximizeWindow();
			}
		}

		c = itemCenter();

		if (maximized)
		{
			drawList->AddRect(ImVec2(c.x - 5.0f, c.y - 3.0f), ImVec2(c.x + 3.0f, c.y + 5.0f), glyphColor, 0.0f, 0, 1.0f);
			ImVec2 points[4] = {
				ImVec2(c.x - 2.5f, c.y - 2.5f),
				ImVec2(c.x - 2.5f, c.y - 4.5f),
				ImVec2(c.x + 4.5f, c.y - 4.5f),
				ImVec2(c.x + 4.5f, c.y + 2.5f),
			};
			drawList->AddPolyline(points, 4, glyphColor, ImDrawFlags_None, 1.0f);
		}
		else
		{
			drawList->AddRect(ImVec2(c.x - 5.0f, c.y - 5.0f), ImVec2(c.x + 5.0f, c.y + 5.0f), glyphColor, 0.0f, 0, 1.0f);
		}

		if (ShowButton("##close", buttonWidth, closeHovered, closeActive))
		{
			seri::WindowManager::SetWindowShouldCloseToTrue();
		}

		c = itemCenter();

		drawList->AddLine(ImVec2(c.x - 5.0f, c.y - 5.0f), ImVec2(c.x + 4.0f, c.y + 4.0f), glyphColor, 1.0f);
		drawList->AddLine(ImVec2(c.x - 5.0f, c.y + 4.0f), ImVec2(c.x + 4.0f, c.y - 5.0f), glyphColor, 1.0f);

		ImGui::PopStyleColor();
		ImGui::PopStyleVar(3);
	}

	void TitleBar::ShowPlayControls()
	{
		seri::scene::SceneState state = seri::scene::SceneManager::GetState();
		bool playing = state != seri::scene::SceneState::edit;
		bool paused = state == seri::scene::SceneState::paused;
		bool running = state == seri::scene::SceneState::play;

		const float buttonWidth = 34.0f;
		const ImVec4 transparent = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		const ImVec4 highlight = ImVec4(1.0f, 1.0f, 1.0f, 0.14f);
		const ImVec4 hovered = ImVec4(1.0f, 1.0f, 1.0f, 0.10f);
		const ImVec4 active = ImVec4(1.0f, 1.0f, 1.0f, 0.06f);

		auto itemCenter = []()
			{
				ImVec2 min = ImGui::GetItemRectMin();
				ImVec2 max = ImGui::GetItemRectMax();
				return ImFloor(ImVec2((min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f));
			};

		_playMinX = ImFloor((ImGui::GetWindowWidth() - buttonWidth * 3.0f) * 0.5f);
		_playMaxX = _playMinX + buttonWidth * 3.0f;

		ImGui::SetCursorPos(ImVec2(_playMinX, 0.0f));

		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

		ImDrawList* drawList = ImGui::GetWindowDrawList();

		ImGui::PushStyleColor(ImGuiCol_Button, running ? highlight : transparent);
		ImGui::BeginDisabled(running);

		if (ShowButton("##play", buttonWidth, hovered, active))
		{
			seri::scene::SceneManager::SetState(seri::scene::SceneState::play);
		}

		ImVec2 c = itemCenter();

		drawList->AddTriangleFilled(
			ImVec2(c.x - 4.0f, c.y - 6.0f),
			ImVec2(c.x - 4.0f, c.y + 6.0f),
			ImVec2(c.x + 6.0f, c.y),
			ImGui::GetColorU32(ImGuiCol_Text)
		);

		ImGui::SetItemTooltip("Play");

		ImGui::EndDisabled();
		ImGui::PopStyleColor();

		ImGui::PushStyleColor(ImGuiCol_Button, paused ? highlight : transparent);
		ImGui::BeginDisabled(!running);

		if (ShowButton("##pause", buttonWidth, hovered, active))
		{
			seri::scene::SceneManager::SetState(seri::scene::SceneState::paused);
		}

		c = itemCenter();

		drawList->AddRectFilled(ImVec2(c.x - 5.0f, c.y - 6.0f), ImVec2(c.x - 2.0f, c.y + 6.0f), ImGui::GetColorU32(ImGuiCol_Text));
		drawList->AddRectFilled(ImVec2(c.x + 2.0f, c.y - 6.0f), ImVec2(c.x + 5.0f, c.y + 6.0f), ImGui::GetColorU32(ImGuiCol_Text));

		ImGui::SetItemTooltip("Pause");

		ImGui::EndDisabled();
		ImGui::PopStyleColor();

		ImGui::PushStyleColor(ImGuiCol_Button, transparent);
		ImGui::BeginDisabled(!playing);

		if (ShowButton("##stop", buttonWidth, hovered, active))
		{
			seri::scene::SceneManager::SetState(seri::scene::SceneState::edit);
		}

		c = itemCenter();

		drawList->AddRectFilled(ImVec2(c.x - 5.0f, c.y - 5.0f), ImVec2(c.x + 5.0f, c.y + 5.0f), ImGui::GetColorU32(ImGuiCol_Text));

		ImGui::SetItemTooltip("Stop");

		ImGui::EndDisabled();
		ImGui::PopStyleColor();

		ImGui::PopStyleVar(3);

		const char* stateText = paused ? "Paused" : (running ? "Playing" : "Edit");

		ImVec2 buttonMin = ImGui::GetItemRectMin();
		ImVec2 buttonMax = ImGui::GetItemRectMax();
		ImVec2 textSize = ImGui::CalcTextSize(stateText);

		ImVec2 textPos = ImFloor(
			ImVec2(
				buttonMax.x + 12.0f,
				(buttonMin.y + buttonMax.y - textSize.y) * 0.5f
			)
		);

		drawList->AddText(textPos, ImGui::GetColorU32(ImGuiCol_Text), stateText);
	}

	bool TitleBar::ShowButton(const char* id, float width, const ImVec4& hoveredColor, const ImVec4& activeColor)
	{
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoveredColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, activeColor);
		bool pressed = ImGui::Button(id, ImVec2(width, kTitleBarHeight));
		ImGui::PopStyleColor(2);
		return pressed;
	}

}
