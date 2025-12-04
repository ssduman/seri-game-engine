project "sdl"
  kind "Makefile"
  language "C"
  staticruntime "Off"

  buildcommands {
    "if not exist build (cmake -S . -B build -A x64)",
    "cmake --build build --config %{cfg.buildcfg}"
  }

  rebuildcommands {
    "if exist build rmdir /S /Q build",
    "if not exist build mkdir build",
    "cmake -S . -B build -A x64",
    "cmake --build build --config %{cfg.buildcfg}"
  }

  cleancommands {
    "if exist build rmdir /S /Q build",
  }

  buildoutputs {
    "build/%{cfg.buildcfg}/SDL3.lib"
  }

  defines {
    "SDL_MAIN_HANDLED",
    "SDL_MAIN_USE_CALLBACKS 1"
  }

  disablewarnings {
  }

  linkoptions {
  }

  files {
  }

  includedirs {
    "include/SDL3/"
  }

  filter "system:windows"
    systemversion "latest"

    files {
    }

  filter "configurations:Debug"
    runtime "Debug"
    symbols "On"

  filter "configurations:Release"
    runtime "Release"
    optimize "On"
