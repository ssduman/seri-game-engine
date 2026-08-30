#pragma once

#include "seri/util/Util.h"
#include "seri/input/KeyCode.h"

#include <vector>

namespace seri
{
	class InputManager
	{
	public:
		InputManager(InputManager const&) = delete;

		void operator=(InputManager const&) = delete;

		static InputManager& GetInstance()
		{
			static InputManager instance;
			return instance;
		}

		static void Init()
		{
			GetInstance()._keys = std::vector<InputAction>(static_cast<int>(KeyCode::len), InputAction::noop);
			GetInstance()._mouse = std::vector<InputAction>(static_cast<int>(MouseButtonCode::len), InputAction::noop);
			GetInstance()._mousePressing = std::vector<bool>(static_cast<int>(MouseButtonCode::len), false);

			Reset();
		}

		static void Reset()
		{
			for (auto& var : GetInstance()._keys)
			{
				var = InputAction::noop;
			}

			for (auto& var : GetInstance()._mouse)
			{
				var = InputAction::noop;
			}

			GetInstance()._scrollDelta.x = 0.0f;
			GetInstance()._scrollDelta.y = 0.0f;
		}

		static void RegisterKey(KeyCode target, InputAction action)
		{
			GetInstance()._keys[static_cast<int>(target)] = action;
		}

		static void RegisterMouse(MouseButtonCode target, InputAction action)
		{
			int index = static_cast<int>(target);

			switch (action)
			{
				case seri::InputAction::press:
					GetInstance()._mousePressing[index] = true;
					break;
				case seri::InputAction::release:
					GetInstance()._mousePressing[index] = false;
					break;
				default:
					break;
			}

			GetInstance()._mouse[index] = action;
		}

		static void RegisterCursorPosition(double x, double y)
		{
			GetInstance()._cursorPosition.x = static_cast<float>(x);
			GetInstance()._cursorPosition.y = static_cast<float>(y);
		}

		static void RegisterScrollDelta(double x, double y)
		{
			GetInstance()._scrollDelta.x = static_cast<float>(x);
			GetInstance()._scrollDelta.y = static_cast<float>(y);
		}

		static bool IsKeyDown(KeyCode target)
		{
			return GetInstance()._keys[static_cast<int>(target)] == InputAction::press;
		}

		static bool IsKeyUp(KeyCode target)
		{
			return GetInstance()._keys[static_cast<int>(target)] == InputAction::release;
		}

		static bool IsKeyPressing(KeyCode target)
		{
			return GetInstance()._keys[static_cast<int>(target)] == InputAction::repeat;
		}

		static bool IsMouseButtonDown(MouseButtonCode target)
		{
			return GetInstance()._mouse[static_cast<int>(target)] == InputAction::press;
		}

		static bool IsMouseButtonUp(MouseButtonCode target)
		{
			return GetInstance()._mouse[static_cast<int>(target)] == InputAction::release;
		}

		static bool IsMouseButtonPressing(MouseButtonCode target)
		{
			return GetInstance()._mousePressing[static_cast<int>(target)];
		}

		static glm::vec2 GetCursorPosition()
		{
			return GetInstance()._cursorPosition;
		}

		static glm::vec2 GetScrollDelta()
		{
			return GetInstance()._scrollDelta;
		}

	private:
		InputManager() = default;

		~InputManager() = default;

		std::vector<InputAction> _keys{};
		std::vector<InputAction> _mouse{};
		std::vector<bool> _mousePressing{};
		glm::vec2 _cursorPosition{ 0.0f, 0.0f };
		glm::vec2 _scrollDelta{ 0.0f, 0.0f };

	};
}
