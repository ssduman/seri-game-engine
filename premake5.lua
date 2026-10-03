is_windows = os.target() == "windows"

platform_name = is_windows and "win" or "linux"
outputdir = platform_name .. "-%{cfg.buildcfg:lower()}-x64"

engine_version = (io.readfile(_MAIN_SCRIPT_DIR .. "/seri/seri/core/Literals.h") or ""):match('kVersion%s*=%s*"([^"]+)"')

vcpkg_triplet = is_windows and "x64-windows" or "x64-linux-dynamic"
vcpkg_root = _MAIN_SCRIPT_DIR .. "/vcpkg_installed/" .. vcpkg_triplet
vcpkg_bin = is_windows and "bin" or "lib"

IncludeDir = {}
IncludeDir["vcpkg"] = vcpkg_root .. "/include"
IncludeDir["glad"] = "%{wks.location}/seri/third_party/glad/include"
IncludeDir["tracy"] = "%{wks.location}/seri/third_party/tracy"

LibDir = {}
LibDir["vcpkg_release"] = vcpkg_root .. "/lib"
LibDir["vcpkg_debug"] = vcpkg_root .. "/debug/lib"

BinDir = {}
BinDir["vcpkg_release"] = vcpkg_root .. "/" .. vcpkg_bin
BinDir["vcpkg_debug"] = vcpkg_root .. "/debug/" .. vcpkg_bin

Lib = {}
Lib["assimp_release"] = is_windows and "assimp-vc145-mt" or "assimp"
Lib["assimp_debug"] = is_windows and "assimp-vc145-mtd" or "assimpd"

include "script/actions/install.lua"
include "script/actions/generate.lua"
include "script/actions/build.lua"
include "script/actions/package.lua"
include "script/actions/docs.lua"
include "script/actions/help.lua"

workspace "Seri Game Engine"
  architecture "x86_64"
  startproject "Editor"
  configurations { "Debug", "Release" }
  multiprocessorcompile "On"

  group "Core"
    include "seri"
  group ""

  group "Editor"
    include "editor"
  group ""

  group "Test"
    include "test"
  group ""

  group "Dependencies"
    include "seri/third_party/glad"
    include "seri/third_party/tracy"
  group ""
