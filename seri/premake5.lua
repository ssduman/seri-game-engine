project "Seri"
  kind "StaticLib"
  language "C++"
  cppdialect "C++20"
  staticruntime "Off"

  pchheader "Seripch.h"
  pchsource "src/seri/core/Seripch.cpp"

  targetdir("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
  objdir("%{wks.location}/bin/int/" .. outputdir .. "/%{prj.name}")

  dependson {
    "sdl",
  }

  files {
    "src/seri/**.h",
    "src/seri/**.cpp",
    "vendor/glm/glm/**.hpp",
    "vendor/glm/glm/**.inl",
    "vendor/stb/stb_image.h",
    "vendor/stb/stb_image.cpp",
    "vendor/stb/stb_image.cpp",
    "vendor/entt/single_include/entt/entt.hpp",
    "vendor/yaml-cpp/src/**.h",
    "vendor/yaml-cpp/src/**.cpp",
    "vendor/yaml-cpp/include/**.h",
  }

  defines {
    "GLFW_INCLUDE_NONE",
    "GLM_ENABLE_EXPERIMENTAL",
    "FMT_UNICODE=0",
    "YAML_CPP_STATIC_DEFINE",
  }

  linkoptions {
    "-IGNORE:4098",
    "-IGNORE:4267",
    "-IGNORE:4244",
  }

  disablewarnings {
    "4244",
  }

  includedirs {
    "src",
    "src/seri/core",
    "%{IncludeDir.glad}",
    "%{IncludeDir.GLFW}",
    "%{IncludeDir.glm}",
    "%{IncludeDir.stb}",
    "%{IncludeDir.ImGui}",
    "%{IncludeDir.assimp}",
    "%{IncludeDir.freetype}",
    "%{IncludeDir.nlohmann}",
    "%{IncludeDir.miniaudio}",
    "%{IncludeDir.sdl}",
    "%{IncludeDir.entt}",
    "%{IncludeDir.yamlcpp}",
    "%{IncludeDir.fmt}",
    "%{IncludeDir.filewatch}",
  }

  libdirs {
    "%{LibDir.sdl}",
  }

  links {
    "fmt",
    "glad",
    "GLFW",
    "SDL3",
    "ImGui",
    "assimp",
    "freetype",
    "opengl32.lib",
  }

  filter "files:vendor/yaml-cpp/src/**.cpp"
	  flags { "NoPCH" }

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
