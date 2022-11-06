cmake_minimum_required(VERSION 3.0.0)

project(maze VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)

add_compile_definitions(GLEW_STATIC)

link_directories(
    libraries/freetype/win64
    libraries/glew/lib/Release/x64
    libraries/glfw/lib-vc2022
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
    libraries/imgui
    libraries/glew/include
    libraries/glfw/include
)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/Texture.cpp
    libraries/imgui/imgui.cpp
    libraries/imgui/imgui_demo.cpp
    libraries/imgui/imgui_draw.cpp
    libraries/imgui/imgui_impl_glfw.cpp
    libraries/imgui/imgui_impl_opengl3.cpp
    libraries/imgui/imgui_tables.cpp
    libraries/imgui/imgui_widgets.cpp
)

file(COPY src/fonts DESTINATION ${CMAKE_BINARY_DIR})
file(COPY src/shaders DESTINATION ${CMAKE_BINARY_DIR})
file(COPY src/textures DESTINATION ${CMAKE_BINARY_DIR})
file(COPY libraries/freetype/win64/freetype.dll DESTINATION ${CMAKE_BINARY_DIR})
