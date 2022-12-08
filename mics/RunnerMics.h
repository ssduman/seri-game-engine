#pragma once

#include "../engine/Model.h"
#include "../engine/IScene.h"
#include "../engine/IRunner.h"

#include "Camera.h"
#include "Control.h"
//#include "Fractal.h"
//#include "MicsShader.h"
#include "PerlinNoise.h"

class RunnerMics : public IRunner {
public:
    RunnerMics() = default;

    ~RunnerMics() override = default;

    void operator()() {
        WindowProperties windowProperties{ /*title*/ "Seri Game Engine - Mics", /*fullscreen*/ false, /*w*/ 800, /*h*/ 800 };
        std::shared_ptr<WindowManager> windowManager = std::make_shared<WindowManager>(windowProperties);
        if (!windowManager->init()) {
            LOGGER(error, "could not create window manager");
            return;
        }
        windowManager->disableCursor();
        windowManager->setPointSize(2.0f);

        std::shared_ptr<State> state = std::make_shared<State>();
        state->gameState() = GameState::game;

        CameraProperties cameraProperties;
        cameraProperties.aspect = windowManager->getAspect();
        cameraProperties.position = glm::vec3{ 0.0f, 0.0f, -6.0f };
        std::shared_ptr<Camera> camera = std::make_shared<Camera>(cameraProperties, state);
        camera->init();

        Layer layers;

        Control control{ windowManager, state, camera };
        control.init();

        IScene scene;
        scene.name = "main";

        if (_showModel) {
            std::shared_ptr<Model> model = std::make_shared<Model>(camera);
            model->getShader().init("mics-assets/shaders/entity_vs.shader", "mics-assets/shaders/entity_fs.shader");
            model->init();
            model->load("mics-assets/models/backpack/backpack.obj");
            //model->load("mics-assets/models/spider.obj");
            //model->getTransform()._scale = glm::vec3{ 0.05f, 0.05f, 0.05f };
            //model->getShader().setMat4("u_model", model->getTransform().apply());
            layers.addLayer(model);
        }

        //if (_showFractal) {
        //    std::shared_ptr<Fractal> fractal = std::make_shared<Fractal>(camera.get());
        //    fractal->BarnsleyFern();
        //    fractal->tree();
        //    layers.addLayer(fractal);
        //}

        if (_showPerlinNoise) {
            std::shared_ptr<PerlinNoise> perlinNoise = std::make_shared<PerlinNoise>(camera);
            perlinNoise->getShader().init("mics-assets/shaders/entity_vs.shader", "mics-assets/shaders/entity_fs.shader");
            perlinNoise->init();
            perlinNoise->generate();
            layers.addLayer(perlinNoise);
        }

        LOGGER(info, "starting mics loop");

        while (!windowManager->windowShouldClose()) {
            windowManager->clear();
            windowManager->clearColor();

            control.processInput(windowManager->updateDeltaTime());

            for (auto& entity : layers.getLayers()) {
                entity->display();
            }

            windowManager->pollEvents();
            windowManager->swapBuffers();
        }

        LOGGER(info, "mics loop stopped");
    }

private:
    bool _showModel = true;
    bool _showFractal = false;
    bool _showPerlinNoise = false;

};
