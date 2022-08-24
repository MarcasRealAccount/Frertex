#include "Frertex/Utils/Profiler.h"

#include <sstream>
#include <unordered_map>
#include <vector>

namespace Frertex::Utils
{
	static std::unordered_map<std::string, ProfilerData> s_ProfilerDatas;
	static std::vector<ProfilerStackEntry>               s_ProfilerStack;

	std::unordered_map<std::string, ProfilerData> GetProfilerDatas()
	{
		return s_ProfilerDatas;
	}

	static void addPoint(std::string&& name, ProfilerPoint&& point)
	{
		auto itr = s_ProfilerDatas.find(name);
		if (itr == s_ProfilerDatas.end())
			s_ProfilerDatas.insert_or_assign(std::move(name), ProfilerData { { std::move(point) } });
		else
			itr->second.m_Points.emplace_back(std::move(point));
	}

	void ProfilerEnter(std::string&& name)
	{
		s_ProfilerStack.emplace_back(std::move(name), ProfilerStackEntry::Clock::now());
	}

	void ProfilerExit()
	{
		if (s_ProfilerStack.empty())
			return;

		auto          end   = ProfilerStackEntry::Clock::now();
		auto&         back  = s_ProfilerStack.back();
		std::uint64_t delta = std::chrono::duration_cast<std::chrono::duration<std::uint64_t, std::nano>>(end - back.m_Start).count();
		addPoint(std::move(back.m_Name), { delta, back.m_InOtherStack });
		s_ProfilerStack.pop_back();

		if (s_ProfilerStack.empty())
			return;

		s_ProfilerStack.back().m_InOtherStack += delta;
	}

	std::string ProfilerToString()
	{
		using Clock = ProfilerPoint::Clock;

		struct DataPoint
		{
		public:
			DataPoint(std::uint64_t total, std::uint64_t longest, std::uint64_t shortest, decltype(s_ProfilerDatas)::iterator iterator)
			    : m_Total(total), m_Longest(longest), m_Shortest(shortest), m_Iterator(iterator) {}

		public:
			std::uint64_t m_Total, m_Longest, m_Shortest;

			decltype(s_ProfilerDatas)::iterator m_Iterator;
		};

		std::vector<DataPoint> sortedData;

		for (auto itr = s_ProfilerDatas.begin(); itr != s_ProfilerDatas.end(); ++itr)
		{
			auto& points = itr->second.m_Points;

			std::uint64_t shortestTime = std::numeric_limits<std::uint64_t>::max();
			std::uint64_t longestTime  = std::numeric_limits<std::uint64_t>::min();
			std::uint64_t totalTime    = 0;

			for (std::size_t i = 0; i < points.size(); ++i)
			{
				auto&         point = points[i];
				std::uint64_t time  = point.m_TotalTime - point.m_InOtherStack;

				totalTime += time;
				if (time < shortestTime)
					shortestTime = time;
				if (time > longestTime)
					longestTime = time;
			}

			bool added = false;
			for (auto itr2 = sortedData.begin(); itr2 != sortedData.end(); ++itr2)
			{
				if (totalTime > itr2->m_Total)
				{
					added = true;
					sortedData.insert(itr2, { totalTime, longestTime, shortestTime, itr });
					break;
				}
			}
			if (!added)
				sortedData.emplace_back(totalTime, longestTime, shortestTime, itr);
		}

		std::ostringstream str;
		for (auto point : sortedData)
		{
			auto& itr    = point.m_Iterator;
			auto& points = itr->second.m_Points;
			str << '"' << itr->first << "\":\n";

			str << "\tPoints:        " << points.size() << '\n';
			str << "\tTotal Time:    " << (point.m_Total / 1'000'000.0) << " ms\n";
			str << "\tAverage Time:  " << ((point.m_Total / points.size()) / 1'000'000.0) << " ms\n";
			str << "\tShortest Time: " << (point.m_Shortest / 1'000'000.0) << " ms\n";
			str << "\tLongest Time:  " << (point.m_Longest / 1'000'000.0) << " ms\n";
			str << "\tTime error:    " << ((point.m_Longest - point.m_Shortest) / 1'000'000.0) << " ms\n";
		}

		str << '\n';

		return str.str();
	}

	Profile::Profile(std::string&& name)
	{
		ProfilerEnter(std::move(name));
	}

	Profile::~Profile()
	{
		ProfilerExit();
	}
} // namespace Frertex::Utils
