project "Editor"
  kind "ConsoleApp"
  language "C++"
  cppdialect "C++20"
  staticruntime "Off"

  targetdir("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
  objdir("%{wks.location}/bin/int/" .. outputdir .. "/%{prj.name}")

  files {
    "src/**.h",
    "src/**.cpp",
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
    "%{wks.location}/seri/src",
    "%{wks.location}/seri/vendor",
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
    "%{IncludeDir.efsw}",
    "%{IncludeDir.lua}",
    "%{IncludeDir.sol2}",
    "%{IncludeDir.ImGuizmo}",
  }

  libdirs {
    "%{LibDir.sdl}",
  }

  links {
    "Seri",
    "SDL3",
    "efsw-static-lib",
  }

  postbuildcommands {
    "{COPYDIR} %{prj.location}assets %{cfg.buildtarget.directory}assets",
    "{COPY} %{LibDir.sdl}/SDL3.dll %{cfg.targetdir}"
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
