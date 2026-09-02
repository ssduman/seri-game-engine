#pragma once

namespace seri
{
	template <typename T>
	struct Singleton
	{
		static T& GetInstance()
		{
			static T instance{};
			return instance;
		}

		Singleton(const Singleton&) = delete;
		Singleton& operator=(const Singleton&) = delete;

	protected:
		Singleton() = default;
		~Singleton() = default;

	};
}
