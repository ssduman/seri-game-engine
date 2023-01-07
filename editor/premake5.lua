project "Editor"
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
    "%{IncludeDir.freetype}",
  }

  links {
    "Engine",
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
