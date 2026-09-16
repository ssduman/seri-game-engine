#pragma once

#include "gui/common/GUICommon.h"

#include <filesystem>

namespace seri::editor
{
	class LauncherGUI : public seri::Object
	{
	public:
		LauncherGUI();

		~LauncherGUI() override;

		void Init() override;

		void Update() override;

		void Render() override;

		const std::filesystem::path& GetProjectPath() const;

	private:
		void Draw();

		void DrawCreateProjectPopup();

		void OpenProject();

		void CreateProject();

		void BrowseProjectLocation();

		void DrawCenteredText(const char* text);

		std::filesystem::path FindFontPath();

		static std::filesystem::path ToPath(const std::string& utf8);

		static std::string ToUTF8(const std::filesystem::path& path);

		static constexpr const char* kCreateProjectPopupName = "Create Project";

		std::filesystem::path _projectPath{};
		std::string _error{};
		bool _nfdInitialized{ false };

		std::string _newProjectName{};
		std::string _newProjectLocation{};
		std::string _createError{};

	};
}
