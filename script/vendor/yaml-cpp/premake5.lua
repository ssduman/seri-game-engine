project "yaml-cpp"
  kind "StaticLib"
  language "C++"
  staticruntime "Off"

  targetdir("bin/" .. outputdir .. "/%{prj.name}")
  objdir("bin/int/" .. outputdir .. "/%{prj.name}")

  files {
    "src/**.cpp",
    "include/**.h",
  }

  includedirs { 
    "include"
  }

  defines {
    "YAML_CPP_STATIC_DEFINE"
  }

  -- removefiles { "src/**/tests/**.*", "src/**/parse/**.*" }

  filter "system:windows"
    systemversion "latest"

  filter "configurations:Debug"
    runtime "Debug"
    symbols "On"

  filter "configurations:Release"
    runtime "Release"
    optimize "On"
