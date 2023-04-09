#pragma once

#include <cstdint>

#include <string_view>

namespace Frertex::Tokenizer
{
	enum class ECharacterClass : std::uint8_t
	{
		Unknown = 0,
		Whitespace,
		Tab,
		Newline,
		NonDigit,
		Digit,
		Symbol
	};

	enum class ETokenClass : std::uint8_t
	{
		Unknown = 0,
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
		MultilineComment,
	};

	std::string_view CharacterClassToString(ECharacterClass clazz);
	std::string_view TokenClassToString(ETokenClass clazz);

	static constexpr std::uint8_t ContextualTokenStep      = 0x01;
	static constexpr std::uint8_t ContextualTokenEnd       = 0x02;
	static constexpr std::uint8_t ContextualTokenPassState = 0x80;

	struct ContextualToken
	{
	public:
		std::uint8_t State = 0;
		ETokenClass  Class = ETokenClass::Unknown;
	};

	struct Token
	{
	public:
		ETokenClass   Class;
		std::uint8_t  Pad[3];
		std::uint32_t Length;
		std::uint64_t Start;
	};

	extern ECharacterClass c_CharacterClassesLUT[128];
	extern ContextualToken c_ContextualTokenLUT[13][256];
} // namespace Frertex::Tokenizer