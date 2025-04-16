#include "Seripch.h"

#define MINIAUDIO_IMPLEMENTATION

#include "seri/sound/SoundManager.h"

void seri::sound::SoundManager::Init(const char* soundFolderPath)
{
	GetInstance();

	ma_result result;

	result = ma_engine_init(NULL, &GetInstance()._engine);
	if (result != MA_SUCCESS)
	{
		LOGGER(error, "[sound] sound manager init failed: " << ma_result_description(result));
		return;
	}

	result = ma_engine_play_sound(&GetInstance()._engine, (std::string{ soundFolderPath } + "/jingles/Preview.wav").c_str(), NULL);
	if (result != MA_SUCCESS)
	{
		LOGGER(error, "[sound] could not play audio: " << ma_result_description(result));
		return;
	}
}
