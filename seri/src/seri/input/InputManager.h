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
			GetInstance()._keys.reserve(static_cast<int>(KeyCode::len));

			Reset();
		}

		static void Reset()
		{
			for (auto& var : GetInstance()._keys)
			{
				var = InputAction::noop;
			}
		}

		static void Register(KeyCode target, InputAction action)
		{
			GetInstance()._keys[static_cast<int>(target)] = action;
		}

		static bool IsPressed(KeyCode target)
		{
			return GetInstance()._keys[static_cast<int>(target)] == InputAction::press;
		}

		static bool IsReleased(KeyCode target)
		{
			return GetInstance()._keys[static_cast<int>(target)] == InputAction::release;
		}

		static bool IsRepeating(KeyCode target)
		{
			return GetInstance()._keys[static_cast<int>(target)] == InputAction::repeat;
		}

		static bool IsPressing(KeyCode target)
		{
			return GetInstance()._keys[static_cast<int>(target)] != InputAction::noop;
		}

	private:
		InputManager() = default;

		~InputManager() = default;

		std::vector<InputAction> _keys{};

	};
}
