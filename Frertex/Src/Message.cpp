#include "Frertex/Message.h"

#include <fmt/format.h>

#include <sstream>

namespace Frertex
{
	std::string_view MessageTypeToString(EMessageType type)
	{
		switch (type)
		{
		case EMessageType::Warning: return "Warning";
		case EMessageType::Error: return "Error";
		default: return "Unknown";
		}
	}

	std::string FormatMessage(const Message& message, const std::vector<std::string>& filenames, LineCallback lineCallback, void* userData)
	{
		std::string_view filename       = filenames[message.m_Span.m_Start.m_File];
		std::string_view actualFilename = filenames[message.m_Span.m_Start.m_ActualFile];
		std::string      line           = lineCallback(actualFilename, message.m_Point, userData);

		std::size_t offset = 0;
		while (offset < line.size())
		{
			offset = line.find_first_of('\t', offset);
			if (offset >= line.size())
				break;
			line.replace(offset, 1, "    ");
			offset += 4;
		}

		if (message.m_Span.m_Start.m_ActualFile != message.m_Span.m_End.m_ActualFile)
			return fmt::format("{} {}:{} {}: {}\n{}\n{}^\nMultifile messages not supported, and never will, sorry :)",
			                   filename,
			                   message.m_Point.m_Line + 1,
			                   message.m_Point.m_Column + 1,
			                   MessageTypeToString(message.m_Type),
			                   message.m_Message,
			                   line,
			                   std::string(message.m_Span.m_Start.m_Column + 1, ' '));

		if (message.m_Span.m_Start.m_Line == message.m_Span.m_End.m_Line)
		{
			return fmt::format("{} {}:{} {}: {}\n{}\n{}{}^{}",
			                   filename,
			                   message.m_Point.m_Line + 1,
			                   message.m_Point.m_Column + 1,
			                   MessageTypeToString(message.m_Type),
			                   message.m_Message,
			                   line,
			                   std::string(message.m_Span.m_Start.m_Column, ' '),
			                   std::string(message.m_Point.m_Column - message.m_Span.m_Start.m_Column, '~'),
			                   std::string(message.m_Span.m_End.m_Column - message.m_Point.m_Column, '~'));
		}
		else
		{
			std::size_t spaces, squigglesStart, squigglesEnd;
			if (message.m_Span.m_Start.m_Line == message.m_Point.m_Line)
			{
				spaces         = message.m_Span.m_Start.m_Column;
				squigglesStart = message.m_Point.m_Column - message.m_Span.m_Start.m_Column;
			}
			else
			{
				spaces         = 0;
				squigglesStart = message.m_Point.m_Column;
			}
			if (message.m_Span.m_End.m_Line == message.m_Point.m_Line)
				squigglesEnd = message.m_Span.m_End.m_Column - message.m_Point.m_Column;
			else
				squigglesEnd = line.size() - message.m_Point.m_Column - 1;
			return fmt::format("{} {}:{} -> {}:{} {}: {}\n{}\n{}{}^{}\nMultiline messages not supported atm, sorry :)",
			                   filename,
			                   message.m_Span.m_Start.m_Line + 1,
			                   message.m_Span.m_Start.m_Column + 1,
			                   message.m_Span.m_End.m_Line + 1,
			                   message.m_Span.m_End.m_Column + 1,
			                   MessageTypeToString(message.m_Type),
			                   message.m_Message,
			                   line,
			                   std::string(spaces, ' '),
			                   std::string(squigglesStart, '~'),
			                   std::string(squigglesEnd, '~'));
		}
	}
} // namespace Frertex
