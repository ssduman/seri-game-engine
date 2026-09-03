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

  linkoptions {
    "-IGNORE:4098",
  }

  disablewarnings {
    "4244",
    "4267",
    "4312",
  }

  includedirs {
    "src",
    "%{IncludeDir.glad}",
  }

  uses { "Seri" }

  links {
    "opengl32.lib",
    "glfw3dll",
    "efsw",
    "glm",
    "imguizmo",
    "lua",
    "SDL3",
  }

  filter "system:windows"
    systemversion "latest"

  filter { "configurations:Debug" }
    defines { "DEBUG" }
    runtime "Debug"
    symbols "On"
    libdirs {
      "%{LibDir.vcpkg_debug}",
    }
    links {
      "%{Lib.assimp_debug}",
      "freetyped",
      "imguid",
      "yaml-cppd",
    }
    postbuildcommands {
      "{COPYDIR} %{prj.location}assets %{cfg.buildtarget.directory}assets",
    }

  filter { "configurations:Release" }
    defines { "NDEBUG" }
    runtime "Release"
    optimize "On"
    libdirs {
      "%{LibDir.vcpkg}",
    }
    links {
      "%{Lib.assimp}",
      "freetype",
      "imgui",
      "yaml-cpp",
    }
    postbuildcommands {
      "{COPYDIR} %{prj.location}assets %{cfg.buildtarget.directory}assets",
    }
