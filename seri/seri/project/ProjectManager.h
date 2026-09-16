#pragma once

#include "seri/core/Singleton.h"

#include <string>
#include <filesystem>

namespace seri::project
{
	class ProjectManager : public seri::Singleton<ProjectManager>
	{
	public:
		static std::filesystem::path CreateProject(const std::filesystem::path& location, const std::string& name);

		static constexpr const char* kProjectExtension = "sproject";

	protected:
		friend struct seri::Singleton<ProjectManager>;

		ProjectManager() = default;
		~ProjectManager() = default;

	};
}
