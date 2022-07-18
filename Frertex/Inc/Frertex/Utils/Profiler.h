#pragma once

#include "Utils.h"

#include <chrono>
#include <source_location>
#include <string>
#include <unordered_map>

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

#define PROFILE_FUNC \
	::Frertex::Utils::Profile _profiler { std::source_location::current().function_name() }