#include "Frertex/Token.h"
#include "Frertex/Utils/Profiler.h"
#include "Frertex/Utils/Utils.h"

#include <sstream>

namespace Frertex
{
	std::ostream& operator<<(std::ostream& stream, SourcePoint point)
	{
		std::ostringstream str;
		str << (point.m_Line + 1) << ':' << (point.m_Column + 1);
		return stream << str.str();
	}

	std::ostream& operator<<(std::ostream& stream, SourceSpan span)
	{
		std::ostringstream str;
		str << span.m_Start << " -> " << span.m_End;
		return stream << str.str();
	}

	Source::Source(Utils::CopyMovable<std::string>&& name, Utils::CopyMovable<std::string>&& str, std::uint32_t id)
	    : m_Name(name.get()), m_Str(str.get()), m_ID(id)
	{
		m_Indices.emplace_back(0);
		for (std::size_t i = 0; i < m_Str.size(); ++i)
			if (m_Str[i] == '\n')
				m_Indices.emplace_back(i + 1);
	}

	std::size_t Source::getLineNumber(std::size_t index) const
	{
		auto low  = m_Indices.begin();
		auto high = m_Indices.end();

		while (high - low > 1)
		{
			auto mid = low + (high - low) / 2;
			if (*mid > index)
				high = mid - 1;
			else
				low = mid;
		}

		std::size_t line = low - m_Indices.begin();
		return line;
	}

	SourcePoint Source::getPoint(std::size_t index) const
	{
		std::size_t line   = getLineNumber(index);
		std::size_t column = index - m_Indices[line];

		return SourcePoint { index, line, column, m_ID, m_ID };
	}

	std::uint32_t Sources::addSource(Utils::CopyMovable<std::string>&& name, Utils::CopyMovable<std::string>&& str)
	{
		return m_Sources.emplace_back(name.get(), str.get(), static_cast<std::uint32_t>(m_Sources.size() + 1)).getID();
	}

	std::uint32_t Sources::getSourceID(std::string_view name) const
	{
		for (auto& source : m_Sources)
			if (source.getName() == name)
				return source.getID();
		return 0;
	}

	Source* Sources::getSource(std::string_view name)
	{
		for (auto& source : m_Sources)
			if (source.getName() == name)
				return &source;
		return nullptr;
	}

	const Source* Sources::getSource(std::string_view name) const
	{
		for (auto& source : m_Sources)
			if (source.getName() == name)
				return &source;
		return nullptr;
	}

	Source* Sources::getSource(std::uint32_t id)
	{
		return (id - 1) < m_Sources.size() ? &m_Sources[id - 1] : nullptr;
	}

	const Source* Sources::getSource(std::uint32_t id) const
	{
		return (id - 1) < m_Sources.size() ? &m_Sources[id - 1] : nullptr;
	}

	Token::Token()
	    : m_Class(ETokenClass::Unknown), m_Index(0ULL), m_Length(0ULL), m_FileID(0U), m_SourceID(0U) {}

	Token::Token(ETokenClass clazz, std::size_t index, std::size_t length, std::uint32_t fileID, std::uint32_t sourceID)
	    : m_Class(clazz), m_Index(index), m_Length(length), m_FileID(fileID), m_SourceID(sourceID) {}

	std::string_view CharacterClassToString(ECharacterClass clazz)
	{
		switch (clazz)
		{
		case ECharacterClass::Unknown: return "Unknown";
		case ECharacterClass::Whitespace: return "Whitespace";
		case ECharacterClass::Tab: return "Tab";
		case ECharacterClass::Newline: return "Newline";
		case ECharacterClass::NonDigit: return "NonDigit";
		case ECharacterClass::Digit: return "Digit";
		case ECharacterClass::Symbol: return "Symbol";
		default: return "Unknown";
		}
	}

	std::string_view TokenClassToString(ETokenClass clazz)
	{
		switch (clazz)
		{
		case ETokenClass::Unknown: return "Unknown";
		case ETokenClass::Identifier: return "Identifier";
		case ETokenClass::String: return "String";
		case ETokenClass::Integer: return "Integer";
		case ETokenClass::BinaryInteger: return "BinaryInteger";
		case ETokenClass::OctalInteger: return "OctalInteger";
		case ETokenClass::HexInteger: return "HexInteger";
		case ETokenClass::Float: return "Float";
		case ETokenClass::HexFloat: return "HexFloat";
		case ETokenClass::Symbol: return "Symbol";
		case ETokenClass::Preprocessor: return "Preprocessor";
		case ETokenClass::Comment: return "Comment";
		case ETokenClass::MultilineComment: return "MultilineComment";
		default: return "Unknown";
		}
	}

	ECharacterClass s_CharacterClasses[128] = {
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Tab,
		ECharacterClass::Newline,
		ECharacterClass::Whitespace,
		ECharacterClass::Whitespace,
		ECharacterClass::Whitespace,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Whitespace,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Digit,
		ECharacterClass::Digit,
		ECharacterClass::Digit,
		ECharacterClass::Digit,
		ECharacterClass::Digit,
		ECharacterClass::Digit,
		ECharacterClass::Digit,
		ECharacterClass::Digit,
		ECharacterClass::Digit,
		ECharacterClass::Digit,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Unknown
	};
} // namespace Frertex
