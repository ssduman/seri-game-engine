:: copy premake5.lua
echo f | xcopy /f /y "premake5/vendor/assimp/premake5.lua" "engine/vendor/assimp/premake5.lua"
echo f | xcopy /f /y "premake5/vendor/freetype/premake5.lua" "engine/vendor/freetype/premake5.lua"
echo f | xcopy /f /y "premake5/vendor/glad/premake5.lua" "engine/vendor/glad/premake5.lua"
echo f | xcopy /f /y "premake5/vendor/glfw/premake5.lua" "engine/vendor/glfw/premake5.lua"
echo f | xcopy /f /y "premake5/vendor/imgui/premake5.lua" "engine/vendor/imgui/premake5.lua"

:: copy aux files
echo f | xcopy /f /y "premake5/vendor/assimp/config.h" "engine/vendor/assimp/include/assimp/config.h"
echo f | xcopy /f /y "premake5/vendor/assimp/revision.h" "engine/vendor/assimp/code/Common/revision.h"
echo f | xcopy /f /y "premake5/vendor/assimp/zconf.h" "engine/vendor/assimp/contrib/zlib/zconf.h"
echo f | xcopy /f /y "premake5/vendor/stb/stb_image.cpp" "engine/vendor/stb/stb_image.cpp"

:: premake
call premake5\premake5.exe vs2022

pause
