#include "Frertex/Tokenizer.h"

#include <sstream>

namespace Frertex
{
	std::vector<Token> Tokenize(std::string_view str, SourcePoint start)
	{
		std::vector<Token> tokens;

		std::size_t     tokenLength  = 0;
		SourcePoint     tokenStart   = start;
		SourcePoint     tokenEnd     = start;
		SourcePoint     current      = start;
		ECharacterClass currentClass = ECharacterClass::Unknown;
		ETokenClass     tokenClass   = ETokenClass::Unknown;
		bool            escaped      = false;
		bool            stringEnded  = false;
		while (current.m_Index < (str.size() + start.m_Index))
		{
			char c = str[current.m_Index - start.m_Index];

			ECharacterClass characterClass = GetCharacterClass(c);
			bool            addToken       = characterClass != currentClass || currentClass == ECharacterClass::Symbol;
			currentClass                   = characterClass;
			if (tokenClass == ETokenClass::Identifier)
			{
				if (characterClass == ECharacterClass::Digit ||
				    (characterClass == ECharacterClass::Symbol && c == '_'))
					addToken = false;
			}
			else if (tokenClass == ETokenClass::String)
			{
				if (!stringEnded)
				{
					if (characterClass == ECharacterClass::Symbol)
					{
						switch (c)
						{
						case '\\':
							escaped = true;
							break;
						case '"':
							if (!escaped)
								stringEnded = true;
							addToken = false;
							escaped  = false;
							break;
						default:
							addToken = false;
							escaped  = false;
							break;
						}
					}
					else if (characterClass == ECharacterClass::Newline)
					{
						if (escaped)
							addToken = false;
						escaped = false;
					}
					else
					{
						addToken = false;
						escaped  = false;
					}
				}
			}
			else if (tokenClass == ETokenClass::Integer)
			{
				if (characterClass == ECharacterClass::Symbol && c == '.')
				{
					tokenClass = ETokenClass::Float;
					addToken   = false;
				}
				else if (characterClass == ECharacterClass::NonDigit)
				{
					switch (c)
					{
					case 'b':
						tokenClass = ETokenClass::BinaryInteger;
						addToken   = false;
						break;
					case 'o':
						tokenClass = ETokenClass::OctalInteger;
						addToken   = false;
						break;
					case 'x':
						tokenClass = ETokenClass::HexInteger;
						addToken   = false;
						break;
					case 'e':
					case 'E':
						tokenClass = ETokenClass::Float;
						addToken   = false;
						break;
					}
				}
			}
			else if (tokenClass == ETokenClass::HexInteger)
			{
				if (characterClass == ECharacterClass::Symbol && c == '.')
				{
					tokenClass = ETokenClass::HexFloat;
					addToken   = false;
				}
				else if (characterClass == ECharacterClass::NonDigit)
				{
					switch (c)
					{
					case 'a':
					case 'b':
					case 'c':
					case 'd':
					case 'e':
					case 'f':
					case 'A':
					case 'B':
					case 'C':
					case 'D':
					case 'E':
					case 'F':
						addToken = false;
						break;
					case 'p':
					case 'P':
						tokenClass = ETokenClass::HexFloat;
						addToken   = false;
						break;
					}
				}
			}
			else if (tokenClass == ETokenClass::Float)
			{
				if (characterClass == ECharacterClass::Digit)
				{
					addToken = false;
				}
				else if (characterClass == ECharacterClass::NonDigit)
				{
					switch (c)
					{
					case 'h':
					case 'H':
					case 'f':
					case 'F':
					case 'd':
					case 'D':
						addToken = false;
						break;
					}
				}
			}
			else if (tokenClass == ETokenClass::HexFloat)
			{
				if (characterClass == ECharacterClass::Digit)
				{
					addToken = false;
				}
				else if (characterClass == ECharacterClass::NonDigit)
				{
					switch (c)
					{
					case 'a':
					case 'b':
					case 'c':
					case 'd':
					case 'e':
					case 'f':
					case 'h':
					case 'A':
					case 'B':
					case 'C':
					case 'D':
					case 'E':
					case 'F':
					case 'H':
						addToken = false;
						break;
					}
				}
			}
			else if (tokenClass == ETokenClass::Preprocessor)
			{
				if (characterClass != ECharacterClass::Newline)
					addToken = false;
			}

			if (addToken)
			{
				if (tokenLength > 0)
				{
					if (tokenClass == ETokenClass::String)
					{
						++tokenStart.m_Index;
						++tokenStart.m_Column;
						--tokenEnd.m_Index;
						--tokenEnd.m_Column;
					}
					tokens.emplace_back(tokenClass, str.substr(tokenStart.m_Index - start.m_Index, 1 + tokenEnd.m_Index - tokenStart.m_Index), SourceSpan { tokenStart, tokenEnd });
					tokenLength = 0;
					tokenClass  = ETokenClass::Unknown;
					stringEnded = false;
				}
				tokenStart = current;
				tokenEnd   = current;
			}

			if (tokenClass == ETokenClass::Unknown)
			{
				switch (characterClass)
				{
				case ECharacterClass::NonDigit:
					tokenClass = ETokenClass::Identifier;
					break;
				case ECharacterClass::Digit:
					tokenClass = ETokenClass::Integer;
					break;
				case ECharacterClass::Symbol:
					tokenClass = (c == '#' && current.m_Column == 0) ? ETokenClass::Preprocessor : (c == '"' ? ETokenClass::String : ETokenClass::Symbol);
					break;
				case ECharacterClass::Whitespace:
				case ECharacterClass::Newline:
					tokenClass = ETokenClass::Unknown;
					break;
				}
			}

			tokenEnd = current;
			++current.m_Index;
			++current.m_Column;
			if (characterClass == ECharacterClass::Newline)
			{
				++current.m_Line;
				current.m_Column = 0;
			}
			if (tokenClass != ETokenClass::Unknown)
				++tokenLength;
		}

		if (tokenLength > 0)
		{
			if (tokenClass == ETokenClass::String)
			{
				++tokenStart.m_Index;
				++tokenStart.m_Column;
				--tokenEnd.m_Index;
				--tokenEnd.m_Column;
			}
			tokens.emplace_back(tokenClass, str.substr(tokenStart.m_Index - start.m_Index, 1 + tokenEnd.m_Index - tokenStart.m_Index), SourceSpan { tokenStart, tokenEnd });
		}

		return tokens;
	}
} // namespace Frertex