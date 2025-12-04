#pragma once

#include "seri/core/Seri.h"
#include "seri/util/Util.h"
#include "seri/texture/Skybox.h"
#include "seri/component/Components.h"

#include <yaml-cpp/yaml.h>

#include <string>

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

			void Deserialize(const std::string& file)
			{
				Scene scene;

				YAML::Node root = YAML::LoadFile(file);

				if (!root["Scene"] || !root["Scene"].IsMap())
				{
					LOGGER(error, fmt::format("[scene] failed to deserialize scene from file: {}", file));
					return;
				}

				YAML::Node sceneNode = root["Scene"];

				_idComponent = IDComponent::Deserialize(sceneNode["IDComponent"]);
				_sceneComponent = SceneComponent::Deserialize(sceneNode["SceneComponent"]);

				if (YAML::Node entities = sceneNode["Entities"])
				{
					for (auto entityItem : entities)
					{
						YAML::Node entityData = entityItem["Entity"];

						for (auto componentNode : entityData)
						{
							std::string componentName = componentNode.first.as<std::string>();
							YAML::Node componentData = componentNode.second;

							if (componentName == "IDComponent")
							{
								IDComponent idComp = IDComponent::Deserialize(componentData);
							}
							else if (componentName == "TransformComponent")
							{
								TransformComponent transformComp = TransformComponent::Deserialize(componentData);
							}
							else if (componentName == "SceneComponent")
							{
								SceneComponent sceneComp = SceneComponent::Deserialize(componentData);
							}
							else
							{
								LOGGER(error, fmt::format("[scene] unknown component type: {}", componentName));
							}
						}
					}
				}
			}

		private:
			std::shared_ptr<seri::Skybox> _skybox;

			IDComponent _idComponent;
			SceneComponent _sceneComponent;

		};
	}
}
