#pragma once

#include "seri/event/EventType.h"
#include "seri/input/KeyCode.h"

#include <vector>
#include <string>
#include <sstream>

namespace seri::event
{
	// interface
	struct IEventData
	{
		IEventData(EventType eventType_) : eventType(eventType_) {}

		virtual ~IEventData() {}

		virtual std::string toString() = 0;

		EventType eventType = EventType::unknown;
	};

	// input
	struct KeyEventData : public IEventData
	{
		KeyEventData(KeyCode key_, int scancode_, InputAction action_, std::vector<InputModifier> mods_)
			: IEventData(EventType::key), key(key_), scancode(scancode_), action(action_), mods(std::move(mods_))
		{}

		~KeyEventData() override = default;

		std::string toString() override
		{
			std::stringstream modss;
			for (auto mod : mods)
			{
				modss << "|" << ::seri::toString(mod) << "|";
			}

			std::stringstream ss;
			ss << ::seri::event::toString(eventType) << ": {"
				<< "'key': " << ::seri::toString(key) << ", "
				<< "'scancode': " << scancode << ", "
				<< "'action': " << ::seri::toString(action) << ", "
				<< "'mods': " << modss.str()
				<< "}";

			return ss.str();
		}

		KeyCode key;
		int scancode;
		InputAction action;
		std::vector<InputModifier> mods;
	};

	struct CharacterEventData : public IEventData
	{
		CharacterEventData(unsigned int codepoint_)
			: IEventData(EventType::character), codepoint(codepoint_)
		{}

		~CharacterEventData() override = default;

		std::string toString() override
		{
			std::stringstream ss;
			ss << ::seri::event::toString(eventType) << ": {"
				<< "'codepoint': " << codepoint
				<< "}";

			return ss.str();
		}

		unsigned int codepoint;
	};

	struct CharacterModsEventData : public IEventData
	{
		CharacterModsEventData(unsigned int codepoint_, std::vector<InputModifier> mods_)
			: IEventData(EventType::character_mods), codepoint(codepoint_), mods(std::move(mods_))
		{}

		~CharacterModsEventData() override = default;

		std::string toString() override
		{
			std::stringstream modss;
			for (auto mod : mods)
			{
				modss << "|" << ::seri::toString(mod) << "|";
			}

			std::stringstream ss;
			ss << ::seri::event::toString(eventType) << ": {"
				<< "'codepoint': " << codepoint << ", "
				<< "'mods': " << modss.str()
				<< "}";

			return ss.str();
		}

		unsigned int codepoint;
		std::vector<InputModifier> mods;
	};

	// mouse
	struct MouseEnterEventData : public IEventData
	{
		MouseEnterEventData(bool entered_)
			: IEventData(EventType::mouse_enter), entered(entered_)
		{}

		~MouseEnterEventData() override = default;

		std::string toString() override
		{
			std::stringstream ss;
			ss << ::seri::event::toString(eventType) << ": {"
				<< "'entered': " << (entered ? "true" : "false")
				<< "}";

			return ss.str();
		}

		bool entered;
	};

	struct MouseButtonEventData : public IEventData
	{
		MouseButtonEventData(MouseButtonCode button_, InputAction action_, InputModifier mods_)
			: IEventData(EventType::mouse_button), button(button_), action(action_), mods(mods_)
		{}

		~MouseButtonEventData() override = default;

		std::string toString() override
		{
			std::stringstream ss;
			ss << ::seri::event::toString(eventType) << ": {"
				<< "'button': " << ::seri::toString(button) << ", "
				<< "'action': " << ::seri::toString(action) << ", "
				<< "'mods': " << ::seri::toString(mods)
				<< "}";

			return ss.str();
		}

		MouseButtonCode button;
		InputAction action;
		InputModifier mods;
	};

	struct MouseScrollEventData : public IEventData
	{
		MouseScrollEventData(double xoffset_, double yoffset_)
			: IEventData(EventType::mouse_scroll), xoffset(xoffset_), yoffset(yoffset_)
		{}

		~MouseScrollEventData() override = default;

		std::string toString() override
		{
			std::stringstream ss;
			ss << ::seri::event::toString(eventType) << ": {"
				<< "'xoffset': " << xoffset << ", "
				<< "'yoffset': " << yoffset
				<< "}";

			return ss.str();
		}

		double xoffset;
		double yoffset;
	};

	struct MousePositionEventData : public IEventData
	{
		MousePositionEventData(double xpos_, double ypos_)
			: IEventData(EventType::mouse_position), xpos(xpos_), ypos(ypos_)
		{}

		~MousePositionEventData() override = default;

		std::string toString() override
		{
			std::stringstream ss;
			ss << ::seri::event::toString(eventType) << ": {"
				<< "'xpos': " << xpos << ", "
				<< "'ypos': " << ypos
				<< "}";

			return ss.str();
		}

		double xpos;
		double ypos;
	};

	// window
	struct WindowDropEventData : public IEventData
	{
		WindowDropEventData(std::vector<std::string> paths_)
			: IEventData(EventType::window_drop), paths(std::move(paths_))
		{}

		~WindowDropEventData() override = default;

		std::string toString() override
		{
			std::stringstream pathss;
			for (auto& path : paths)
			{
				pathss << "|" << path << "|";
			}

			std::stringstream ss;
			ss << ::seri::event::toString(eventType) << ": {"
				<< "'paths': " << pathss.str()
				<< "}";

			return ss.str();
		}

		std::vector<std::string> paths;
	};

	struct WindowCloseEventData : public IEventData
	{
		WindowCloseEventData()
			: IEventData(EventType::window_close)
		{}

		~WindowCloseEventData() override = default;

		std::string toString() override
		{
			std::stringstream ss;
			ss << ::seri::event::toString(eventType) << ": {"
				<< "'window_close'"
				<< "}";

			return ss.str();
		}
	};

	struct WindowResizeEventData : public IEventData
	{
		WindowResizeEventData(int width_, int height_)
			: IEventData(EventType::window_resize), width(width_), height(height_)
		{}

		~WindowResizeEventData() override = default;

		std::string toString() override
		{
			std::stringstream ss;
			ss << ::seri::event::toString(eventType) << ": {"
				<< "'width': " << width << ", "
				<< "'height': " << height
				<< "}";

			return ss.str();
		}

		double width;
		double height;
	};
}
