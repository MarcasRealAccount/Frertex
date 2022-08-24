#pragma once

#include "Frertex/Utils/Utils.h"

#include <filesystem>
#include <string>

namespace Frertex::Source
{
	enum class EIncludeStatus
	{
		Failure = 0,
		Success
	};

	struct IncludeData
	{
		EIncludeStatus        m_Status;
		std::string           m_Source;
		std::filesystem::path m_Filepath;
	};

	using IncludePathHandler = std::filesystem::path (*)(std::string_view filename, const std::filesystem::path& originalFilepath);
	using IncludeHandler     = IncludeData (*)(const std::filesystem::path& filepath);

	struct SourcePoint
	{
		std::size_t   m_Index = 0, m_Line = 0, m_Column = 0;
		std::uint32_t m_FileID = 0, m_SourceID = 0;
	};

	struct SourceSpan
	{
	public:
		SourcePoint start() const { return { m_StartIndex, m_StartLine, m_StartColumn, m_FileID, m_SourceID }; }
		SourcePoint end() const { return { m_EndIndex, m_EndLine, m_EndColumn, m_FileID, m_SourceID }; }

	public:
		std::size_t   m_StartIndex = 0, m_StartLine = 0, m_StartColumn = 0;
		std::size_t   m_EndIndex = 0, m_EndLine = 0, m_EndColumn = 0;
		std::uint32_t m_FileID = 0, m_SourceID = 0;
	};

	std::string SourcePointToString(const SourcePoint& point);
	std::string SourceSpanToString(const SourceSpan& span);

	struct Source
	{
	public:
		Source(Utils::CopyMovable<std::string>&& name, Utils::CopyMovable<std::string>&& str, std::uint32_t id);

		std::size_t      getLineNumber(std::size_t index) const;
		SourcePoint      getPoint(std::size_t index) const;
		SourceSpan       getSpan(std::size_t index, std::size_t length) const;
		std::string_view getView(std::size_t index, std::size_t length) const { return std::string_view { m_Str.data() + index, std::min(m_Str.size() - index, length) }; }

		auto& getName() const { return m_Name; }
		auto& getStr() const { return m_Str; }
		auto& getIndices() const { return m_Indices; }
		auto  getID() const { return m_ID; }

	private:
		std::string              m_Name;
		std::string              m_Str;
		std::vector<std::size_t> m_Indices;

		std::uint32_t m_ID;
	};

	class Sources
	{
	public:
		std::uint32_t addSource(Utils::CopyMovable<std::string>&& name, Utils::CopyMovable<std::string>&& str);
		std::uint32_t addSource(Utils::CopyMovable<IncludeData>&& data)
		{
			IncludeData in = data.get();
			if (in.m_Status == EIncludeStatus::Success)
				return addSource(in.m_Filepath.string(), std::move(in.m_Source));
			return 0;
		}

		std::uint32_t getSourceID(std::string_view name) const;
		Source*       getSource(std::string_view name);
		const Source* getSource(std::string_view name) const;
		Source*       getSource(std::uint32_t id);
		const Source* getSource(std::uint32_t id) const;

		SourcePoint getPoint(std::size_t index, std::uint32_t sourceID) const
		{
			auto source = getSource(sourceID);
			return source ? source->getPoint(index) : SourcePoint {};
		}
		SourceSpan getSpan(std::size_t index, std::size_t length, std::uint32_t sourceID) const
		{
			auto source = getSource(sourceID);
			return source ? source->getSpan(index, length) : SourceSpan {};
		}
		std::string_view getView(std::size_t index, std::size_t length, std::uint32_t sourceID) const
		{
			auto source = getSource(sourceID);
			return source ? source->getView(index, length) : std::string_view {};
		}
		std::string_view getSourceName(std::uint32_t id) const
		{
			auto source = getSource(id);
			return source ? source->getName() : std::string_view {};
		}

		auto& getSources() const { return m_Sources; }

	private:
		std::vector<Source> m_Sources;
	};
} // namespace Frertex::Source