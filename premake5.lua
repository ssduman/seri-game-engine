outputdir = "%{cfg.buildcfg}-%{cfg.architecture}"

vcpkg_dir = "%{wks.location}/vcpkg_installed/" .. "x64-windows"

boost_ver = "vc145-mt-x64-1_92"
boost_ver_debug = "vc145-mt-gd-x64-1_92"

IncludeDir = {}
IncludeDir["vcpkg"] = vcpkg_dir .. "/include"
IncludeDir["glad"] = "%{wks.location}/seri/third_party/glad/include"

LibDir = {}
LibDir["vcpkg"] = vcpkg_dir .. "/lib"
LibDir["vcpkg_debug"] = vcpkg_dir .. "/debug/lib"

BinDir = {}
BinDir["vcpkg"] = vcpkg_dir .. "/bin"
BinDir["vcpkg_debug"] = vcpkg_dir .. "/debug/bin"

Lib = {}
Lib["assimp"] = "assimp-vc145-mt"
Lib["assimp_debug"] = "assimp-vc145-mtd"
Lib["boost_log"] = "boost_log-" .. boost_ver
Lib["boost_log_debug"] = "boost_log-" .. boost_ver_debug
Lib["boost_log_setup"] = "boost_log_setup-" .. boost_ver
Lib["boost_log_setup_debug"] = "boost_log_setup-" .. boost_ver_debug

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

  group "Misc"
    -- include "misc/misc"
    -- include "misc/maze"
    -- include "misc/snake"
    -- include "misc/tetris"
  group ""

  group "Dependencies"
    include "seri/third_party/glad"
  group ""
