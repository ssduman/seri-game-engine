#pragma once

#include <sol/forward.hpp>

namespace seri::script
{
	class LuaBindings
	{
	public:
		static void Register(sol::state& lua);

	private:
		template<typename T, typename Constructors>
		static sol::usertype<T> RegisterVector(sol::state& lua, const char* name);

		static void RegisterMath(sol::state& lua);
		static void RegisterCore(sol::state& lua);
		static void RegisterInput(sol::state& lua);
		static void RegisterComponents(sol::state& lua);
		static void RegisterEntity(sol::state& lua);
		static void RegisterScene(sol::state& lua);

	};
}
