#pragma once

namespace seri
{
	struct RendererBackendBase
	{

		virtual ~RendererBackendBase() = default;

		virtual void draw() = 0;

		virtual void release() = 0;

	};
}
