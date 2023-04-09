#include "TokenLUT.h"

#include <cctype>

#include <fstream>
#include <sstream>
#include <string>

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
	case ETokenClass::BinaryInteger: return "BinaryInteger";
	case ETokenClass::OctalInteger: return "OctalInteger";
	case ETokenClass::DecimalInteger: return "DecimalInteger";
	case ETokenClass::HexInteger: return "HexInteger";
	case ETokenClass::Float: return "Float";
	case ETokenClass::HexFloat: return "HexFloat";
	case ETokenClass::Symbol: return "Symbol";
	case ETokenClass::Comment: return "Comment";
	case ETokenClass::MultilineComment: return "MultilineComment";
	default: return "Unknown";
	}
}

using Callback = ContextualToken (*)(ECharacterClass characterClass, std::uint8_t character);

static ContextualToken UnknownToken(ECharacterClass characterClass, std::uint8_t character)
{
	switch (characterClass)
	{
	case ECharacterClass::NonDigit: return { ContextualTokenStep, ETokenClass::Identifier };
	case ECharacterClass::Digit: return { ContextualTokenStep, ETokenClass::DecimalInteger };
	case ECharacterClass::Symbol:
		switch (character & 0x7F)
		{
		case '"': return { ContextualTokenStep, ETokenClass::String };
		case '/': return { static_cast<std::uint8_t>(ContextualTokenPassState | ContextualTokenStep), ETokenClass::Comment };
		default: return { ContextualTokenPassState | ContextualTokenStep, ETokenClass::Symbol };
		}
	default: return { ContextualTokenEnd | ContextualTokenStep };
	}
}

static ContextualToken IdentifierToken(ECharacterClass characterClass, std::uint8_t character)
{
	if (characterClass == ECharacterClass::NonDigit ||
		characterClass == ECharacterClass::Digit ||
		(character & 0x7F) == '_')
		return { ContextualTokenStep, ETokenClass::Identifier };
	return { ContextualTokenEnd };
}

static ContextualToken StringToken(ECharacterClass characterClass, std::uint8_t character)
{
	switch (character & 0x7F)
	{
	case '\\':
		if (character & ContextualTokenPassState)
			return { ContextualTokenStep, ETokenClass::String };
		else
			return { ContextualTokenPassState | ContextualTokenStep, ETokenClass::String };
	case '"':
		if (character & ContextualTokenPassState)
			return { ContextualTokenStep, ETokenClass::String };
		else
			return { ContextualTokenEnd | ContextualTokenStep };
	case '\n':
		if (character & ContextualTokenPassState)
			return { ContextualTokenStep, ETokenClass::String };
		else
			return { ContextualTokenEnd };
	default:
		return { ContextualTokenStep, ETokenClass::String };
	}
}

static ContextualToken BinaryIntegerToken(ECharacterClass characterClass, std::uint8_t character)
{
	if (characterClass == ECharacterClass::Digit)
		return { ContextualTokenStep, ETokenClass::BinaryInteger };

	switch (character & 0x7F)
	{
	case '\'': return { ContextualTokenStep, ETokenClass::BinaryInteger };
	default: return { ContextualTokenEnd };
	}
}

static ContextualToken OctalIntegerToken(ECharacterClass characterClass, std::uint8_t character)
{
	if (characterClass == ECharacterClass::Digit)
		return { ContextualTokenStep, ETokenClass::OctalInteger };

	switch (character & 0x7F)
	{
	case '\'': return { ContextualTokenStep, ETokenClass::OctalInteger };
	default: return { ContextualTokenEnd };
	}
}

static ContextualToken DecimalIntegerToken(ECharacterClass characterClass, std::uint8_t character)
{
	if (characterClass == ECharacterClass::Digit)
		return { ContextualTokenStep, ETokenClass::DecimalInteger };

	switch (character & 0x7F)
	{
	case 'b':
	case 'B': return { ContextualTokenStep, ETokenClass::BinaryInteger };
	case 'o':
	case 'O': return { ContextualTokenStep, ETokenClass::OctalInteger };
	case 'x':
	case 'X': return { ContextualTokenStep, ETokenClass::HexInteger };
	case 'e':
	case 'E':
	case '.': return { ContextualTokenStep, ETokenClass::Float };
	case '\'': return { ContextualTokenStep, ETokenClass::DecimalInteger };
	default: return { ContextualTokenEnd };
	}
}

static ContextualToken HexIntegerToken(ECharacterClass characterClass, std::uint8_t character)
{
	if (characterClass == ECharacterClass::Digit)
		return { ContextualTokenStep, ETokenClass::HexInteger };

	switch (character & 0x7F)
	{
	case 'p':
	case 'P':
	case '.': return { ContextualTokenStep, ETokenClass::HexFloat };
	case 'a':
	case 'A':
	case 'b':
	case 'B':
	case 'c':
	case 'C':
	case 'd':
	case 'D':
	case 'e':
	case 'E':
	case 'f':
	case 'F':
	case '\'': return { ContextualTokenStep, ETokenClass::HexInteger };
	default: return { ContextualTokenEnd };
	}
}

static ContextualToken FloatToken(ECharacterClass characterClass, std::uint8_t character)
{
	if (characterClass == ECharacterClass::Digit)
		return { ContextualTokenStep, ETokenClass::Float };

	switch (character & 0x7F)
	{
	case 'h':
	case 'H':
	case 'f':
	case 'F':
	case 'd':
	case 'D':
	case 'e':
	case 'E':
	case '\'': return { ContextualTokenStep, ETokenClass::Float };
	default: return { ContextualTokenEnd };
	}
}

static ContextualToken HexFloatToken(ECharacterClass characterClass, std::uint8_t character)
{
	if (characterClass == ECharacterClass::Digit)
		return { ContextualTokenStep, ETokenClass::HexFloat };

	switch (character & 0x7F)
	{
	case 'a':
	case 'A':
	case 'b':
	case 'B':
	case 'c':
	case 'C':
	case 'd':
	case 'D':
	case 'e':
	case 'E':
	case 'f':
	case 'F':
	case 'p':
	case 'P':
	case '\'': return { ContextualTokenStep, ETokenClass::HexFloat };
	default: return { ContextualTokenEnd };
	}
}

static ContextualToken SymbolToken(ECharacterClass characterClass, std::uint8_t character)
{
	return { ContextualTokenEnd };
	// if (character & ContextualTokenPassState)
	//{
	//	if (characterClass != ECharacterClass::Symbol)
	//		return { ContextualTokenEnd };
	//	else
	//		return { ContextualTokenEnd | ContextualTokenStep };
	// }
	// return { ContextualTokenPassState | ContextualTokenStep, ETokenClass::Symbol };
}

static ContextualToken CommentToken(ECharacterClass characterClass, std::uint8_t character)
{
	if (character & ContextualTokenPassState)
	{
		switch (character & 0x7F)
		{
		case '/': return { ContextualTokenStep, ETokenClass::Comment };
		case '*': return { ContextualTokenStep, ETokenClass::MultilineComment };
		default: return { 0, ETokenClass::Symbol };
		}
	}

	if ((character & 0x7F) == '\n')
		return { ContextualTokenEnd };
	return { ContextualTokenStep, ETokenClass::Comment };
}

static ContextualToken MultilineCommentToken(ECharacterClass characterClass, std::uint8_t character)
{
	switch (character & 0x7F)
	{
	case '*':
		if (character & ContextualTokenPassState)
			return { ContextualTokenStep, ETokenClass::MultilineComment };
		else
			return { ContextualTokenPassState | ContextualTokenStep, ETokenClass::MultilineComment };
	case '/':
		if (character & ContextualTokenPassState)
			return { ContextualTokenEnd | ContextualTokenStep, ETokenClass::Unknown };
		else
			return { ContextualTokenStep, ETokenClass::MultilineComment };
	default:
		return { ContextualTokenStep, ETokenClass::MultilineComment };
	}
}

static Callback s_Callbacks[12] {
	&UnknownToken,
	&IdentifierToken,
	&StringToken,
	&BinaryIntegerToken,
	&OctalIntegerToken,
	&DecimalIntegerToken,
	&HexIntegerToken,
	&FloatToken,
	&HexFloatToken,
	&SymbolToken,
	&CommentToken,
	&MultilineCommentToken
};

void GenerateTokenLUT()
{
	ECharacterClass characterClasses[128];
	ContextualToken contextualTokens[12][256];

	// Generate character classes
	for (std::uint8_t i = 0; i < 128; ++i)
	{
		switch (i)
		{
		case '\t': characterClasses[i] = ECharacterClass::Tab; break;
		case '\n': characterClasses[i] = ECharacterClass::Newline; break;
		case '\v': characterClasses[i] = ECharacterClass::Whitespace; break;
		case '\f': characterClasses[i] = ECharacterClass::Whitespace; break;
		case '\r': characterClasses[i] = ECharacterClass::Whitespace; break;
		case ' ': characterClasses[i] = ECharacterClass::Whitespace; break;
		default:
		{
			if (std::isalpha(i))
			{
				characterClasses[i] = ECharacterClass::NonDigit;
			}
			else if (std::isdigit(i))
			{
				characterClasses[i] = ECharacterClass::Digit;
			}
			else if (std::ispunct(i))
			{
				characterClasses[i] = ECharacterClass::Symbol;
			}
			else
			{
				characterClasses[i] = ECharacterClass::Unknown;
			}
			break;
		}
		}
	}

	// Generate contextual tokens
	for (std::uint8_t i = 0; i < 12; ++i)
	{
		auto callback = s_Callbacks[i];
		for (std::uint16_t j = 0; j < 256; ++j)
		{
			auto value             = callback(characterClasses[j & 0b0111'1111], static_cast<std::uint8_t>(j));
			contextualTokens[i][j] = value;
		}
	}

	// Convert to string
	std::ostringstream str {};
	str << "ECharacterClass c_CharacterClassesLUT[128] { ";
	for (std::uint8_t i = 0; i < 128; ++i)
	{
		if (i > 0)
			str << ", ";
		str << "ECharacterClass::" << CharacterClassToString(characterClasses[i]);
	}
	str << " };\n";

	str << "ContextualToken c_ContextualTokenLUT[13][256] {\n";
	for (std::uint8_t i = 0; i < 12; ++i)
	{
		auto& tokens = contextualTokens[i];
		str << "\t{\n";
		for (std::uint16_t j = 0; j < 256; ++j)
		{
			auto value = tokens[j];
			str << "\t\t{ ";
			str << static_cast<std::uint64_t>(value.State) << ", "
				<< "ETokenClass::" << TokenClassToString(value.NewTokenClass);
			str << " }";
			if (j < 255)
				str << ",\n";
			else
				str << '\n';
		}
		str << "\t}";
		if (i < 11)
			str << ",\n";
		else
			str << '\n';
	}
	str << "};\n";

	std::ofstream file("TokenLUT.cpp");
	if (!file)
		return;
	file << str.str();
	file.close();
}