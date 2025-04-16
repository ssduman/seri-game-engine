project "glad"
  kind "StaticLib"
  language "C"
  staticruntime "Off"

  targetdir("bin/" .. outputdir .. "/%{prj.name}")
  objdir("bin/int/" .. outputdir .. "/%{prj.name}")

  files {
    "include/glad/gl.h",
    "include/KHR/khrplatform.h",
    "src/gl.c"
  }

  includedirs {
    "include"
  }

  filter "system:windows"
    systemversion "latest"

  filter "configurations:Debug"
    runtime "Debug"
    symbols "On"

  filter "configurations:Release"
    runtime "Release"
    optimize "On"
