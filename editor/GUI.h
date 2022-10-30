#pragma once

#include "../engine/Layer.h"
#include "../engine/State.h"
#include "../engine/Object.h"
#include "../engine/Logger.h"
#include "../engine/WindowManager.h"

#include "Camera.h"
#include "Factory.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

class GUI : public Object {
public:
    GUI(WindowManager* windowManager, Camera* camera, Layer* layers, State* state) : _windowManager(windowManager), _camera(camera), _layers(layers), _state{ state } {
        LOGGER(info, "gui init succeeded");
    }

    ~GUI() override {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        LOGGER(info, "gui delete succeeded");
    }

    void init() override {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        setIO();
        setStyle();
        setWindowFlags();

        ImGui_ImplGlfw_InitForOpenGL(_windowManager->getWindow(), true);
        ImGui_ImplOpenGL3_Init(glsl_version);
    }

    void update() override {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //showDemoWindow();

        showMainMenuBar();

        showEntityWindow();
    }

    void render() override {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void registerEntity(Entity* entity) {
        _currentEntity = entity;
    }

private:
    void helpMarker(const char* desc) {
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    void setIO() {
        _io = &ImGui::GetIO();
        _io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        _io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        _io->FontGlobalScale = 1.5;
        _io->ConfigWindowsMoveFromTitleBarOnly = true;
        //_io->Fonts->AddFontFromFileTTF(font_filename, 14.0f);
    }

    void setStyle() {
        ImGui::StyleColorsDark();
        _style = &ImGui::GetStyle();

        _style->WindowRounding = 6.0f;
        _style->ChildRounding = 6.0f;
        _style->FrameRounding = 6.0f;
        _style->PopupRounding = 6.0f;
        _style->ScrollbarRounding = 6.0f;
        _style->GrabRounding = 6.0f;

        _style->FrameBorderSize = 1.0f;
        _style->WindowTitleAlign = ImVec2(0.5f, 0.50f);
        _style->WindowMenuButtonPosition = ImGuiDir_Right;

        _style->Colors[ImGuiCol_TitleBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
        _style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
        _style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

        _style->ScaleAllSizes(1.5f);
    }

    void setWindowFlags() {
        _windowFlags = 0;
        _windowFlags |= ImGuiWindowFlags_MenuBar;
        //_windowFlags |= ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    }

    void showDemoWindow() {
        static bool show_demo_window = true;
        if (show_demo_window) {
            ImGui::ShowDemoWindow(&show_demo_window);
        }
    }

    void showMainMenuBar() {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                showMenuFile();
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit")) {
                if (ImGui::MenuItem("Undo", "CTRL+Z")) {
                }
                if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Cut", "CTRL+X")) {
                }
                if (ImGui::MenuItem("Copy", "CTRL+C")) {
                }
                if (ImGui::MenuItem("Paste", "CTRL+V")) {
                }
                ImGui::EndMenu();
            }

            buttonPlayCenter();

            textRight("Seri Game Engine");

            ImGui::EndMainMenuBar();
        }
    }

    void showMenuFile() {
        if (ImGui::MenuItem("New")) {
        }
        if (ImGui::MenuItem("Open", "Ctrl+O")) {
        }
        if (ImGui::MenuItem("Save", "Ctrl+S")) {
        }
        if (ImGui::MenuItem("Save As..")) {
        }
        if (ImGui::MenuItem("Quit", "Alt+F4")) {
            _windowManager->windowShouldClose();
        }
    }

    void showEntityWindow() {
        static bool show_entity_window = true;
        if (show_entity_window) {
            if (!ImGui::Begin("Maze", &show_entity_window, _windowFlags)) {
                ImGui::End();
                LOGGER(error, "gui begin failed");
                return;
            }

            showEntityWindowMenuBar();

            showEntityCreateButtons();

            showEntityTransformationOptions();

            showGUIOptions();

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            ImGui::End();
        }
    }

    void showEntityWindowMenuBar() {
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
    }

    void showEntityCreateButtons() {
        if (ImGui::Button("Create point")) {
            _layers->addLayer(Factory::CreateEntity(_camera, EntityType::point));
        }
        if (ImGui::Button("Create line")) {
            _layers->addLayer(Factory::CreateEntity(_camera, EntityType::line));
        }
        if (ImGui::Button("Create triangle")) {
            _layers->addLayer(Factory::CreateEntity(_camera, EntityType::triangle));
        }
        if (ImGui::Button("Create rectangle")) {
            _layers->addLayer(Factory::CreateEntity(_camera, EntityType::rectangle));
        }
        if (ImGui::Button("Create circle")) {
            _layers->addLayer(Factory::CreateEntity(_camera, EntityType::circle));
        }
        if (ImGui::Button("Create cube")) {
            _layers->addLayer(Factory::CreateEntity(_camera, EntityType::cube));
        }
        if (ImGui::Button("Delete entity")) {
            _currentEntity = nullptr;
            _layers->deleteLayer();
        }

        ImGui::Separator();
    }

    void showEntityTransformationOptions() {
        if (_currentEntity) {
            ImGui::SliderFloat3("position", &_currentEntity->getTransform()._position[0], -1.0f, 1.0f, "%.4f");
            ImGui::SliderFloat3("rotation", &_currentEntity->getTransform()._rotation[0], -180.0f, 180.0f, "%.4f");
            ImGui::SliderFloat3("scale", &_currentEntity->getTransform()._scale[0], 0.0f, 100.0f, "%.4f");
            _currentEntity->getShader().setMat4("u_model", _currentEntity->getTransform().apply());

            ImGui::Separator();

            ImGui::ColorEdit4("color", &_currentEntity->getColor().r);
            _currentEntity->getShader().setVec4("u_color", _currentEntity->getColor().getColorRGBA());

            ImGui::Separator();

            ImGui::SliderFloat("speed", &_camera->getCameraProperties().speed, 0.0f, 100.0f, "%.4f");

            ImGui::Separator();

            static char str0[128] = "Entity";
            ImGui::InputText("name", str0, IM_ARRAYSIZE(str0));

            ImGui::Separator();
        }
    }

    void showGUIOptions() {
        if (ImGui::Button("Enable cursor")) {
            _windowManager->enableCursor();
        }
        if (ImGui::Button("Disable cursor")) {
            _windowManager->disableCursor();
        }

        ImGui::Separator();

        ImGui::CheckboxFlags("io.ConfigFlags: NavEnableKeyboard", &_io->ConfigFlags, ImGuiConfigFlags_NavEnableKeyboard);
        ImGui::SameLine(); helpMarker("Enable keyboard controls.");

        ImGui::Separator();
    }

    void textCenter(const std::string& text) {
        auto windowWidth = ImGui::GetWindowSize().x;
        auto textWidth = ImGui::CalcTextSize(text.c_str()).x;

        ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
        ImGui::Button(text.c_str());
    }

    void buttonPlayCenter() {
        static std::string text = "Play";
        auto windowWidth = ImGui::GetWindowSize().x;
        auto textWidth = ImGui::CalcTextSize(text.c_str()).x;

        ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);

        ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor::HSV(0.3f, 0.6f, 0.6f)));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor::HSV(0.3f, 0.7f, 0.7f)));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor::HSV(0.3f, 0.8f, 0.8f)));
        if (ImGui::Button(text.c_str())) {
            if (text == "Play") {
                text = "Stop";
                _state->gameState() = GameState::game;
                //_io->MouseDrawCursor = true;
                _windowManager->disableCursor();
            } else {
                text = "Play";
                _state->gameState() = GameState::idle;
                _windowManager->enableCursor();
                //_io->MouseDrawCursor = false;
            }
            LOGGER(info, "game status changed to '" << to_string(_state->gameState()) << "'");
        }
        ImGui::PopStyleColor(3);
    }

    void textRight(const std::string& text) {
        auto windowWidth = ImGui::GetWindowSize().x;
        auto textWidth = ImGui::CalcTextSize(text.c_str()).x;

        ImGui::SetCursorPosX(windowWidth - textWidth - 5.0f);
        ImGui::Text(text.c_str());
    }

    WindowManager* _windowManager = nullptr;
    Camera* _camera = nullptr;
    Layer* _layers = nullptr;
    State* _state = nullptr;
    Entity* _currentEntity = nullptr;
    ImGuiIO* _io = nullptr;
    ImGuiStyle* _style = nullptr;
    ImGuiWindowFlags _windowFlags = 0;
    const char* glsl_version = "#version 130";
    const char* font_filename = "editor-assets/fonts/En Bloc.ttf";

};
