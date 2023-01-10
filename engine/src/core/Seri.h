#pragma once

#include "../util/Util.h"

#include "../core/State.h"
#include "../core/Object.h"
#include "../core/Entity.h"
#include "../core/IRunner.h"
#include "../core/AuxiliaryStructs.h"
#include "../core/AuxiliaryStructsBuilder.h"

#include "../scene/Layer.h"
#include "../scene/Scene.h"
#include "../scene/SceneBuilder.h"
#include "../scene/SceneVisitor.h"
#include "../scene/SceneIterator.h"

#include "../model/Model.h"

#include "../camera/ICamera.h"

#include "../texture/Color.h"
#include "../texture/Typer.h"
#include "../texture/Skybox.h"

#include "../logging/Logger.h"

#include "../event/IEvent.h"
#include "../event/EventManager.h"

#include "../window/IWindowManager.h"
#include "../window/WindowManagerFactory.h"

#include "../renderer/OpenGLEngineBackend.h"
