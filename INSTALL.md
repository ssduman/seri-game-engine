# Install

* Packages declared in `vcpkg.json` are built into `vcpkg_installed/` folder inside this repository with `vcpkg`
* Only project that distributed with this project is `glad`
* Only binary that distributed with this project is `Premake`

## Fresh install

### 1. Get vcpkg

```bat
git clone https://github.com/microsoft/vcpkg
cd <path-to-vcpkg>
bootstrap-vcpkg.bat
```

### 2. Clone this repository

```bat
git clone https://github.com/ssduman/seri-game-engine
cd seri-game-engine
```

### 3. Install packages

```bat
set "VCPKG_ROOT=<path-to-vcpkg>" && install.bat
```

### 4. Generate solution

```bat
generate.bat
```

### 5. Build

* Open `Seri Game Engine.slnx` with Visual Studio 2026, build and run

## Adding package

1. Look up the port and version data for the package from `vcpkg`

2. Add it to `dependencies` and `overrides` fields in `vcpkg.json`

3. Reinstall with `install.bat` and update `Premake`

## Upgrade

1. Update your `vcpkg` clone and re-bootstrap

```bat
cd <path-to-vcpkg>
git pull
bootstrap-vcpkg.bat
```

2. Get the commit SHA to put in `vcpkg.json` for the new baseline

3. Look up the new versions and ports of each packages you want to upgrade

4. Re-run `install.bat`

## Removing package

1. Delete it from `vcpkg.json` entirely

2. Delete it from `vcpkg_installed/`

3. Delete all related settings from `Premake`

4. Re-run `install.bat`

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
* [Boost](https://www.boost.org/)
* [doctest](https://github.com/doctest/doctest/)
* [nlohmann-json](https://github.com/nlohmann/json)
* [vcpkg](https://github.com/microsoft/vcpkg)
* [Premake](https://github.com/premake/premake-core)
