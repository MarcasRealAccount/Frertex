#pragma once

#include "Core.h"
#include "Utils.h"

#if __cpp_lib_source_location
#include <source_location>
#endif
#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>

namespace Frertex::Utils
{
	struct ProfilerPoint
	{
	public:
		using Clock = std::chrono::high_resolution_clock;

		std::uint64_t m_TotalTime;
		std::uint64_t m_InOtherStack;
	};

	struct ProfilerData
	{
	public:
		std::vector<ProfilerPoint> m_Points;
	};

	struct ProfilerStackEntry
	{
	public:
		using Clock = ProfilerPoint::Clock;

	public:
		ProfilerStackEntry(Utils::CopyMovable<std::string>&& name, Clock::time_point start)
		    : m_Name(name.get()), m_Start(start) {}

	public:
		std::string       m_Name;
		Clock::time_point m_Start;
		std::uint64_t     m_InOtherStack = 0;
	};

	std::unordered_map<std::string, ProfilerData> GetProfilerDatas();

	void ProfilerEnter(std::string&& name);
	void ProfilerExit();

	std::string ProfilerToString();

	struct Profile
	{
	public:
		Profile(std::string&& name);
		~Profile();
	};
} // namespace Frertex::Utils

#if BUILD_IS_CONFIG_DEBUG
#if __cpp_lib_source_location
#define PROFILE_FUNC                                    \
	::Frertex::Utils::Profile _profiler                 \
	{                                                   \
		std::source_location::current().function_name() \
	}
#else
#define PROFILE_FUNC                    \
	::Frertex::Utils::Profile _profiler \
	{                                   \
		__FUNCTION__                    \
	}
#endif
#else
#define PROFILE_FUNC
#endif
