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
		LOGGER(error, "[sound] sound manager init failed: " << ma_result_description(result));
		return;
	}
}

void seri::sound::SoundManager::Play(std::string soundFilePath)
{
	ma_result result;

	result = ma_engine_play_sound(&GetInstance()._engine, (GetInstance()._soundFolderPath + soundFilePath).c_str(), nullptr);
	if (result != MA_SUCCESS)
	{
		LOGGER(error, "[sound] could not play audio: " << ma_result_description(result));
		return;
	}
}
