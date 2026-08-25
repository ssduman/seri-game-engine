# Seri Game Engine

## Project Overview

A 2D/3D OpenGL game engine written in C++20, featuring an ECS architecture, PBR lighting, shadow mapping, skeletal animation, and an ImGui-based editor.

## Build System

### Project Layout in Solution
- **Core group:** Seri (static library)
- **Editor group:** Editor (console application)
- **Dependencies group:** glad only (vendored; see below). Everything else comes from vcpkg as prebuilt libraries.

## Technology Stack

| Tech | Purpose |
|------|---------|
| C++20 | Language standard |
| OpenGL | Graphics API |
| glad | OpenGL loader |
| GLFW 3.4 | Window management |
| GLM | Math library |
| EnTT | ECS framework |
| assimp | 3D model loading (FBX) |
| Dear ImGui | Editor GUI |
| ImGuizmo | 3D editor gizmos |
| miniaudio | Audio |
| FreeType | Font rendering |
| yaml-cpp | Scene serialization |
| fmt | String formatting |
| efsw | File system watching |
| Lua + sol2 | Scripting |
| stb_image | Image loading |
| nlohmann/json | JSON |

## Key Design Patterns

- **Singletons:** Application, Graphic, RenderingManager, AssetManager
- **Abstraction layers:** Base classes (`ShaderBase`, `TextureBase`, `BufferBase`, etc.) with OpenGL-specific implementations
- **Factory pattern:** `Create()` methods for shaders, textures, buffers
- **Command buffer:** Render commands queued before execution
- **YAML serialization:** Components serialize to/from YAML for scene persistence

## Coding Conventions

- **Language:** C++20
- **Source layout:** engine sources live directly under `seri/src/<area>/` (e.g. `seri/src/asset/`, `seri/src/core/`); there is no nested `seri/src/seri/`. Includes therefore carry no `seri/` prefix: `#include "asset/AssetManager.h"`, and from the editor `#include <core/Seri.h>`.
- **Include shadowing:** `editor/src` is searched before `seri/src`, and both contain `behaviour/`, `event/` and `layer/`. No filenames collide today, but a new editor file whose name matches an engine header in those directories would silently shadow it.
- **Vendored code:** glad lives in `seri/third_party/glad` (it is a separate premake project). Vendored translation units that build into Seri itself live in `seri/src/third_party/` (e.g. the `STB_IMAGE_IMPLEMENTATION` TU), so they are picked up by the `src/**` glob.
- **Windowing backend:** GLFW and SDL3 are both supported and both build clean. They are a compile-time either/or, selected in `seri/src/core/Core.h` by `SERI_USE_WINDOW_GLFW` / `SERI_USE_WINDOW_SDL3` — exactly one must be defined. GLFW is the default. Both backends' dependencies are always installed and linked, so switching needs no build-file change; an unused import lib costs nothing.
- **PCH:** Include `Seripch.h` in all core library `.cpp` files
- **Namespaces:** Engine code lives in the `seri` namespace
- **Naming:** PascalCase for classes, camelCase for methods/variables
- **Component data:** Stored directly on components (no separate data files)
- **Static APIs:** Prefer static facade classes (Graphic, RenderingManager) over direct access to implementations

## Important Notes

- The engine is under active development
- Third-party dependencies come from **vcpkg in manifest mode**. They are declared in `vcpkg.json`, pinned by `builtin-baseline` plus explicit `overrides`, and built into `vcpkg_installed/` (gitignored). There are no git submodules.
- Run `generate.bat` to restore dependencies and regenerate the solution. See `INSTALL.md` for fresh-install and upgrade paths.
- To change a dependency version, edit `vcpkg.json` — never edit anything under `vcpkg_installed/`.
- **glad is the one exception** and stays vendored in `seri/third_party/glad`: the vcpkg `glad` port is glad1 (`<glad/glad.h>`), while this engine uses the glad2 API (`<glad/gl.h>`, `gladLoadGL(GLADloadfunc)`).
- Linking uses the `x64-windows` triplet (dynamic libs, `/MD`). Editor post-build copies the vcpkg DLLs next to the executable.

## Rules

- Do not write comment unless stricly necessary.
- Only use ASCII characters.
- Do not overengineer things, do what asked.
- Do not try to write test cases.
