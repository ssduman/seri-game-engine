#pragma once

#define PI 3.14159265359f

#include "seri/core/Literals.h"
#include "seri/logging/Logger.h"

#include <fmt/core.h>

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>
#include <ctime>
#include <regex>
#include <cctype>
#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <string_view>
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

		static float ToDegree(float radian);

		static float ToRadian(float degree);

		static float Map(float x, float in_min, float in_max, float out_min, float out_max);

		static int RountToInt(float value);

		static void ToLower(std::string& s);

		static bool Contains(std::string_view str, std::string_view substr);

		static glm::quat ToQuaternion(const glm::vec3& euler);

		static glm::mat4 GetTRS(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scale);

		static glm::mat4 GetTRS(const glm::vec3& pos, const glm::vec3& euler, const glm::vec3& scale);

		static glm::mat4 GetIdentityMatrix();

		static void Decompose(const glm::mat4& matrix, glm::vec3& pos, glm::vec3& euler, glm::vec3& scale);

		static std::string ReadFileAtPath(const char* path);

		static std::string GetContentOfToken(const std::string& text, const std::string& token_beg, const std::string& token_end);

		static bool IsIgnoredUniform(std::string_view uniformName);

	};
}
