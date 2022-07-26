#pragma once

#include "Utils/Utils.h"

#include <cstddef>
#include <cstdint>

#include <ostream>
#include <string_view>

namespace Frertex
{
	enum class EIncludeStatus
	{
		Failure = 0,
		Success
	};

	struct IncludeData
	{
		EIncludeStatus m_Status;
		std::string    m_Source;
		std::string    m_Filename;
	};

	using IncludeHandler = IncludeData (*)(std::string_view filename, std::string_view originalFilename);

	struct SourcePoint
	{
		std::size_t   m_Index = 0, m_Line = 0, m_Column = 0;
		std::uint32_t m_FileID = 0, m_SourceID = 0;
	};

	struct SourceSpan
	{
		SourcePoint m_Start, m_End;
	};

	std::ostream& operator<<(std::ostream& stream, SourcePoint point);
	std::ostream& operator<<(std::ostream& stream, SourceSpan span);

	struct Source
	{
	public:
		Source(Utils::CopyMovable<std::string>&& name, Utils::CopyMovable<std::string>&& str, std::uint32_t id);

		std::size_t      getLineNumber(std::size_t index) const;
		SourcePoint      getPoint(std::size_t index) const;
		SourceSpan       getSpan(std::size_t index, std::size_t length) const { return { getPoint(index), getPoint(index + length) }; }
		std::string_view getView(std::size_t index, std::size_t length) const { return std::string_view { m_Str.data() + index, length }; }

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

	struct Sources
	{
	public:
		std::uint32_t addSource(Utils::CopyMovable<std::string>&& name, Utils::CopyMovable<std::string>&& str);
		std::uint32_t addSource(Utils::CopyMovable<IncludeData>&& data)
		{
			IncludeData in = data.get();
			if (in.m_Status == EIncludeStatus::Success)
				return addSource(std::move(in.m_Filename), std::move(in.m_Source));
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

	enum class ECharacterClass : std::uint32_t
	{
		Unknown = 0,
		Whitespace,
		Tab,
		Newline,
		NonDigit,
		Digit,
		Symbol
	};

	enum class ETokenClass : std::uint32_t
	{
		Unknown = 0,
		Identifier,
		String,
		Integer,
		BinaryInteger,
		OctalInteger,
		HexInteger,
		Float,
		HexFloat,
		Symbol,
		Preprocessor,
		Comment,
		MultilineComment
	};

	struct Token
	{
	public:
		Token();
		Token(ETokenClass clazz, std::size_t index, std::size_t length, std::uint32_t fileID, std::uint32_t sourceID);

		SourcePoint      getStart(const Sources& sources) const { return sources.getPoint(m_Index, m_SourceID); }
		SourcePoint      getEnd(const Sources& sources) const { return sources.getPoint(m_Index + m_Length, m_SourceID); }
		SourceSpan       getSpan(const Sources& sources) const { return sources.getSpan(m_Index, m_Length, m_SourceID); }
		std::string_view getView(const Sources& sources) const { return sources.getView(m_Index, m_Length, m_SourceID); }
		std::string_view getFilename(const Sources& sources) const { return sources.getSourceName(m_FileID); }

	public:
		ETokenClass   m_Class;
		std::size_t   m_Index, m_Length;
		std::uint32_t m_FileID, m_SourceID;
	};

	std::string_view CharacterClassToString(ECharacterClass clazz);
	std::string_view TokenClassToString(ETokenClass clazz);

	extern ECharacterClass s_CharacterClasses[128];
} // namespace Frertex
