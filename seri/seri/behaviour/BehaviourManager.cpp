#include "Seripch.h"

#include "seri/behaviour/BehaviourManager.h"
#include "seri/behaviour/BehaviourBase.h"

namespace seri
{
	void BehaviourManager::Add(BehaviourBase* behaviour)
	{
		GetInstance()._behaviours.push_back(behaviour);
	}

	void BehaviourManager::InitBehaviours()
	{
		for (auto& behaviour : GetInstance()._behaviours)
		{
			behaviour->Init();
		}
	}

	void BehaviourManager::UpdateBehaviours()
	{
		for (auto& behaviour : GetInstance()._behaviours)
		{
			behaviour->Update();
		}
	}

	void BehaviourManager::DestroyBehaviours()
	{
		for (auto& behaviour : GetInstance()._behaviours)
		{
			behaviour->Destroy();
		}
	}
}
