#pragma once

#include "../util/Util.h"

#include "../core/Object.h"
#include "../core/Entity.h"
#include "../core/IRunner.h"

#include "../scene/Layer.h"
#include "../scene/Scene.h"
#include "../scene/SceneBuilder.h"
#include "../scene/SceneVisitor.h"
#include "../scene/SceneIterator.h"

#include "../model/Model.h"

#include "../logging/Logger.h"

#include "../camera/ICamera.h"

#include "../texture/Color.h"
#include "../texture/Typer.h"
#include "../texture/Skybox.h"

#include "../input/Input.h"

#include "../event/IEvent.h"
#include "../event/EventDispatcher.h"

#include "../window/IWindowManager.h"
#include "../window/WindowManagerFactory.h"

#include "../renderer/OpenGLEngineBackend.h"
#include "../renderer/AuxiliaryStructsBuilder.h"
