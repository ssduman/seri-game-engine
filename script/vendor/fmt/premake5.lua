project "fmt"
  kind "StaticLib"
  language "C++"
  staticruntime "Off"
  cppdialect "C++20"

  targetdir("bin/" .. outputdir .. "/%{prj.name}")
  objdir("bin/int/" .. outputdir .. "/%{prj.name}")

  files {
    "src/os.cc",
    "src/format.cc",
    "include/**.h",
  }

  includedirs { 
    "include"
  }

  defines {
    "FMT_UNICODE=0",
  }

  disablewarnings {
    "4244",
    "4251",
    "4275",
  }

  filter "system:windows"
    systemversion "latest"

  filter "configurations:Debug"
    runtime "Debug"
    symbols "On"

  filter "configurations:Release"
    runtime "Release"
    optimize "On"
