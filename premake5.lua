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
    -- include "mics/mics"
    -- include "mics/maze"
    -- include "mics/snake"
    -- include "mics/tetris"
  group ""
  
  group "Dependencies"
    include "seri/vendor/glad"
    include "seri/vendor/glfw"
    include "seri/vendor/imgui"
    include "seri/vendor/assimp"
    include "seri/vendor/freetype"
  group ""
