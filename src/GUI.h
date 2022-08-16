#pragma once

#include "Object.h"
#include "WindowManager.h"
#include "Factory.h"
#include "Layer.h"
#include "Camera.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

class GUI : public Object {
public:
    GUI(WindowManager windowManager, Layer* layers) : _windowManager(windowManager), _layers(layers) {}

    virtual ~GUI() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void init() override {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        _io = &ImGui::GetIO();
        _io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        _io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        _io->Fonts->AddFontFromFileTTF(font_filename, 20.0f);

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(_windowManager.getWindow(), true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        _window_flags = 0;
        _window_flags |= ImGuiWindowFlags_MenuBar;
    }

    void update() override {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        static bool no_open = true;
        if (!ImGui::Begin("Maze", &no_open, _window_flags)) {
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
            _layers->addLayer(Factory::CreateEntity(_camera, EntityType::POINT));
        }
        if (ImGui::Button("Create line")) {
            _layers->addLayer(Factory::CreateEntity(_camera, EntityType::LINE));
        }
        if (ImGui::Button("Create triangle")) {
            _layers->addLayer(Factory::CreateEntity(_camera, EntityType::TRIANGLE));
        }
        if (ImGui::Button("Create rectangle")) {
            _layers->addLayer(Factory::CreateEntity(_camera, EntityType::RECTANGLE));
        }
        if (ImGui::Button("Create circle")) {
            _layers->addLayer(Factory::CreateEntity(_camera, EntityType::CIRCLE));
        }
        if (ImGui::Button("Create cube")) {
            _layers->addLayer(Factory::CreateEntity(_camera, EntityType::CUBE));
        }
        if (ImGui::Button("Delete entity")) {
            _currentEntity = nullptr;
            _layers->deleteLayer();
        }

        ImGui::Separator();

        if (ImGui::Button("Enable cursor")) {
            _windowManager.enableCursor();
        }
        if (ImGui::Button("Disable cursor")) {
            _windowManager.disableCursor();
        }

        ImGui::Separator();

        ImGui::CheckboxFlags("io.ConfigFlags: NavEnableKeyboard", &_io->ConfigFlags, ImGuiConfigFlags_NavEnableKeyboard);
        ImGui::SameLine(); HelpMarker("Enable keyboard controls.");

        ImGui::Separator();

        static char str0[128] = "Entity";
        ImGui::InputText("name", str0, IM_ARRAYSIZE(str0));

        ImGui::Separator();

        if (_currentEntity) {
            ImGui::SliderFloat3("position", &_currentEntity->getTransform()._position[0], -1.0f, 1.0f, "%.4f");
            ImGui::SliderFloat3("rotation", &_currentEntity->getTransform()._rotation[0], -180.0f, 180.0f, "%.4f");
            ImGui::SliderFloat3("scale", &_currentEntity->getTransform()._scale[0], 0.0f, 100.0f, "%.4f");
            ImGui::Separator();

            _currentEntity->getShader().setMat4("u_model", _currentEntity->getTransform().apply());

            ImGui::ColorEdit4("color", &_currentEntity->getColor()._color[0]);
            _currentEntity->getShader().setVec4("u_color", _currentEntity->getColor()._color);

            ImGui::SliderFloat("speed", &_camera->getSpeed(), 0.0f, 100.0f, "%.4f");

            ImGui::SliderFloat3("camera", &_camera->_cameraProperties.position[0], 0.0f, 100.0f, "%.4f");
            _currentEntity->getShader().setMat4("u_view", _camera->view());
        }

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        ImGui::End();
    }

    void render() override {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void registerEntity(Entity* entity) {
        _currentEntity = entity;
    }

    void registerCamera(Camera* camera) {
        _camera = camera;
    }

private:
    void HelpMarker(const char* desc) {
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    WindowManager _windowManager;
    Layer* _layers = nullptr;
    Camera* _camera = nullptr;
    Entity* _currentEntity = nullptr;
    ImGuiIO* _io = nullptr;
    ImGuiWindowFlags _window_flags = 0;
    bool show_demo_window = true;
    const char* glsl_version = "#version 130";
    const char* font_filename = "fonts/En Bloc.ttf";
};
