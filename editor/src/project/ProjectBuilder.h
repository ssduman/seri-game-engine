#pragma once

#include <string>
#include <filesystem>

namespace seri::editor
{
	class ProjectBuilder
	{
	public:
		static bool PickLocation(std::filesystem::path& location);

		static bool Build(const std::filesystem::path& location, std::string& error);

	private:
		static bool CheckOutputDirectory(const std::filesystem::path& outputDirectory, std::string& error);

		static bool ShouldCopyToBuild(const std::filesystem::path& path, const std::filesystem::path& executablePath);

	};
}
