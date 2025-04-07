#pragma once

#include "seri/texture/Texture.h"
#include "seri/font/Utils.h"

#include <memory>
#include <string>
#include <iostream>
#include <algorithm>
#include <filesystem>

#include <stb_image_write.h>

namespace seri::font
{
	struct ImageWriterParams
	{
		std::string font_name;
		std::string output_folder_path;
		int texture_width;
		int texture_height;
		int texture_comp;
	};

	class ImageWriter
	{
	public:
		ImageWriter(ImageWriterParams params);
		~ImageWriter();

		void Reset();
		void Draw(unsigned char* buffer, int pitch, int uvW, int uvH, int uvL, int uvB, DrawMode draw_mode);
		int Write(std::string name, std::string style, int pixel_size);

		std::shared_ptr<Texture> CreateTexture();

	private:
		ImageWriterParams _params;

		const std::string output_folder_name = "images";
		std::filesystem::path output_path;

		int _maximum_height = 0;

		unsigned char* _buffer;

	};
}
