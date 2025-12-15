project "lua"
  kind "StaticLib"
  language "C"
  staticruntime "Off"

  targetdir("bin/" .. outputdir .. "/%{prj.name}")
  objdir("bin/int/" .. outputdir .. "/%{prj.name}")

  files {
    "*.c",
    "*.h",
  }

  removefiles {
    "manual/*.c",
    "manual/*.h",
    "testes/*.h",
    "testes/*.c",
    "testes/*.lua",
    "onelua.c",
  }

  includedirs {
    "*.c",
    "*.h",
  }

  defines {
    "LUA_USE_APICHECK"
  }

  filter "system:windows"
    systemversion "latest"
    defines {
      "LUA_USE_WINDOWS"
    }

  filter "configurations:Debug"
    runtime "Debug"
    symbols "On"

  filter "configurations:Release"
    runtime "Release"
    optimize "On"