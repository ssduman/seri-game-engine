outputdir = "%{cfg.buildcfg}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["glad"] = "%{wks.location}/seri/vendor/glad/include"
IncludeDir["GLFW"] = "%{wks.location}/seri/vendor/glfw/include"
IncludeDir["glm"] = "%{wks.location}/seri/vendor/glm"
IncludeDir["stb"] = "%{wks.location}/seri/vendor/stb"
IncludeDir["ImGui"] = "%{wks.location}/seri/vendor/imgui"
IncludeDir["assimp"] = "%{wks.location}/seri/vendor/assimp/include"
IncludeDir["freetype"] = "%{wks.location}/seri/vendor/freetype/include"
IncludeDir["nlohmann"] = "%{wks.location}/seri/vendor/nlohmann"
IncludeDir["miniaudio"] = "%{wks.location}/seri/vendor/miniaudio"
IncludeDir["sdl"] = "%{wks.location}/seri/vendor/sdl/include"
IncludeDir["entt"] = "%{wks.location}/seri/vendor/entt/single_include"
IncludeDir["yamlcpp"] = "%{wks.location}/seri/vendor/yaml-cpp/include"
IncludeDir["fmt"] = "%{wks.location}/seri/vendor/fmt/include"
IncludeDir["filewatch"] = "%{wks.location}/seri/vendor/filewatch"

LibDir = {}
LibDir["sdl"] = "%{wks.location}/seri/vendor/sdl/build/%{cfg.buildcfg}"

workspace "Seri Game Engine"
  architecture "x86_64"
  startproject "Editor"
  configurations { "Debug", "Release" }
  flags { "MultiProcessorCompile" }

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
    include "seri/vendor/glad"
    include "seri/vendor/glfw"
    include "seri/vendor/imgui"
    include "seri/vendor/assimp"
    include "seri/vendor/freetype"
    include "seri/vendor/sdl"
    include "seri/vendor/fmt"
  group ""
