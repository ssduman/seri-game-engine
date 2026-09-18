#pragma once

#include "seri/core/Singleton.h"
#include "seri/component/Components.h"

#include <string>
#include <filesystem>

namespace seri::project
{
	class ProjectManager : public seri::Singleton<ProjectManager>
	{
	public:
		static std::filesystem::path CreateProject(const std::filesystem::path& location, const std::string& name);

		static bool OpenProject(const std::filesystem::path& projectFile, std::string& error);

		static uint64_t GetID()
		{
			return GetInstance()._idComponent.id;
		}

		static const std::string& GetName()
		{
			return GetInstance()._idComponent.name;
		}

		static const std::filesystem::path& GetProjectDirectory()
		{
			return GetInstance()._projectDirectory;
		}

		static std::filesystem::path GetAssetDirectory()
		{
			return GetProjectDirectory() / kAssetFolder;
		}

		static std::filesystem::path GetEngineAssetDirectory()
		{
			return GetAssetDirectory() / kEngineAssetFolder;
		}

		static std::filesystem::path GetProjectAssetDirectory()
		{
			return GetAssetDirectory() / kProjectAssetFolder;
		}

		static std::filesystem::path GetEngineSourceDirectory()
		{
			std::filesystem::path assets = std::filesystem::current_path() / kAssetFolder;
			std::filesystem::path engineAssets = assets / kEngineAssetFolder;

			std::error_code ec;
			return std::filesystem::is_directory(engineAssets, ec) ? engineAssets : assets;
		}

		static const std::filesystem::path& GetStartupScene()
		{
			return GetInstance()._startupScene;
		}

		static constexpr const char* kProjectExtension = "sproject";
		static constexpr const char* kSceneExtension = "sscene";
		static constexpr const char* kAssetFolder = "assets";
		static constexpr const char* kEngineAssetFolder = "engine";
		static constexpr const char* kProjectAssetFolder = "project";
		static constexpr const char* kDefaultSceneName = "main";

	protected:
		friend struct seri::Singleton<ProjectManager>;

		ProjectManager() = default;
		~ProjectManager() = default;

	private:
		bool CopyEngineAssets(const std::filesystem::path& projectDirectory, std::string& error);

		std::filesystem::path FindOrCreateStartupScene(const std::filesystem::path& projectDirectory);

		static void WriteYAML(const std::filesystem::path& path, const YAML::Node& root);

		seri::component::IDComponent _idComponent{};
		std::filesystem::path _projectFile{};
		std::filesystem::path _projectDirectory{};
		std::filesystem::path _startupScene{};

	};
}
