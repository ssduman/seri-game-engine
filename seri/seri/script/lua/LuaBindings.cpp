#include "Seripch.h"

#include "seri/script/lua/LuaBindings.h"
#include "seri/script/lua/LuaScript.h"
#include "seri/system/ScriptSystem.h"
#include "seri/scene/SceneManager.h"
#include "seri/component/Components.h"
#include "seri/input/InputManager.h"
#include "seri/sound/SoundManager.h"
#include "seri/random/Random.h"
#include "seri/core/Entity.h"
#include "seri/core/Application.h"

#include <sol/sol.hpp>

namespace seri::script
{
	void LuaBindings::Register(sol::state& lua)
	{
		RegisterMath(lua);
		RegisterCore(lua);
		RegisterInput(lua);
		RegisterComponents(lua);
		RegisterEntity(lua);
		RegisterScene(lua);
	}

	template<typename T, typename Constructors>
	sol::usertype<T> LuaBindings::RegisterVector(sol::state& lua, const char* name)
	{
		sol::usertype<T> type = lua.new_usertype<T>(name, sol::call_constructor, Constructors{});

		type[sol::meta_function::addition] = [](const T& a, const T& b) { return T{ a + b }; };
		type[sol::meta_function::subtraction] = [](const T& a, const T& b) { return T{ a - b }; };
		type[sol::meta_function::unary_minus] = [](const T& a) { return T{ -a }; };
		type[sol::meta_function::equal_to] = [](const T& a, const T& b) { return a == b; };
		type[sol::meta_function::multiplication] = sol::overload(
			[](const T& a, const T& b) { return T{ a * b }; },
			[](const T& a, float b) { return T{ a * b }; },
			[](float a, const T& b) { return T{ a * b }; }
		);
		type[sol::meta_function::division] = sol::overload(
			[](const T& a, const T& b) { return T{ a / b }; },
			[](const T& a, float b) { return T{ a / b }; }
		);

		type["Length"] = [](const T& a) { return glm::length(a); };
		type["Normalized"] = [](const T& a) { return glm::length(a) > 0.0f ? T{ glm::normalize(a) } : a; };
		type["Dot"] = [](const T& a, const T& b) { return glm::dot(a, b); };
		type["Distance"] = [](const T& a, const T& b) { return glm::distance(a, b); };
		type["Lerp"] = [](const T& a, const T& b, float t) { return T{ glm::mix(a, b, t) }; };

		return type;
	}

	void LuaBindings::RegisterMath(sol::state& lua)
	{
		sol::usertype<glm::vec2> vec2 = RegisterVector<glm::vec2, sol::constructors<glm::vec2(), glm::vec2(float), glm::vec2(float, float)>>(lua, "Vec2");
		vec2["x"] = &glm::vec2::x;
		vec2["y"] = &glm::vec2::y;
		vec2[sol::meta_function::to_string] = [](const glm::vec2& v) { return fmt::format("({}, {})", v.x, v.y); };

		sol::usertype<glm::vec3> vec3 = RegisterVector<glm::vec3, sol::constructors<glm::vec3(), glm::vec3(float), glm::vec3(float, float, float)>>(lua, "Vec3");
		vec3["x"] = &glm::vec3::x;
		vec3["y"] = &glm::vec3::y;
		vec3["z"] = &glm::vec3::z;
		vec3["Cross"] = [](const glm::vec3& a, const glm::vec3& b) { return glm::cross(a, b); };
		vec3[sol::meta_function::to_string] = [](const glm::vec3& v) { return fmt::format("({}, {}, {})", v.x, v.y, v.z); };

		sol::usertype<glm::vec4> vec4 = RegisterVector<glm::vec4, sol::constructors<glm::vec4(), glm::vec4(float), glm::vec4(float, float, float, float)>>(lua, "Vec4");
		vec4["x"] = &glm::vec4::x;
		vec4["y"] = &glm::vec4::y;
		vec4["z"] = &glm::vec4::z;
		vec4["w"] = &glm::vec4::w;
		vec4[sol::meta_function::to_string] = [](const glm::vec4& v) { return fmt::format("({}, {}, {}, {})", v.x, v.y, v.z, v.w); };

		sol::table random = lua.create_named_table("Random");
		random["Range"] = [](float min, float max) { return seri::Random::Range(min, max); };
	}

	void LuaBindings::RegisterCore(sol::state& lua)
	{
		lua.set_function("print", +[](lua_State* state) -> int
			{
				std::string message{};

				int count = lua_gettop(state);
				for (int i = 1; i <= count; i++)
				{
					size_t length = 0;
					const char* text = luaL_tolstring(state, i, &length);
					if (i > 1)
					{
						message += " ";
					}
					message.append(text, length);
					lua_pop(state, 1);
				}

				LIB_LOGGER(info, lua_bindings) << message;

				return 0;
			}
		);

		sol::table logTable = lua.create_named_table("Log");
		logTable["Info"] = [](const std::string& message) { LIB_LOGGER(info, lua_bindings) << message; };
		logTable["Warning"] = [](const std::string& message) { LIB_LOGGER(warning, lua_bindings) << message; };
		logTable["Error"] = [](const std::string& message) { LIB_LOGGER(error, lua_bindings) << message; };

		sol::table timeTable = lua.create_named_table("Time");
		timeTable["delta_time"] = 0.0f;
		timeTable["time"] = 0.0f;
		timeTable["frame_count"] = 0;

		sol::table audioTable = lua.create_named_table("Audio");
		audioTable["Play"] = [](const std::string& path) { sound::SoundManager::Play(path); };

		sol::table applicationTable = lua.create_named_table("Application");
		applicationTable["Quit"] = []() { Application::Quit(); };
	}

	void LuaBindings::RegisterInput(sol::state& lua)
	{
		sol::table keyTable = lua.create_named_table("Key");
		for (int i = 0; i < static_cast<int>(KeyCode::last); i++)
		{
			keyTable[ToString(static_cast<KeyCode>(i))] = i;
		}

		sol::table mouseTable = lua.create_named_table("Mouse");
		for (int i = 0; i < static_cast<int>(MouseButtonCode::len); i++)
		{
			mouseTable[ToString(static_cast<MouseButtonCode>(i))] = i;
		}

		auto isKey = [](int key) { return key >= 0 && key < static_cast<int>(KeyCode::len); };
		auto isButton = [](int button) { return button >= 0 && button < static_cast<int>(MouseButtonCode::len); };

		sol::table inputTable = lua.create_named_table("Input");
		inputTable["GetKeyDown"] = [isKey](int key) { return isKey(key) && InputManager::IsKeyDown(static_cast<KeyCode>(key)); };
		inputTable["GetKeyUp"] = [isKey](int key) { return isKey(key) && InputManager::IsKeyUp(static_cast<KeyCode>(key)); };
		inputTable["GetKey"] = [isKey](int key) { return isKey(key) && InputManager::IsKeyPressing(static_cast<KeyCode>(key)); };
		inputTable["GetMouseButtonDown"] = [isButton](int button) { return isButton(button) && InputManager::IsMouseButtonDown(static_cast<MouseButtonCode>(button)); };
		inputTable["GetMouseButtonUp"] = [isButton](int button) { return isButton(button) && InputManager::IsMouseButtonUp(static_cast<MouseButtonCode>(button)); };
		inputTable["GetMouseButton"] = [isButton](int button) { return isButton(button) && InputManager::IsMouseButtonPressing(static_cast<MouseButtonCode>(button)); };
		inputTable["GetMousePosition"] = []() { return InputManager::GetCursorPosition(); };
		inputTable["GetScrollDelta"] = []() { return InputManager::GetScrollDelta(); };
	}

	void LuaBindings::RegisterComponents(sol::state& lua)
	{
		lua.new_usertype<component::TransformComponent>(
			"Transform",
			sol::no_constructor,
			"position", &component::TransformComponent::position,
			"rotation", &component::TransformComponent::rotation,
			"scale", &component::TransformComponent::scale,
			"active", &component::TransformComponent::isActive,
			"active_in_hierarchy", sol::readonly(&component::TransformComponent::isActiveInHierarchy)
		);

		lua.new_usertype<component::RectComponent>(
			"Rect",
			sol::no_constructor,
			"anchor_min", &component::RectComponent::anchorMin,
			"anchor_max", &component::RectComponent::anchorMax,
			"pivot", &component::RectComponent::pivot,
			"anchored_position", &component::RectComponent::anchoredPosition,
			"size_delta", &component::RectComponent::sizeDelta
		);

		lua.new_usertype<component::ButtonComponent>(
			"Button",
			sol::no_constructor,
			"interactable", &component::ButtonComponent::interactable,
			"normal_color", &component::ButtonComponent::normalColor,
			"hover_color", &component::ButtonComponent::hoverColor,
			"pressed_color", &component::ButtonComponent::pressedColor,
			"disabled_color", &component::ButtonComponent::disabledColor,
			"hovered", sol::readonly(&component::ButtonComponent::hovered),
			"pressed", sol::readonly(&component::ButtonComponent::pressed),
			"clicked", sol::readonly(&component::ButtonComponent::clicked)
		);

		lua.new_usertype<component::TextComponent>(
			"Text",
			sol::no_constructor,
			"text", &component::TextComponent::text,
			"color", &component::TextComponent::color,
			"font_size", &component::TextComponent::fontSize
		);

		lua.new_usertype<component::SpriteRendererComponent>(
			"Sprite",
			sol::no_constructor,
			"color", &component::SpriteRendererComponent::color,
			"flip_x", &component::SpriteRendererComponent::flipX,
			"flip_y", &component::SpriteRendererComponent::flipY
		);
	}

	void LuaBindings::RegisterEntity(sol::state& lua)
	{
		lua.new_usertype<seri::Entity>(
			"Entity",
			sol::no_constructor,
			"IsValid", &seri::Entity::IsValid,
			"name", sol::readonly_property([](const seri::Entity& entity)
				{
					auto* id = entity.TryGet<component::IDComponent>();
					return id ? id->name : std::string{};
				}
			),
			"active", sol::property(
				[](const seri::Entity& entity)
				{
					auto* transform = entity.TryGet<component::TransformComponent>();
					return transform != nullptr && transform->isActive;
				},
				[](const seri::Entity& entity, bool active)
				{
					if (auto* transform = entity.TryGet<component::TransformComponent>())
					{
						transform->isActive = active;
					}
				}
			),
			"transform", sol::readonly_property([](const seri::Entity& entity) { return entity.TryGet<component::TransformComponent>(); }),
			"rect", sol::readonly_property([](const seri::Entity& entity) { return entity.TryGet<component::RectComponent>(); }),
			"button", sol::readonly_property([](const seri::Entity& entity) { return entity.TryGet<component::ButtonComponent>(); }),
			"text", sol::readonly_property([](const seri::Entity& entity) { return entity.TryGet<component::TextComponent>(); }),
			"sprite", sol::readonly_property([](const seri::Entity& entity) { return entity.TryGet<component::SpriteRendererComponent>(); }),
			"GetScript", [](const seri::Entity& entity, const std::string& name) -> sol::optional<sol::table>
			{
				auto* script = dynamic_cast<LuaScript*>(system::ScriptSystem::FindScript(entity.GetHandle(), name));
				if (!script)
				{
					return sol::nullopt;
				}
				return script->GetSelf();
			},
			sol::meta_function::equal_to, [](const seri::Entity& a, const seri::Entity& b) { return a == b; }
		);
	}

	void LuaBindings::RegisterScene(sol::state& lua)
	{
		sol::table sceneTable = lua.create_named_table("Scene");
		sceneTable["Find"] = [](const std::string& name) -> sol::optional<seri::Entity>
			{
				auto& registry = scene::SceneManager::GetRegistry();

				for (auto [entity, id] : registry.view<component::IDComponent>().each())
				{
					if (id.name == name)
					{
						return seri::Entity{ entity, &registry };
					}
				}

				return sol::nullopt;
			};
		sceneTable["Reload"] = []() { scene::SceneManager::ReloadScene(); };
	}
}
