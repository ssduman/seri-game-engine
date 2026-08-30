#include "Seripch.h"

#include "seri/logging/Logger.h"

#define STB_RECT_PACK_IMPLEMENTATION

#include "GlyphPacker.h"

namespace seri::font
{
	GlyphPacker::GlyphPacker(GlyphPackerParams params)
		: _params{ params }
	{
		_nodes = (stbrp_node*)malloc(sizeof(stbrp_node) * _params.num_rect);
		Reset();
	}

	GlyphPacker::~GlyphPacker()
	{
		free(_nodes);
	}

	void GlyphPacker::Reset()
	{
		memset(&_ctx, 0, sizeof(_ctx));
		memset(_nodes, 0, sizeof(_nodes));
		stbrp_init_target(&_ctx, _params.texture_width, _params.texture_height, _nodes, _params.num_rect);
	}

	std::pair<int, int> GlyphPacker::PackRect(int id, int w, int h)
	{
		stbrp_rect rect{};
		rect.id = id;
		rect.w = w + _params.padding;
		rect.h = h + _params.padding;
		rect.x = 0;
		rect.y = 0;
		rect.was_packed = false;
		stbrp_pack_rects(&_ctx, &rect, 1);
		if (!rect.was_packed)
		{
			LOGGER(error, "[font] unicode: " << id << " was not packed");
		}
		return { rect.x + _params.padding, rect.y + _params.padding };
	}
}
