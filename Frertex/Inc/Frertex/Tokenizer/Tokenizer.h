#pragma once

#include "Token.h"

#include <vector>

namespace Frertex::Tokenizer
{
	std::size_t Tokenize(const void* data, std::size_t size, std::vector<Token>& tokens);
}