project "Editor"
  kind "ConsoleApp"
  language "C++"
  cppdialect "C++20"
  staticruntime "Off"

  pchheader "Editorpch.h"
  pchsource "src/core/Editorpch.cpp"

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
    "src/core",
    "%{IncludeDir.glad}",
  }

  uses { "Seri" }

  postbuildcommands {
    'xcopy /Q /E /Y /I /D "' .. path.translate(_MAIN_SCRIPT_DIR .. "/editor/assets") .. '" "%{cfg.targetdir}/assets"',
  }

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
    editandcontinue "Off"
    libdirs {
      "%{LibDir.vcpkg_debug}",
    }
    links {
      "%{Lib.assimp_debug}",
      "freetyped",
      "imguid",
      "yaml-cppd",
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
