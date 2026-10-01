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

  includedirs {
    "src",
    "src/core",
    "%{IncludeDir.glad}",
  }

  uses { "Seri" }

  links {
    "efsw",
    "glm",
    "imguizmo",
    "Jolt",
    "lua",
    "nfd",
    "SDL3",
  }

  filter "system:windows"
    systemversion "latest"
    files {
      "resources/**.rc",
    }
    linkoptions {
      "-IGNORE:4098",
    }
    disablewarnings {
      "4244",
      "4267",
      "4312",
    }
    links {
      "opengl32.lib",
      "glfw3dll",
    }
    postbuildcommands {
      'if exist "%{cfg.targetdir}/assets" rmdir /S /Q "%{cfg.targetdir}/assets"',
      'xcopy /Q /E /Y /I "' .. path.translate(_MAIN_SCRIPT_DIR .. "/editor/assets") .. '" "%{cfg.targetdir}/assets"',
    }

  filter "system:linux"
    linkoptions {
      "-Wl,-rpath,'$$ORIGIN'",
    }
    links {
      "glfw",
    }
    postbuildcommands {
      'rm -rf "%{cfg.targetdir}/assets"',
      'cp -r "' .. _MAIN_SCRIPT_DIR .. '/editor/assets" "%{cfg.targetdir}/assets"',
    }

  filter { "configurations:Debug" }
    defines {
      "DEBUG",
      "SERI_OPENGL_DEBUG_CONTEXT=true",
    }
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
    defines {
      "NDEBUG",
      "SERI_OPENGL_DEBUG_CONTEXT=false",
    }
    runtime "Release"
    optimize "On"
    symbols "On"
    libdirs {
      "%{LibDir.vcpkg_release}",
    }
    links {
      "%{Lib.assimp_release}",
      "freetype",
      "imgui",
      "yaml-cpp",
    }
