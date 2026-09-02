#pragma once

#include "seri/core/Singleton.h"

#include "seri/logging/Logger.h"
#include "seri/script/ScriptBase.h"

#include <fmt/format.h>

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
			static_assert(std::is_base_of_v<ScriptBase, T>, "script type must derive from ScriptBase");

			std::string key{ name };

			if (GetInstance()._factories.contains(key))
			{
				LOGGER(warning) << fmt::format("[script] '{}' is already registered", key);
				return;
			}

			GetInstance()._factories.emplace(key, []() { return std::unique_ptr<ScriptBase>{ new T{} }; });
			GetInstance()._names.emplace_back(key);

			LOGGER(info) << fmt::format("[script] '{}' registered", key);
		}

		static std::unique_ptr<ScriptBase> Create(const std::string& name);

		static bool Contains(const std::string& name);

		static const std::vector<std::string>& GetNames();

	protected:
		friend struct seri::Singleton<ScriptRegistry>;

		ScriptRegistry() = default;
		~ScriptRegistry() = default;

	private:
		std::unordered_map<std::string, Factory> _factories{};
		std::vector<std::string> _names{};

	};
}
