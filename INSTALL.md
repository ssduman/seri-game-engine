# Install

* Packages declared in `vcpkg.json` are built into `vcpkg_installed/` folder inside this repository with `vcpkg`
* Packages distributed with this project: `glad`, `tracy`
* Binaries distributed with this project: `Premake`

## Actions

* Utility actions
* For Linux: `./seri.sh  help`
* For Windows: `seri.bat help`

## Windows and Linux

### 1. Get vcpkg

```
git clone https://github.com/microsoft/vcpkg
cd <path-to-vcpkg>
```
* For Linux: `./bootstrap-vcpkg.sh`
* For Windows: `bootstrap-vcpkg.bat`

### 2. Clone Seri Game Engine

```
git clone https://github.com/ssduman/seri-game-engine
cd seri-game-engine
```

### 3. Install packages

* For Linux: `VCPKG_ROOT=<path-to-vcpkg> ./seri.sh install`
* For Windows: `set "VCPKG_ROOT=<path-to-vcpkg>" && seri.bat install`

### 4. Generate Visual Studio solution or Makefiles

* For Linux: `./seri.sh generate`
* For Windows: `seri.bat generate`

### 5. Build

* Open `Seri Game Engine.slnx` or
* For Linux: `./seri.sh build`
* For Windows: `seri.bat build`
    - Debug is default
    - `--config=[debug|release]`
    - `--rebuild` to rebuild

### 6. Project

* Select a `sproject` inside `project/` or create a new project

## Dependencies ##

* [GLFW](https://github.com/glfw/glfw)
* [glad](https://github.com/Dav1dde/glad)
* [stb](https://github.com/nothings/stb)
* [GLM](https://github.com/g-truc/glm)
* [FreeType](https://github.com/freetype/freetype)
* [assimp](https://github.com/assimp/assimp)
* [Dear ImGui](https://github.com/ocornut/imgui)
* [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo)
* [miniaudio](https://github.com/mackron/miniaudio)
* [SDL](https://github.com/libsdl-org/SDL)
* [EnTT](https://github.com/skypjack/entt)
* [yaml-cpp](https://github.com/jbeder/yaml-cpp)
* [fmt](https://github.com/fmtlib/fmt)
* [efsw](https://github.com/SpartanJ/efsw)
* [Lua](https://github.com/lua/lua)
* [sol2](https://github.com/ThePhD/sol2)
* [spdlog](https://github.com/gabime/spdlog)
* [doctest](https://github.com/doctest/doctest/)
* [Native File Dialog Extended](https://github.com/btzy/nativefiledialog-extended)
* [Jolt Physics](https://github.com/jrouwe/joltphysics)
* [Tracy Profiler](https://github.com/wolfpld/tracy)
* [vcpkg](https://github.com/microsoft/vcpkg)
* [Premake](https://github.com/premake/premake-core)
