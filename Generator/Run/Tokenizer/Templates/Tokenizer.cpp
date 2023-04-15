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

	static $TYPE$ GetCharState($TYPE$ value)
	{
		return (value & $CHARMASK$) >> $CHARBIT$;
	}

	static $TYPE$ SetCharState($TYPE$ value, $CHARTYPE$ c)
	{
		return (value & ~$CHARMASK$) | (static_cast<$TYPE$>(c) << $CHARBIT$);
	}

	static ETokenClass GetTokenClass($TYPE$ value)
	{
		return static_cast<ETokenClass>((value & $TOKENCLASSMASK$) >> $TOKENCLASSBIT$);
	}

	std::size_t Tokenize(const void* data, std::size_t size, std::vector<Token>& tokens)
	{
		if (!data || !size)
			return 0;

		const $CHARTYPE$* pChars       = reinterpret_cast<const $CHARTYPE$*>(data);
		std::size_t       adjustedSize = size / sizeof($CHARTYPE$);
		std::size_t       offset       = 0;

		$TYPE$      curState   = SetCharState(static_cast<$TYPE$>(ETokenClass::$INITIALTOKENCLASS$) << $TOKENCLASSBIT$, pChars[0]);
		std::size_t tokenStart = offset;
		std::size_t tokenEnd   = offset;

		while (offset < adjustedSize)
		{
			$TYPE$      nextState  = c_TokenLUT[curState];
			$TYPE$      charState  = GetCharState(nextState);
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
				if (c_IncludedTokenClasses[static_cast<$TYPE$>(tokenClass)])
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
		return offset * sizeof($CHARTYPE$);
	}
} // namespace Frertex::Tokenizer