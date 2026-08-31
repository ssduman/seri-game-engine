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
    "FMT_SHARED",
    "ASSIMP_DLL",
    "BOOST_LOG_DYN_LINK",
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
    "%{wks.location}/seri",
    "%{IncludeDir.glad}",
  }

  externalincludedirs {
    "%{IncludeDir.vcpkg}",
  }
  externalwarnings "Off"

  links {
    "Seri",
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
      "%{Lib.boost_log_debug}",
      "%{Lib.boost_log_setup_debug}",
      "%{Lib.boost_thread_debug}",
      "%{Lib.assimp_debug}",
      "fmtd",
      "freetyped",
      "imguid",
      "yaml-cppd",
    }
    postbuildcommands {
      '{COPYDIR} "%{BinDir.vcpkg_debug}" "%{cfg.targetdir}"',
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
      "%{Lib.boost_log}",
      "%{Lib.boost_log_setup}",
      "%{Lib.boost_thread}",
      "%{Lib.assimp}",
      "fmt",
      "freetype",
      "imgui",
      "yaml-cpp",
    }
    postbuildcommands {
      '{COPYDIR} "%{BinDir.vcpkg}" "%{cfg.targetdir}"',
      "{COPYDIR} %{prj.location}assets %{cfg.buildtarget.directory}assets",
    }
