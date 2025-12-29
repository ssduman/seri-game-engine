#pragma once

#include <seri/core/Seri.h>
#include <seri/behaviour/BehaviourBase.h>

namespace seri::editor
{
	class SystemBehaviour : public seri::BehaviourBase
	{
	public:
		void Init() override
		{
		}

		void Update() override
		{
			if (_draw)
			{
				std::shared_ptr<seri::Model> model = seri::asset::AssetManager::GetAssetByID<seri::Model>(8933691566797930938);
				std::shared_ptr<seri::Material> material = seri::asset::AssetManager::GetAssetByID<seri::Material>(5128232576114054222);
				seri::Graphic::DrawModel(
					model,
					material,
					seri::Util::GetTRS(
						{ 0.0f, 0.0f, 0.0f },
						{ 0.0f, 0.0f, 0.0f },
						{ 0.01f, 0.01f, 0.01f }
					),
					seri::Graphic::GetCameraPerspective(),
					seri::RenderingManager::GetEditorRT()
				);
			}
		}

		void Destroy() override
		{
		}

	private:
		bool _draw{ false };

	};
}
