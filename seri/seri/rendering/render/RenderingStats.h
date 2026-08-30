#pragma once

#include "seri/util/Util.h"

namespace seri
{
	struct RenderingStats
	{
		uint32_t drawCalls{ 0 };
		uint32_t triangles{ 0 };

		void Reset()
		{
			drawCalls = 0;
			triangles = 0;
		}
	};
}
