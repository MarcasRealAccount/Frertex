#pragma once

#include "Frertex/Source.h"
#include "Token.h"

#include <cstddef>

#include <vector>

namespace Frertex::Tokenizer
{
	std::vector<Token> Tokenize(const Source::Source* source, std::size_t start = 0ULL, std::size_t end = ~0ULL);
}