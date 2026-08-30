#pragma once

#include <utility>
#include <iostream>

#include <stb_rect_pack.h>

#include "Utils.h"

namespace seri::font
{
	struct GlyphPackerParams
	{
		std::string output_folder_path;
		int texture_width;
		int texture_height;
		int num_rect;
		int padding;
	};

	class GlyphPacker
	{
	public:
		GlyphPacker(GlyphPackerParams params);
		~GlyphPacker();

		void Reset();
		std::pair<int, int> PackRect(int id, int w, int h);
	private:
		GlyphPackerParams _params;

		stbrp_context _ctx;
		stbrp_node* _nodes = nullptr;
	};
}
