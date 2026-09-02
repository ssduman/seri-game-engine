#pragma once

#include "seri/core/Singleton.h"

#include "seri/util/Util.h"

#include <sol/sol.hpp>

namespace seri::scripting
{
	class ScriptingManager : public seri::Singleton<ScriptingManager>
	{
	public:
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

	protected:
		friend struct seri::Singleton<ScriptingManager>;

		ScriptingManager() = default;
		~ScriptingManager() = default;

	private:
		sol::state _luaState;

	};
}
