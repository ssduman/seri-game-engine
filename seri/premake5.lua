project "Seri"
  kind "StaticLib"
  language "C++"
  cppdialect "C++20"
  staticruntime "Off"

  pchheader "Seripch.h"
  pchsource "seri/core/Seripch.cpp"

  targetdir("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
  objdir("%{wks.location}/bin/int/" .. outputdir .. "/%{prj.name}")

  files {
    "seri/**.h",
    "seri/**.cpp",
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
    "%{wks.location}/seri",
    "seri/core",
    "%{IncludeDir.glad}",
  }

  externalincludedirs {
    "%{IncludeDir.vcpkg}",
  }
  externalwarnings "Off"

  links {
    "glad",
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

  usage "PUBLIC"
    includedirs {
      "%{wks.location}/seri",
    }

    externalincludedirs {
      "%{IncludeDir.vcpkg}",
    }
    externalwarnings "Off"

    defines {
      "GLFW_INCLUDE_NONE",
      "GLM_ENABLE_EXPERIMENTAL",
      "FMT_UNICODE=0",
      "FMT_SHARED",
      "ASSIMP_DLL",
      "BOOST_LOG_DYN_LINK",
    }

  usage "INTERFACE"
    links { "Seri" }

    filter { "configurations:Debug" }
      libdirs {
        "%{LibDir.vcpkg_debug}",
      }
      links {
        "%{Lib.boost_log_debug}",
        "%{Lib.boost_log_setup_debug}",
        "%{Lib.boost_thread_debug}",
        "fmtd",
      }
      postbuildcommands {
        '{COPYDIR} "%{BinDir.vcpkg_debug}" "%{cfg.targetdir}"',
      }

    filter { "configurations:Release" }
      libdirs {
        "%{LibDir.vcpkg}",
      }
      links {
        "%{Lib.boost_log}",
        "%{Lib.boost_log_setup}",
        "%{Lib.boost_thread}",
        "fmt",
      }
      postbuildcommands {
        '{COPYDIR} "%{BinDir.vcpkg}" "%{cfg.targetdir}"',
      }
