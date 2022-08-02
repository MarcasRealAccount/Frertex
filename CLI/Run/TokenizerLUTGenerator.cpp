#include <iostream>
#include <vector>

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

static constexpr ECharacterClass s_CharacterClasses[256] = {
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
	ECharacterClass::Unknown,
	ECharacterClass::Unknown,
	ECharacterClass::Unknown
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

struct ContextualTokenLUT
{
public:
	std::uint8_t m_State         = 0;
	ETokenClass  m_NewTokenClass = ETokenClass::Unknown;
};

using Callback = ContextualTokenLUT (*)(ECharacterClass characterClass, std::uint16_t character);

int main()
{
	std::vector<Callback> callbacks = {
		[](ECharacterClass characterClass, std::uint16_t character) -> ContextualTokenLUT
		{
		    switch (characterClass)
		    {
		    case ECharacterClass::NonDigit: return { 1, ETokenClass::Identifier };
		    case ECharacterClass::Digit: return { 1, ETokenClass::Integer };
		    case ECharacterClass::Symbol:
			    if ((character & 0b0111'1111) == '#' && character & 0b1000'0000)
				    return { 1, ETokenClass::Preprocessor };
			    else if ((character & 0b0111'1111) == '"')
				    return { 1, ETokenClass::String };
			    else if ((character & 0b0111'1111) == '/')
				    return { 1, ETokenClass::Comment };
			    else
				    return { 1, ETokenClass::Symbol };
		    default: return { 0b10 };
		    }
		},
		[](ECharacterClass characterClass, std::uint16_t character) -> ContextualTokenLUT
		{
		    if (characterClass == ECharacterClass::NonDigit ||
		        characterClass == ECharacterClass::Digit ||
		        (character & 0b0111'1111) == '_')
			    return { 1, ETokenClass::Identifier };
		    return {};
		},
		[](ECharacterClass characterClass, std::uint16_t character) -> ContextualTokenLUT
		{
		    switch (character & 0b0111'111)
		    {
		    case '\\':
			    if (character & 0b1000'0000)
				    return { 1, ETokenClass::String };
			    else
				    return { 0b1000'0001, ETokenClass::String };
		    case '"':
			    if (character & 0b1000'0000)
				    return { 1, ETokenClass::String };
			    else
				    return { 0b101, ETokenClass::String };
		    case '\n':
			    if (character & 0b1000'0000)
				    return { 1, ETokenClass::String };
			    else
				    return {};
		    default:
			    return { 1, ETokenClass::String };
		    }
		},
		[](ECharacterClass characterClass, std::uint16_t character) -> ContextualTokenLUT
		{
		    if (characterClass == ECharacterClass::Digit)
			    return { 1, ETokenClass::Integer };

		    switch (character & 0b0111'1111)
		    {
		    case 'b':
		    case 'B': return { 1, ETokenClass::BinaryInteger };
		    case 'o':
		    case 'O': return { 1, ETokenClass::OctalInteger };
		    case 'x':
		    case 'X': return { 1, ETokenClass::HexInteger };
		    case 'e':
		    case 'E':
		    case 'h':
		    case 'H':
		    case 'f':
		    case 'F':
		    case 'd':
		    case 'D':
		    case '.': return { 1, ETokenClass::Float };
		    case 'u':
		    case 'U':
		    case 's':
		    case 'S':
		    case 'l':
		    case 'L':
		    case '\'': return { 1, ETokenClass::Integer };
		    default: return {};
		    }
		},
		[](ECharacterClass characterClass, std::uint16_t character) -> ContextualTokenLUT
		{
		    if (characterClass == ECharacterClass::Digit)
			    return { 1, ETokenClass::BinaryInteger };

		    switch (character & 0b0111'1111)
		    {
		    case 's':
		    case 'S':
		    case 'l':
		    case 'L':
		    case '\'': return { 1, ETokenClass::BinaryInteger };
		    default: return {};
		    }
		},
		[](ECharacterClass characterClass, std::uint16_t character) -> ContextualTokenLUT
		{
		    if (characterClass == ECharacterClass::Digit)
			    return { 1, ETokenClass::OctalInteger };

		    switch (character & 0b0111'1111)
		    {
		    case 's':
		    case 'S':
		    case 'l':
		    case 'L':
		    case '\'': return { 1, ETokenClass::OctalInteger };
		    default: return {};
		    }
		},
		[](ECharacterClass characterClass, std::uint16_t character) -> ContextualTokenLUT
		{
		    if (characterClass == ECharacterClass::Digit)
			    return { 1, ETokenClass::HexInteger };

		    switch (character & 0b0111'1111)
		    {
		    case 'p':
		    case 'P':
		    case '.': return { 1, ETokenClass::Float };
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
		    case 's':
		    case 'S':
		    case 'l':
		    case 'L':
		    case '\'': return { 1, ETokenClass::HexInteger };
		    default: return {};
		    }
		},
		[](ECharacterClass characterClass, std::uint16_t character) -> ContextualTokenLUT
		{
		    if (characterClass == ECharacterClass::Digit)
			    return { 1, ETokenClass::Float };

		    switch (character)
		    {
		    case 'h':
		    case 'H':
		    case 'f':
		    case 'F':
		    case 'd':
		    case 'D': return { 1, ETokenClass::Float };
		    default: return {};
		    }
		},
		[](ECharacterClass characterClass, std::uint16_t character) -> ContextualTokenLUT
		{
		    if (characterClass == ECharacterClass::Digit)
			    return { 1, ETokenClass::HexFloat };

		    switch (character)
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
		    case 'h':
		    case 'H': return { 1, ETokenClass::HexFloat };
		    default: return {};
		    }
		},
		[](ECharacterClass characterClass, std::uint16_t character) -> ContextualTokenLUT
		{
		    return {};
		},
		[](ECharacterClass characterClass, std::uint16_t character) -> ContextualTokenLUT
		{
		    // TODO(MarcasRealAccount): Enable this:
		    /*switch (character & 0b0111'111)
            {
            case '\\':
                if (character & 0b1000'0000)
                    return { 1, ETokenClass::Preprocessor };
                else
                    return { 0b1000'0001, ETokenClass::Preprocessor };
            case '\n':
                if (character & 0b1000'0000)
                    return { 1, ETokenClass::Preprocessor };
                else
                    return {};
            default:
                return { 1, ETokenClass::Preprocessor };
            }*/
		    if ((character & 0b0111'1111) == '\n')
			    return {};
		    return { 1, ETokenClass::Preprocessor };
		},
		[](ECharacterClass characterClass, std::uint16_t character) -> ContextualTokenLUT
		{
		    if ((character & 0b1000'0000) && (character & 0b0111'1111) == '*')
			    return { 1, ETokenClass::MultilineComment };

		    if ((character & 0b0111'1111) == '\n')
			    return {};
		    return { 1, ETokenClass::Comment };
		},
		[](ECharacterClass characterClass, std::uint16_t character) -> ContextualTokenLUT
		{
		    switch (character)
		    {
		    case '/': return { 0b1001, ETokenClass::MultilineComment };
		    case '*':
			    if (character & 0b1000'0000)
				    return { 0b101, ETokenClass::MultilineComment };
			    else
				    return { 1, ETokenClass::MultilineComment };
		    default: return { 1, ETokenClass::MultilineComment };
		    }
		}
	};

	std::cout << "static constexpr ContextualTokenLUT s_ContextualTokenLUT[13][256] =\n{\n";
	for (std::size_t i = 0; i < 13; ++i)
	{
		auto& callback = callbacks[i];

		if (i > 0)
			std::cout << ",\n";
		std::cout << "\t{\n";
		for (std::uint16_t j = 0; j < 256; ++j)
		{
			if (j > 0)
				std::cout << ",\n";
			std::cout << "\t\t{\n";

			auto value = callback(s_CharacterClasses[j & 0b0111'1111], j);
			std::cout << "\t\t\t" << static_cast<std::uint64_t>(value.m_State) << ",\n";
			std::cout << "\t\t\tETokenClass::" << TokenClassToString(value.m_NewTokenClass) << "\n";

			std::cout << "\t\t}";
		}
		std::cout << "\n\t}";
	}
	std::cout << "\n};\n";
}
