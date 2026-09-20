#include "Seripch.h"

#include "seri/script/lua/LuaScript.h"
#include "seri/script/lua/LuaScriptManager.h"
#include "seri/logging/Logger.h"

#include <algorithm>

namespace seri::script
{
	LuaScript::LuaScript(const std::string& name) : _name(name)
	{
		sol::state& lua = LuaScriptManager::GetState();

		_self = lua.create_table();

		sol::table metatable = LuaScriptManager::GetClass(_name);
		if (!metatable.valid())
		{
			return;
		}

		_self[sol::metatable_key] = metatable;

		CreateFields(metatable["__index"]);

		_loaded = true;
	}

	void LuaScript::OnCreate()
	{
		if (!_loaded)
		{
			throw std::runtime_error("could not load lua script: " + _name);
		}

		_self["entity"] = entity;

		Call("OnCreate");
	}

	void LuaScript::OnStart()
	{
		Call("OnStart");
	}

	void LuaScript::OnFixedUpdate(float fixedDeltaTime)
	{
		Call("OnFixedUpdate", fixedDeltaTime);
	}

	void LuaScript::OnUpdate(float deltaTime)
	{
		Call("OnUpdate", deltaTime);
	}

	void LuaScript::OnLateUpdate(float deltaTime)
	{
		Call("OnLateUpdate", deltaTime);
	}

	void LuaScript::OnEnable()
	{
		Call("OnEnable");
	}

	void LuaScript::OnDisable()
	{
		Call("OnDisable");
	}

	void LuaScript::OnDestroy()
	{
		Call("OnDestroy");
	}

	void LuaScript::OnClick()
	{
		Call("OnClick");
	}

	void LuaScript::OnCollisionEnter(const seri::Entity& other)
	{
		Call("OnCollisionEnter", seri::Entity{ other });
	}

	void LuaScript::OnCollisionExit(const seri::Entity& other)
	{
		Call("OnCollisionExit", seri::Entity{ other });
	}

	void LuaScript::OnTriggerEnter(const seri::Entity& other)
	{
		Call("OnTriggerEnter", seri::Entity{ other });
	}

	void LuaScript::OnTriggerExit(const seri::Entity& other)
	{
		Call("OnTriggerExit", seri::Entity{ other });
	}

	std::vector<ScriptField> LuaScript::GetSerializedFields()
	{
		SyncFields();

		std::vector<ScriptField> fields{};
		fields.reserve(_fields.size());

		for (LuaField& field : _fields)
		{
			void* ptr = std::visit([](auto& value) -> void* { return &value; }, field.value);
			fields.push_back({ field.name, field.type, ptr });
		}

		return fields;
	}

	void LuaScript::ApplySerializedFields()
	{
		SyncFields();
	}

	void LuaScript::CreateFields(const sol::table& classTable)
	{
		sol::object fieldsObject = classTable["fields"];
		if (fieldsObject.get_type() != sol::type::table)
		{
			return;
		}

		for (const auto& [key, value] : fieldsObject.as<sol::table>())
		{
			if (key.get_type() != sol::type::string)
			{
				continue;
			}

			LuaField field{};
			field.name = key.as<std::string>();

			switch (value.get_type())
			{
				case sol::type::boolean:
					field.type = ScriptField::Type::boolean;
					field.value = value.as<bool>();
					break;
				case sol::type::number:
					{
						lua_State* state = value.lua_state();
						value.push(state);
						bool isInteger = lua_isinteger(state, -1) != 0;
						lua_pop(state, 1);

						if (isInteger)
						{
							field.type = ScriptField::Type::integer;
							field.value = value.as<int>();
						}
						else
						{
							field.type = ScriptField::Type::floating;
							field.value = value.as<float>();
						}
					}
					break;
				case sol::type::string:
					field.type = ScriptField::Type::text;
					field.value = value.as<std::string>();
					break;
				case sol::type::userdata:
					if (!value.is<glm::vec3>())
					{
						LIB_LOGGER(warning, lua_script) << "'" << _name << "' field '" << field.name << "' has unsupported type";
						continue;
					}
					field.type = ScriptField::Type::vec3;
					field.value = value.as<glm::vec3>();
					break;
				default:
					LIB_LOGGER(warning, lua_script) << "'" << _name << "' field '" << field.name << "' has unsupported type";
					continue;
			}

			field.synced = field.value;

			_self[field.name] = field.value;

			_fields.push_back(std::move(field));
		}

		std::sort(_fields.begin(), _fields.end(), [](const LuaField& a, const LuaField& b) { return a.name < b.name; });
	}

	void LuaScript::SyncFields()
	{
		for (LuaField& field : _fields)
		{
			if (field.value != field.synced)
			{
				_self[field.name] = field.value;
			}
			else
			{
				sol::object object = _self[field.name];

				std::visit([&](auto& value)
					{
						using T = std::decay_t<decltype(value)>;
						if (object.is<T>())
						{
							value = object.as<T>();
						}
					}, field.value);
			}

			field.synced = field.value;
		}
	}
}
