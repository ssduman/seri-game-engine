#pragma once

#include <glm/glm.hpp>

class Color
{
public:
	Color() = default;

	Color(float c)
	{
		r = c;
		g = c;
		b = c;
		normalize();
	}

	Color(float _r, float _g, float _b)
	{
		r = _r;
		g = _g;
		b = _b;
		normalize();
	}

	Color(float _r, float _g, float _b, float _a)
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
		normalize();
	}

	Color(glm::vec3& vec)
	{
		r = vec.r;
		g = vec.g;
		b = vec.b;
		normalize();
	}

	Color(const glm::vec3& vec)
	{
		r = vec.r;
		g = vec.g;
		b = vec.b;
		normalize();
	}

	Color(glm::vec4& vec)
	{
		r = vec.r;
		g = vec.g;
		b = vec.b;
		a = vec.a;
		normalize();
	}

	Color(const glm::vec4& vec)
	{
		r = vec.r;
		g = vec.g;
		b = vec.b;
		a = vec.a;
		normalize();
	}

	glm::vec3 getColor()
	{
		return { r, g, b };
	}

	glm::vec4 getColorRGBA()
	{
		return { r, g, b, a };
	}

	float r = 1.0f;
	float g = 1.0f;
	float b = 1.0f;
	float a = 1.0f;

private:
	void normalize()
	{
		if (r > 1.0f)
		{
			r /= 255.0f;
		}
		if (g > 1.0f)
		{
			g /= 255.0f;
		}
		if (b > 1.0f)
		{
			b /= 255.0f;
		}
		if (a > 1.0f)
		{
			a /= 255.0f;
		}
	}

};
