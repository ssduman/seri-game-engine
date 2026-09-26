#pragma once

#include "seri/logging/Logger.h"

#include <tracy/Tracy.hpp>
#include <common/TracyVersion.hpp>

namespace seri::profiling
{
	class Profiler
	{
	public:
		static void Init()
		{
			LIB_LOGGER(info, profiling) << "tracy version: " << tracy::Version::Major << "." << tracy::Version::Minor << "." << tracy::Version::Patch;
		}
	};
}

#define SERI_PROFILER_FRAME_END_MARK FrameMark
#define SERI_PROFILER_ZONE_SCOPED ZoneScoped
#define SERI_PROFILER_ZONE_SCOPED_N(name) ZoneScopedN(name)
#define SERI_PROFILER_ZONE_SCOPED_C(color) ZoneScopedC(tracy::Color::color)
#define SERI_PROFILER_ZONE_TRANSIENT(name) ZoneTransientN(___tracy_scoped_zone, name, true)
#define SERI_PROFILER_ZONE_NAME(text, size) ZoneName(text, size)
