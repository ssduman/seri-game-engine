#pragma once

#include "seri/script/ScriptBase.h"
#include "seri/logging/Logger.h"

#include <sol/sol.hpp>
#include <glm/glm.hpp>

#include <string>
#include <variant>
#include <vector>
#include <stdexcept>

namespace seri::script
{
	struct LuaField
	{
		using Value = std::variant<bool, int, float, glm::vec3, std::string>;

		std::string name{ "" };
		ScriptField::Type type{ ScriptField::Type::floating };
		Value value{};
		Value synced{};
	};

	class LuaScript : public ScriptBase
	{
	public:
		explicit LuaScript(const std::string& name);

		void OnCreate() override;
		void OnStart() override;
		void OnUpdate(float deltaTime) override;
		void OnLateUpdate(float deltaTime) override;
		void OnEnable() override;
		void OnDisable() override;
		void OnDestroy() override;

		std::vector<ScriptField> GetSerializedFields() override;

		const std::string& GetName() const
		{
			return _name;
		}

		const sol::table& GetSelf() const
		{
			return _self;
		}

	private:
		template<typename... Args>
		void Call(const char* functionName, Args&&... args)
		{
			sol::object object = _self[functionName];
			if (object.get_type() != sol::type::function)
			{
				return;
			}

			SyncFields();

			sol::protected_function function = object;
			sol::protected_function_result result = function(_self, std::forward<Args>(args)...);

			SyncFields();

			if (!result.valid())
			{
				sol::error error = result;
				// throw std::runtime_error(error.what());
				LIB_LOGGER(error, lua_scripting) << "exception occurred while calling function '" << functionName << "': " << error.what();
			}
		}

		void CreateFields(const sol::table& classTable);
		void SyncFields();

		std::string _name{ "" };
		bool _loaded{ false };
		sol::table _self{};
		std::vector<LuaField> _fields{};

	};
}
