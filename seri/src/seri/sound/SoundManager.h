#pragma once

#include "seri/util/Util.h"

#include <miniaudio.h>

#include <string>

namespace seri::sound
{
	class SoundManager
	{
	public:
		SoundManager(SoundManager const&) = delete;

		void operator=(SoundManager const&) = delete;

		static SoundManager& GetInstance()
		{
			static SoundManager instance;
			return instance;
		}

		static void Init(const char* soundFolderPath);

	private:
		SoundManager()
		{
		}

		~SoundManager()
		{
			ma_engine_uninit(&_engine);
		}

		ma_engine _engine{};

	};
}
