#pragma once

#include "seri/util/Util.h"

#include <sol/sol.hpp>

namespace seri::scripting
{
	class ScriptingManager
	{
	public:
		ScriptingManager(ScriptingManager const&) = delete;

		void operator=(ScriptingManager const&) = delete;

		ScriptingManager() = default;

		static ScriptingManager& GetInstance()
		{
			static ScriptingManager instance;
			return instance;
		}

		static void Init()
		{
			GetInstance();

			GetInstance().InitLuaLibraries();
			GetInstance().InitLuaUserTypes();
			GetInstance().InitLuaFunctions();
		}

		static void Update()
		{
		}

		void InitLuaLibraries();
		void InitLuaUserTypes();
		void InitLuaFunctions();

	private:
		sol::state _luaState;

	};
}
