#pragma once

//#include "seri/behaviour/BehaviourManager.h"

class BehaviourManager;

class IBehaviour
{
public:
	IBehaviour()
	{
		BehaviourManager::Add(this);
	}

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Destroy() = 0;

protected:

private:

};
