project "Editor"
  kind "ConsoleApp"
  language "C++"
  cppdialect "C++17"
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
  }

  linkoptions {
    "-IGNORE:4098",
    "-IGNORE:4267",
    "-IGNORE:4244",
  }

  disablewarnings {
    "4244"
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
  }

  links {
    "Seri",
  }

  postbuildcommands {
    "{COPYDIR} %{prj.location}assets %{cfg.buildtarget.directory}assets",
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
