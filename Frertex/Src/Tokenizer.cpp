#include "Frertex/Tokenizer.h"
#include "Frertex/Utils/Profiler.h"

#include <iostream>
#include <sstream>

namespace Frertex
{
	std::vector<Token> Tokenize(const Source* source, std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (!source || end <= start)
			return {};

		auto  id  = source->getID();
		auto& str = source->getStr();
		end       = std::min(end, str.size());

		std::vector<Token> tokens;

		std::size_t current            = start;
		ETokenClass tokenClass         = ETokenClass::Unknown;
		ETokenClass previousTokenClass = tokenClass;

		std::size_t tokenLength       = 0;
		std::size_t tokenStart        = start;
		std::size_t tokenEnd          = start;
		bool        escaped           = false;
		std::size_t multilineCommentI = 0;
		bool        firstColumn       = true;

		char c              = str[start];
		auto characterClass = s_CharacterClasses[static_cast<std::uint8_t>(c) & 0x7F];
		while (current < end)
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
				extraState = escaped;
				break;
			case ETokenClass::Comment:
				extraState = tokenLength == 1;
				break;
			case ETokenClass::MultilineComment:
				extraState = multilineCommentI == 0;
				break;
			default:
				break;
			}

			auto contextualLookup = s_ContextualTokenLUT[static_cast<std::uint32_t>(tokenClass)][(c & 0b0111'1111) | (extraState << 7)];

			if (tokenClass == ETokenClass::Comment && contextualLookup.m_NewTokenClass == ETokenClass::MultilineComment)
				multilineCommentI = 1;
			if (tokenClass == ETokenClass::MultilineComment && contextualLookup.m_State & 0b1000)
				--multilineCommentI;

			escaped            = contextualLookup.m_State & 0b1000'0000;
			previousTokenClass = tokenClass;
			tokenClass         = contextualLookup.m_NewTokenClass;

			bool addToToken = contextualLookup.m_State & 1;

			if (contextualLookup.m_State & 0b111)
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
					characterClass = s_CharacterClasses[static_cast<std::uint8_t>(c) & 0x7F];
				}
			}

			if ((!addToToken || contextualLookup.m_State & 0b100) && tokenLength)
			{
				if (tokenClass == ETokenClass::String)
				{
					++tokenStart;
					--tokenEnd;
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
} // namespace Frertex
