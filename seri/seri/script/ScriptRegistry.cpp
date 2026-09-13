#include "Seripch.h"

#include "seri/script/ScriptRegistry.h"

namespace seri::script
{
	bool ScriptRegistry::Register(std::string_view name, ScriptKind kind, ScriptFactory factory)
	{
		std::string key{ name };

		if (GetInstance()._scriptEntries.contains(key))
		{
			LIB_LOGGER(warning, script_registry) << "'" << key << "' is already registered";
			return false;
		}

		GetInstance()._scriptEntries.emplace(key, ScriptEntry{ kind, std::move(factory) });
		GetInstance()._scriptNames.emplace_back(key);

		LIB_LOGGER(info, script_registry) << "'" << key << "' registered";

		return true;
	}

	void ScriptRegistry::Unregister(const std::string& name)
	{
		if (GetInstance()._scriptEntries.erase(name) == 0)
		{
			return;
		}

		std::erase(GetInstance()._scriptNames, name);

		LIB_LOGGER(info, script_registry) << "'" << name << "' unregistered";
	}

	std::unique_ptr<ScriptBase> ScriptRegistry::Create(const std::string& name)
	{
		auto it = GetInstance()._scriptEntries.find(name);
		if (it == GetInstance()._scriptEntries.end())
		{
			return nullptr;
		}

		return it->second.factory();
	}

	bool ScriptRegistry::Contains(const std::string& name)
	{
		return GetInstance()._scriptEntries.contains(name);
	}

	ScriptKind ScriptRegistry::GetKind(const std::string& name)
	{
		auto it = GetInstance()._scriptEntries.find(name);
		if (it == GetInstance()._scriptEntries.end())
		{
			return ScriptKind::system;
		}

		return it->second.kind;
	}

	const std::vector<std::string>& ScriptRegistry::GetNames()
	{
		return GetInstance()._scriptNames;
	}
}
