#pragma once

#include "Token.h"

#include <string_view>
#include <vector>

namespace Frertex
{
	std::vector<Token> Tokenize(std::string_view str, SourcePoint start = { 0, 0, 0 });
} // namespace Frertex