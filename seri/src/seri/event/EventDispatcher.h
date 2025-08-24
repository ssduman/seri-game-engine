#pragma once

#include "seri/scene/Scene.h"
#include "seri/event/EventData.h"

#include <stdexcept>
#include <functional>

namespace seri::event
{
	struct EventDispatcher
	{
		void operator()(const std::shared_ptr<IScene>& rootScene, IEventData& data)
		{
			//LOGGER(info, "event: " << data.ToString());

			switch (data.eventType)
			{
				case EventType::key:
					{
						auto& d = CastEventData<KeyEventData&>(data);
						rootScene->Visit(MakeSceneVisitor(
							[&d](std::shared_ptr<IScene>& scene)
							{
								if (auto& object = scene->GetUnderlyingObject())
								{
									object->OnKeyEvent(d);
								}
							})
						);
						break;
					}
				case EventType::character:
					{
						auto d = CastEventData<CharacterEventData&>(data);
						rootScene->Visit(MakeSceneVisitor(
							[&d](std::shared_ptr<IScene>& scene)
							{
								if (auto& object = scene->GetUnderlyingObject())
								{
									object->OnCharacterEvent(d);
								}
							})
						);
						break;
					}
				case EventType::character_mods:
					{
						auto& d = CastEventData<CharacterModsEventData&>(data);
						rootScene->Visit(MakeSceneVisitor(
							[&d](std::shared_ptr<IScene>& scene)
							{
								if (auto& object = scene->GetUnderlyingObject())
								{
									object->OnCharacterModsEvent(d);
								}
							})
						);
						break;
					}
				case EventType::mouse_enter:
					{
						auto& d = CastEventData<MouseEnterEventData&>(data);
						rootScene->Visit(MakeSceneVisitor(
							[&d](std::shared_ptr<IScene>& scene)
							{
								if (auto& object = scene->GetUnderlyingObject())
								{
									object->OnMouseEnterEvent(d);
								}
							})
						);
						break;
					}
				case EventType::mouse_button:
					{
						auto& d = CastEventData<MouseButtonEventData&>(data);
						rootScene->Visit(MakeSceneVisitor(
							[&d](std::shared_ptr<IScene>& scene)
							{
								if (auto& object = scene->GetUnderlyingObject())
								{
									object->OnMouseButtonEvent(d);
								}
							})
						);
						break;
					}
				case EventType::mouse_scroll:
					{
						auto& d = CastEventData<MouseScrollEventData&>(data);
						rootScene->Visit(MakeSceneVisitor(
							[&d](std::shared_ptr<IScene>& scene)
							{
								if (auto& object = scene->GetUnderlyingObject())
								{
									object->OnMouseScrollEvent(d);
								}
							})
						);
						break;
					}
				case EventType::mouse_position:
					{
						auto& d = CastEventData<MousePositionEventData&>(data);
						rootScene->Visit(MakeSceneVisitor(
							[&d](std::shared_ptr<IScene>& scene)
							{
								if (auto& object = scene->GetUnderlyingObject())
								{
									object->OnMousePositionEvent(d);
								}
							})
						);
						break;
					}
				case EventType::window_drop:
					{
						auto& d = CastEventData<WindowDropEventData&>(data);
						rootScene->Visit(MakeSceneVisitor(
							[&d](std::shared_ptr<IScene>& scene)
							{
								if (auto& object = scene->GetUnderlyingObject())
								{
									object->OnWindowDropEvent(d);
								}
							})
						);
						break;
					}
				case EventType::window_close:
					{
						auto& d = CastEventData<WindowCloseEventData&>(data);
						rootScene->Visit(MakeSceneVisitor(
							[&d](std::shared_ptr<IScene>& scene)
							{
								if (auto& object = scene->GetUnderlyingObject())
								{
									object->OnWindowCloseEvent(d);
								}
							})
						);
						break;
					}
				case EventType::window_resize:
					{
						auto& d = CastEventData<WindowResizeEventData&>(data);
						rootScene->Visit(MakeSceneVisitor(
							[&d](std::shared_ptr<IScene>& scene)
							{
								if (auto& object = scene->GetUnderlyingObject())
								{
									object->OnWindowResizeEvent(d);
								}
							})
						);
						break;
					}
				case EventType::tick:
					break;
				case EventType::user:
					{
						rootScene->Visit(MakeSceneVisitor(
							[&data](std::shared_ptr<IScene>& scene)
							{
								if (auto& object = scene->GetUnderlyingObject())
								{
									object->OnUserEvent(data);
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

		template<typename T>
		static T& CastEventData(IEventData& data)
		{
			return dynamic_cast<T&>(data);
		}

	private:

	};
}
