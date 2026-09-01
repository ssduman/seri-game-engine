#include "Seripch.h"

#include "seri/script/ScriptRegistry.h"

namespace seri::script
{
	std::unique_ptr<ScriptBase> ScriptRegistry::Create(const std::string& name)
	{
		auto it = GetInstance()._factories.find(name);
		if (it == GetInstance()._factories.end())
		{
			return nullptr;
		}

		return it->second();
	}

	bool ScriptRegistry::Contains(const std::string& name)
	{
		return GetInstance()._factories.contains(name);
	}

	const std::vector<std::string>& ScriptRegistry::GetNames()
	{
		return GetInstance()._names;
	}
}
