#include "Frertex/Tokenizer/Tokenizer.h"

namespace Frertex::Tokenizer
{
	static constexpr std::uint16_t ResultStateStep  = 0x01;
	static constexpr std::uint16_t ResultStateEnd   = 0x02;
	static constexpr std::uint16_t ResultStateError = 0x04;

	extern ECharacterClass c_CharacterClasses[0x0080];
	extern bool            c_IncludedTokenClasses[0x000C];
	extern std::uint16_t          c_TokenLUT[0x10000];

	static std::uint16_t GetCharState(std::uint16_t value)
	{
		return (value & 0x0FE0) >> 5;
	}

	static std::uint16_t SetCharState(std::uint16_t value, std::uint8_t c)
	{
		return (value & ~0x0FE0) | (static_cast<std::uint16_t>(c) << 5);
	}

	static ETokenClass GetTokenClass(std::uint16_t value)
	{
		return static_cast<ETokenClass>((value & 0xF000) >> 12);
	}

	std::size_t Tokenize(const void* data, std::size_t size, std::vector<Token>& tokens)
	{
		if (!data || !size)
			return 0;

		const std::uint8_t* pChars       = reinterpret_cast<const std::uint8_t*>(data);
		std::size_t       adjustedSize = size / sizeof(std::uint8_t);
		std::size_t       offset       = 0;

		std::uint16_t      curState   = SetCharState(static_cast<std::uint16_t>(ETokenClass::Unknown) << 12, pChars[0]);
		std::size_t tokenStart = offset;
		std::size_t tokenEnd   = offset;

		while (offset < adjustedSize)
		{
			std::uint16_t      nextState  = c_TokenLUT[curState];
			std::uint16_t      charState  = GetCharState(nextState);
			ETokenClass tokenClass = GetTokenClass(curState);
			if (charState & ResultStateStep)
			{
				++offset;
				++tokenEnd;
			}
			curState = SetCharState(nextState, pChars[offset]);
			if (charState & ResultStateEnd)
			{
				std::size_t previousTokenStart = tokenStart;
				std::size_t previousTokenEnd   = tokenEnd;

				tokenStart = tokenEnd = offset;
				if (c_IncludedTokenClasses[static_cast<std::uint16_t>(tokenClass)])
				{
					std::size_t tokenLength = previousTokenEnd - previousTokenStart;
					if (tokenLength <= 0xFFFF'FFFF && tokenLength > 0)
					{
						tokens.emplace_back(Token {
							.Class  = tokenClass,
							.Length = static_cast<std::uint32_t>(tokenLength),
							.Start  = previousTokenStart });
					}
				}
			}
		}
		{
			std::size_t tokenLength = tokenEnd - tokenStart;
			if (tokenLength <= 0xFFFF'FFFF && tokenLength > 0)
			{
				tokens.emplace_back(Token {
					.Class  = GetTokenClass(curState),
					.Length = static_cast<std::uint32_t>(tokenLength),
					.Start  = tokenStart });
			}
		}
		return offset * sizeof(std::uint8_t);
	}
} // namespace Frertex::Tokenizer