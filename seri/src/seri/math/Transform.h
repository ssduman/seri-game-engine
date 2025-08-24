#pragma once

#include "seri/util/Util.h"

#include <vector>

namespace seri
{
	class Transform
	{
	public:
		Transform() = default;

		~Transform() = default;

		std::vector<glm::vec3>& GetCoordinates()
		{
			return coordinates;
		}

		glm::mat4& Apply()
		{
			_transform = glm::mat4{ 1.0f };
			_transform = glm::scale(_transform, _scale);
			_transform = glm::rotate(_transform, glm::radians(_rotation.x), { 1.0f, 0.0f, 0.0f });
			_transform = glm::rotate(_transform, glm::radians(_rotation.y), { 0.0f, 1.0f, 0.0f });
			_transform = glm::rotate(_transform, glm::radians(_rotation.z), { 0.0f, 0.0f, 1.0f });
			_transform = glm::translate(_transform, _position);
			return _transform;
		}

		glm::mat4 _transform{ 1.0f };
		glm::vec3 _position{ 0.0f, 0.0f, 0.0f };
		glm::vec3 _rotation{ 0.0f, 0.0f, 0.0f };
		glm::vec3 _scale{ 1.0f, 1.0f, 1.0f };

	private:
		std::vector<glm::vec3> coordinates;

	};
}
