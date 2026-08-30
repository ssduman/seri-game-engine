#pragma once

#if 0

namespace seri
{
	struct RendererBackendBase
	{

		virtual ~RendererBackendBase() = default;

		virtual void Draw() = 0;

		virtual void Release() = 0;

	};
}

#endif