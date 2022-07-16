#pragma once

#include "Utils/Utils.h"

#include <cstddef>
#include <cstdint>

#include <ostream>
#include <string_view>

namespace Frertex
{
	enum class ECharacterClass : std::uint32_t
	{
		Unknown = 0,
		Whitespace,
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

	struct SourcePoint
	{
		std::size_t m_Index, m_Line, m_Column;
		std::size_t m_File = 0, m_ActualFile = 0;
	};

	struct SourceSpan
	{
		SourcePoint m_Start, m_End;
	};

	struct Token
	{
		Token();
		Token(ETokenClass clazz, Utils::CopyMovable<std::string>&& str, SourceSpan span);

		ETokenClass m_Class;
		std::string m_Str;
		SourceSpan  m_Span;
	};

	std::string_view CharacterClassToString(ECharacterClass clazz);
	std::string_view TokenClassToString(ETokenClass clazz);

	std::ostream& operator<<(std::ostream& stream, SourcePoint point);
	std::ostream& operator<<(std::ostream& stream, SourceSpan span);
	std::ostream& operator<<(std::ostream& stream, const Token& token);

	ECharacterClass GetCharacterClass(char c);
} // namespace Frertex