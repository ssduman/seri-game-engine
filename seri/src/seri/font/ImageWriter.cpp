#include "Seripch.h"

#include "seri/logging/Logger.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "ImageWriter.h"

namespace seri::font
{
	ImageWriter::ImageWriter(ImageWriterParams params)
		: _params{ params }
	{
		_buffer = (unsigned char*)calloc(_params.texture_width * _params.texture_height * _params.texture_comp, 1);

		std::filesystem::path root{ _params.output_folder_path };
		std::filesystem::path folder{ get_pretty_font_name(_params.font_name) + "_" + output_folder_name };
		output_path = root / folder;
		if (!std::filesystem::is_directory(output_path) || !std::filesystem::exists(output_path))
		{
			std::filesystem::create_directory(output_path);
		}

		Reset();
	}

	ImageWriter::~ImageWriter()
	{
		free(_buffer);
	}

	void ImageWriter::Reset()
	{
		memset(_buffer, 0, sizeof(unsigned char) * _params.texture_width * _params.texture_height * _params.texture_comp);
	}

	void ImageWriter::Draw(unsigned char* buffer, int pitch, int uv_w, int uv_h, int uv_l, int uv_b, DrawMode draw_mode)
	{
		static int R = 0;
		static int G = 1;
		static int B = 2;
		static int A = 3;

		_maximum_height = std::max(_maximum_height, uv_b + uv_h);

		for (int h = 0; h < uv_h; h++)
		{
			for (int w = 0; w < uv_w; w++)
			{
				unsigned char data = buffer[h * pitch + w];

				int base = ((uv_b + h) * _params.texture_width + (uv_l + w)) * _params.texture_comp;

				switch (draw_mode)
				{
					case seri::font::DrawMode::draw_main:
					{
						unsigned char val = data;
						_buffer[base + R] |= val;
						_buffer[base + A] |= val;
						break;
					}
				}
			}
		}
	}

	int ImageWriter::Write(std::string name, std::string style, int pixel_size)
	{
		int calculated_height = get_next_power_of_two(_maximum_height);

		std::string pretty_name = get_pretty_name(name, style, pixel_size);
		std::string write_path = (output_path / std::filesystem::path{ pretty_name + ".png" }).string();

		int write_error = stbi_write_png(
			write_path.c_str(),
			_params.texture_width,
			calculated_height,
			_params.texture_comp,
			_buffer,
			_params.texture_width * _params.texture_comp
		);
		if (write_error == 0)
		{
			LOGGER(error, "[font] error in writing to stb png: " << write_error);
			return 0;
		}

		return calculated_height;
	}
}
