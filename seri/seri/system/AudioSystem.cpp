#include "Seripch.h"
#include "seri/system/AudioSystem.h"

#include "seri/scene/SceneManager.h"
#include "seri/component/Components.h"
#include "seri/sound/SoundManager.h"
#include <entt/entt.hpp>

namespace seri::system
{
	void AudioSystem::Update()
	{
		auto& registry = seri::scene::SceneManager::GetRegistry();

		bool isPlaying = seri::scene::SceneManager::GetState() == seri::scene::SceneState::play;

		auto view = registry.view<seri::component::AudioComponent>();

		for (entt::entity entity : view)
		{
			auto& audio = view.get<seri::component::AudioComponent>(entity);

			if (audio.soundPath != audio.builtPath)
			{
				if (audio.handle != 0)
				{
					seri::sound::SoundManager::Destroy(audio.handle);
				}

				audio.handle = seri::sound::SoundManager::Create(audio.soundPath);
				audio.builtPath = audio.soundPath;
				audio.started = false;
			}

			if (audio.handle == 0)
			{
				continue;
			}

			seri::sound::SoundManager::SetVolume(audio.handle, audio.volume);
			seri::sound::SoundManager::SetLooping(audio.handle, audio.loop);

			auto* transform = registry.try_get<seri::component::TransformComponent>(entity);
			bool isActive = transform == nullptr || transform->isActiveInHierarchy;

			if (isPlaying && isActive)
			{
				if (audio.playOnStart && !audio.started)
				{
					seri::sound::SoundManager::Start(audio.handle);
					audio.started = true;
				}
			}
			else if (audio.started)
			{
				seri::sound::SoundManager::Stop(audio.handle);
				audio.started = false;
			}
		}
	}
}
