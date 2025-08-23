project "ImGui"
  kind "StaticLib"
  language "C++"
  cppdialect "C++17"
  staticruntime "Off"

  targetdir("bin/" .. outputdir .. "/%{prj.name}")
  objdir("bin/int/" .. outputdir .. "/%{prj.name}")

  files {
    "./*.h",
    "./*.cpp",
    "backends/imgui_impl_glfw.h",
    "backends/imgui_impl_glfw.cpp",
    "backends/imgui_impl_sdl3.h",
    "backends/imgui_impl_sdl3.cpp",
    "backends/imgui_impl_opengl3.h",
    "backends/imgui_impl_opengl3.cpp",
    "backends/imgui_impl_opengl3_loader.h",
  }

  includedirs {
    "./",
    "%{IncludeDir.GLFW}",
    "%{IncludeDir.sdl}",
  }

  filter "system:windows"
    systemversion "latest"

  filter "configurations:Debug"
    runtime "Debug"
    symbols "On"

  filter "configurations:Release"
    runtime "Release"
    optimize "On"
