#pragma once

#define PI 3.14159265359f

#include "seri/logging/Logger.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <fstream>
#include <sstream>
#include <iostream>

struct Util
{
	static float toDegree(float radian)
	{
		return radian * (180.0f / PI);
	}

	static float toRadian(float degree)
	{
		return degree * (PI / 180.0f);
	}

	static float map(float x, float in_min, float in_max, float out_min, float out_max)
	{
		return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	}

	static glm::mat4 GetTRS(glm::vec3 pos, glm::quat rot, glm::vec3 scale)
	{
		glm::mat4 T = glm::translate(glm::mat4{ 1.0f }, pos);
		glm::mat4 R = glm::mat4_cast(rot);
		glm::mat4 S = glm::scale(glm::mat4{ 1.0f }, scale);
		return T * R * S;
	}
	
	static glm::mat4 GetIdentityMatrix()
	{
		return glm::mat4{ 1.0f };
	}

	static std::string readFileAtPath(const char* path)
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

};
