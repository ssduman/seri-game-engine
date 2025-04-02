#pragma once

class BehaviourManager;

class BehaviourBase
{
public:
	BehaviourBase()
	{
		BehaviourManager::Add(this);
	}

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Destroy() = 0;

protected:

private:

};
