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
