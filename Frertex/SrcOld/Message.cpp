#include "Frertex/Message.h"
#include "Frertex/Utils/Profiler.h"

#include <fmt/format.h>

#include <sstream>

namespace Frertex::Message
{
	std::string_view MessageTypeToString(EMessageType type)
	{
		switch (type)
		{
		case EMessageType::Warning: return "Warning";
		case EMessageType::Error: return "Error";
		}
		return "?";
	}

	std::string Message::format(const Source::Sources* sources, const std::vector<Tokenizer::Token>& tokens) const
	{
		PROFILE_FUNC;

		std::string_view fileName = sources->getSourceName(m_Point.m_FileID);

		std::size_t      firstLine  = tokens[m_StartToken].getSpan(sources).m_StartLine;
		auto             source     = Tokenizer::ReconstructSource(sources, tokens, m_StartToken, m_EndToken);
		std::string_view sourceView = source.m_Str;

		std::vector<std::string_view> sourceLines;
		std::vector<std::size_t>      sourceLineIndices;
		std::size_t                   lineStart = 0;
		sourceLineIndices.emplace_back(0);
		for (std::size_t i = 0; i < sourceView.size(); ++i)
		{
			if (sourceView[i] == '\n')
			{
				sourceLines.emplace_back(sourceView.substr(lineStart, i - lineStart));
				lineStart = i + 1;
				sourceLineIndices.emplace_back(lineStart);
			}
		}
		if (lineStart < sourceView.size())
			sourceLines.emplace_back(sourceView.substr(lineStart));

		std::vector<std::string> squiggleLines(sourceLines.size());
		for (std::size_t i = 0; i < sourceLines.size(); ++i)
			squiggleLines[i] = std::string(sourceLines[i].size(), ' ');

		for (auto& span : m_Spans)
		{
			auto s = source.getEnvelopingSpan(span);

			std::size_t offset = s.m_Offset + (span.m_StartIndex - s.m_SourceSpan.m_StartIndex);
			std::size_t length = span.m_EndIndex - span.m_StartIndex;

			std::size_t line = 0;
			for (; line < sourceLineIndices.size() - 1; ++line)
			{
				if (sourceLineIndices[line] > offset)
				{
					--line;
					break;
				}
			}
			std::size_t column = offset - sourceLineIndices[line];
			for (std::size_t i = 0; i < length; ++i)
			{
				auto& squiggleLine = squiggleLines[line];
				if (column >= squiggleLine.size())
				{
					++line;
					column = 0;
					if (line > squiggleLines.size())
						break;
					continue;
				}
				squiggleLine[column] = '~';
				++column;
			}
		}

		{
			auto s = source.getEnvelopingSpan({ m_Point.m_Index, m_Point.m_Line, m_Point.m_Column,
			                                    m_Point.m_Index, m_Point.m_Line, m_Point.m_Column,
			                                    m_Point.m_FileID, m_Point.m_SourceID });

			std::size_t offset = s.m_Offset + (m_Point.m_Index - s.m_SourceSpan.m_StartIndex);

			std::size_t line = 0;
			for (; line < sourceLineIndices.size() - 1; ++line)
			{
				if (sourceLineIndices[line] > offset)
				{
					--line;
					break;
				}
			}
			if (offset < sourceLineIndices[line])
				--line;

			std::size_t column = offset - sourceLineIndices[line];
			if (column >= squiggleLines[line].size())
				squiggleLines[line].resize(column + 1, ' ');
			squiggleLines[line][column] = '^';
		}

		std::ostringstream str;
		str << fmt::format("{} ({}:{})\n{}: {}\n",
		                   fileName,
		                   m_Point.m_Line + 1,
		                   m_Point.m_Column + 1,
		                   MessageTypeToString(m_Type),
		                   m_Message);
		for (std::size_t i = 0; i < sourceLines.size(); ++i)
		{
			if (i > 0)
				str << '\n';
			str << sourceLines[i];
			if (squiggleLines[i].find_first_not_of(' ') < squiggleLines[i].size())
				str << '\n'
				    << squiggleLines[i];
		}
		return str.str();
	}

	std::string TranspilerMessage::format(const FIL::Binary& fil) const
	{
		// TODO(MarcasRealAccount): Disassemble instructions
		std::ostringstream str;
		str << fmt::format("{}: {}\nAssociated binary: [\n",
		                   MessageTypeToString(m_Type),
		                   m_Message);

		str << ']';
		return str.str();
	}
} // namespace Frertex::Message
