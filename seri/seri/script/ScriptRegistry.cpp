#include "Seripch.h"

#include "seri/script/ScriptRegistry.h"

namespace seri::script
{
	bool ScriptRegistry::Register(std::string_view name, ScriptKind kind, Factory factory)
	{
		std::string key{ name };

		if (GetInstance()._entries.contains(key))
		{
			LIB_LOGGER(warning, script) << "'" << key << "' is already registered";
			return false;
		}

		GetInstance()._entries.emplace(key, Entry{ kind, std::move(factory) });
		GetInstance()._names.emplace_back(key);

		LIB_LOGGER(info, script) << "'" << key << "' registered";

		return true;
	}

	void ScriptRegistry::Unregister(const std::string& name)
	{
		if (GetInstance()._entries.erase(name) == 0)
		{
			return;
		}

		std::erase(GetInstance()._names, name);

		LIB_LOGGER(info, script) << "'" << name << "' unregistered";
	}

	std::unique_ptr<ScriptBase> ScriptRegistry::Create(const std::string& name)
	{
		auto it = GetInstance()._entries.find(name);
		if (it == GetInstance()._entries.end())
		{
			return nullptr;
		}

		return it->second.factory();
	}

	bool ScriptRegistry::Contains(const std::string& name)
	{
		return GetInstance()._entries.contains(name);
	}

	ScriptKind ScriptRegistry::GetKind(const std::string& name)
	{
		auto it = GetInstance()._entries.find(name);
		if (it == GetInstance()._entries.end())
		{
			return ScriptKind::system;
		}

		return it->second.kind;
	}

	const std::vector<std::string>& ScriptRegistry::GetNames()
	{
		return GetInstance()._names;
	}
}
