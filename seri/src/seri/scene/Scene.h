#pragma once

#include "seri/core/Seri.h"
#include "seri/util/Util.h"
#include "seri/texture/Skybox.h"
#include "seri/component/Components.h"

namespace seri
{
	namespace scene
	{
		class Scene
		{
		public:
			Scene() = default;

			~Scene() = default;

			void Init()
			{
				_skybox = std::make_shared<seri::Skybox>();
			}

			void Update()
			{
				_skybox->Update();
			}

		private:
			std::shared_ptr<seri::Skybox> _skybox;

		};
	}
}
