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
		using Factory = std::function<std::unique_ptr<ScriptBase>()>;

		template<typename T>
		static void Register(std::string_view name)
		{
			static_assert(std::is_base_of_v<SystemScript, T>, "script type must derive from SystemScript");

			Register(name, ScriptKind::system, []() { return std::unique_ptr<ScriptBase>{ new T{} }; });
		}

		static bool Register(std::string_view name, ScriptKind kind, Factory factory);

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
		struct Entry
		{
			ScriptKind kind{ ScriptKind::system };
			Factory factory{};
		};

		std::unordered_map<std::string, Entry> _entries{};
		std::vector<std::string> _names{};

	};
}
