#pragma once

#include "seri/scene/Scene.h"
#include "seri/event/EventData.h"

#include <stdexcept>
#include <functional>

struct EventDispatcher {
    void operator()(const std::shared_ptr<IScene>& rootScene, IEventData& data) {
        LOGGER(info, "event: " << data.toString());
        switch (data.eventType) {
            case EventType::key:
            {
                auto& d = getData<KeyEventData&>(data);
                rootScene->visit(makeSceneVisitor(
                    [&d](std::shared_ptr<IScene>& scene) {
                        if (auto& object = scene->getObject()) {
                            object->onKeyEvent(d);
                        }
                    })
                );
                break;
            }
            case EventType::character:
            {
                auto d = getData<CharacterEventData&>(data);
                rootScene->visit(makeSceneVisitor(
                    [&d](std::shared_ptr<IScene>& scene) {
                        if (auto& object = scene->getObject()) {
                            object->onCharacterEvent(d);
                        }
                    })
                );
                break;
            }
            case EventType::character_mods:
            {
                auto& d = getData<CharacterModsEventData&>(data);
                rootScene->visit(makeSceneVisitor(
                    [&d](std::shared_ptr<IScene>& scene) {
                        if (auto& object = scene->getObject()) {
                            object->onCharacterModsEvent(d);
                        }
                    })
                );
                break;
            }
            case EventType::mouse_enter:
            {
                auto& d = getData<MouseEnterEventData&>(data);
                rootScene->visit(makeSceneVisitor(
                    [&d](std::shared_ptr<IScene>& scene) {
                        if (auto& object = scene->getObject()) {
                            object->onMouseEnterEvent(d);
                        }
                    })
                );
                break;
            }
            case EventType::mouse_button:
            {
                auto& d = getData<MouseButtonEventData&>(data);
                rootScene->visit(makeSceneVisitor(
                    [&d](std::shared_ptr<IScene>& scene) {
                        if (auto& object = scene->getObject()) {
                            object->onMouseButtonEvent(d);
                        }
                    })
                );
                break;
            }
            case EventType::mouse_scroll:
            {
                auto& d = getData<MouseScrollEventData&>(data);
                rootScene->visit(makeSceneVisitor(
                    [&d](std::shared_ptr<IScene>& scene) {
                        if (auto& object = scene->getObject()) {
                            object->onMouseScrollEvent(d);
                        }
                    })
                );
                break;
            }
            case EventType::mouse_position:
            {
                auto& d = getData<MousePositionEventData&>(data);
                rootScene->visit(makeSceneVisitor(
                    [&d](std::shared_ptr<IScene>& scene) {
                        if (auto& object = scene->getObject()) {
                            object->onMousePositionEvent(d);
                        }
                    })
                );
                break;
            }
            case EventType::window_drop:
            {
                auto& d = getData<WindowDropEventData&>(data);
                rootScene->visit(makeSceneVisitor(
                    [&d](std::shared_ptr<IScene>& scene) {
                        if (auto& object = scene->getObject()) {
                            object->onWindowDropEvent(d);
                        }
                    })
                );
                break;
            }
            case EventType::window_close:
            {
                auto& d = getData<WindowCloseEventData&>(data);
                rootScene->visit(makeSceneVisitor(
                    [&d](std::shared_ptr<IScene>& scene) {
                        if (auto& object = scene->getObject()) {
                            object->onWindowCloseEvent(d);
                        }
                    })
                );
                break;
            }
            case EventType::window_resize:
            {
                auto& d = getData<WindowResizeEventData&>(data);
                rootScene->visit(makeSceneVisitor(
                    [&d](std::shared_ptr<IScene>& scene) {
                        if (auto& object = scene->getObject()) {
                            object->onWindowResizeEvent(d);
                        }
                    })
                );
                break;
            }
            case EventType::tick:
                break;
            case EventType::user:
            {
                rootScene->visit(makeSceneVisitor(
                    [&data](std::shared_ptr<IScene>& scene) {
                        if (auto& object = scene->getObject()) {
                            object->onUserEvent(data);
                        }
                    })
                );
                break;
            }
            case EventType::unknown:
                break;
            default:
                break;
        }
    }

private:
    template<typename T>
    T& getData(IEventData& data) {
        return dynamic_cast<T&>(data);
    }

};
