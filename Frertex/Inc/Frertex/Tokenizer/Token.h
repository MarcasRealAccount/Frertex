// Auto generated

#pragma once

#include <cstdint>

#include <string_view>

namespace Frertex::Tokenizer
{
	enum class ECharacterClass : std::uint16_t
	{
		Unknown = 0,
		Whitespace,
		Newline,
		NonDigit,
		Digit,
		Symbol
	};

	enum class ETokenClass : std::uint16_t
	{
		Unknown,
		Identifier,
		String,
		BinaryInteger,
		OctalInteger,
		DecimalInteger,
		HexInteger,
		Float,
		HexFloat,
		Symbol,
		Comment,
		MultilineComment
	};

	std::string_view TokenClassToString(ETokenClass clazz);

	struct Token
	{
	public:
		ETokenClass   Class;
		std::uint32_t Length;
		std::uint64_t Start;
	};
} // namespace Frertex::Tokenizer