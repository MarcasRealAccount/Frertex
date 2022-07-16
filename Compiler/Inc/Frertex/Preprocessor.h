#pragma once

#include "Message.h"
#include "Token.h"

#include <vector>

namespace Frertex
{
	using IncludeHandler = std::string (*)(std::string_view filename);

	std::vector<Token> Preprocess(std::vector<Token>&& tokens, IncludeHandler includeHandler, std::string_view filename, std::vector<std::string> & includedFilenames, std::vector<Message>& errors);
} // namespace Frertex