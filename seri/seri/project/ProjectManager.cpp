#include "Seripch.h"

#include "seri/project/ProjectManager.h"
#include "seri/random/Random.h"

#include <yaml-cpp/yaml.h>

#include <fstream>
#include <algorithm>

namespace seri::project
{
	std::filesystem::path ProjectManager::CreateProject(const std::filesystem::path& location, const std::string& name)
	{
		std::filesystem::path projectDirectory = location / name;
		std::filesystem::path projectFile = projectDirectory / fmt::format("{}.{}", name, kProjectExtension);

		std::error_code ec;
		std::filesystem::create_directories(projectDirectory, ec);
		if (ec)
		{
			LIB_LOGGER(error, project) << "could not create project directory " << projectDirectory.string() << ": " << ec.message();
			return {};
		}

		seri::component::IDComponent idComponent{
			.id = seri::Random::UUID(),
			.parentId = 0,
			.name = name,
		};

		YAML::Node root;
		root[std::string{ seri::component::IDComponent::kCompName }] = seri::component::IDComponent::Serialize(idComponent);

		WriteYAML(projectFile, root);

		if (!std::filesystem::exists(projectFile))
		{
			return {};
		}

		LIB_LOGGER(info, project) << "project created: " << projectFile.string();

		return projectFile;
	}

	bool ProjectManager::OpenProject(const std::filesystem::path& projectFile, std::string& error)
	{
		ProjectManager& instance = GetInstance();

		seri::component::IDComponent idComponent{};
		try
		{
			YAML::Node root = YAML::LoadFile(projectFile.string());
			YAML::Node idNode = root[std::string{ seri::component::IDComponent::kCompName }];
			if (!idNode || !idNode.IsMap())
			{
				error = "project file has no IDComponent";
				return false;
			}

			idComponent = seri::component::IDComponent::Deserialize(idNode);
		}
		catch (const std::exception& ex)
		{
			error = fmt::format("could not read project file: {}", ex.what());
			return false;
		}

		if (idComponent.id == 0)
		{
			error = "project file has no valid ID";
			return false;
		}

		std::filesystem::path projectDirectory = std::filesystem::absolute(projectFile).parent_path();

		if (!instance.CopyEngineAssets(projectDirectory, error))
		{
			return false;
		}

		std::error_code ec;
		std::filesystem::create_directories(projectDirectory / kAssetFolder / kProjectAssetFolder, ec);
		if (ec)
		{
			error = fmt::format("could not create project asset folder: {}", ec.message());
			return false;
		}

		instance._idComponent = idComponent;
		instance._projectFile = projectFile;
		instance._projectDirectory = projectDirectory;
		instance._startupScene = instance.FindOrCreateStartupScene(projectDirectory);

		LIB_LOGGER(info, project) << "project opened: " << projectFile.string() << ", startup scene: " << instance._startupScene.string();

		return true;
	}

	bool ProjectManager::CopyEngineAssets(const std::filesystem::path& projectDirectory, std::string& error)
	{
		std::error_code ec;

		std::filesystem::path source = std::filesystem::weakly_canonical(GetEngineSourceDirectory(), ec);
		if (ec || !std::filesystem::is_directory(source))
		{
			error = fmt::format("engine assets not found at {}", GetEngineSourceDirectory().string());
			return false;
		}

		std::filesystem::path destination = std::filesystem::weakly_canonical(projectDirectory / kAssetFolder / kEngineAssetFolder, ec);
		if (ec)
		{
			error = fmt::format("invalid project directory: {}", ec.message());
			return false;
		}

		std::filesystem::path relative = destination.lexically_relative(source);
		if (!relative.empty() && *relative.begin() != "..")
		{
			error = "project cannot be inside the engine asset folder";
			return false;
		}

		std::filesystem::remove_all(destination, ec);
		if (ec)
		{
			error = fmt::format("could not remove old engine assets: {}", ec.message());
			return false;
		}

		std::filesystem::create_directories(destination, ec);
		std::filesystem::copy(source, destination, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing, ec);
		if (ec)
		{
			error = fmt::format("could not copy engine assets: {}", ec.message());
			return false;
		}

		return true;
	}

	std::filesystem::path ProjectManager::FindOrCreateStartupScene(const std::filesystem::path& projectDirectory)
	{
		std::filesystem::path projectAssets = projectDirectory / kAssetFolder / kProjectAssetFolder;
		std::string sceneExtension = fmt::format(".{}", kSceneExtension);

		std::vector<std::filesystem::path> scenes{};

		std::error_code ec;
		for (const auto& entry : std::filesystem::recursive_directory_iterator(projectAssets, ec))
		{
			if (entry.is_regular_file() && entry.path().extension() == sceneExtension)
			{
				scenes.push_back(entry.path());
			}
		}

		if (!scenes.empty())
		{
			std::sort(scenes.begin(), scenes.end());
			return scenes.front();
		}

		std::filesystem::path scenePath = projectAssets / "scenes" / fmt::format("{}{}", kDefaultSceneName, sceneExtension);
		std::filesystem::create_directories(scenePath.parent_path(), ec);

		seri::component::IDComponent idComponent{
			.id = seri::Random::UUID(),
			.parentId = 0,
			.name = kDefaultSceneName,
		};

		YAML::Node root;
		root[std::string{ seri::component::IDComponent::kCompName }] = seri::component::IDComponent::Serialize(idComponent);
		root[std::string{ seri::component::SceneComponent::kCompName }] = seri::component::SceneComponent::Serialize(seri::component::SceneComponent{ .version = "0.1" });
		root["Entities"] = YAML::Node(YAML::NodeType::Sequence);

		WriteYAML(scenePath, root);

		LIB_LOGGER(info, project) << "default scene created: " << scenePath.string();

		return scenePath;
	}

	void ProjectManager::WriteYAML(const std::filesystem::path& path, const YAML::Node& root)
	{
		std::ofstream fout(path);
		if (!fout.is_open())
		{
			LIB_LOGGER(error, project) << "could not write " << path.string();
			return;
		}

		fout << root;
	}
}
