@echo off

:: git
call git --git-dir seri/vendor/imgui/.git switch docking
call git --git-dir seri/vendor/glfw/.git switch --detach 3.3.8
call git --git-dir seri/vendor/assimp/.git switch --detach v5.2.5
call git --git-dir seri/vendor/freetype/.git switch --detach VER-2-12-1

:: :: gitignore
:: echo * >> seri/vendor/stb/.gitignore
:: echo * >> seri/vendor/glm/.gitignore
:: echo * >> seri/vendor/glfw/.gitignore
:: echo * >> seri/vendor/imgui/.gitignore
:: echo * >> seri/vendor/assimp/.gitignore
:: echo * >> seri/vendor/freetype/.gitignore

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
echo f | xcopy /f /y "script/vendor/assimp/revision.h" "seri/vendor/assimp/code/Common/revision.h"

:: premake
call script\premake5.exe vs2022

pause
