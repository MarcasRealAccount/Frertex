#include "Frertex/Token.h"
#include "Frertex/Utils/Profiler.h"
#include "Frertex/Utils/Utils.h"

#include <sstream>

namespace Frertex
{
	Token::Token()
	    : m_Class(ETokenClass::Unknown), m_Span({ { 0, 0, 0 }, { 0, 0, 0 } }) {}

	Token::Token(ETokenClass clazz, Utils::CopyMovable<std::string>&& str, SourceSpan span)
	    : m_Class(clazz), m_Str(str.get()), m_Span(span) {}

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

	std::ostream& operator<<(std::ostream& stream, const Token& token)
	{
		std::ostringstream str;
		str << TokenClassToString(token.m_Class) << ": " << token.m_Span << ": \"" << Utils::EscapeString(token.m_Str) << '"';
		return stream << str.str();
	}

	ECharacterClass GetCharacterClass(char c)
	{
		PROFILE_FUNC;

		switch (c)
		{
		case '\t': return ECharacterClass::Tab;
		case '\n': return ECharacterClass::Newline;
		case '\v': return ECharacterClass::Whitespace;
		case '\f': return ECharacterClass::Whitespace;
		case '\r': return ECharacterClass::Whitespace;
		case ' ': return ECharacterClass::Whitespace;
		case '!': return ECharacterClass::Symbol;
		case '"': return ECharacterClass::Symbol;
		case '#': return ECharacterClass::Symbol;
		case '$': return ECharacterClass::Symbol;
		case '%': return ECharacterClass::Symbol;
		case '&': return ECharacterClass::Symbol;
		case '\'': return ECharacterClass::Symbol;
		case '(': return ECharacterClass::Symbol;
		case ')': return ECharacterClass::Symbol;
		case '*': return ECharacterClass::Symbol;
		case '+': return ECharacterClass::Symbol;
		case ',': return ECharacterClass::Symbol;
		case '-': return ECharacterClass::Symbol;
		case '.': return ECharacterClass::Symbol;
		case '/': return ECharacterClass::Symbol;
		case '0': return ECharacterClass::Digit;
		case '1': return ECharacterClass::Digit;
		case '2': return ECharacterClass::Digit;
		case '3': return ECharacterClass::Digit;
		case '4': return ECharacterClass::Digit;
		case '5': return ECharacterClass::Digit;
		case '6': return ECharacterClass::Digit;
		case '7': return ECharacterClass::Digit;
		case '8': return ECharacterClass::Digit;
		case '9': return ECharacterClass::Digit;
		case ':': return ECharacterClass::Symbol;
		case ';': return ECharacterClass::Symbol;
		case '<': return ECharacterClass::Symbol;
		case '=': return ECharacterClass::Symbol;
		case '>': return ECharacterClass::Symbol;
		case '?': return ECharacterClass::Symbol;
		case '@': return ECharacterClass::Symbol;
		case 'A': return ECharacterClass::NonDigit;
		case 'B': return ECharacterClass::NonDigit;
		case 'C': return ECharacterClass::NonDigit;
		case 'D': return ECharacterClass::NonDigit;
		case 'E': return ECharacterClass::NonDigit;
		case 'F': return ECharacterClass::NonDigit;
		case 'G': return ECharacterClass::NonDigit;
		case 'H': return ECharacterClass::NonDigit;
		case 'I': return ECharacterClass::NonDigit;
		case 'J': return ECharacterClass::NonDigit;
		case 'K': return ECharacterClass::NonDigit;
		case 'L': return ECharacterClass::NonDigit;
		case 'M': return ECharacterClass::NonDigit;
		case 'N': return ECharacterClass::NonDigit;
		case 'O': return ECharacterClass::NonDigit;
		case 'P': return ECharacterClass::NonDigit;
		case 'Q': return ECharacterClass::NonDigit;
		case 'R': return ECharacterClass::NonDigit;
		case 'S': return ECharacterClass::NonDigit;
		case 'T': return ECharacterClass::NonDigit;
		case 'U': return ECharacterClass::NonDigit;
		case 'V': return ECharacterClass::NonDigit;
		case 'W': return ECharacterClass::NonDigit;
		case 'X': return ECharacterClass::NonDigit;
		case 'Y': return ECharacterClass::NonDigit;
		case 'Z': return ECharacterClass::NonDigit;
		case '[': return ECharacterClass::Symbol;
		case '\\': return ECharacterClass::Symbol;
		case ']': return ECharacterClass::Symbol;
		case '^': return ECharacterClass::Symbol;
		case '_': return ECharacterClass::Symbol;
		case '`': return ECharacterClass::Symbol;
		case 'a': return ECharacterClass::NonDigit;
		case 'b': return ECharacterClass::NonDigit;
		case 'c': return ECharacterClass::NonDigit;
		case 'd': return ECharacterClass::NonDigit;
		case 'e': return ECharacterClass::NonDigit;
		case 'f': return ECharacterClass::NonDigit;
		case 'g': return ECharacterClass::NonDigit;
		case 'h': return ECharacterClass::NonDigit;
		case 'i': return ECharacterClass::NonDigit;
		case 'j': return ECharacterClass::NonDigit;
		case 'k': return ECharacterClass::NonDigit;
		case 'l': return ECharacterClass::NonDigit;
		case 'm': return ECharacterClass::NonDigit;
		case 'n': return ECharacterClass::NonDigit;
		case 'o': return ECharacterClass::NonDigit;
		case 'p': return ECharacterClass::NonDigit;
		case 'q': return ECharacterClass::NonDigit;
		case 'r': return ECharacterClass::NonDigit;
		case 's': return ECharacterClass::NonDigit;
		case 't': return ECharacterClass::NonDigit;
		case 'u': return ECharacterClass::NonDigit;
		case 'v': return ECharacterClass::NonDigit;
		case 'w': return ECharacterClass::NonDigit;
		case 'x': return ECharacterClass::NonDigit;
		case 'y': return ECharacterClass::NonDigit;
		case 'z': return ECharacterClass::NonDigit;
		case '{': return ECharacterClass::Symbol;
		case '|': return ECharacterClass::Symbol;
		case '}': return ECharacterClass::Symbol;
		case '~': return ECharacterClass::Symbol;
		default: return ECharacterClass::Unknown;
		}
	}
} // namespace Frertex
