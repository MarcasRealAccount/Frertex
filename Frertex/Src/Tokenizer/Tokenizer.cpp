// Auto generated

#include "Frertex/Tokenizer/Tokenizer.h"

namespace Frertex::Tokenizer
{
	static constexpr std::uint16_t ResultStateStep  = 0x01;
	static constexpr std::uint16_t ResultStateEnd   = 0x02;
	static constexpr std::uint16_t ResultStateError = 0x04;

	extern ECharacterClass c_CharacterClasses[0x0080];
	extern bool            c_IncludedTokenClasses[0x000C];
	extern std::uint16_t   c_TokenLUT[0x10000];

	void AddToken(std::uint16_t state, std::size_t& start, std::uint32_t& length, std::vector<Token>& tokens)
	{
		if (!length)
			return;

		std::uint16_t tokenClass = (state & 0xF000) >> 12;
		if (c_IncludedTokenClasses[tokenClass])
		{
			tokens.emplace_back(Token {
				.Class  = static_cast<ETokenClass>(tokenClass),
				.Length = length,
				.Start  = start });
		}
		start  += length;
		length = 0;
	}

	std::size_t Tokenize(const void* data, std::size_t size, std::vector<Token>& tokens)
	{
		if (!data || !size)
			return 0;

		const std::uint8_t* pChars = reinterpret_cast<const std::uint8_t*>(data);
		size                       /= sizeof(std::uint8_t);

		std::uint16_t curState    = ((static_cast<std::uint16_t>(ETokenClass::Unknown) << 12) & ~0x0FE0) | (static_cast<std::uint16_t>(*pChars) << 5);
		std::size_t   tokenStart  = 0;
		std::uint32_t tokenLength = 0;

		std::size_t iters = 0;
		while (tokenStart < size)
		{
			++iters;
			std::uint16_t result    = c_TokenLUT[curState];
			std::uint16_t charState = result >> 5;
			std::uint16_t step      = charState & ResultStateStep;
			pChars                  += step;
			tokenLength             += step;
			if (charState & ResultStateEnd)
				AddToken(curState, tokenStart, tokenLength, tokens);
			curState = (result & ~0x0FE0) | (static_cast<std::uint16_t>(*pChars) << 5);
		}
		AddToken(curState, tokenStart, tokenLength, tokens);
		return iters;
	}
} // namespace Frertex::Tokenizer