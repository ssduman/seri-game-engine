#pragma once

#include "Object.h"
#include "Window.h"
#include "Factory.h"
#include "Layer.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

class GUI : public Object {
public:
    GUI(Window window, Layer* layers) : _window(window), _layers(layers) {}

    virtual ~GUI() {
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

        ImGuiWindowFlags window_flags = 0;
        window_flags |= ImGuiWindowFlags_MenuBar;

        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

        static bool no_open = true;
        if (!ImGui::Begin("Maze", &no_open, window_flags)) {
            ImGui::End();
            return;
        }

        ImGui::PushItemWidth(ImGui::GetFontSize() * -12);

        static bool show_app_main_menu_bar = false;
        static bool show_app_example_menu_bar = false;
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Menu")) {
                ImGui::MenuItem("Option 1", nullptr, &show_app_main_menu_bar);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Settings")) {
                ImGui::MenuItem("Setting 1", nullptr, &show_app_example_menu_bar);
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::Separator();

        ImGui::Text("Maze");
        if (ImGui::Button("Create point")) {
            _layers->addLayer(Factory::Create(EntityType::POINT));
        }
        if (ImGui::Button("Create line")) {
            _layers->addLayer(Factory::Create(EntityType::LINE));
        }
        if (ImGui::Button("Create triangle")) {
            _layers->addLayer(Factory::Create(EntityType::TRIANGLE));
        }
        if (ImGui::Button("Create rectangle")) {
            _layers->addLayer(Factory::Create(EntityType::RECTANGLE));
        }
        if (ImGui::Button("Create circle")) {
            _layers->addLayer(Factory::Create(EntityType::CIRCLE));
        }
        if (ImGui::Button("Delete entity")) {
            _layers->deleteLayer();
        }

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        ImGui::End();
    }

    void render() override {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

private:
    Window _window;
    Layer* _layers;
    bool show_demo_window = true;
    const char* glsl_version = "#version 130";
    const char* font_filename = "fonts/En Bloc.ttf";
};
