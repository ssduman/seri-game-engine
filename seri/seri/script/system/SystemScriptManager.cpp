#include "Seripch.h"

#include "seri/script/system/SystemScriptManager.h"
#include "seri/script/system/RotatorScript.h"
#include "seri/script/ScriptRegistry.h"

namespace seri::script
{
	void SystemScriptManager::Init()
	{
		ScriptRegistry::Register<RotatorScript>("Rotator");
	}
}
