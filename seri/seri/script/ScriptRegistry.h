#pragma once

#include "seri/core/Singleton.h"

#include "seri/logging/Logger.h"
#include "seri/script/ScriptBase.h"
#include "seri/script/system/SystemScript.h"

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace seri::script
{
	class ScriptRegistry : public seri::Singleton<ScriptRegistry>
	{
	public:
		using ScriptFactory = std::function<std::unique_ptr<ScriptBase>()>;

		static bool Register(std::string_view name, ScriptKind kind, ScriptFactory factory);

		static void Unregister(const std::string& name);

		static std::unique_ptr<ScriptBase> Create(const std::string& name);

		static bool Contains(const std::string& name);

		static ScriptKind GetKind(const std::string& name);

		static const std::vector<std::string>& GetNames();

	protected:
		friend struct seri::Singleton<ScriptRegistry>;

		ScriptRegistry() = default;
		~ScriptRegistry() = default;

	private:
		struct ScriptEntry
		{
			ScriptKind kind{ ScriptKind::system };
			ScriptFactory factory{};
		};

		std::unordered_map<std::string, ScriptEntry> _scriptEntries{};
		std::vector<std::string> _scriptNames{};

	};
}
