#pragma once

#include "Token.h"

#include <string_view>
#include <vector>

namespace Frertex
{
	struct ContextualToken
	{
	public:
		std::uint8_t m_State         = 0;
		ETokenClass  m_NewTokenClass = ETokenClass::Unknown;
	};

	extern ContextualToken s_ContextualTokenLUT[13][256];

	std::vector<Token> Tokenize(const Source* source, std::size_t start = 0ULL, std::size_t end = ~0ULL);
} // namespace Frertex