#pragma once

#include "seri/math/Vec4.h"
#include "seri/util/Util.h"

#include <cmath>
#include <iostream>
#include <stdexcept>

namespace seri
{
	template <typename T = float>
	class Mat4
	{
	public:
		Mat4() {}

		Mat4(const Vec4<T>& row0, const Vec4<T>& row1, const Vec4<T>& row2, const Vec4<T>& row3)
		{
			rows[0] = row0;
			rows[1] = row1;
			rows[2] = row2;
			rows[3] = row3;
		}

		Mat4(const Mat4<T>& rhs)
		{
			rows[0] = rhs.rows[0];
			rows[1] = rhs.rows[1];
			rows[2] = rhs.rows[2];
			rows[3] = rhs.rows[3];
		}

		Mat4(T val, bool fill = false)
		{
			if (!fill)
			{
				rows[0].x = val;
				rows[1].y = val;
				rows[2].z = val;
				rows[3].w = val;
			}
			else
			{
				rows[0].x = rows[0].y = rows[0].z = rows[0].w = val;
				rows[1].x = rows[1].y = rows[1].z = rows[1].w = val;
				rows[2].x = rows[2].y = rows[2].z = rows[2].w = val;
				rows[3].x = rows[3].y = rows[3].z = rows[3].w = val;
			}
		}

		~Mat4() = default;

		/* mat4[] */

		Vec4<T>& operator[](const int& index)
		{
			if (index < 0 || index > 3)
			{
				throw std::invalid_argument("invalid index");
			}
			return rows[index];
		}

		/* std::cout << mat4 */

		friend std::ostream& operator<< (std::ostream& out, const Mat4& mat4)
		{
			out << "Mat4["
				<< mat4.rows[0]
				<< mat4.rows[1]
				<< mat4.rows[2]
				<< mat4.rows[3]
				<< "]";
			return out;
		}

		/* mat4_lhs + 3.14f; */

		Mat4 operator+(const T& rhs)
		{
			Mat4 temp{ this };
			temp.rows[0] += rhs;
			temp.rows[1] += rhs;
			temp.rows[2] += rhs;
			temp.rows[3] += rhs;
			return temp;
		}

		Mat4 operator-(const T& rhs)
		{
			Mat4 temp{ this };
			temp.rows[0] -= rhs;
			temp.rows[1] -= rhs;
			temp.rows[2] -= rhs;
			temp.rows[3] -= rhs;
			return temp;
		}

		Mat4 operator*(const T& rhs)
		{
			Mat4 temp{ this };
			temp.rows[0] *= rhs;
			temp.rows[1] *= rhs;
			temp.rows[2] *= rhs;
			temp.rows[3] *= rhs;
			return temp;
		}

		Mat4 operator/(const T& rhs)
		{
			Mat4 temp{ this };
			temp.rows[0] /= rhs;
			temp.rows[1] /= rhs;
			temp.rows[2] /= rhs;
			temp.rows[3] /= rhs;
			return temp;
		}

		/* mat4_lhs + mat4_rhs; */

		friend Mat4 operator+(const Mat4& lhs, const Mat4& rhs)
		{
			Mat4 temp{ lhs };
			temp.rows[0] += rhs.rows[0];
			temp.rows[1] += rhs.rows[1];
			temp.rows[2] += rhs.rows[2];
			temp.rows[3] += rhs.rows[3];
			return temp;
		}

		friend Mat4 operator-(const Mat4& lhs, const Mat4& rhs)
		{
			Mat4 temp{ lhs };
			temp.rows[0] -= rhs.rows[0];
			temp.rows[1] -= rhs.rows[1];
			temp.rows[2] -= rhs.rows[2];
			temp.rows[3] -= rhs.rows[3];
			return temp;
		}

		friend Mat4 operator*(const Mat4& lhs, const Mat4& rhs)
		{
			Mat4 temp{ lhs };
			temp.rows[0] *= rhs.rows[0];
			temp.rows[1] *= rhs.rows[1];
			temp.rows[2] *= rhs.rows[2];
			temp.rows[3] *= rhs.rows[3];
			return temp;
		}

		friend Mat4 operator/(const Mat4& lhs, const Mat4& rhs)
		{
			Mat4 temp{ lhs };
			temp.rows[0] /= rhs.rows[0];
			temp.rows[1] /= rhs.rows[1];
			temp.rows[2] /= rhs.rows[2];
			temp.rows[3] /= rhs.rows[3];
			return temp;
		}

		/* mat4_lhs + vec4_rhs; */

		friend Mat4 operator+(const Mat4& lhs, const Vec4<T>& rhs)
		{
			Mat4 temp{ lhs };
			temp.rows[0] += rhs;
			temp.rows[1] += rhs;
			temp.rows[2] += rhs;
			temp.rows[3] += rhs;
			return temp;
		}

		friend Mat4 operator-(const Mat4& lhs, const Vec4<T>& rhs)
		{
			Mat4 temp{ lhs };
			temp.rows[0] -= rhs;
			temp.rows[1] -= rhs;
			temp.rows[2] -= rhs;
			temp.rows[3] -= rhs;
			return temp;
		}

		friend Vec4<T> operator*(const Mat4& lhs, const Vec4<T>& rhs)
		{
			Mat4 temp{ lhs };
			return lhs.multiply(rhs);
		}

		friend Mat4 operator/(const Mat4& lhs, const Vec4<T>& rhs)
		{
			Mat4 temp{ lhs };
			temp.rows[0] /= rhs;
			temp.rows[1] /= rhs;
			temp.rows[2] /= rhs;
			temp.rows[3] /= rhs;
			return temp;
		}

		/* mat4_lhs += 3.14f; */

		friend Mat4 operator+=(Mat4& lhs, const T& rhs)
		{
			lhs.rows[0] += rhs;
			lhs.rows[1] += rhs;
			lhs.rows[2] += rhs;
			lhs.rows[3] += rhs;
			return lhs;
		}

		friend Mat4 operator-=(Mat4& lhs, const T& rhs)
		{
			lhs.rows[0] -= rhs;
			lhs.rows[1] -= rhs;
			lhs.rows[2] -= rhs;
			lhs.rows[3] -= rhs;
			return lhs;
		}

		friend Mat4 operator*=(Mat4& lhs, const T& rhs)
		{
			lhs.rows[0] *= rhs;
			lhs.rows[1] *= rhs;
			lhs.rows[2] *= rhs;
			lhs.rows[3] *= rhs;
			return lhs;
		}

		friend Mat4 operator/=(Mat4& lhs, const T& rhs)
		{
			lhs.rows[0] /= rhs;
			lhs.rows[1] /= rhs;
			lhs.rows[2] /= rhs;
			lhs.rows[3] /= rhs;
			return lhs;
		}

		/* mat4_lhs += mat4_rhs; */

		friend Mat4 operator+=(Mat4& lhs, const Mat4& rhs)
		{
			lhs.rows[0] += rhs.rows[0];
			lhs.rows[1] += rhs.rows[1];
			lhs.rows[2] += rhs.rows[2];
			lhs.rows[3] += rhs.rows[3];
			return lhs;
		}

		friend Mat4 operator-=(Mat4& lhs, const Mat4& rhs)
		{
			lhs.rows[0] -= rhs.rows[0];
			lhs.rows[1] -= rhs.rows[1];
			lhs.rows[2] -= rhs.rows[2];
			lhs.rows[3] -= rhs.rows[3];
			return lhs;
		}

		friend Mat4 operator*=(Mat4& lhs, const Mat4& rhs)
		{
			lhs.rows[0] *= rhs.rows[0];
			lhs.rows[1] *= rhs.rows[1];
			lhs.rows[2] *= rhs.rows[2];
			lhs.rows[3] *= rhs.rows[3];
			return lhs;
		}

		friend Mat4 operator/=(Mat4& lhs, const Mat4& rhs)
		{
			lhs.rows[0] /= rhs.rows[0];
			lhs.rows[1] /= rhs.rows[1];
			lhs.rows[2] /= rhs.rows[2];
			lhs.rows[3] /= rhs.rows[3];
			return lhs;
		}

		/* aux */

		Vec4<T> GetRow(const int& index)
		{
			return rows[index];
		}

		Vec4<T> GetCol(const int& index)
		{
			return Vec4{ rows[0][index], rows[1][index], rows[2][index], rows[3][index] };
		}

		/* transformations */

		Vec4<float> Multiply(Vec4<T>& rhs)
		{
			return Vec4<float>(
				GetRow(0).Dot(rhs),
				GetRow(1).Dot(rhs),
				GetRow(2).Dot(rhs),
				GetRow(3).Dot(rhs)
			);
		}

		Vec4<float> Multiply(Vec3<T>& rhs)
		{
			return Vec4<float>(
				GetRow(0).Dot(rhs),
				GetRow(1).Dot(rhs),
				GetRow(2).Dot(rhs),
				1
			);
		}

		Vec4<float> Scale(Vec3<T>& rhs)
		{
			return Multiply(rhs);
		}

		Vec4<float> Scale(Vec4<T>& rhs)
		{
			return Multiply(rhs);
		}

		Vec4<float> Translate(Vec3<T>& rhs)
		{
			return Multiply(rhs);
		}

		Vec4<float> Translate(Vec4<T>& rhs)
		{
			return Multiply(rhs);
		}

		/* static transformations */

		static Vec4<float> Multiply(Mat4<T>& mat4, Vec3<T>& vec3)
		{
			return Vec4<float>(
				mat4.GetRow(0).Dot(vec3),
				mat4.GetRow(1).Dot(vec3),
				mat4.GetRow(2).Dot(vec3),
				1.0f
			);
		}

		static Vec4<float> Multiply(Mat4<T>& mat4, Vec4<T>& vec4)
		{
			return Vec4<float>(
				mat4.GetRow(0).Dot(vec4),
				mat4.GetRow(1).Dot(vec4),
				mat4.GetRow(2).Dot(vec4),
				mat4.GetRow(3).Dot(vec4)
			);
		}

		static Mat4<float> Scale(Mat4<T>& mat4, Vec3<T> scaler)
		{
			Mat4 temp{ 1.0f };
			temp.rows[0] = mat4[0] * scaler[0];
			temp.rows[1] = mat4[1] * scaler[1];
			temp.rows[2] = mat4[2] * scaler[2];
			temp.rows[3] = mat4[3];
			return temp;
		}

		static Mat4<float> Translate(Mat4<T>& mat4, Vec3<T> translator)
		{
			Mat4 temp{ mat4 };
			temp[3] = mat4[0] * translator[0] + mat4[1] * translator[1] + mat4[2] * translator[2] + mat4[3];
			return temp;
		}

		static Mat4<float> Rotate(Mat4<T>& mat4, float angle, Vec3<T> axis)
		{
			float theta = Util::ToRadian(angle);
			float cosTheta = cos(theta);
			float sinTheta = sin(theta);

			Vec3<float> axisNormalized(axis.normalize());
			Vec3<float> tempAxis((1.0f - cosTheta) * axisNormalized);

			Mat4<float> rotationMat;
			rotationMat[0][0] = cosTheta + tempAxis[0] * axisNormalized[0];
			rotationMat[0][1] = tempAxis[0] * axisNormalized[1] + sinTheta * axisNormalized[2];
			rotationMat[0][2] = tempAxis[0] * axisNormalized[2] - sinTheta * axisNormalized[1];

			rotationMat[1][0] = tempAxis[1] * axisNormalized[0] - sinTheta * axisNormalized[2];
			rotationMat[1][1] = cosTheta + tempAxis[1] * axisNormalized[1];
			rotationMat[1][2] = tempAxis[1] * axisNormalized[2] + sinTheta * axisNormalized[0];

			rotationMat[2][0] = tempAxis[2] * axisNormalized[0] + sinTheta * axisNormalized[1];
			rotationMat[2][1] = tempAxis[2] * axisNormalized[1] - sinTheta * axisNormalized[0];
			rotationMat[2][2] = cosTheta + tempAxis[2] * axisNormalized[2];

			Mat4<float> temp;
			temp[0] = mat4[0] * rotationMat[0][0] + mat4[1] * rotationMat[0][1] + mat4[2] * rotationMat[0][2];
			temp[1] = mat4[0] * rotationMat[1][0] + mat4[1] * rotationMat[1][1] + mat4[2] * rotationMat[1][2];
			temp[2] = mat4[0] * rotationMat[2][0] + mat4[1] * rotationMat[2][1] + mat4[2] * rotationMat[2][2];
			temp[3] = mat4[3];
			return temp;
		}

		/* combinations */

		Mat4<float> Multiply(Mat4<T>& rhs)
		{
			return Mat4<float>(
				Vec4<float>{ GetRow(0).Dot(rhs.GetCol(0)), GetRow(0).Dot(rhs.GetCol(1)), GetRow(0).Dot(rhs.GetCol(2)), GetRow(0).Dot(rhs.GetCol(3)) },
				Vec4<float>{ GetRow(1).Dot(rhs.GetCol(0)), GetRow(1).Dot(rhs.GetCol(1)), GetRow(1).Dot(rhs.GetCol(2)), GetRow(1).Dot(rhs.GetCol(3)) },
				Vec4<float>{ GetRow(2).Dot(rhs.GetCol(0)), GetRow(2).Dot(rhs.GetCol(1)), GetRow(2).Dot(rhs.GetCol(2)), GetRow(2).Dot(rhs.GetCol(3)) },
				Vec4<float>{ GetRow(3).Dot(rhs.GetCol(0)), GetRow(3).Dot(rhs.GetCol(1)), GetRow(3).Dot(rhs.GetCol(2)), GetRow(3).Dot(rhs.GetCol(3)) }
			);
		}

		/* util */

		template <typename U = float>
		static glm::mat4 ToGLMMat4(Mat4<float>& rhs)
		{
			return glm::mat4(
				Vec4<U>::ToGLMVec4(rhs.rows[0]),
				Vec4<U>::ToGLMVec4(rhs.rows[1]),
				Vec4<U>::ToGLMVec4(rhs.rows[2]),
				Vec4<U>::ToGLMVec4(rhs.rows[3])
			);
		}

		/* data*/

		Vec4<T> rows[4];

	};
}
