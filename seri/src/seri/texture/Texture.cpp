#include "Seripch.h"

#include "seri/texture/Texture.h"

#include <stb_image_write.h>

void seri::Texture::WriteToFile(std::string path, unsigned char* image, int width, int height)
{
	stbi_write_png(path.c_str(), width, height, 4, image, width * 4);
}
