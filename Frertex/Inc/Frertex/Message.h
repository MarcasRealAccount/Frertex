#pragma once

#include "Token.h"
#include "Utils/Utils.h"

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
		Message(EMessageType type, SourceSpan span, SourcePoint point, Utils::CopyMovable<std::string>&& message)
		    : m_Type(type), m_Span(span), m_Point(point), m_Message(message.get()) {}

		EMessageType m_Type;
		SourceSpan   m_Span;
		SourcePoint  m_Point;
		std::string  m_Message;
	};

	using LineCallback = std::string (*)(std::string_view filename, SourcePoint line, Sources* sources, void* userData);

	std::string_view MessageTypeToString(EMessageType type);
	std::string      FormatMessage(const Message& message, LineCallback lineCallback, Sources* sources, void* userData = nullptr);
} // namespace Frertex
