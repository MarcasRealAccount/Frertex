// Auto generated

#include "Frertex/Tokenizer/Tokenizer.h"

namespace Frertex::Tokenizer
{
	static constexpr $TYPE$ ResultStateStep  = 0x01;
	static constexpr $TYPE$ ResultStateEnd   = 0x02;
	static constexpr $TYPE$ ResultStateError = 0x04;

	extern ECharacterClass c_CharacterClasses[$CHARCLASSESCOUNT$];
	extern bool            c_IncludedTokenClasses[$TOKENCLASSCOUNT$];
	extern $TYPE$          c_TokenLUT[$TOKENLUTSIZE$];

	void AddToken($TYPE$ state, std::size_t& start, std::uint32_t& length, std::vector<Token>& tokens)
	{
		if (!length)
			return;

		$TYPE$ tokenClass = (state & $TOKENCLASSMASK$) >> $TOKENCLASSBIT$;
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

		const $CHARTYPE$* pChars = reinterpret_cast<const $CHARTYPE$*>(data);
		size                     /= sizeof($CHARTYPE$);

		$TYPE$        curState    = ((static_cast<$TYPE$>(ETokenClass::$INITIALTOKENCLASS$) << $TOKENCLASSBIT$) & ~$CHARMASK$) | (static_cast<$TYPE$>(*pChars) << $CHARBIT$);
		std::size_t   tokenStart  = 0;
		std::uint32_t tokenLength = 0;

		std::size_t iters = 0;
		while (tokenStart < size)
		{
			++iters;
			$TYPE$ result    = c_TokenLUT[curState];
			$TYPE$ charState = result >> $CHARBIT$;
			$TYPE$ step      = charState & ResultStateStep;
			pChars           += step;
			tokenLength      += step;
			if (charState & ResultStateEnd)
				AddToken(curState, tokenStart, tokenLength, tokens);
			curState = (result & ~$CHARMASK$) | (static_cast<$TYPE$>(*pChars) << $CHARBIT$);
		}
		AddToken(curState, tokenStart, tokenLength, tokens);
		return iters;
	}
} // namespace Frertex::Tokenizer