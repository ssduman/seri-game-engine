project "Editor"
  kind "ConsoleApp"
  language "C++"
  cppdialect "C++17"
  staticruntime "Off"

  targetdir("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
  objdir("%{wks.location}/bin/int/" .. outputdir .. "/%{prj.name}")

  defines {
    "GLFW_INCLUDE_NONE",
  }
  
  files {
    "src/**.h",
    "src/**.cpp",
  }

  linkoptions {
    "-IGNORE:4098",
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
