#pragma once

#include "Object.h"
#include "Window.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

class GUI : public Object {
public:
    GUI(Window window) : _window(window) {
    };

    ~GUI() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void init() override {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.Fonts->AddFontFromFileTTF(font_filename, 20.0f);

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(_window.getWindow(), true);
        ImGui_ImplOpenGL3_Init(glsl_version);
    }

    void update() override {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        if (show_demo_window) {
            ImGui::ShowDemoWindow(&show_demo_window);
        }
    }

    void render() override {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

private:
    Window _window;
    bool show_demo_window = true;
    const char* glsl_version = "#version 130";
    const char* font_filename = "fonts/DungeonFont.ttf";
};
