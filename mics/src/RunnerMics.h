#pragma once

#include <core/Seri.h>
#include <model/Model.h>

#include "Camera.h"
#include "Control.h"
#include "Fractal.h"
#include "PerlinNoise.h"

#include <memory>
#include <stdexcept>

class RunnerMics : public IRunner {
public:
    RunnerMics() = default;

    ~RunnerMics() override = default;

    void operator()() {
        WindowProperties windowProperties{ /*title*/ "Seri Game Engine - Mics", /*fullscreen*/ false, /*w*/ 800, /*h*/ 800 };
        auto windowManager = std::make_shared<WindowManager>(std::move(windowProperties));
        if (!windowManager->init()) {
            throw std::runtime_error("could not create window manager");
        }
        windowManager->disableCursor();
        windowManager->setPointSize(2.0f);

        auto state = std::make_shared<State>();
        state->gameState() = GameState::game;

        CameraProperties cameraProperties;
        cameraProperties.aspect = windowManager->getAspect();
        cameraProperties.position = glm::vec3{ 0.0f, 0.0f, -6.0f };
        auto camera = std::make_shared<Camera>(std::move(cameraProperties), state);
        camera->init();

        Control control{ windowManager, state, camera };
        control.init();

        SceneManager sceneManager;
        auto scene = std::make_shared<Scene>();
        scene->name = "main";
        sceneManager.addScene(std::move(scene));

        if (_showModel) {
            auto model = std::make_shared<Model>(camera);
            model->getShader().init("mics-assets/shaders/entity_vs.shader", "mics-assets/shaders/entity_fs.shader");
            model->init();

            if (_loadBackpackModel) {
                model->load("mics-assets/models/backpack/backpack.obj");
            }
            else if (_loadSpiderModel) {
                model->load("mics-assets/models/spider.obj");
                model->getTransform()._scale = glm::vec3{ 0.05f, 0.05f, 0.05f };
                model->getShaderManager().setModel(model->getTransform().apply());
                model->getShaderManager().setColor({ 0.45f, 0.45f, 0.45f, 1.0f });
            }

            auto scene = std::make_shared<Scene>();
            scene->name = "model";
            scene->object = std::move(model);
            sceneManager.addChild(sceneManager.getRoot(), std::move(scene));
        }

        if (_showFractal) {
            auto fractal = std::make_shared<Fractal>(camera);
            fractal->getShader().init("mics-assets/shaders/entity_vs.shader", "mics-assets/shaders/entity_fs.shader");
            fractal->init();
            fractal->fern();
            //fractal->tree();

            auto scene = std::make_shared<Scene>();
            scene->name = "fractal";
            scene->object = std::move(fractal);
            sceneManager.addChild(sceneManager.getRoot(), std::move(scene));
        }

        if (_showPerlinNoise) {
            auto perlinNoise = std::make_shared<PerlinNoise>(camera);
            perlinNoise->getShader().init("mics-assets/shaders/entity_vs.shader", "mics-assets/shaders/entity_fs.shader");
            perlinNoise->init();
            perlinNoise->generate();

            auto scene = std::make_shared<Scene>();
            scene->name = "perlin";
            scene->object = std::move(perlinNoise);
            sceneManager.addChild(sceneManager.getRoot(), std::move(scene));
        }

        EventManager eventManager;
        eventManager.addInputEventListener(
            [](KeyCode keycode, KeyAction keyAction) {
                LOGGER(info, "input event: " << toString(keycode) << ", action: " << toString(keyAction));
            }
        );
        eventManager.addInputEventListener(
            [](KeyCode keycode, KeyAction keyAction) {
                LOGGER(info, "other input event: " << toString(keycode) << ", action: " << toString(keyAction));
            }
        );
        eventManager.addWindowEventListener(
            []() {
                LOGGER(info, "other window event");
            }
        );
        eventManager.addMouseButtonEventListener(
            [](MouseButtonCode mouseButtonCode, MouseButtonAction mouseButtonAction) {
                LOGGER(info, "other mouse button event: " << toString(mouseButtonCode) << ", action : " << toString(mouseButtonAction));
            }
        );
        eventManager.triggerInputEvent(KeyCode::up, KeyAction::repeat);
        eventManager.triggerInputEvent(KeyCode::caps_lock, KeyAction::press);
        eventManager.triggerMouseButtonEvent(MouseButtonCode::button_left, MouseButtonAction::press);
        eventManager.triggerWindowEvent();

        LOGGER(info, "starting mics loop");

        while (!windowManager->windowShouldClose()) {
            windowManager->clear();
            windowManager->clearColor();

            control.processInput(windowManager->updateDeltaTime());

            for (auto& scene : sceneManager.getRoot()->children) {
                if (scene->object) {
                    scene->object->display();
                }
            }

            windowManager->pollEvents();
            windowManager->swapBuffers();
        }

        LOGGER(info, "mics loop stopped");
    }

private:
    bool _showModel{ true };
    bool _showFractal{ true };
    bool _showPerlinNoise{ true };

    bool _loadSpiderModel{ false };
    bool _loadBackpackModel{ true };

};
