#include "Frertex/Tokenizer/Tokenizer.h"

namespace Frertex::Tokenizer
{
	std::size_t Tokenize(const void* data, std::size_t size, std::vector<Token>& tokens)
	{
		if (!data || !size)
			return 0;

		const std::uint8_t* pData  = reinterpret_cast<const std::uint8_t*>(data);
		std::size_t         offset = 0;

		ETokenClass tokenClass = ETokenClass::Unknown;
		std::size_t tokenStart = offset;
		std::size_t tokenEnd   = offset;

		std::uint8_t state = pData[offset] & 0x7F;
		while (offset < size)
		{
			auto nextState = c_ContextualTokenLUT[static_cast<std::uint8_t>(tokenClass)][state];

			if (nextState.State & ContextualTokenStep)
			{
				++offset;
				++tokenEnd;
			}
			auto previousClass = tokenClass;
			state              = (nextState.State & ContextualTokenPassState) | (pData[offset] & 0x7F);
			tokenClass         = nextState.Class;
			if (nextState.State & ContextualTokenEnd)
			{
				auto previousTokenStart = tokenStart;
				auto previousTokenEnd   = tokenEnd;
				tokenEnd = tokenStart = offset;
				if (previousClass != ETokenClass::Unknown)
				{
					std::size_t tokenLength = previousTokenEnd - previousTokenStart;
					if (tokenLength <= 0xFFFF'FFFF && tokenLength > 0)
					{
						bool add = true;
						if (previousClass == ETokenClass::Symbol && !tokens.empty())
						{
							auto& back = tokens.back();
							if (back.Class == ETokenClass::Symbol && back.Length == 1 && back.Start == offset - 2)
							{
								back.Length = 2;
								add         = false;
							}
						}

						if (add)
						{
							tokens.emplace_back(Token {
								.Class  = previousClass,
								.Pad    = {0, 0, 0},
								.Length = static_cast<std::uint32_t>(tokenLength),
								.Start  = previousTokenStart
                            });
						}
					}
				}
			}
		}
		{
			std::size_t tokenLength = tokenEnd - tokenStart;
			if (tokenLength <= 0xFFFF'FFFF && tokenLength > 0)
			{
				tokens.emplace_back(Token {
					.Class  = tokenClass,
					.Pad    = {0, 0, 0},
					.Length = static_cast<std::uint32_t>(tokenLength),
					.Start  = tokenStart
                });
			}
		}
		return offset;
	}
} // namespace Frertex::Tokenizer