#pragma once

#include "seri/logging/Logger.h"
//#include "seri/behaviour/IBehaviour.h"

#include <vector>
#include <memory>

class IBehaviour;

class BehaviourManager
{
public:
	BehaviourManager(BehaviourManager const&) = delete;

	void operator=(BehaviourManager const&) = delete;

	static void Init()
	{
		LOGGER(info, "shader manager init done");
	}

	static BehaviourManager& GetInstance()
	{
		static BehaviourManager instance;
		return instance;
	}

	static void Add(IBehaviour* behaviour);

	static void InitBehaviours();

	static void UpdateBehaviours();

	static void DestroyBehaviours();

private:
	BehaviourManager()
	{
		LOGGER(info, "behaviour manager init");
	}

	~BehaviourManager()
	{
		LOGGER(info, "behaviour manager release");
	}

	std::vector<IBehaviour*> _behaviours;

};
