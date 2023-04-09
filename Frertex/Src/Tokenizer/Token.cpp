#include "Frertex/Tokenizer/Token.h"

namespace Frertex::Tokenizer
{
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
		}
		return "Unknown";
	}

	std::string_view TokenClassToString(ETokenClass clazz)
	{
		switch (clazz)
		{
		case ETokenClass::Unknown: return "Unknown";
		case ETokenClass::Identifier: return "Identifier";
		case ETokenClass::String: return "String";
		case ETokenClass::BinaryInteger: return "BinaryInteger";
		case ETokenClass::OctalInteger: return "OctalInteger";
		case ETokenClass::DecimalInteger: return "DecimalInteger";
		case ETokenClass::HexInteger: return "HexInteger";
		case ETokenClass::Float: return "Float";
		case ETokenClass::HexFloat: return "HexFloat";
		case ETokenClass::Symbol: return "Symbol";
		case ETokenClass::Comment: return "Comment";
		case ETokenClass::MultilineComment: return "MultilineComment";
		}
		return "Unknown";
	}
} // namespace Frertex::Tokenizer