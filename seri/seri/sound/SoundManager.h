#pragma once

#include "seri/core/Singleton.h"

#include "seri/util/Util.h"

#include <miniaudio.h>

#include <string>

namespace seri::sound
{
	class SoundManager : public seri::Singleton<SoundManager>
	{
	public:
		static void Init(const char* soundFolderPath);

		static void Play(std::string soundFilePath);

	protected:
		friend struct seri::Singleton<SoundManager>;

		SoundManager()
		{
		}

		~SoundManager()
		{
			ma_engine_uninit(&_engine);
		}

	private:
		ma_engine _engine{};
		std::string _soundFolderPath{};

	};
}
