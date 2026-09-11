#pragma once

#include "seri/core/Singleton.h"

#include "seri/util/Util.h"

#include <miniaudio.h>

#include <string>
#include <memory>
#include <unordered_map>

namespace seri::sound
{
	class SoundManager : public seri::Singleton<SoundManager>
	{
	public:
		static void Init(const char* soundFolderPath);

		static void Play(std::string soundFilePath);

		static uint64_t Create(const std::string& soundFilePath);
		static void Destroy(uint64_t handle);

		static void Start(uint64_t handle);
		static void Stop(uint64_t handle);

		static void SetLooping(uint64_t handle, bool loop);
		static void SetVolume(uint64_t handle, float volume);
		static bool IsPlaying(uint64_t handle);

	protected:
		friend struct seri::Singleton<SoundManager>;

		SoundManager()
		{
		}

		~SoundManager()
		{
			for (auto& [handle, sound] : _sounds)
			{
				ma_sound_uninit(sound.get());
			}

			_sounds.clear();

			ma_engine_uninit(&_engine);
		}

	private:
		static ma_sound* Find(uint64_t handle);

		ma_engine _engine{};
		std::string _soundFolderPath{};

		std::unordered_map<uint64_t, std::unique_ptr<ma_sound>> _sounds{};
		uint64_t _nextHandle{ 1 };

	};
}
