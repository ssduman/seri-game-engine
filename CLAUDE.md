# Seri Game Engine

## Project Overview

A 2D/3D OpenGL game engine written in C++20, featuring an ECS architecture, PBR lighting, shadow mapping, skeletal animation, and an ImGui-based editor.

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
- **Command buffer:** Render commands queued before execution
- **YAML serialization:** Components serialize to/from YAML for scene persistence

## Coding Conventions

- **Language:** C++20
- **PCH:** Include `Seripch.h` in all core library `.cpp` files (unprefixed; `seri/core` is on the Seri project's include path for this)
- **Namespaces:** Engine code lives in the `seri` namespace
- **Naming:** PascalCase for classes, camelCase for methods/variables
- **Component data:** Stored directly on components (no separate data files)
- **Static APIs:** Prefer static facade classes (Graphic, RenderingManager) over direct access to implementations

## Important Notes

- Ignore misc/ folder.
- The engine is under active development.
- Run `generate.bat` to restore dependencies and regenerate the solution. See `INSTALL.md` for fresh-install and upgrade paths.
- To change a dependency version, edit `vcpkg.json` — never edit anything under `vcpkg_installed/`.
- Linking uses the `x64-windows` triplet (dynamic libs, `/MD`). Editor post-build copies the vcpkg DLLs next to the executable.

## Rules

- Do not write comment unless stricly necessary.
- Only use ASCII characters.
- Do not overengineer things, do what asked.
- Do not try to write test cases.
