#include "Seripch.h"

#include "seri/scripting/ScriptingManager.h"

namespace seri::scripting
{
	void ScriptingManager::InitLuaLibraries()
	{
		_luaState.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::table, sol::lib::string, sol::lib::os);

		_luaState["os"] = sol::nil;
		_luaState["io"] = sol::nil;
	}

	void ScriptingManager::InitLuaUserTypes()
	{
		_luaState.new_usertype<glm::vec3>(
			"Vec3",
			sol::constructors<glm::vec3(), glm::vec3(float, float, float)>(),
			"x", &glm::vec3::x,
			"y", &glm::vec3::y,
			"z", &glm::vec3::z
		);
	}

	void ScriptingManager::InitLuaFunctions()
	{
		_luaState.set_function("Logger", [&](const std::string& msg)
			{
				LOGGER(info, fmt::format("[lua] {}", msg));
			}
		);

		_luaState.set_function("Vec3", [](float x, float y, float z)
			{
				return glm::vec3{ x, y, z };
			}
		);

		_luaState.script(
			R"(
			vec = Vec3(0, 1, 2)
			Logger("lua and sol2 are working, vec.x: " .. vec.x .. ", vec.y: " .. vec.y .. ", vec.z: " .. vec.z)
			)"
		);
	}

}
