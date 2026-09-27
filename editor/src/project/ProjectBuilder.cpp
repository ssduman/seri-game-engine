#include "Editorpch.h"

#include "project/ProjectBuilder.h"

#include <seri/platform/Platform.h>

#include <nfd.h>

namespace seri::editor
{
	bool ProjectBuilder::PickLocation(std::filesystem::path& location)
	{
		if (NFD_Init() != NFD_OKAY)
		{
			LIB_LOGGER(error, build) << "folder dialog init failed: " << NFD_GetError();
			return false;
		}

		nfdu8char_t* outPath = nullptr;

		nfdresult_t result = NFD_PickFolderU8(&outPath, seri::project::ProjectManager::GetProjectDirectory().string().c_str());
		if (result == NFD_OKAY)
		{
			location = outPath;
			NFD_FreePathU8(outPath);
		}
		else if (result == NFD_ERROR)
		{
			LIB_LOGGER(error, build) << "folder dialog failed: " << NFD_GetError();
		}

		NFD_Quit();

		return result == NFD_OKAY;
	}

	bool ProjectBuilder::Build(const std::filesystem::path& location, std::string& error)
	{
		std::filesystem::path projectPath = seri::project::ProjectManager::GetProjectFile();
		std::filesystem::path executablePath = seri::platform::GetExecutablePath();
		std::filesystem::path executableName = projectPath.filename().replace_extension(executablePath.extension());

		std::error_code ec;
		std::filesystem::path outputDirectory = std::filesystem::weakly_canonical(location / projectPath.stem(), ec);
		if (ec)
		{
			error = fmt::format("invalid output directory: {}", ec.message());
			return false;
		}

		if (!CheckOutputDirectory(outputDirectory, error))
		{
			return false;
		}

		LIB_LOGGER(info, build) << "build started: " << outputDirectory.string();

		try
		{
			std::filesystem::remove_all(outputDirectory / seri::project::ProjectManager::kAssetFolder);
			std::filesystem::create_directories(outputDirectory);

			std::filesystem::copy_file(executablePath, outputDirectory / executableName, std::filesystem::copy_options::overwrite_existing);

			for (const auto& entry : std::filesystem::directory_iterator(executablePath.parent_path()))
			{
				if (entry.is_regular_file() && entry.path().extension() == seri::platform::GetSharedLibraryExtension())
				{
					std::filesystem::copy_file(entry.path(), outputDirectory / entry.path().filename(), std::filesystem::copy_options::overwrite_existing);
				}
			}

			std::filesystem::copy(
				seri::project::ProjectManager::GetAssetDirectory(),
				outputDirectory / seri::project::ProjectManager::kAssetFolder,
				std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing
			);

			std::filesystem::copy_file(projectPath, outputDirectory / projectPath.filename(), std::filesystem::copy_options::overwrite_existing);
		}
		catch (const std::filesystem::filesystem_error& ex)
		{
			error = ex.what();
			return false;
		}

		LIB_LOGGER(info, build) << "build finished: " << (outputDirectory / executableName).string();

		return true;
	}

	bool ProjectBuilder::CheckOutputDirectory(const std::filesystem::path& outputDirectory, std::string& error)
	{
		std::error_code ec;

		std::filesystem::path projectDirectory = std::filesystem::weakly_canonical(seri::project::ProjectManager::GetProjectDirectory(), ec);
		if (outputDirectory == projectDirectory)
		{
			error = "output directory cannot be the project directory";
			return false;
		}

		std::filesystem::path relative = outputDirectory.lexically_relative(projectDirectory / seri::project::ProjectManager::kAssetFolder);
		if (!relative.empty() && *relative.begin() != "..")
		{
			error = "output directory cannot be inside the project asset directory";
			return false;
		}

		bool isPreviousBuild = std::filesystem::exists(outputDirectory / seri::project::ProjectManager::GetProjectFile().filename(), ec);
		if (std::filesystem::is_directory(outputDirectory, ec) && !std::filesystem::is_empty(outputDirectory, ec) && !isPreviousBuild)
		{
			error = fmt::format("output directory {} is not empty and is not a previous build", outputDirectory.string());
			return false;
		}

		return true;
	}
}
