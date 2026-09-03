# Install

* Third-party dependencies come from `vcpkg` in manifest mode
* Packages declared in `vcpkg.json` are built into a `vcpkg_installed/` folder inside this repository
* The only dependency not coming from `vcpkg` is `glad`

## Prerequisites

* Visual Studio 2026
* Git
* vcpkg

## Fresh install

### 1. Clone this repository

```bat
git clone https://github.com/ssduman/seri-game-engine
cd seri-game-engine
```

### 2. Get vcpkg

```bat
git clone https://github.com/microsoft/vcpkg
cd <path-to-vcpkg>
bootstrap-vcpkg.bat
```

* Add `VCPKG_ROOT` and set to `<path-to-vcpkg>` as Environment Variable so `generate.bat` can find it

### 3. Generate the solution

```bat
generate.bat
```

### 4. Build

* Open `Seri Game Engine.slnx`, build and run

## Adding library

1. Look up the port name and version for the library from `vcpkg`

2. Add it to `dependencies` and `overrides` fields in `vcpkg.json`, then add version, port or features if suitable

3. Reinstall with `generate.bat` and update `premake` configs

## Upgrade library

1. Update your `vcpkg` clone and re-bootstrap

```bat
cd <path-to-vcpkg>
git pull
bootstrap-vcpkg.bat
```

2. Get the new baseline commit SHA to put in `vcpkg.json`

3. Look up the new versions and ports of each packages you want to upgrade

4. Re-run `generate.bat`

## Removing a package

1. Delete it from `vcpkg.json` entirely

2. Delete all related settings from `premake` configs

3. Delete it from `vcpkg_installed/` if exists

## Troubleshooting

### Rebuilding a dependency from scratch

* Delete `vcpkg_installed/` and re-run `generate.bat`. To also bypass the cache, delete `%LOCALAPPDATA%/vcpkg/archives`

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
* [vcpkg](https://github.com/microsoft/vcpkg)
* [Premake](https://github.com/premake/premake-core)
