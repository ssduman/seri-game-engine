project "Mics"
  kind "ConsoleApp"
  language "C++"
  cppdialect "C++17"
  staticruntime "off"

  targetdir("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
  objdir("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

  files {
    "src/**.h",
    "src/**.cpp",
  }

  defines {}

  linkoptions {
    "-IGNORE:4098",
  }

  includedirs {
    "src",
    "%{wks.location}/engine/src",
    "%{wks.location}/engine/vendor",
    "%{IncludeDir.glad}",
    "%{IncludeDir.GLFW}",
    "%{IncludeDir.glm}",
    "%{IncludeDir.stb}",
    "%{IncludeDir.ImGui}",
    "%{IncludeDir.assimp}",
  }

  links {
    "Engine",
  }

  libdirs {
    "%{wks.location}/engine/vendor/assimp/bin/Debug-x86_64/assimp",
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
