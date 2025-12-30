project "ImGuizmo"
  kind "StaticLib"
  language "C++"
  cppdialect "C++17"
  staticruntime "Off"

  targetdir("bin/" .. outputdir .. "/%{prj.name}")
  objdir("bin/int/" .. outputdir .. "/%{prj.name}")

  files {
    "./ImGuizmo.h",
    "./ImGuizmo.cpp",
  }

  includedirs {
    "./",
    "%{IncludeDir.ImGui}",
  }

  filter "system:windows"
    systemversion "latest"

  filter "configurations:Debug"
    runtime "Debug"
    symbols "On"

  filter "configurations:Release"
    runtime "Release"
    optimize "On"
