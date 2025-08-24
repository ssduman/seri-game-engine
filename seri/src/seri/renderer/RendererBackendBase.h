#pragma once

namespace seri
{
	struct RendererBackendBase
	{

		virtual ~RendererBackendBase() = default;

		virtual void Draw() = 0;

		virtual void Release() = 0;

	};
}
