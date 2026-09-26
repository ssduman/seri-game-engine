project "tracy"
  kind "StaticLib"
  language "C++"
  cppdialect "C++20"
  staticruntime "Off"

  targetdir("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
  objdir("%{wks.location}/bin/int/" .. outputdir .. "/%{prj.name}")

  files {
    "TracyClient.cpp",
    "tracy/**.h",
    "tracy/**.hpp",
  }

  defines {
    "TRACY_ENABLE",
    "TRACY_ON_DEMAND",
    "TRACY_ONLY_LOCALHOST",
    "TRACY_NO_BROADCAST",
  }

  filter "system:windows"
    systemversion "latest"

  filter "configurations:Debug"
    runtime "Debug"
    symbols "On"
    editandcontinue "Off"

  filter "configurations:Release"
    runtime "Release"
    optimize "On"
    symbols "On"
