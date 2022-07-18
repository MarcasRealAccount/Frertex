#include "Frertex/Token.h"
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
		switch (c)
		{
		case '\t': return ECharacterClass::Whitespace;
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
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': return ECharacterClass::Digit;
		case ':': return ECharacterClass::Symbol;
		case ';': return ECharacterClass::Symbol;
		case '<': return ECharacterClass::Symbol;
		case '=': return ECharacterClass::Symbol;
		case '>': return ECharacterClass::Symbol;
		case '?': return ECharacterClass::Symbol;
		case '@': return ECharacterClass::Symbol;
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'H':
		case 'I':
		case 'J':
		case 'K':
		case 'L':
		case 'M':
		case 'N':
		case 'O':
		case 'P':
		case 'Q':
		case 'R':
		case 'S':
		case 'T':
		case 'U':
		case 'V':
		case 'W':
		case 'X':
		case 'Y':
		case 'Z': return ECharacterClass::NonDigit;
		case '[': return ECharacterClass::Symbol;
		case '\\': return ECharacterClass::Symbol;
		case ']': return ECharacterClass::Symbol;
		case '^': return ECharacterClass::Symbol;
		case '_': return ECharacterClass::Symbol;
		case '`': return ECharacterClass::Symbol;
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z': return ECharacterClass::NonDigit;
		case '{': return ECharacterClass::Symbol;
		case '|': return ECharacterClass::Symbol;
		case '}': return ECharacterClass::Symbol;
		case '~': return ECharacterClass::Symbol;
		default: return ECharacterClass::Unknown;
		}
	}
} // namespace Frertex