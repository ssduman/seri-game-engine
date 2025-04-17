#pragma once

#include "seri/logging/Logger.h"

#include <vector>
#include <memory>

namespace seri
{
	class BehaviourBase;

	class BehaviourManager
	{
	public:
		BehaviourManager(BehaviourManager const&) = delete;

		void operator=(BehaviourManager const&) = delete;

		static BehaviourManager& GetInstance()
		{
			static BehaviourManager instance;
			return instance;
		}

		static void Init()
		{
			GetInstance();
		}

		static void Add(BehaviourBase* behaviour);

		static void InitBehaviours();

		static void UpdateBehaviours();

		static void DestroyBehaviours();

	private:
		BehaviourManager()
		{
		}

		~BehaviourManager()
		{
		}

		std::vector<BehaviourBase*> _behaviours;

	};
}
