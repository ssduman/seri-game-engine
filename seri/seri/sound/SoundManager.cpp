#include "Seripch.h"

#define MINIAUDIO_IMPLEMENTATION

#include "seri/sound/SoundManager.h"

void seri::sound::SoundManager::Init(const char* soundFolderPath)
{
	GetInstance()._soundFolderPath = soundFolderPath;

	ma_result result;

	ma_engine_config engineConfig;

	engineConfig = ma_engine_config_init();

	result = ma_engine_init(&engineConfig, &GetInstance()._engine);
	if (result != MA_SUCCESS)
	{
		LIB_LOGGER(error, sound) << "sound manager init failed: " << ma_result_description(result);
		return;
	}
}

void seri::sound::SoundManager::Play(std::string soundFilePath)
{
	ma_result result;

	result = ma_engine_play_sound(&GetInstance()._engine, (GetInstance()._soundFolderPath + soundFilePath).c_str(), nullptr);
	if (result != MA_SUCCESS)
	{
		LIB_LOGGER(error, sound) << "could not play audio: " << ma_result_description(result);
		return;
	}
}

uint64_t seri::sound::SoundManager::Create(const std::string& soundFilePath)
{
	if (soundFilePath.empty())
	{
		return 0;
	}

	auto sound = std::make_unique<ma_sound>();

	std::string fullPath = GetInstance()._soundFolderPath + soundFilePath;

	ma_result result = ma_sound_init_from_file(&GetInstance()._engine, fullPath.c_str(), 0, nullptr, nullptr, sound.get());
	if (result != MA_SUCCESS)
	{
		LIB_LOGGER(error, sound) << "could not load audio '" << fullPath << "': " << ma_result_description(result);
		return 0;
	}

	uint64_t handle = GetInstance()._nextHandle++;
	GetInstance()._sounds[handle] = std::move(sound);

	return handle;
}

void seri::sound::SoundManager::Destroy(uint64_t handle)
{
	auto it = GetInstance()._sounds.find(handle);
	if (it == GetInstance()._sounds.end())
	{
		return;
	}

	ma_sound_uninit(it->second.get());
	GetInstance()._sounds.erase(it);
}

void seri::sound::SoundManager::Start(uint64_t handle)
{
	if (ma_sound* sound = Find(handle))
	{
		ma_sound_start(sound);
	}
}

void seri::sound::SoundManager::Stop(uint64_t handle)
{
	if (ma_sound* sound = Find(handle))
	{
		ma_sound_stop(sound);
		ma_sound_seek_to_pcm_frame(sound, 0);
	}
}

void seri::sound::SoundManager::SetLooping(uint64_t handle, bool loop)
{
	if (ma_sound* sound = Find(handle))
	{
		ma_sound_set_looping(sound, loop ? MA_TRUE : MA_FALSE);
	}
}

void seri::sound::SoundManager::SetVolume(uint64_t handle, float volume)
{
	if (ma_sound* sound = Find(handle))
	{
		ma_sound_set_volume(sound, volume);
	}
}

bool seri::sound::SoundManager::IsPlaying(uint64_t handle)
{
	if (ma_sound* sound = Find(handle))
	{
		return ma_sound_is_playing(sound) == MA_TRUE;
	}

	return false;
}

ma_sound* seri::sound::SoundManager::Find(uint64_t handle)
{
	auto it = GetInstance()._sounds.find(handle);
	if (it == GetInstance()._sounds.end())
	{
		return nullptr;
	}

	return it->second.get();
}
