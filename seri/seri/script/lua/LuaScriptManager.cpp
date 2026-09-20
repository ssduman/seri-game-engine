#include "Seripch.h"

#include "seri/script/lua/LuaScriptManager.h"
#include "seri/script/lua/LuaScript.h"
#include "seri/script/lua/LuaBindings.h"
#include "seri/system/ScriptSystem.h"
#include "seri/script/ScriptRegistry.h"
#include "seri/asset/AssetManager.h"
#include "seri/project/ProjectManager.h"
#include "seri/core/TimeWrapper.h"

#include <sol/sol.hpp>

#include <unordered_set>

namespace seri::script
{
	LuaScriptManager::LuaScriptManager() = default;

	LuaScriptManager::~LuaScriptManager() = default;

	void LuaScriptManager::Init()
	{
		LuaScriptManager& instance = GetInstance();

		instance._luaState = std::make_unique<sol::state>();
		instance.InitLuaLibraries();

		LuaBindings::Register(*instance._luaState);

		instance._assetTreeVersion = asset::AssetManager::GetAssetTreeVersion();
		instance.SyncScripts();
	}

	void LuaScriptManager::Update()
	{
		LuaScriptManager& instance = GetInstance();

		instance.UpdateTime();

		uint64_t version = asset::AssetManager::GetAssetTreeVersion();
		if (instance._assetTreeVersion != version)
		{
			instance._assetTreeVersion = version;
			instance.SyncScripts();
		}
	}

	sol::state& LuaScriptManager::GetState()
	{
		return *GetInstance()._luaState;
	}

	sol::table LuaScriptManager::GetClass(const std::string& name)
	{
		sol::table classes = GetState().registry()[kClassesKey];

		sol::object object = classes[name];
		if (object.get_type() != sol::type::table)
		{
			return sol::table{};
		}

		return object.as<sol::table>();
	}

	void LuaScriptManager::InitLuaLibraries()
	{
		sol::state& lua = *_luaState;

		lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::table, sol::lib::string, sol::lib::utf8);

		lua["package"]["path"] = fmt::format("{}/?.lua", project::ProjectManager::GetProjectAssetDirectory().generic_string());

		lua.registry()[kClassesKey] = lua.create_table();
	}

	void LuaScriptManager::UpdateTime()
	{
		sol::table time = (*_luaState)["Time"];
		time["delta_time"] = TimeWrapper::GetDeltaTime();
		time["fixed_delta_time"] = TimeWrapper::GetFixedDeltaTime();
		time["time"] = TimeWrapper::GetTime();
		time["frame_count"] = TimeWrapper::GetFrameCount();
	}

	void LuaScriptManager::SyncScripts()
	{
		std::unordered_set<std::string> found{};

		for (const asset::AssetMetadata& metadata : asset::AssetManager::GetAssetsByType(asset::AssetType::script))
		{
			std::string name = metadata.source.stem().string();

			if (found.contains(name))
			{
				LIB_LOGGER(warning, lua) << "'" << name << "' is defined more than once, ignoring " << metadata.source.string();
				continue;
			}

			std::error_code ec;
			std::filesystem::file_time_type time = std::filesystem::last_write_time(metadata.source, ec);

			auto it = _classes.find(name);
			bool known = it != _classes.end();

			if (known && it->second.path == metadata.source && it->second.time == time)
			{
				found.insert(name);
				continue;
			}

			if (!known && !ScriptRegistry::Register(name, ScriptKind::lua, [name]() { return std::make_unique<LuaScript>(name); }))
			{
				continue;
			}

			found.insert(name);
			_classes[name] = LuaClassInfo{ metadata.source, time };

			if (LoadClass(name, metadata.source) && known)
			{
				system::ScriptSystem::Rebuild(name);

				LIB_LOGGER(info, lua) << "'" << name << "' reloaded";
			}
		}

		for (auto it = _classes.begin(); it != _classes.end();)
		{
			if (found.contains(it->first))
			{
				++it;
				continue;
			}

			sol::table classes = _luaState->registry()[kClassesKey];
			classes[it->first] = sol::lua_nil;

			ScriptRegistry::Unregister(it->first);
			system::ScriptSystem::Rebuild(it->first);

			it = _classes.erase(it);
		}
	}

	bool LuaScriptManager::LoadClass(const std::string& name, const std::filesystem::path& path)
	{
		sol::state& lua = *_luaState;

		std::ifstream file(path, std::ios::binary);
		if (!file.is_open())
		{
			LIB_LOGGER(error, lua) << "could not open '" << path.string() << "'";
			return false;
		}

		std::stringstream buffer;
		buffer << file.rdbuf();

		std::error_code ec;
		std::filesystem::path relative = std::filesystem::relative(path, project::ProjectManager::GetProjectDirectory(), ec);
		std::string chunkName = "@" + (ec ? path : relative).generic_string();

		sol::load_result chunk = lua.load(buffer.str(), chunkName);
		if (!chunk.valid())
		{
			sol::error error = chunk;
			LIB_LOGGER(error, lua) << "could not load '" << name << "': " << error.what();
			return false;
		}

		sol::protected_function function = chunk;
		sol::protected_function_result result = function();
		if (!result.valid())
		{
			sol::error error = result;
			LIB_LOGGER(error, lua) << "could not run '" << name << "': " << error.what();
			return false;
		}

		sol::object classObject = result.return_count() > 0 ? result.get<sol::object>() : lua[name].get<sol::object>();
		if (classObject.get_type() != sol::type::table)
		{
			LIB_LOGGER(error, lua) << "'" << chunkName.substr(1) << "' must return a table or define a global table named '" << name << "'";
			return false;
		}

		sol::table metatable = lua.create_table();
		metatable["__index"] = classObject;

		sol::table classes = lua.registry()[kClassesKey];
		classes[name] = metatable;

		return true;
	}
}
