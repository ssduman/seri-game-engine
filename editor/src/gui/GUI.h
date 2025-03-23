#pragma once

#include <seri/core/Seri.h>

#include "app/Factory.h"
#include "camera/Camera.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <misc/freetype/imgui_freetype.h>

#include <memory>

struct FreeTypeTest {
    enum FontBuildMode { FontBuildMode_FreeType, FontBuildMode_Stb };

    FontBuildMode   BuildMode = FontBuildMode_FreeType;
    bool            WantRebuild = true;
    float           RasterizerMultiply = 1.0f;
    unsigned int    FreeTypeBuilderFlags = 0;

    // Call _BEFORE_ NewFrame()
    bool PreNewFrame() {
        if (!WantRebuild)
            return false;

        ImFontAtlas* atlas = ImGui::GetIO().Fonts;
        for (int n = 0; n < atlas->ConfigData.Size; n++)
            ((ImFontConfig*)&atlas->ConfigData[n])->RasterizerMultiply = RasterizerMultiply;

        // Allow for dynamic selection of the builder. 
        // In real code you are likely to just define IMGUI_ENABLE_FREETYPE and never assign to FontBuilderIO.
#ifdef IMGUI_ENABLE_FREETYPE
        if (BuildMode == FontBuildMode_FreeType) {
            atlas->FontBuilderIO = ImGuiFreeType::GetBuilderForFreeType();
            atlas->FontBuilderFlags = FreeTypeBuilderFlags;
        }
#endif
#ifdef IMGUI_ENABLE_STB_TRUETYPE
        if (BuildMode == FontBuildMode_Stb) {
            atlas->FontBuilderIO = ImFontAtlasGetBuilderForStbTruetype();
            atlas->FontBuilderFlags = 0;
        }
#endif
        atlas->Build();
        WantRebuild = false;
        return true;
    }

    // Call to draw UI
    void ShowFontsOptionsWindow() {
        ImFontAtlas* atlas = ImGui::GetIO().Fonts;

        ImGui::Begin("FreeType Options");
        ImGui::ShowFontSelector("Fonts");
        WantRebuild |= ImGui::RadioButton("FreeType", (int*)&BuildMode, FontBuildMode_FreeType);
        ImGui::SameLine();
        WantRebuild |= ImGui::RadioButton("Stb (Default)", (int*)&BuildMode, FontBuildMode_Stb);
        WantRebuild |= ImGui::DragInt("TexGlyphPadding", &atlas->TexGlyphPadding, 0.1f, 1, 16);
        WantRebuild |= ImGui::DragFloat("RasterizerMultiply", &RasterizerMultiply, 0.001f, 0.0f, 2.0f);
        ImGui::Separator();

        if (BuildMode == FontBuildMode_FreeType) {
#ifndef IMGUI_ENABLE_FREETYPE
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "Error: FreeType builder not compiled!");
#endif
            WantRebuild |= ImGui::CheckboxFlags("NoHinting", &FreeTypeBuilderFlags, ImGuiFreeTypeBuilderFlags_NoHinting);
            WantRebuild |= ImGui::CheckboxFlags("NoAutoHint", &FreeTypeBuilderFlags, ImGuiFreeTypeBuilderFlags_NoAutoHint);
            WantRebuild |= ImGui::CheckboxFlags("ForceAutoHint", &FreeTypeBuilderFlags, ImGuiFreeTypeBuilderFlags_ForceAutoHint);
            WantRebuild |= ImGui::CheckboxFlags("LightHinting", &FreeTypeBuilderFlags, ImGuiFreeTypeBuilderFlags_LightHinting);
            WantRebuild |= ImGui::CheckboxFlags("MonoHinting", &FreeTypeBuilderFlags, ImGuiFreeTypeBuilderFlags_MonoHinting);
            WantRebuild |= ImGui::CheckboxFlags("Bold", &FreeTypeBuilderFlags, ImGuiFreeTypeBuilderFlags_Bold);
            WantRebuild |= ImGui::CheckboxFlags("Oblique", &FreeTypeBuilderFlags, ImGuiFreeTypeBuilderFlags_Oblique);
            WantRebuild |= ImGui::CheckboxFlags("Monochrome", &FreeTypeBuilderFlags, ImGuiFreeTypeBuilderFlags_Monochrome);
        }

        if (BuildMode == FontBuildMode_Stb) {
#ifndef IMGUI_ENABLE_STB_TRUETYPE
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "Error: stb_truetype builder not compiled!");
#endif
        }
        ImGui::End();
    }
};


class GUI : public Object {
public:
    GUI(std::shared_ptr<Camera> camera, std::shared_ptr<IScene> scene) : _windowManager(WindowManagerFactory::instance()), _camera(camera), _scene(scene) {
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

        ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(_windowManager->getWindow()), true);
        ImGui_ImplOpenGL3_Init("#version 460");
    }

    void update() override {
        if (freetype_test.PreNewFrame()) {
            ImGui_ImplOpenGL3_DestroyDeviceObjects();
            ImGui_ImplOpenGL3_CreateDeviceObjects();
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        freetype_test.ShowFontsOptionsWindow();

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
        auto& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigWindowsMoveFromTitleBarOnly = true;
    }

    void setStyle() {
        ImGui::StyleColorsDark();
        auto& style = ImGui::GetStyle();

        style.GrabRounding = 6.0f;
        style.ChildRounding = 6.0f;
        style.FrameRounding = 6.0f;
        style.PopupRounding = 6.0f;
        style.WindowRounding = 6.0f;
        style.ScrollbarRounding = 6.0f;

        style.FrameBorderSize = 1.0f;
        style.WindowTitleAlign = ImVec2(0.5f, 0.50f);
        style.WindowMenuButtonPosition = ImGuiDir_Right;

        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
        style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    void showDemoWindow() {
        static bool show_demo_window = true;
        if (show_demo_window) {
            ImGui::ShowDemoWindow(&show_demo_window);
        }
    }

    void showMainMenuBar() {
        static bool show_menu_window = true;
        if (show_menu_window) {
            if (!ImGui::Begin("Seri", &show_menu_window)) {
                ImGui::End();
                return;
            }

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

            ImGui::End();
        }
    }

    void showSceneWindow() {
        static bool show_scene_window = true;
        if (show_scene_window) {
            auto windowFlags = 0;
            windowFlags |= ImGuiWindowFlags_MenuBar;
            if (!ImGui::Begin("Scene", &show_scene_window, windowFlags)) {
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
                            ImGui::EndPopup();
                            ImGui::TreePop();
                            break;
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
            _windowManager->setWindowShouldCloseToTrue();
        }
    }

    void showEntityWindow() {
        static bool show_entity_window = true;
        if (show_entity_window) {
            auto windowFlags = 0;
            windowFlags |= ImGuiWindowFlags_MenuBar;
            if (!ImGui::Begin("Components", &show_entity_window, windowFlags)) {
                ImGui::End();
                return;
            }

            showEntityWindowMenuBar();

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
                _windowManager->fireEvent(UserGameStateEventData{ GameState::game });
                _windowManager->disableCursor();
                //_io->MouseDrawCursor = true;
            }
            else {
                text = "Play";
                _windowManager->fireEvent(UserGameStateEventData{ GameState::idle });
                _windowManager->enableCursor();
                //_io->MouseDrawCursor = false;
            }
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

    FreeTypeTest freetype_test;

    std::shared_ptr<IWindowManager> _windowManager;
    std::shared_ptr<Camera> _camera;
    std::shared_ptr<IScene> _scene;
    std::shared_ptr<IScene> _currentScene;
    std::shared_ptr<Entity> _currentEntity;

};
