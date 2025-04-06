#pragma once

#include <cmath>
#include <iostream>
#include <stdexcept>

namespace seri
{
	template <typename T = float>
	class Vec3
	{
	public:
		Vec3() {}

		Vec3(T x_)
		{
			x = x_;
		}

		Vec3(T x_, T y_)
		{
			x = x_;
			y = y_;
		}

		Vec3(T x_, T y_, T z_)
		{
			x = x_;
			y = y_;
			z = z_;
		}

		Vec3(const Vec3& vec3)
		{
			x = vec3.x;
			y = vec3.y;
			z = vec3.z;
		}

		~Vec3() = default;

		/* vec3[] */

		T& operator[](const int& index)
		{
			if (index == 0)
			{
				return x;
			}
			if (index == 1)
			{
				return y;
			}
			if (index == 2)
			{
				return z;
			}
			throw std::invalid_argument("invalid index");
		}

		/* std::cout << vec3 */

		friend std::ostream& operator<< (std::ostream& out, const Vec3& vec3)
		{
			out << "Vec3(" << vec3.x << ", " << vec3.y << ", " << vec3.z << ")";
			return out;
		}

		/* vec3_lhs + 3.14f; */

		Vec3 operator+(const T& rhs)
		{
			return Vec3{ x + rhs, y + rhs, z + rhs };
		}

		Vec3 operator-(const T& rhs)
		{
			return Vec3{ x - rhs, y - rhs, z - rhs };
		}

		Vec3 operator*(const T& rhs)
		{
			return Vec3{ x * rhs, y * rhs, z * rhs };
		}

		Vec3 operator/(const T& rhs)
		{
			return Vec3{ x / rhs, y / rhs, z / rhs };
		}

		/* vec3_lhs + vec3_rhs; */

		friend Vec3 operator+(const Vec3& lhs, const Vec3& rhs)
		{
			return Vec3{ lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
		}

		friend Vec3 operator-(const Vec3& lhs, const Vec3& rhs)
		{
			return Vec3{ lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
		}

		friend Vec3 operator*(const Vec3& lhs, const Vec3& rhs)
		{
			return Vec3{ lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z };
		}

		friend Vec3 operator/(const Vec3& lhs, const Vec3& rhs)
		{
			return Vec3{ lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z };
		}

		/* vec3_lhs += 3.14f; */

		friend Vec3 operator+=(Vec3& lhs, const T& rhs)
		{
			lhs.x += rhs;
			lhs.y += rhs;
			lhs.z += rhs;
			return lhs;
		}

		friend Vec3 operator-=(Vec3& lhs, const T& rhs)
		{
			lhs.x -= rhs;
			lhs.y -= rhs;
			lhs.z -= rhs;
			return lhs;
		}

		friend Vec3 operator*=(Vec3& lhs, const T& rhs)
		{
			lhs.x *= rhs;
			lhs.y *= rhs;
			lhs.z *= rhs;
			return lhs;
		}

		friend Vec3 operator/=(Vec3& lhs, const T& rhs)
		{
			lhs.x /= rhs;
			lhs.y /= rhs;
			lhs.z /= rhs;
			return lhs;
		}

		/* vec3_lhs += vec3_rhs; */

		friend Vec3 operator+=(Vec3& lhs, const Vec3& rhs)
		{
			lhs.x += rhs.x;
			lhs.y += rhs.y;
			lhs.z += rhs.z;
			return lhs;
		}

		friend Vec3 operator-=(Vec3& lhs, const Vec3& rhs)
		{
			lhs.x -= rhs.x;
			lhs.y -= rhs.y;
			lhs.z -= rhs.z;
			return lhs;
		}

		friend Vec3 operator*=(Vec3& lhs, const Vec3& rhs)
		{
			lhs.x *= rhs.x;
			lhs.y *= rhs.y;
			lhs.z *= rhs.z;
			return lhs;
		}

		friend Vec3 operator/=(Vec3& lhs, const Vec3& rhs)
		{
			lhs.x /= rhs.x;
			lhs.y /= rhs.y;
			lhs.z /= rhs.z;
			return lhs;
		}

		/* aux */

		float sum()
		{
			return static_cast<float>(x) + static_cast<float>(y) + static_cast<float>(z);
		}

		float norm()
		{
			return (float)sqrt(dot(*this));
		}

		Vec3<T> normalize()
		{
			return (*this) / norm();
		}

		float dot(const Vec3<T>& rhs)
		{
			return ((*this) * rhs).sum();
		}

		Vec3<T> cross(const Vec3<T>& rhs)
		{
			auto c_x = y * rhs.z - z * rhs.y;
			auto c_y = z * rhs.x - x * rhs.z;
			auto c_z = x * rhs.y - y * rhs.x;
			return Vec3{ c_x, c_y, c_z };
		}

		float distance(const Vec3<T>& rhs)
		{
			return (float)sqrt(dot((*this) - rhs));
		}

		float angle(const Vec3<T>& rhs)
		{
			return (float)dot(rhs) / (norm() * rhs.norm());
		}

		T x = 0;
		T y = 0;
		T z = 0;

	};
}
