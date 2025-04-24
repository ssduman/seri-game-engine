#pragma once

#include "seri/util/Util.h"

#include "seri/core/Time.h"
#include "seri/core/Object.h"
#include "seri/core/IRunner.h"
#include "seri/core/Application.h"

#include "seri/scene/Layer.h"
#include "seri/scene/Scene.h"
#include "seri/scene/SceneBuilder.h"
#include "seri/scene/SceneVisitor.h"
#include "seri/scene/SceneIterator.h"

#include "seri/model/ModelImporter.h"

#include "seri/shader/ShaderManager.h"

#include "seri/behaviour/BehaviourManager.h"

#include "seri/graphic/Graphic.h"

#include "seri/logging/Logger.h"

#include "seri/camera/ICamera.h"

#include "seri/sound/SoundManager.h"

#include "seri/texture/Color.h"
#include "seri/texture/Typer.h"
#include "seri/texture/Skybox.h"
#include "seri/texture/Texture.h"

#include "seri/font/FontManager.h"

#include "seri/input/Input.h"
#include "seri/input/InputManager.h"

#include "seri/event/IEvent.h"
#include "seri/event/EventDispatcher.h"

#include "seri/window/IWindowManager.h"
#include "seri/window/WindowManagerFactory.h"

#include "seri/renderer/OpenGLEngineBackend.h"
#include "seri/renderer/AuxiliaryStructsBuilder.h"
