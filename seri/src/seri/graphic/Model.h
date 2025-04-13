#pragma once

#include "seri/logging/Logger.h"
#include "seri/graphic/Mesh.h"

#include <vector>
#include <memory>

namespace seri
{
	class Model
	{
	public:
		std::vector<std::unique_ptr<Mesh>> meshes{};

		void UpdateAnimations()
		{
			for (const auto& mesh : meshes)
			{
				mesh->UpdateAnimation();
			}
		}

	private:
	};
}
