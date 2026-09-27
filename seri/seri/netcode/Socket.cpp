#include "Seripch.h"

#include "seri/core/Core.h"
#include "seri/netcode/Socket.h"

#ifdef __linux__

static_assert(false, "linux platform not supported");

#elif _WIN32

#include "seri/netcode/SocketWindows.h"

namespace seri::netcode
{
	std::unique_ptr<Socket> Socket::Create(SocketType st)
	{
		return std::make_unique<SocketWindows>(st);
	}
}

#else

static_assert(false, "unknown platform");

#endif
