outputdir = "%{cfg.buildcfg}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["glad"] = "%{wks.location}/engine/vendor/glad/include"
IncludeDir["GLFW"] = "%{wks.location}/engine/vendor/glfw/include"
IncludeDir["glm"] = "%{wks.location}/engine/vendor/glm"
IncludeDir["stb"] = "%{wks.location}/engine/vendor/stb"
IncludeDir["ImGui"] = "%{wks.location}/engine/vendor/imgui"
IncludeDir["assimp"] = "%{wks.location}/engine/vendor/assimp/include"
IncludeDir["freetype"] = "%{wks.location}/engine/vendor/freetype/include"

workspace "Seri Game Engine"
  architecture "x86_64"
  startproject "Mics"
  configurations { "Debug", "Release" }
  flags { "MultiProcessorCompile" }

  group "Core"
    include "engine"
  group ""

  group "Editor"
    include "editor"
  group ""

  group "Misc"
    include "mics"
    --[[
    include "maze"
    include "snake"
    include "tetris"
    --]]
  group ""
  
  group "Dependencies"
    include "engine/vendor/glad"
    include "engine/vendor/glfw"
    include "engine/vendor/imgui"
    include "engine/vendor/assimp"
    include "engine/vendor/freetype"
  group ""
