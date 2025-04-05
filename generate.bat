@echo off

:: git
call git --git-dir seri/vendor/imgui/.git switch docking
call git --git-dir seri/vendor/glfw/.git switch --detach 3.4
call git --git-dir seri/vendor/glm/.git switch --detach 1.0.1
call git --git-dir seri/vendor/stb/.git switch --detach master
call git --git-dir seri/vendor/assimp/.git switch --detach v5.4.3
call git --git-dir seri/vendor/freetype/.git switch --detach VER-2-13-3

:: copy premake5.lua
echo f | xcopy /f /y "script/vendor/glad/premake5.lua" "seri/vendor/glad/premake5.lua"
echo f | xcopy /f /y "script/vendor/glfw/premake5.lua" "seri/vendor/glfw/premake5.lua"
echo f | xcopy /f /y "script/vendor/imgui/premake5.lua" "seri/vendor/imgui/premake5.lua"
echo f | xcopy /f /y "script/vendor/assimp/premake5.lua" "seri/vendor/assimp/premake5.lua"
echo f | xcopy /f /y "script/vendor/freetype/premake5.lua" "seri/vendor/freetype/premake5.lua"

:: copy aux files
echo f | xcopy /f /y "script/vendor/stb/stb_image.cpp" "seri/vendor/stb/stb_image.cpp"
echo f | xcopy /f /y "script/vendor/assimp/zconf.h" "seri/vendor/assimp/contrib/zlib/zconf.h"
echo f | xcopy /f /y "script/vendor/assimp/config.h" "seri/vendor/assimp/include/assimp/config.h"
echo f | xcopy /f /y "script/vendor/assimp/revision.h" "seri/vendor/assimp/code/assimp/revision.h"

:: premake
call script\premake5\premake5.exe vs2022

pause
