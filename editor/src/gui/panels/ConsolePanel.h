#pragma once

#include "gui/EditorCommon.h"

namespace seri::editor
{
	class ConsolePanel
	{
	public:
		void Update();

		void Draw();

	private:
		void ShowToolbar();

		void ShowLevelFilter(seri::LogLevel level);

		void ShowEntries();

		void ShowDetail();

		void ShowMenuItems();

		void CopyEntries();

		void Clear();

		void RebuildFilter();

		bool MatchFilter(const seri::LogEntry& entry);

		std::string BuildLine(const seri::LogEntry& entry, bool detailed);

		ImVec4 GetLevelColor(seri::LogLevel level);

		std::vector<seri::LogEntry> _entries;
		std::vector<int> _visibleEntries;
		std::string _search;
		bool _levelEnabled[5]{ true, true, true, true, true };
		int _levelCounts[5]{};
		bool _autoScroll{ true };
		bool _showTimeStamp{ true };
		bool _showThreadId{ false };
		bool _showModule{ true };
		bool _showSource{ false };
		bool _filterDirty{ true };
		bool _clearPending{ false };
		bool _scrollToBottom{ false };
		bool _showDetail{ true };
		int _selectedEntry{ -1 };
		size_t _maxEntries{ 4096 };

	};
}
