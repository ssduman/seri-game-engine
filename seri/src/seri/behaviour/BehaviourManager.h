#pragma once

#include "seri/logging/Logger.h"

#include <vector>
#include <memory>

class BehaviourBase;

class BehaviourManager
{
public:
	BehaviourManager(BehaviourManager const&) = delete;

	void operator=(BehaviourManager const&) = delete;

	static void Init()
	{
		//LOGGER(info, "shader manager init done");
	}

	static BehaviourManager& GetInstance()
	{
		static BehaviourManager instance;
		return instance;
	}

	static void Add(BehaviourBase* behaviour);

	static void InitBehaviours();

	static void UpdateBehaviours();

	static void DestroyBehaviours();

private:
	BehaviourManager()
	{
		//LOGGER(info, "behaviour manager init");
	}

	~BehaviourManager()
	{
		LOGGER(info, "behaviour manager release");
	}

	std::vector<BehaviourBase*> _behaviours;

};
