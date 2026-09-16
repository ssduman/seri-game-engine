#include "Editorpch.h"

#include "gui/launcher/LauncherGUI.h"
#include "gui/common/GUIStyle.h"
#include "gui/common/GUIBackend.h"
#include "gui/common/GUIWidgets.h"

#include <seri/project/ProjectManager.h>

#include <nfd.h>

#include <algorithm>

namespace seri::editor
{
	LauncherGUI::LauncherGUI()
	{
		LIB_LOGGER(info, launcher) << "created";
	}

	LauncherGUI::~LauncherGUI()
	{
		if (_nfdInitialized)
		{
			NFD_Quit();
		}

		GUIBackend::Shutdown();

		LIB_LOGGER(info, launcher) << "destroyed";
	}

	void LauncherGUI::Init()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGui::GetIO().IniFilename = nullptr;

		SetFonts(FindFontPath());
		SetStyle();

		GUIBackend::Init();

		if (NFD_Init() == NFD_OKAY)
		{
			_nfdInitialized = true;
		}
		else
		{
			_error = NFD_GetError();
			LIB_LOGGER(error, launcher) << "file dialog init failed: " << _error;
		}
	}

	void LauncherGUI::Update()
	{
		GUIBackend::NewFrame();

		Draw();
	}

	void LauncherGUI::Render()
	{
		GUIBackend::Render();
	}

	const std::filesystem::path& LauncherGUI::GetProjectPath() const
	{
		return _projectPath;
	}

	void LauncherGUI::Draw()
	{
		ImGuiWindowFlags flags =
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoBringToFrontOnFocus
			;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);

		ImGui::Begin("Launcher", nullptr, flags);

		float buttonWidth = 180.0f;
		float buttonHeight = 40.0f;
		float buttonSpacing = ImGui::GetStyle().ItemSpacing.x;

		ImGui::SetCursorPosY(viewport->WorkSize.y * 0.5f - 110.0f);

		ImGui::PushFont(nullptr, 48.0f);
		DrawCenteredText(seri::literals::kEngineName);
		ImGui::PopFont();

		ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
		DrawCenteredText(fmt::format("version {}", seri::literals::kVersion).c_str());
		ImGui::PopStyleColor();

		ImGui::Dummy(ImVec2(0.0f, 32.0f));

		ImGui::SetCursorPosX((ImGui::GetWindowWidth() - buttonWidth * 2.0f - buttonSpacing) * 0.5f);
		if (ImGui::Button("Create Project", ImVec2(buttonWidth, buttonHeight)))
		{
			_newProjectName = "NewProject";
			_newProjectLocation = ToUTF8(std::filesystem::current_path());
			_createError.clear();

			ImGui::OpenPopup(kCreateProjectPopupName);
		}

		ImGui::SameLine(0.0f, buttonSpacing);
		if (ImGui::Button("Open Project", ImVec2(buttonWidth, buttonHeight)))
		{
			OpenProject();
		}

		DrawCreateProjectPopup();

		if (!_error.empty())
		{
			ImGui::Dummy(ImVec2(0.0f, 8.0f));

			ImGui::PushStyleColor(ImGuiCol_Text, RGBNormalized(230, 90, 90));
			DrawCenteredText(_error.c_str());
			ImGui::PopStyleColor();
		}

		ImGui::End();
	}

	void LauncherGUI::DrawCreateProjectPopup()
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(560.0f, 0.0f));

		if (!ImGui::BeginPopupModal(kCreateProjectPopupName, nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
		{
			return;
		}

		float browseWidth = 80.0f;

		DrawTextInput("Name", _newProjectName);

		ImGui::PushID("Location");
		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnWidth(0, 90.0f);
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Location");
		ImGui::NextColumn();
		ImGui::SetNextItemWidth(-(browseWidth + ImGui::GetStyle().ItemSpacing.x));
		ImGui::InputText("##value", &_newProjectLocation);
		ImGui::SameLine();
		if (ImGui::Button("Browse", ImVec2(browseWidth, 0.0f)))
		{
			BrowseProjectLocation();
		}
		ImGui::Columns(1);
		ImGui::PopID();

		ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
		ImGui::TextWrapped("%s", ToUTF8(ToPath(_newProjectLocation) / _newProjectName).c_str());
		ImGui::PopStyleColor();

		if (!_createError.empty())
		{
			ImGui::PushStyleColor(ImGuiCol_Text, RGBNormalized(230, 90, 90));
			ImGui::TextWrapped("%s", _createError.c_str());
			ImGui::PopStyleColor();
		}

		ImGui::Spacing();

		float buttonWidth = 100.0f;
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - buttonWidth * 2.0f - ImGui::GetStyle().ItemSpacing.x - ImGui::GetStyle().WindowPadding.x);

		if (ImGui::Button("Create", ImVec2(buttonWidth, 0.0f)))
		{
			CreateProject();
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0.0f)))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	void LauncherGUI::CreateProject()
	{
		std::filesystem::path location = ToPath(_newProjectLocation);

		bool validName = !_newProjectName.empty() && std::all_of(_newProjectName.begin(), _newProjectName.end(),
			[](char c)
			{
				return std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '-' || c == ' ';
			}
		);

		std::error_code ec;
		if (!validName)
		{
			_createError = "project name can only contain letters, digits, spaces, '-' and '_'";
		}
		else if (!std::filesystem::is_directory(location, ec))
		{
			_createError = "location is not an existing folder";
		}
		else if (std::filesystem::exists(location / _newProjectName, ec))
		{
			_createError = "a folder with this name already exists in the location";
		}
		else
		{
			std::filesystem::path projectFile = seri::project::ProjectManager::CreateProject(location, _newProjectName);
			if (projectFile.empty())
			{
				_createError = "could not create project";
				return;
			}

			_createError.clear();
			_error.clear();
			_projectPath = projectFile;

			ImGui::CloseCurrentPopup();
		}
	}

	void LauncherGUI::BrowseProjectLocation()
	{
		if (!_nfdInitialized)
		{
			return;
		}

		nfdu8char_t* outPath = nullptr;

		nfdresult_t result = NFD_PickFolderU8(&outPath, _newProjectLocation.c_str());
		if (result == NFD_CANCEL)
		{
			return;
		}

		if (result != NFD_OKAY)
		{
			_createError = NFD_GetError();
			LIB_LOGGER(error, launcher) << "folder dialog failed: " << _createError;
			return;
		}

		_newProjectLocation = outPath;
		NFD_FreePathU8(outPath);
	}

	void LauncherGUI::OpenProject()
	{
		if (!_nfdInitialized)
		{
			return;
		}

		nfdu8char_t* outPath = nullptr;
		nfdu8filteritem_t filters[] = { { "Seri Project", seri::project::ProjectManager::kProjectExtension } };

		std::string defaultPath = ToUTF8(std::filesystem::current_path());

		nfdresult_t result = NFD_OpenDialogU8(&outPath, filters, 1, defaultPath.c_str());
		if (result == NFD_CANCEL)
		{
			return;
		}

		if (result != NFD_OKAY)
		{
			_error = NFD_GetError();
			LIB_LOGGER(error, launcher) << "file dialog failed: " << _error;
			return;
		}

		std::filesystem::path path = ToPath(outPath);
		NFD_FreePathU8(outPath);

		if (path.extension() != fmt::format(".{}", seri::project::ProjectManager::kProjectExtension) || !std::filesystem::is_regular_file(path))
		{
			_error = "selected file is not a .sproject file";
			LIB_LOGGER(warning, launcher) << "invalid project file: " << path.string();
			return;
		}

		_error.clear();
		_projectPath = path;

		LIB_LOGGER(info, launcher) << "project selected: " << _projectPath.string();
	}

	void LauncherGUI::DrawCenteredText(const char* text)
	{
		ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(text).x) * 0.5f);
		ImGui::TextUnformatted(text);
	}

	std::filesystem::path LauncherGUI::FindFontPath()
	{
		std::error_code ec;
		for (const auto& entry : std::filesystem::recursive_directory_iterator(seri::asset::AssetManager::GetAssetDirectory(), ec))
		{
			if (entry.path().filename() == seri::literals::kDefaultFontName)
			{
				return entry.path();
			}
		}

		return {};
	}

	std::filesystem::path LauncherGUI::ToPath(const std::string& utf8)
	{
		return std::filesystem::path{ std::u8string{ utf8.begin(), utf8.end() } };
	}

	std::string LauncherGUI::ToUTF8(const std::filesystem::path& path)
	{
		std::u8string text = path.u8string();
		return std::string{ text.begin(), text.end() };
	}
}
