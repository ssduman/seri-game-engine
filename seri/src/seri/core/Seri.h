#pragma once

#include "seri/util/Util.h"

#include "seri/core/Object.h"
#include "seri/core/Entity.h"
#include "seri/core/IRunner.h"

#include "seri/scene/Layer.h"
#include "seri/scene/Scene.h"
#include "seri/scene/SceneBuilder.h"
#include "seri/scene/SceneVisitor.h"
#include "seri/scene/SceneIterator.h"

#include "seri/model/Model.h"

#include "seri/logging/Logger.h"

#include "seri/camera/ICamera.h"

#include "seri/texture/Color.h"
#include "seri/texture/Typer.h"
#include "seri/texture/Skybox.h"

#include "seri/input/Input.h"

#include "seri/event/IEvent.h"
#include "seri/event/EventDispatcher.h"

#include "seri/window/IWindowManager.h"
#include "seri/window/WindowManagerFactory.h"

#include "seri/renderer/OpenGLEngineBackend.h"
#include "seri/renderer/AuxiliaryStructsBuilder.h"
