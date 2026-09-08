#include "gui/panels/GamePanel.h"

namespace seri::editor
{
	const GamePanel::AspectEntry GamePanel::kAspects[GamePanel::kAspectCount] = {
		{ "Free Aspect", 0.0f, 0, 0 },
		{ "16:9", 16.0f / 9.0f, 0, 0 },
		{ "16:10", 16.0f / 10.0f, 0, 0 },
		{ "4:3", 4.0f / 3.0f, 0, 0 },
		{ "5:4", 5.0f / 4.0f, 0, 0 },
		{ "1920x1080", 0.0f, 1920, 1080 },
		{ "1280x720", 0.0f, 1280, 720 },
		{ "800x600", 0.0f, 800, 600 },
	};

	void GamePanel::Update(GUIContext& ctx)
	{
		seri::scene::SceneState state = seri::scene::SceneManager::GetState();
		if (state == _statePrev)
		{
			return;
		}

		bool wasEdit = _statePrev == seri::scene::SceneState::edit;
		bool isEdit = state == seri::scene::SceneState::edit;

		_statePrev = state;

		if (wasEdit && !isEdit)
		{
			ctx.showGame = true;
			_focusRequested = true;
			_maximized = _maximizeOnPlay;
		}
		else if (!wasEdit && isEdit && _maximizeOnPlay)
		{
			_maximized = false;
		}
	}

	void GamePanel::Draw(GUIContext& ctx)
	{
		if (_focusRequested)
		{
			ImGui::SetWindowFocus();
			_focusRequested = false;
		}

		ShowToolbar();

		ImGui::Separator();

		ShowView();
	}

	void GamePanel::ShowToolbar()
	{
		ImGui::SetNextItemWidth(140.0f);

		if (ImGui::BeginCombo("##GameAspect", kAspects[_aspectIndex].name))
		{
			for (int i = 0; i < kAspectCount; i++)
			{
				if (ImGui::Selectable(kAspects[i].name, _aspectIndex == i))
				{
					_aspectIndex = i;
				}
			}

			ImGui::EndCombo();
		}

		ImGui::SameLine();
		ImGui::Checkbox("Maximize", &_maximized);

		ImGui::SameLine();
		ImGui::Checkbox("Maximize On Play", &_maximizeOnPlay);

		ImGui::SameLine();
		ImGui::Checkbox("Stats", &_showStats);
	}

	void GamePanel::ShowView()
	{
		ImVec2 viewSize = ImGui::GetContentRegionAvail();
		if (viewSize.x <= 0.0f || viewSize.y <= 0.0f)
		{
			return;
		}

		ImVec2 viewMin = ImGui::GetCursorScreenPos();
		ImVec2 viewMax = ImVec2(viewMin.x + viewSize.x, viewMin.y + viewSize.y);

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		drawList->AddRectFilled(viewMin, viewMax, IM_COL32(0, 0, 0, 255));

		ImVec2 renderSize;
		ImVec2 imageSize;
		ComputeSizes(viewSize, renderSize, imageSize);

		auto gameRT = seri::RenderingManager::GetGameRT();
		gameRT->Resize(static_cast<uint32_t>(renderSize.x), static_cast<uint32_t>(renderSize.y));

		if (seri::Graphic::GetRuntimeCamera() == nullptr)
		{
			ShowNoCamera(viewMin, viewMax);
			return;
		}

		ImVec2 imageMin = ImFloor(
			ImVec2(
				viewMin.x + (viewSize.x - imageSize.x) * 0.5f,
				viewMin.y + (viewSize.y - imageSize.y) * 0.5f
			)
		);
		ImVec2 imageMax = ImVec2(imageMin.x + imageSize.x, imageMin.y + imageSize.y);

		auto tex = (ImTextureID)(intptr_t)gameRT->GetFirstColorTextureHandle();

		drawList->AddImage(
			tex,
			imageMin,
			imageMax,
			ImVec2(0, 1),
			ImVec2(1, 0)
		);

		if (_showStats)
		{
			ShowStats(viewMin, viewMax);
		}
	}

	void GamePanel::ShowNoCamera(const ImVec2& viewMin, const ImVec2& viewMax)
	{
		const char* text = "No cameras rendering";

		ImVec2 textSize = ImGui::CalcTextSize(text);
		ImVec2 textPos = ImFloor(
			ImVec2(
				(viewMin.x + viewMax.x - textSize.x) * 0.5f,
				(viewMin.y + viewMax.y - textSize.y) * 0.5f
			)
		);

		ImGui::GetWindowDrawList()->AddText(textPos, ImGui::GetColorU32(ImGuiCol_TextDisabled), text);
	}

	void GamePanel::ShowStats(const ImVec2& viewMin, const ImVec2& viewMax)
	{
		auto gameRT = seri::RenderingManager::GetGameRT();
		seri::RenderingStats stats = seri::RenderingManager::GetRenderingStats();

		std::string lines[] = {
			fmt::format("resolution: {}x{}", gameRT->GetWidth(), gameRT->GetHeight()),
			fmt::format("fps: {}", seri::TimeWrapper::GetFPS()),
			fmt::format("draw calls: {}", stats.drawCalls),
			fmt::format("triangles: {}", stats.triangles),
		};

		float padding = 8.0f;
		float margin = 10.0f;
		float lineHeight = ImGui::GetTextLineHeightWithSpacing();

		float textWidth = 0.0f;
		for (const std::string& line : lines)
		{
			textWidth = ImMax(textWidth, ImGui::CalcTextSize(line.c_str()).x);
		}

		ImVec2 boxMin = ImVec2(viewMax.x - textWidth - padding * 2.0f - margin, viewMin.y + margin);
		ImVec2 boxMax = ImVec2(
			boxMin.x + textWidth + padding * 2.0f,
			boxMin.y + lineHeight * IM_ARRAYSIZE(lines) + padding * 2.0f
		);

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		drawList->AddRectFilled(boxMin, boxMax, IM_COL32(0, 0, 0, 180), 4.0f);

		ImU32 color = ImGui::GetColorU32(ImGuiCol_Text);
		for (int i = 0; i < IM_ARRAYSIZE(lines); i++)
		{
			drawList->AddText(
				ImVec2(boxMin.x + padding, boxMin.y + padding + lineHeight * i),
				color,
				lines[i].c_str()
			);
		}
	}

	void GamePanel::ComputeSizes(const ImVec2& viewSize, ImVec2& renderSize, ImVec2& imageSize)
	{
		const AspectEntry& entry = kAspects[_aspectIndex];

		if (entry.width > 0 && entry.height > 0)
		{
			renderSize = ImVec2(static_cast<float>(entry.width), static_cast<float>(entry.height));
			imageSize = FitToView(renderSize.x / renderSize.y, viewSize);
		}
		else if (entry.ratio > 0.0f)
		{
			imageSize = FitToView(entry.ratio, viewSize);
			renderSize = imageSize;
		}
		else
		{
			imageSize = viewSize;
			renderSize = viewSize;
		}

		renderSize.x = ImClamp(ImFloor(renderSize.x), kMinResolution, kMaxResolution);
		renderSize.y = ImClamp(ImFloor(renderSize.y), kMinResolution, kMaxResolution);
	}

	ImVec2 GamePanel::FitToView(float ratio, const ImVec2& viewSize)
	{
		if (viewSize.x / viewSize.y > ratio)
		{
			return ImVec2(viewSize.y * ratio, viewSize.y);
		}

		return ImVec2(viewSize.x, viewSize.x / ratio);
	}
}
