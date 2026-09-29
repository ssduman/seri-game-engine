outputdir = "%{cfg.buildcfg}-%{cfg.architecture}"

vcpkg_root = _MAIN_SCRIPT_DIR .. "/vcpkg_installed/" .. "x64-windows"

IncludeDir = {}
IncludeDir["vcpkg"] = vcpkg_root .. "/include"
IncludeDir["glad"] = "%{wks.location}/seri/third_party/glad/include"
IncludeDir["tracy"] = "%{wks.location}/seri/third_party/tracy"

LibDir = {}
LibDir["vcpkg"] = vcpkg_root .. "/lib"
LibDir["vcpkg_debug"] = vcpkg_root .. "/debug/lib"

BinDir = {}
BinDir["vcpkg"] = vcpkg_root .. "/bin"
BinDir["vcpkg_debug"] = vcpkg_root .. "/debug/bin"

Lib = {}
Lib["assimp"] = "assimp-vc145-mt"
Lib["assimp_debug"] = "assimp-vc145-mtd"

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

  group "Misc"
    -- include "misc/misc"
    -- include "misc/maze"
    -- include "misc/snake"
    -- include "misc/tetris"
  group ""

  group "Dependencies"
    include "seri/third_party/glad"
    include "seri/third_party/tracy"
  group ""
