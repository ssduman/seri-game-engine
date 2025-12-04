#pragma once

#define PI 3.14159265359f

#include "seri/logging/Logger.h"

#include <fmt/core.h>

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>
#include <ctime>
#include <regex>
#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <unordered_map>

namespace seri
{
	struct Util
	{
		template <typename T>
		static T Max(T a, T b)
		{
			if (a > b)
			{
				return a;
			}
			else
			{
				return b;
			}
		}

		static float ToDegree(float radian)
		{
			return radian * (180.0f / PI);
		}

		static float ToRadian(float degree)
		{
			return degree * (PI / 180.0f);
		}

		static float Map(float x, float in_min, float in_max, float out_min, float out_max)
		{
			return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
		}

		static int RountToInt(float value)
		{
			return static_cast<int>(std::round(value));
		}

		static glm::mat4 GetTRS(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scale)
		{
			glm::mat4 T = glm::translate(glm::mat4{ 1.0f }, pos);
			glm::mat4 R = glm::mat4_cast(rot);
			glm::mat4 S = glm::scale(glm::mat4{ 1.0f }, scale);
			return T * R * S;
		}

		static glm::mat4 GetTRS(const glm::vec3& pos, const glm::vec3& euler, const glm::vec3& scale)
		{
			return GetTRS(pos, glm::quat(glm::radians(euler)), scale);
		}

		static glm::mat4 GetIdentityMatrix()
		{
			return glm::mat4{ 1.0f };
		}

		static std::string ReadFileAtPath(const char* path)
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

		static std::string GetContentOfToken(const std::string& text, const std::string& token_beg, const std::string& token_end)
		{
			size_t beg = text.find(token_beg);
			size_t end = text.find(token_end);

			if (beg != std::string::npos && end != std::string::npos && end > beg)
			{
				beg += token_beg.length();
				return text.substr(beg, end - beg);
			}

			return "";
		}

	};
}
