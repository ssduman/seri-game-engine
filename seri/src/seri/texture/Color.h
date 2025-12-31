#pragma once

#include <glm/glm.hpp>

namespace seri
{
	class Color
	{
	public:
		Color();
		explicit Color(float c);
		Color(float r, float g, float b, float a = 1.0f);
		Color(const glm::vec3& v);
		Color(const glm::vec4& v);

		glm::vec3 GetColor() const;
		glm::vec4 GetColorRGBA() const;

		static Color Lerp(const Color& a, const Color& b, float t);
		static Color LerpUnclamped(const Color& a, const Color& b, float t);

		float& operator[](size_t index);
		const float& operator[](size_t index) const;

		static const Color White;
		static const Color Black;
		static const Color Red;
		static const Color Green;
		static const Color Blue;
		static const Color Yellow;
		static const Color Cyan;
		static const Color Magenta;
		static const Color Gray;

		float r;
		float g;
		float b;
		float a;

	};
}
