cmake_minimum_required(VERSION 3.0.0)

project(maze VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)

add_compile_definitions(GLEW_STATIC)

link_directories(
    libraries/freetype/win64
    libraries/glew-2.1.0/lib/Release/x64
    libraries/glfw-3.3.7.bin.WIN64/lib-vc2022
)

link_libraries(
    opengl32
    freetype
    glew32s
    glfw3
)

include_directories(${PROJECT_NAME}
    src
    libraries/glm
    libraries/freetype
    libraries/stb_image
    libraries/imgui-1.88
    libraries/glew-2.1.0/include
    libraries/glfw-3.3.7.bin.WIN64/include
)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/Game.cpp
    src/Texture.cpp
    libraries/imgui-1.88/imgui.cpp
    libraries/imgui-1.88/imgui_demo.cpp
    libraries/imgui-1.88/imgui_draw.cpp
    libraries/imgui-1.88/imgui_impl_glfw.cpp
    libraries/imgui-1.88/imgui_impl_opengl3.cpp
    libraries/imgui-1.88/imgui_tables.cpp
    libraries/imgui-1.88/imgui_widgets.cpp
)

file(COPY src/fonts DESTINATION ${CMAKE_BINARY_DIR})
file(COPY src/shaders DESTINATION ${CMAKE_BINARY_DIR})
file(COPY src/textures DESTINATION ${CMAKE_BINARY_DIR})
file(COPY libraries/freetype/win64/freetype.dll DESTINATION ${CMAKE_BINARY_DIR})
