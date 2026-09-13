#pragma once

#include "seri/core/Singleton.h"

#include <sol/forward.hpp>

#include <string>
#include <memory>
#include <filesystem>
#include <unordered_map>

namespace seri::script
{
	struct LuaClassInfo
	{
		std::filesystem::path path{};
		std::filesystem::file_time_type time{};
	};

	class LuaScriptManager : public seri::Singleton<LuaScriptManager>
	{
	public:
		static void Init();
		static void Update();

		static sol::state& GetState();
		static sol::table GetClass(const std::string& name);

	protected:
		friend struct seri::Singleton<LuaScriptManager>;

		LuaScriptManager();
		~LuaScriptManager();

	private:
		void InitLuaLibraries();
		void UpdateTime();
		void SyncScripts();
		bool LoadClass(const std::string& name, const std::filesystem::path& path);

		static constexpr const char* kClassesKey = "seri_classes";

		std::unique_ptr<sol::state> _luaState;
		std::unordered_map<std::string, LuaClassInfo> _classes{};
		uint64_t _assetTreeVersion{ 0 };

	};
}
