#include "gui/panels/ConsolePanel.h"

namespace seri::editor
{
	void ConsolePanel::Update()
	{
		size_t previousCount = _entries.size();

		seri::LogBuffer::Drain(_entries);

		if (_entries.size() == previousCount)
		{
			return;
		}

		for (size_t i = previousCount; i < _entries.size(); i++)
		{
			_levelCounts[static_cast<int>(_entries[i].level)]++;
		}

		if (_entries.size() > _maxEntries)
		{
			size_t removeCount = _entries.size() - _maxEntries;

			for (size_t i = 0; i < removeCount; i++)
			{
				_levelCounts[static_cast<int>(_entries[i].level)]--;
			}

			_entries.erase(_entries.begin(), _entries.begin() + removeCount);
			_selectedEntry = -1;
			_filterDirty = true;
		}
		else if (!_filterDirty)
		{
			for (size_t i = previousCount; i < _entries.size(); i++)
			{
				if (MatchFilter(_entries[i]))
				{
					_visibleEntries.push_back(static_cast<int>(i));
				}
			}
		}

		if (_autoScroll)
		{
			_scrollToBottom = true;
		}
	}

	void ConsolePanel::Draw()
	{
		ShowToolbar();

		ImGui::Separator();

		ShowEntries();
		ShowDetail();

		if (_clearPending)
		{
			Clear();
			_clearPending = false;
		}
	}

	void ConsolePanel::ShowToolbar()
	{
		if (ImGui::Button("Options"))
		{
			ImGui::OpenPopup("ConsoleOptions");
		}

		if (ImGui::BeginPopup("ConsoleOptions"))
		{
			ImGui::Checkbox("Auto scroll", &_autoScroll);
			ImGui::Checkbox("Show detail", &_showDetail);

			ImGui::Separator();

			ImGui::Checkbox("Timestamp", &_showTimeStamp);
			ImGui::Checkbox("Thread id", &_showThreadId);
			ImGui::Checkbox("Module", &_showModule);
			ImGui::Checkbox("Source", &_showSource);

			ImGui::EndPopup();
		}

		ImGui::SameLine();
		ShowLevelFilter(seri::LogLevel::error);

		ImGui::SameLine();
		ShowLevelFilter(seri::LogLevel::warning);

		ImGui::SameLine();
		ShowLevelFilter(seri::LogLevel::info);

		ImGui::SameLine();
		ShowLevelFilter(seri::LogLevel::verbose);

		ImGui::SameLine();

		float clearButtonWidth = _search.empty() ? 0.0f : ImGui::CalcTextSize("X").x + ImGui::GetStyle().FramePadding.x * 2.0f + ImGui::GetStyle().ItemSpacing.x;
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - clearButtonWidth);

		if (ImGui::InputTextWithHint("##ConsoleSearch", "search", &_search))
		{
			_filterDirty = true;
		}

		if (!_search.empty())
		{
			ImGui::SameLine();

			if (ImGui::Button("X##ConsoleSearchClear"))
			{
				_search.clear();
				_filterDirty = true;
			}
		}
	}

	void ConsolePanel::ShowLevelFilter(seri::LogLevel level)
	{
		int index = static_cast<int>(level);

		bool enabled = _levelEnabled[index];

		ImGui::PushStyleColor(ImGuiCol_Text, enabled ? GetLevelColor(level) : ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);

		std::string label = std::string(seri::Logger::ToString3(level)) + " " + std::to_string(_levelCounts[index]) + "##ConsoleLevel" + std::to_string(index);

		if (ImGui::Button(label.c_str()))
		{
			_levelEnabled[index] = !enabled;
			_filterDirty = true;
		}

		ImGui::PopStyleColor();
	}

	void ConsolePanel::ShowEntries()
	{
		if (_filterDirty)
		{
			RebuildFilter();
		}

		bool hasDetail = _showDetail && _selectedEntry >= 0 && _selectedEntry < static_cast<int>(_entries.size());

		float detailHeight = hasDetail ? seri::Util::Min(ImGui::GetTextLineHeightWithSpacing() * 4.0f, ImGui::GetContentRegionAvail().y * 0.4f) : 0.0f;

		ImGui::BeginChild("ConsoleEntries", ImVec2(0.0f, -detailHeight), ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar);

		ImGuiListClipper clipper;
		clipper.Begin(static_cast<int>(_visibleEntries.size()));

		while (clipper.Step())
		{
			for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
			{
				int index = _visibleEntries[row];

				const seri::LogEntry& entry = _entries[index];

				std::string line = BuildLine(entry, false);

				ImGui::PushID(index);
				ImGui::PushStyleColor(ImGuiCol_Text, GetLevelColor(entry.level));

				if (ImGui::Selectable(line.c_str(), _selectedEntry == index))
				{
					_selectedEntry = index;
				}

				ImGui::PopStyleColor();

				if (ImGui::BeginPopupContextItem("ConsoleEntryMenu"))
				{
					_selectedEntry = index;

					if (ImGui::MenuItem("Copy"))
					{
						ImGui::SetClipboardText(BuildLine(entry, true).c_str());
					}

					ImGui::Separator();

					ShowMenuItems();

					ImGui::EndPopup();
				}

				ImGui::PopID();
			}
		}

		clipper.End();

		if (ImGui::BeginPopupContextWindow("ConsoleMenu", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
		{
			ShowMenuItems();

			ImGui::EndPopup();
		}

		if (_scrollToBottom)
		{
			ImGui::SetScrollHereY(1.0f);
			_scrollToBottom = false;
		}

		ImGui::EndChild();
	}

	void ConsolePanel::ShowDetail()
	{
		if (!_showDetail || _selectedEntry < 0 || _selectedEntry >= static_cast<int>(_entries.size()))
		{
			return;
		}

		const seri::LogEntry& entry = _entries[_selectedEntry];

		ImGui::BeginChild("ConsoleDetail", ImVec2(0.0f, 0.0f), ImGuiChildFlags_Borders);

		ImGuiStyle& style = ImGui::GetStyle();

		float closeWidth = ImGui::CalcTextSize("x").x + style.FramePadding.x * 2.0f;

		ImGui::PushTextWrapPos(ImGui::GetContentRegionMax().x - closeWidth - style.ItemSpacing.x);
		ImGui::TextUnformatted(BuildLine(entry, true).c_str());
		ImGui::PopTextWrapPos();

		ImGui::SetCursorScreenPos(
			ImVec2(
				ImGui::GetWindowPos().x + ImGui::GetContentRegionMax().x - closeWidth,
				ImGui::GetWindowPos().y + style.WindowPadding.y
			)
		);

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, style.Colors[ImGuiCol_TextDisabled]);

		if (ImGui::SmallButton("x##ConsoleDetailClose"))
		{
			_selectedEntry = -1;
		}

		ImGui::PopStyleColor(2);

		ImGui::EndChild();
	}

	void ConsolePanel::ShowMenuItems()
	{
		if (ImGui::MenuItem("Copy all"))
		{
			CopyEntries();
		}

		if (ImGui::MenuItem("Clear"))
		{
			_clearPending = true;
		}
	}

	void ConsolePanel::CopyEntries()
	{
		std::string text;

		for (int index : _visibleEntries)
		{
			text += BuildLine(_entries[index], false);
			text += "\n";
		}

		ImGui::SetClipboardText(text.c_str());
	}

	void ConsolePanel::Clear()
	{
		seri::LogBuffer::Clear();

		_entries.clear();
		_visibleEntries.clear();
		_selectedEntry = -1;
		_filterDirty = true;

		for (int& count : _levelCounts)
		{
			count = 0;
		}
	}

	void ConsolePanel::RebuildFilter()
	{
		_visibleEntries.clear();

		for (size_t i = 0; i < _entries.size(); i++)
		{
			if (MatchFilter(_entries[i]))
			{
				_visibleEntries.push_back(static_cast<int>(i));
			}
		}

		_filterDirty = false;
	}

	bool ConsolePanel::MatchFilter(const seri::LogEntry& entry)
	{
		if (!_levelEnabled[static_cast<int>(entry.level)])
		{
			return false;
		}

		if (_search.empty())
		{
			return true;
		}

		return seri::Util::ContainsIgnoreCase(entry.message, _search) ||
			seri::Util::ContainsIgnoreCase(entry.module, _search) ||
			seri::Util::ContainsIgnoreCase(entry.file, _search);
	}

	std::string ConsolePanel::BuildLine(const seri::LogEntry& entry, bool detailed)
	{
		std::string line;

		if (detailed || _showTimeStamp)
		{
			line += entry.timeStamp;
			line += " ";
		}

		if ((detailed || _showThreadId) && !entry.threadId.empty())
		{
			line += "[" + entry.threadId + "] ";
		}

		line += "[";
		line += seri::Logger::ToString3(entry.level);
		line += "] ";

		if ((detailed || _showModule) && !entry.module.empty())
		{
			line += "[" + entry.module + "] ";
		}

		if ((detailed || _showSource) && !entry.file.empty())
		{
			line += "(" + entry.file + ":" + std::to_string(entry.line);

			if (detailed && !entry.function.empty())
			{
				line += " " + entry.function;
			}

			line += ") ";
		}

		line += entry.message;

		return line;
	}

	ImVec4 ConsolePanel::GetLevelColor(seri::LogLevel level)
	{
		switch (level)
		{
			case seri::LogLevel::error:
				return RGBNormalized(235, 94, 94);
			case seri::LogLevel::warning:
				return RGBNormalized(230, 190, 90);
			case seri::LogLevel::info:
				return RGBNormalized(205, 210, 220);
			case seri::LogLevel::verbose:
				return RGBNormalized(140, 146, 158);
			default:
				return RGBNormalized(205, 210, 220);
		}
	}
}
