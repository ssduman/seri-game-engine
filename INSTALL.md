# Install

* Third-party dependencies come from `vcpkg` in manifest mode.
* Nothing is installed machine-wide.
* Packages declared in `vcpkg.json` are built into a `vcpkg_installed/` folder inside this repository.
* The only dependency still vendored is `glad`.

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

* If you do not already have a vcpkg clone:

```bat
git clone https://github.com/microsoft/vcpkg C:\vcpkg
C:\vcpkg\bootstrap-vcpkg.bat
```

* `bootstrap-vcpkg.bat`  must be re-run whenever you update the clone
* Set `VCPKG_ROOT` to `vcpkg` as Environment Variable so `generate.bat` can find it anywhere

### 3. Generate the solution

```bat
generate.bat
```

### 4. Build

* Open `Seri Game Engine.sln` and build. `Editor` is the startup project.

## Adding a package

1. Look up the port name and version from `vcpkg`:

1. Add it to `dependencies` in `vcpkg.json`, add version, port or features if suitable:

1. Install it with `generate.bat` and update `premake` configs:

## Upgrading packages

1. Update your vcpkg clone and re-bootstrap:

```bat
cd C:\vcpkg
git pull
bootstrap-vcpkg.bat
```

1. Get the new baseline commit SHA to put in `vcpkg.json`.

1. Look up the new version and port of each package you want to move.

1. Re-run `generate.bat`:

## Removing a package

1. Delete it from `dependencies` in `vcpkg.json`, and its `overrides` entry if it has one.

1. Delete all related settings from `premake` configs.

## Troubleshooting

### Rebuilding a dependency from scratch

* Delete `vcpkg_installed\` and re-run `generate.bat`. To also bypass the binary cache, delete `%LOCALAPPDATA%\vcpkg\archives`.

## Dependencies: ##

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
* [vcpkg](https://github.com/microsoft/vcpkg)
* [Premake](https://github.com/premake/premake-core)
