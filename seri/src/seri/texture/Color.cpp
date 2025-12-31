#include "Seripch.h"

#include "seri/texture/Color.h"

namespace seri
{
	Color::Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f)
	{
	}

	Color::Color(float c) : r(c), g(c), b(c), a(1.0f)
	{
	}

	Color::Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a)
	{
	}

	Color::Color(const glm::vec3& v) : r(v.r), g(v.g), b(v.b), a(1.0f)
	{
	}

	Color::Color(const glm::vec4& v) : r(v.r), g(v.g), b(v.b), a(v.a)
	{
	}

	glm::vec3 Color::GetColor() const
	{
		return { r, g, b };
	}

	glm::vec4 Color::GetColorRGBA() const
	{
		return glm::vec4{ r, g, b, a };
	}

	Color Color::Lerp(const Color& a, const Color& b, float t)
	{
		t = std::clamp(t, 0.0f, 1.0f);
		return LerpUnclamped(a, b, t);
	}

	Color Color::LerpUnclamped(const Color& a, const Color& b, float t)
	{
		return {
			a.r + (b.r - a.r) * t,
			a.g + (b.g - a.g) * t,
			a.b + (b.b - a.b) * t,
			a.a + (b.a - a.a) * t
		};
	}

	float& Color::operator[](size_t index)
	{
		return (&r)[index];
	}

	const float& Color::operator[](size_t index) const
	{
		return (&r)[index];
	}

	const Color Color::White{ 1.0f, 1.0f, 1.0f, 1.0f };
	const Color Color::Black{ 0.0f, 0.0f, 0.0f, 1.0f };
	const Color Color::Red{ 1.0f, 0.0f, 0.0f, 1.0f };
	const Color Color::Green{ 0.0f, 1.0f, 0.0f, 1.0f };
	const Color Color::Blue{ 0.0f, 0.0f, 1.0f, 1.0f };
	const Color Color::Yellow{ 1.0f, 1.0f, 0.0f, 1.0f };
	const Color Color::Cyan{ 0.0f, 1.0f, 1.0f, 1.0f };
	const Color Color::Magenta{ 1.0f, 0.0f, 1.0f, 1.0f };
	const Color Color::Gray{ 0.5f, 0.5f, 0.5f, 1.0f };

}
