#pragma once

#define SERI_USE_WINDOW_GLFW
//#define SERI_USE_WINDOW_SDL3

#define SERI_USE_RENDERING_OPENGL
//#define SERI_USE_RENDERING_D3D11
//#define SERI_USE_RENDERING_D3D12
//#define SERI_USE_RENDERING_VULKAN
//#define SERI_USE_RENDERING_NULL

#if defined (SERI_USE_WINDOW_SDL3)
#define SDL_MAIN_HANDLED
#define SDL_MAIN_USE_CALLBACKS 1
#endif

#include "seri/util/Util.h"

#include "seri/core/Object.h"
#include "seri/core/IRunner.h"
#include "seri/core/Application.h"
#include "seri/core/TimeWrapper.h"

#include "seri/scene/Layer.h"
#include "seri/scene/Scene.h"
#include "seri/scene/SceneBuilder.h"
#include "seri/scene/SceneVisitor.h"
#include "seri/scene/SceneIterator.h"

#include "seri/model/ModelImporter.h"

#include "seri/shader/ShaderBase.h"
#include "seri/shader/ShaderLibrary.h"

#include "seri/graphic/Graphic.h"
#include "seri/graphic/Material.h"

#include "seri/behaviour/BehaviourManager.h"

#include "seri/logging/Logger.h"

#include "seri/camera/CameraBase.h"

#include "seri/shape/ShapeFactory.h"

#include "seri/sound/SoundManager.h"

#include "seri/texture/Color.h"
#include "seri/texture/Skybox.h"
#include "seri/texture/TextureBase.h"

#include "seri/font/FontManager.h"

#include "seri/input/Input.h"
#include "seri/input/InputManager.h"

#include "seri/event/IEvent.h"
#include "seri/event/EventDispatcher.h"

#include "seri/window/WindowManager.h"
#include "seri/window/WindowManagerBase.h"

#include "seri/rendering/BufferBase.h"
#include "seri/rendering/FramebufferBase.h"
#include "seri/rendering/VertexArrayBase.h"
#include "seri/rendering/RenderingManager.h"
#include "seri/rendering/RenderingManagerBase.h"
