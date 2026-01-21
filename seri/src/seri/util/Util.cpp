#include "Seripch.h"

#include "seri/util/Util.h"

namespace seri
{
	float Util::ToDegree(float radian)
	{
		return radian * (180.0f / PI);
	}

	float Util::ToRadian(float degree)
	{
		return degree * (PI / 180.0f);
	}

	float Util::Map(float x, float in_min, float in_max, float out_min, float out_max)
	{
		return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	}

	int Util::RountToInt(float value)
	{
		return static_cast<int>(std::round(value));
	}

	void Util::ToLower(std::string& s)
	{
		std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
	}

	bool Util::Contains(std::string_view str, std::string_view substr)
	{
		return str.find(substr) != std::string::npos;
	}

	glm::quat Util::ToQuaternion(const glm::vec3& euler)
	{
		return glm::quat(glm::radians(euler));
	}

	glm::mat4 Util::GetTRS(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scale)
	{
		glm::mat4 T = glm::translate(glm::mat4{ 1.0f }, pos);
		glm::mat4 R = glm::mat4_cast(rot);
		glm::mat4 S = glm::scale(glm::mat4{ 1.0f }, scale);
		return T * R * S;
	}

	glm::mat4 Util::GetTRS(const glm::vec3& pos, const glm::vec3& euler, const glm::vec3& scale)
	{
		return GetTRS(pos, ToQuaternion(euler), scale);
	}

	glm::mat4 Util::GetIdentityMatrix()
	{
		return glm::mat4{ 1.0f };
	}

	void Util::Decompose(const glm::mat4& matrix, glm::vec3& pos, glm::vec3& euler, glm::vec3& scale)
	{
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::quat rot;
		glm::decompose(matrix, scale, rot, pos, skew, perspective);
		euler = glm::degrees(glm::eulerAngles(rot));
	}

	std::string Util::ReadFileAtPath(const char* path)
	{
		try
		{
			std::stringstream ss;
			std::ifstream stream(path);
			if (stream.is_open())
			{
				ss << stream.rdbuf();
				stream.close();
			}
			return ss.str();
		}
		catch (const std::exception& ex)
		{
			LOGGER(error, "exception occurred while reading path: " << path << ", exception: " << ex.what());
			return std::string{};
		}
	}

	std::string Util::GetContentOfToken(const std::string& text, const std::string& tokenBeg, const std::string& tokenEnd)
	{
		size_t beg = text.find(tokenBeg);
		size_t end = text.find(tokenEnd);

		if (beg != std::string::npos && end != std::string::npos && end > beg)
		{
			beg += tokenBeg.length();
			return text.substr(beg, end - beg);
		}

		return "";
	}

	bool Util::IsIgnoredUniform(std::string_view uniformName)
	{
		if (uniformName.starts_with(seri::literals::kUniformColor) ||
			uniformName.starts_with(seri::literals::kUniformViewPos) ||
			uniformName.starts_with(seri::literals::kUniformLightDir) ||
			uniformName.starts_with(seri::literals::kUniformLightColor) ||
			uniformName.starts_with(seri::literals::kUniformBones) ||
			uniformName.starts_with(seri::literals::kUniformView) ||
			uniformName.starts_with(seri::literals::kUniformModel) ||
			uniformName.starts_with(seri::literals::kUniformProjection) ||
			uniformName.starts_with(seri::literals::kUniformCameraPos) ||
			false
			)
		{
			return true;
		}

		return false;
	}

}
