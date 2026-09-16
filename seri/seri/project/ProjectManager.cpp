#include "Seripch.h"

#include "seri/project/ProjectManager.h"
#include "seri/component/Components.h"
#include "seri/random/Random.h"

#include <yaml-cpp/yaml.h>

#include <fstream>

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

		std::ofstream fout(projectFile);
		if (!fout.is_open())
		{
			LIB_LOGGER(error, project) << "could not write project file " << projectFile.string();
			return {};
		}

		fout << root;

		LIB_LOGGER(info, project) << "project created: " << projectFile.string();

		return projectFile;
	}
}
