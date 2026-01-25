#include "Seripch.h"

#include "seri/core/Seri.h"
#include "seri/texture/TextureBase.h"

#if defined (SERI_USE_RENDERING_OPENGL)
#include "seri/texture/TextureOpenGL.h"
#endif

#include <stb_image.h>
#include <stb_image_write.h>

namespace seri
{
	void* TextureBase::LoadTexture(const std::string& texturePath, int& width, int& height, int& components, bool flip)
	{
		stbi_set_flip_vertically_on_load(flip);
		void* image = stbi_load(texturePath.c_str(), &width, &height, &components, 0);
		if (!image)
		{
			LOGGER(error, "init texture '" << texturePath << "' failed: " << stbi_failure_reason());
		}
		return image;
	}

	void* TextureBase::LoadTexture(const void* data, unsigned int size, int& width, int& height, int& components, bool flip)
	{
		stbi_set_flip_vertically_on_load(flip);
		void* image = stbi_load_from_memory((const stbi_uc*)data, size, &width, &height, &components, 0);
		if (!image)
		{
			LOGGER(error, "init texture from buffer with size " << size << " failed: " << stbi_failure_reason());
		}
		return image;
	}

	void TextureBase::UnloadTexture(void* image)
	{
		if (image)
		{
			stbi_image_free(image);
		}
	}

	void TextureBase::WriteTexture(const std::string& path, const void* data, int width, int height, int components, bool flip)
	{
		stbi_write_png(path.c_str(), width, height, components, data, width * components);
	}

	void TextureBase::UnbindTex2D(int slot)
	{
#if defined (SERI_USE_RENDERING_OPENGL)
		return seri::TextureOpenGL::UnbindTex2DImpl(slot);
#else
		static_assert(false, "unknown rendering type for unbind texture");
#endif
	}

	std::shared_ptr<TextureBase> TextureBase::Create()
	{
#if defined (SERI_USE_RENDERING_OPENGL)
		return std::make_shared<seri::TextureOpenGL>();
#else
		static_assert(false, "unknown rendering type for creating texture");
#endif

		return nullptr;
	}

}
