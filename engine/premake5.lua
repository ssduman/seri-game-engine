project "Engine"
  kind "StaticLib"
  language "C++"
  cppdialect "C++17"
  staticruntime "off"

  pchheader "Seripch.h"
  pchsource "src/core/Seripch.cpp"

  targetdir("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
  objdir("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

  files {
    "src/**.h",
    "src/**.cpp",
    "vendor/glm/glm/**.hpp",
    "vendor/glm/glm/**.inl",
    "vendor/stb/stb_image.h",
    "vendor/stb/stb_image.cpp",
  }

  defines {}

  includedirs {
    "src",
    "src/core",
    "%{IncludeDir.glad}",
    "%{IncludeDir.GLFW}",
    "%{IncludeDir.glm}",
    "%{IncludeDir.stb}",
    "%{IncludeDir.ImGui}",
    "%{IncludeDir.assimp}",
    "%{IncludeDir.freetype}",
  }

  links {
    "glad",
    "GLFW",
    "ImGui",
    "assimp",
    "freetype",
    "opengl32.lib",
  }

  filter "system:windows"
    systemversion "latest"

  filter { "configurations:Debug" }
    defines { "DEBUG" }
    runtime "Debug"
    symbols "On"

  filter { "configurations:Release" }
    defines { "NDEBUG" }
    runtime "Release"
    optimize "On"
