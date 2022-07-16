#pragma once

#include "Token.h"

#include <string>
#include <vector>

namespace Frertex
{
	enum class EMessageType
	{
		Warning,
		Error
	};

	struct Message
	{
		EMessageType m_Type;
		SourceSpan   m_Span;
		SourcePoint  m_Point;
		std::string  m_Message;
	};

	using LineCallback = std::string (*)(std::string_view filename, SourcePoint line);

	std::string_view MessageTypeToString(EMessageType type);
	std::string      FormatMessage(const Message& message, const std::vector<std::string>& filenames, LineCallback lineCallback);
} // namespace Frertex