#include "Seripch.h"

#include "seri/core/Core.h"
#include "seri/platform/Platform.h"

#ifdef __linux__

#include "seri/platform/PlatformLinux.h"

#elif _WIN32

#include "seri/platform/PlatformWindows.h"

#else

static_assert(false, "unknown platform");

#endif
