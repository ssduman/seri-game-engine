@echo off

:: git
call git --git-dir seri/vendor/imgui/.git switch docking
call git --git-dir seri/vendor/glfw/.git switch --detach 3.4
call git --git-dir seri/vendor/glm/.git switch --detach 1.0.1
call git --git-dir seri/vendor/stb/.git switch --detach master
call git --git-dir seri/vendor/assimp/.git switch --detach v5.4.3
call git --git-dir seri/vendor/freetype/.git switch --detach VER-2-13-3
call git --git-dir seri/vendor/sdl/.git switch --detach release-3.2.20
call git --git-dir seri/vendor/entt/.git switch --detach v3.16.0
call git --git-dir seri/vendor/yaml-cpp/.git switch --detach 0.8.0
call git --git-dir seri/vendor/fmt/.git switch --detach 12.1.0
call git --git-dir seri/vendor/efsw/.git switch --detach master
call git --git-dir seri/vendor/lua/.git switch --detach v5.4.8
call git --git-dir seri/vendor/sol2/.git switch --detach master
call git --git-dir seri/vendor/imguizmo/.git switch --detach master

:: copy premake5.lua
echo f | xcopy /f /y "script/vendor/glad/premake5.lua" "seri/vendor/glad/premake5.lua"
echo f | xcopy /f /y "script/vendor/glfw/premake5.lua" "seri/vendor/glfw/premake5.lua"
echo f | xcopy /f /y "script/vendor/imgui/premake5.lua" "seri/vendor/imgui/premake5.lua"
echo f | xcopy /f /y "script/vendor/assimp/premake5.lua" "seri/vendor/assimp/premake5.lua"
echo f | xcopy /f /y "script/vendor/freetype/premake5.lua" "seri/vendor/freetype/premake5.lua"
echo f | xcopy /f /y "script/vendor/sdl/premake5.lua" "seri/vendor/sdl/premake5.lua"
echo f | xcopy /f /y "script/vendor/fmt/premake5.lua" "seri/vendor/fmt/premake5.lua"
echo f | xcopy /f /y "script/vendor/efsw/premake5_project.lua" "seri/vendor/efsw/premake5_project.lua"
echo f | xcopy /f /y "script/vendor/lua/premake5.lua" "seri/vendor/lua/premake5.lua"
echo f | xcopy /f /y "script/vendor/imguizmo/premake5.lua" "seri/vendor/imguizmo/premake5.lua"

:: copy aux files
echo f | xcopy /f /y "script/vendor/stb/stb_image.cpp" "seri/vendor/stb/stb_image.cpp"
echo f | xcopy /f /y "script/vendor/assimp/zconf.h" "seri/vendor/assimp/contrib/zlib/zconf.h"
echo f | xcopy /f /y "script/vendor/assimp/config.h" "seri/vendor/assimp/include/assimp/config.h"
echo f | xcopy /f /y "script/vendor/assimp/revision.h" "seri/vendor/assimp/code/assimp/revision.h"

:: premake
call script\premake5\premake5.exe vs2022

pause
