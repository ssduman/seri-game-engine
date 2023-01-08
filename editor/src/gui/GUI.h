#pragma once

#include <core/Seri.h>

#include "app/Factory.h"
#include "camera/Camera.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <memory>

class GUI : public Object {
public:
    GUI(
        std::shared_ptr<Camera> camera,
        std::shared_ptr<IScene> scene,
        std::shared_ptr<State> state)
        : _windowManager(WindowManagerFactory::instance()), _camera(camera), _scene(scene), _state{ state } {
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

        ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(_windowManager->getWindow()), true);
        ImGui_ImplOpenGL3_Init(glsl_version);
    }

    void update() override {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        showDemoWindow();

        showMainMenuBar();

        showSceneWindow();

        showEntityWindow();
    }

    void render() override {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
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
        _io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        _io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        _io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        _io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        _io->FontGlobalScale = 1.5;
        _io->ConfigWindowsMoveFromTitleBarOnly = true;
        //_io->Fonts->AddFontFromFileTTF(font_filename, 14.0f);
    }

    void setStyle() {
        ImGui::StyleColorsDark();
        _style = &ImGui::GetStyle();

        _style->GrabRounding = 6.0f;
        _style->ChildRounding = 6.0f;
        _style->FrameRounding = 6.0f;
        _style->PopupRounding = 6.0f;
        _style->WindowRounding = 6.0f;
        _style->ScrollbarRounding = 6.0f;

        _style->FrameBorderSize = 1.0f;
        _style->WindowTitleAlign = ImVec2(0.5f, 0.50f);
        _style->WindowMenuButtonPosition = ImGuiDir_Right;

        _style->Colors[ImGuiCol_WindowBg].w = 1.0f;
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

    void showSceneWindow() {
        static bool show_scene_window = true;
        if (show_scene_window) {
            if (!ImGui::Begin("Scene", &show_scene_window, _windowFlags)) {
                ImGui::End();
                return;
            }

            ImGui::SetNextItemOpen(true, ImGuiCond_Once);
            if (ImGui::TreeNode(_scene->getName().c_str())) {
                showSceneWindowImpl(_scene);
                ImGui::TreePop();
            }

            ImGui::End();
        }
    }

    void showSceneWindowImpl(std::shared_ptr<IScene> scenes) {
        static ImGuiTreeNodeFlags treeBaseFlags =
            ImGuiTreeNodeFlags_OpenOnArrow |
            ImGuiTreeNodeFlags_SpanAvailWidth |
            ImGuiTreeNodeFlags_OpenOnDoubleClick;

        static int nodeClickedId = -1;

        for (auto& scene : scenes->getChildren()) {
            // flags
            ImGuiTreeNodeFlags treeNodeFlags = treeBaseFlags;
            if (scene->isLeaf()) {
                treeNodeFlags |= ImGuiTreeNodeFlags_Leaf;
            }
            if (nodeClickedId == scene->getId()) {
                registerScene(scene);
                treeNodeFlags |= ImGuiTreeNodeFlags_Selected;
            }

            if (ImGui::TreeNodeEx((void*)(intptr_t)scene->getId(), treeNodeFlags, scene->getName().c_str())) {
                // context menu
                if (ImGui::BeginPopupContextItem()) {
                    if (ImGui::BeginMenu("Add ...")) {
                        SceneBuilder builder;

                        if (ImGui::MenuItem("Point")) {
                            scene->add(std::move(builder.setName("Point").setObject(Factory::CreateEntity(_camera, EntityType::point)).build()));
                        }

                        if (ImGui::MenuItem("Line")) {
                            scene->add(std::move(builder.setName("Line").setObject(Factory::CreateEntity(_camera, EntityType::line)).build()));
                        }

                        if (ImGui::MenuItem("Triangle")) {
                            scene->add(std::move(builder.setName("Triangle").setObject(Factory::CreateEntity(_camera, EntityType::triangle)).build()));
                        }

                        if (ImGui::MenuItem("Rectangle")) {
                            scene->add(std::move(builder.setName("Rectangle").setObject(Factory::CreateEntity(_camera, EntityType::rectangle)).build()));
                        }

                        if (ImGui::MenuItem("Circle")) {
                            scene->add(std::move(builder.setName("Circle").setObject(Factory::CreateEntity(_camera, EntityType::circle)).build()));
                        }

                        if (ImGui::MenuItem("Cube")) {
                            scene->add(std::move(builder.setName("Cube").setObject(Factory::CreateEntity(_camera, EntityType::cube)).build()));
                        }

                        if (ImGui::MenuItem("Polygon")) {
                            scene->add(std::move(builder.setName("Polygon").setObject(Factory::CreateEntity(_camera, EntityType::polygon)).build()));
                        }

                        ImGui::EndMenu();
                    }

                    if (ImGui::Button("Delete")) {
                        auto parentWeak = scene->getParent();
                        if (auto parent = parentWeak.lock()) {
                            parent->remove(scene);
                        }
                    }

                    ImGui::EndPopup();
                }

                // selection
                if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
                    nodeClickedId = scene->getId();
                }

                showSceneWindowImpl(scene);

                ImGui::TreePop();
            }
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
        /*
        if (ImGui::Button("Create point")) {
            _layers.addLayer(Factory::CreateEntity(_camera, EntityType::point));
        }
        if (ImGui::Button("Create line")) {
            _layers.addLayer(Factory::CreateEntity(_camera, EntityType::line));
        }
        if (ImGui::Button("Create triangle")) {
            _layers.addLayer(Factory::CreateEntity(_camera, EntityType::triangle));
        }
        if (ImGui::Button("Create rectangle")) {
            _layers.addLayer(Factory::CreateEntity(_camera, EntityType::rectangle));
        }
        if (ImGui::Button("Create circle")) {
            _layers.addLayer(Factory::CreateEntity(_camera, EntityType::circle));
        }
        if (ImGui::Button("Create cube")) {
            _layers.addLayer(Factory::CreateEntity(_camera, EntityType::cube));
        }
        if (ImGui::Button("Create polygon")) {
            _layers.addLayer(Factory::CreateEntity(_camera, EntityType::polygon));
        }
        if (ImGui::Button("Delete entity")) {
            _currentEntity = nullptr;
            _layers.deleteLayer();
        }
        */

        ImGui::Separator();
    }

    void showEntityTransformationOptions() {
        if (_currentEntity) {
            _currentEntity->getShader().use();

            ImGui::SliderFloat3("position", &_currentEntity->getTransform()._position[0], -1.0f, 1.0f, "%.4f");
            ImGui::SliderFloat3("rotation", &_currentEntity->getTransform()._rotation[0], -180.0f, 180.0f, "%.4f");
            ImGui::SliderFloat3("scale", &_currentEntity->getTransform()._scale[0], 0.0f, 100.0f, "%.4f");
            _currentEntity->getShaderManager().setModel(_currentEntity->getTransform().apply());

            ImGui::Separator();

            ImGui::ColorEdit4("color", &_currentEntity->getColor().r);
            _currentEntity->getShaderManager().setColor(_currentEntity->getColor().getColorRGBA());

            ImGui::Separator();

            auto& sceneName = _currentScene->getName();
            ImGui::InputText("name", (char*)sceneName.c_str(), sceneName.size() + 1);

            ImGui::Separator();

            _currentEntity->getShader().disuse();
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
            }
            else {
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

    void registerScene(std::shared_ptr<IScene> scene) {
        _currentScene = scene;
        registerEntity(scene->getObject());
    }

    void registerEntity(std::shared_ptr<Object> entity) {
        _currentEntity = std::dynamic_pointer_cast<Entity>(entity);
    }

    std::shared_ptr<IWindowManager> _windowManager;
    std::shared_ptr<Camera> _camera;
    std::shared_ptr<IScene> _scene;
    std::shared_ptr<State> _state;
    std::shared_ptr<IScene> _currentScene;
    std::shared_ptr<Entity> _currentEntity;

    ImGuiIO* _io{ nullptr };
    ImGuiStyle* _style{ nullptr };
    ImGuiWindowFlags _windowFlags{ 0 };

    const char* glsl_version{ "#version 130" };
    const char* font_filename{ "editor-assets/fonts/En Bloc.ttf" };

};
