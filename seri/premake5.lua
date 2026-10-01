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
    "SPDLOG_COMPILED_LIB",
    "SPDLOG_SHARED_LIB",
    "TRACY_ENABLE",
    "TRACY_ON_DEMAND",
  }

  includedirs {
    "%{wks.location}/seri",
    "seri/core",
    "%{IncludeDir.glad}",
  }

  externalincludedirs {
    "%{IncludeDir.vcpkg}",
    "%{IncludeDir.tracy}",
  }
  externalwarnings "Off"

  links {
    "glad",
    "tracy",
  }

  filter "system:windows"
    systemversion "latest"
    linkoptions {
      "-IGNORE:4098",
    }
    buildoptions {
      "/bigobj",
    }
    disablewarnings {
      "4100",
      "4244",
      "4267",
      "4312",
    }

  filter { "configurations:Debug" }
    defines { "DEBUG" }
    runtime "Debug"
    symbols "On"
    editandcontinue "Off"

  filter { "configurations:Release" }
    defines { "NDEBUG" }
    runtime "Release"
    optimize "On"
    symbols "On"

  usage "PUBLIC"
    includedirs {
      "%{wks.location}/seri",
    }

    externalincludedirs {
      "%{IncludeDir.vcpkg}",
      "%{IncludeDir.tracy}",
    }
    externalwarnings "Off"

    defines {
      "GLFW_INCLUDE_NONE",
      "GLM_ENABLE_EXPERIMENTAL",
      "FMT_UNICODE=0",
      "FMT_SHARED",
      "ASSIMP_DLL",
      "SPDLOG_COMPILED_LIB",
      "SPDLOG_SHARED_LIB",
      "TRACY_ENABLE",
      "TRACY_ON_DEMAND",
    }

  usage "INTERFACE"
    links { "Seri" }

    filter "system:linux"
      links {
        "glad",
        "tracy",
        "dl",
        "pthread",
      }

    filter { "configurations:Debug" }
      libdirs {
        "%{LibDir.vcpkg_debug}",
      }
      links {
        "fmtd",
        "spdlogd",
      }

    filter { "configurations:Release" }
      libdirs {
        "%{LibDir.vcpkg_release}",
      }
      links {
        "fmt",
        "spdlog",
      }

    filter { "system:windows", "configurations:Debug" }
      postbuildcommands {
        'xcopy /Q /Y /I /D "' .. path.translate(BinDir.vcpkg_debug .. "/*.dll") .. '" "%{cfg.targetdir}/"',
      }

    filter { "system:windows", "configurations:Release" }
      postbuildcommands {
        'xcopy /Q /Y /I /D "' .. path.translate(BinDir.vcpkg_release .. "/*.dll") .. '" "%{cfg.targetdir}/"',
      }

    filter { "system:linux", "configurations:Debug" }
      postbuildcommands {
        'cp -a -u "' .. BinDir.vcpkg_debug .. '"/*.so* "%{cfg.targetdir}/"',
      }

    filter { "system:linux", "configurations:Release" }
      postbuildcommands {
        'cp -a -u "' .. BinDir.vcpkg_release .. '"/*.so* "%{cfg.targetdir}/"',
      }
