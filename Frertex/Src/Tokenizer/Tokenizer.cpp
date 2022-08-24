#include "Frertex/Tokenizer/Tokenizer.h"
#include "Frertex/Utils/Profiler.h"

namespace Frertex::Tokenizer
{
	std::vector<Token> Tokenize(const Source::Source* source, std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (!source || end < start)
			return {};

		auto  id  = source->getID();
		auto& str = source->getStr();
		end       = std::min(end, str.size());

		std::vector<Token> tokens;

		std::size_t current            = start;
		ETokenClass tokenClass         = ETokenClass::Unknown;
		ETokenClass previousTokenClass = tokenClass;

		std::size_t tokenLength = 0;
		std::size_t tokenStart  = start;
		std::size_t tokenEnd    = start;
		bool        escaped     = false;
		bool        firstColumn = true;

		char c              = str[start];
		auto characterClass = s_CharacterClassLUT[static_cast<std::uint8_t>(c) & 0x7F];
		while (current <= end)
		{
			bool extraState = false;
			switch (tokenClass)
			{
			case ETokenClass::Unknown:
				extraState  = firstColumn;
				tokenStart  = current;
				tokenEnd    = current;
				tokenLength = 0;
				break;
			case ETokenClass::String:
			case ETokenClass::Preprocessor:
			case ETokenClass::MultilineComment:
				extraState = escaped;
				break;
			case ETokenClass::Comment:
				extraState = tokenLength == 1;
				break;
			default:
				break;
			}

			auto contextualToken = s_ContextualTokenLUT[static_cast<std::uint32_t>(tokenClass) & 0x7FFF'FFFF][(c & 0b0111'1111) | (extraState << 7)];

			escaped            = contextualToken.m_State & 0b1000'0000;
			previousTokenClass = tokenClass;
			tokenClass         = contextualToken.m_NewTokenClass;

			bool addToToken = contextualToken.m_State & 1;

			if (contextualToken.m_State & 0b111)
			{
				firstColumn = false;
				tokenEnd    = current;
				++current;
				if (characterClass == ECharacterClass::Newline)
					firstColumn = true;
				if (tokenClass != ETokenClass::Unknown)
					++tokenLength;
				if (current <= end)
				{
					c              = str[current];
					characterClass = s_CharacterClassLUT[static_cast<std::uint8_t>(c) & 0x7F];
				}
			}

			if ((!addToToken || contextualToken.m_State & 0b100) && tokenLength)
			{
				if (tokenClass == ETokenClass::String)
				{
					++tokenStart;
					--tokenEnd;
				}
				else if (previousTokenClass == ETokenClass::LastSymbol && characterClass == ECharacterClass::Symbol)
				{
					previousTokenClass = ETokenClass::FirstSymbol;
				}

				tokens.emplace_back(previousTokenClass, tokenStart, tokenEnd - tokenStart + 1, id, id);
				tokenLength = 0;
				tokenClass  = ETokenClass::Unknown;
			}
		}

		if (tokenLength)
		{
			if (tokenClass == ETokenClass::String)
			{
				++tokenStart;
				--tokenEnd;
			}

			tokens.emplace_back(tokenClass, tokenStart, tokenEnd - tokenStart + 1, id, id);
		}

		return tokens;
	}
} // namespace Frertex::Tokenizer