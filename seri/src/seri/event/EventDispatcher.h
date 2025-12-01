#pragma once

#include "seri/scene/Scene.h"
#include "seri/event/EventData.h"

#include <stdexcept>
#include <functional>

namespace seri::event
{
	struct EventDispatcher
	{
		void operator()(const std::shared_ptr<IScene>& rootScene, const IEventData& data)
		{
			//LOGGER(info, "event: " << data.ToString());

			switch (data.eventType)
			{
				case EventType::key:
					{
						auto& d = CastEventData<KeyEventData&>(data);
						auto visitor = MakeSceneVisitor(
							[&d](std::shared_ptr<IScene>& scene)
							{
								if (auto& object = scene->GetUnderlyingObject())
								{
									object->OnKeyEvent(d);
								}
							});
						rootScene->Visit(visitor);
						break;
					}
				case EventType::character:
					{
						auto d = CastEventData<CharacterEventData&>(data);
						auto visitor = MakeSceneVisitor(
							[&d](std::shared_ptr<IScene>& scene)
							{
								if (auto& object = scene->GetUnderlyingObject())
								{
									object->OnCharacterEvent(d);
								}
							});
						rootScene->Visit(visitor);
						break;
					}
				case EventType::character_mods:
					{
						auto& d = CastEventData<CharacterModsEventData&>(data);
						auto visitor = MakeSceneVisitor(
							[&d](std::shared_ptr<IScene>& scene)
							{
								if (auto& object = scene->GetUnderlyingObject())
								{
									object->OnCharacterModsEvent(d);
								}
							});
						rootScene->Visit(visitor);
						break;
					}
				case EventType::mouse_enter:
					{
						auto& d = CastEventData<MouseEnterEventData&>(data);
						auto visitor = MakeSceneVisitor(
							[&d](std::shared_ptr<IScene>& scene)
							{
								if (auto& object = scene->GetUnderlyingObject())
								{
									object->OnMouseEnterEvent(d);
								}
							});
						rootScene->Visit(visitor);
						break;
					}
				case EventType::mouse_button:
					{
						auto& d = CastEventData<MouseButtonEventData&>(data);
						auto visitor = MakeSceneVisitor(
							[&d](std::shared_ptr<IScene>& scene)
							{
								if (auto& object = scene->GetUnderlyingObject())
								{
									object->OnMouseButtonEvent(d);
								}
							});
						rootScene->Visit(visitor);
						break;
					}
				case EventType::mouse_scroll:
					{
						auto& d = CastEventData<MouseScrollEventData&>(data);
						auto visitor = MakeSceneVisitor(
							[&d](std::shared_ptr<IScene>& scene)
							{
								if (auto& object = scene->GetUnderlyingObject())
								{
									object->OnMouseScrollEvent(d);
								}
							});
						rootScene->Visit(visitor);
						break;
					}
				case EventType::mouse_position:
					{
						auto& d = CastEventData<MousePositionEventData&>(data);
						auto visitor = MakeSceneVisitor(
							[&d](std::shared_ptr<IScene>& scene)
							{
								if (auto& object = scene->GetUnderlyingObject())
								{
									object->OnMousePositionEvent(d);
								}
							});
						rootScene->Visit(visitor);
						break;
					}
				case EventType::window_drop:
					{
						auto& d = CastEventData<WindowDropEventData&>(data);
						auto visitor = MakeSceneVisitor(
							[&d](std::shared_ptr<IScene>& scene)
							{
								if (auto& object = scene->GetUnderlyingObject())
								{
									object->OnWindowDropEvent(d);
								}
							});
						rootScene->Visit(visitor);
						break;
					}
				case EventType::window_close:
					{
						auto& d = CastEventData<WindowCloseEventData&>(data);
						auto visitor = MakeSceneVisitor(
							[&d](std::shared_ptr<IScene>& scene)
							{
								if (auto& object = scene->GetUnderlyingObject())
								{
									object->OnWindowCloseEvent(d);
								}
							});
						rootScene->Visit(visitor);
						break;
					}
				case EventType::window_resize:
					{
						auto& d = CastEventData<WindowResizeEventData&>(data);
						auto visitor = MakeSceneVisitor(
							[&d](std::shared_ptr<IScene>& scene)
							{
								if (auto& object = scene->GetUnderlyingObject())
								{
									object->OnWindowResizeEvent(d);
								}
							});
						rootScene->Visit(visitor);
						break;
					}
				case EventType::tick:
					break;
				case EventType::user:
					{
						auto visitor = MakeSceneVisitor(
							[&data](std::shared_ptr<IScene>& scene)
							{
								if (auto& object = scene->GetUnderlyingObject())
								{
									object->OnUserEvent(data);
								}
							});
						rootScene->Visit(visitor);
						break;
					}
				case EventType::unknown:
					break;
				default:
					break;
			}
		}

		template<typename T>
		static T& CastEventData(const IEventData& data)
		{
			return dynamic_cast<T&>(const_cast<IEventData&>(data));
		}

	private:

	};
}
