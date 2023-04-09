#include "Frertex/Tokenizer/Token.h"
#include "Frertex/Utils/Profiler.h"

namespace Frertex::Tokenizer
{
	std::string_view CharacterClassToString(ECharacterClass clazz)
	{
		switch (clazz)
		{
		case ECharacterClass::Unknown: return "Unknown";
		case ECharacterClass::Whitespace: return "Whitespace";
		case ECharacterClass::Tab: return "Tab";
		case ECharacterClass::Newline: return "Newline";
		case ECharacterClass::NonDigit: return "NonDigit";
		case ECharacterClass::Digit: return "Digit";
		case ECharacterClass::Symbol: return "Symbol";
		}
		return "?";
	}

	std::string_view TokenClassToString(ETokenClass clazz)
	{
		switch (clazz)
		{
		case ETokenClass::Unknown: return "Unknown";
		case ETokenClass::Identifier: return "Identifier";
		case ETokenClass::String: return "String";
		case ETokenClass::Integer: return "Integer";
		case ETokenClass::BinaryInteger: return "BinaryInteger";
		case ETokenClass::OctalInteger: return "OctalInteger";
		case ETokenClass::HexInteger: return "HexInteger";
		case ETokenClass::Float: return "Float";
		case ETokenClass::HexFloat: return "HexFloat";
		case ETokenClass::LastSymbol: return "LastSymbol";
		case ETokenClass::Preprocessor: return "Preprocessor";
		case ETokenClass::Comment: return "Comment";
		case ETokenClass::MultilineComment: return "MultilineComment";
		case ETokenClass::FirstSymbol: return "FirstSymbol";
		}
		return "?";
	}

	Token::Token()
	    : m_Class(ETokenClass::Unknown),
	      m_Index(0ULL),
	      m_Length(0ULL),
	      m_FileID(0U),
	      m_SourceID(0U) {}

	Token::Token(ETokenClass clazz, std::size_t index, std::size_t length, std::uint32_t fileID, std::uint32_t sourceID)
	    : m_Class(clazz),
	      m_Index(index),
	      m_Length(length),
	      m_FileID(fileID),
	      m_SourceID(sourceID) {}

	ReconstructionData::Span ReconstructionData::getEnvelopingSpan(const Source::SourceSpan& span)
	{
		PROFILE_FUNC;

		for (auto& s : m_Spans)
		{
			auto& sp = s.m_SourceSpan;
			if (sp.m_SourceID == span.m_SourceID &&
			    sp.m_StartIndex <= span.m_StartIndex &&
			    sp.m_EndIndex >= span.m_EndIndex)
				return s;
		}

		return { ~0ULL, ~0ULL, {} };
	}

	ReconstructionData ReconstructSource(const Source::Sources*    sources,
	                                     const std::vector<Token>& tokens,
	                                     std::size_t               start,
	                                     std::size_t               end)
	{
		PROFILE_FUNC;

		end = std::min(end, tokens.size());

		std::size_t startLine = tokens[start].getEnd(sources).m_Line;
		std::size_t endLine   = tokens[end].getStart(sources).m_Line;
		while (start > 0)
		{
			if (tokens[start].getEnd(sources).m_Line != startLine)
			{
				++start;
				break;
			}

			--start;
		}
		while (end < tokens.size())
		{
			if (tokens[end].getStart(sources).m_Line != endLine)
			{
				--end;
				break;
			}

			++end;
		}

		ReconstructionData data;

		std::size_t   previousLine     = 0;
		std::size_t   previousColumn   = 0;
		std::uint32_t previousSourceID = 0;
		for (std::size_t i = start; i < end; ++i)
		{
			auto& token    = tokens[i];
			auto  span     = token.getSpan(sources);
			auto  sourceID = token.m_SourceID;

			bool isString = token.m_Class == ETokenClass::String;

			if (sourceID == previousSourceID)
			{
				if (span.m_StartLine < previousLine)
					if (span.m_StartColumn < previousColumn)
						data.m_Str += ' ';
					else
						data.m_Str += std::string(span.m_StartColumn - previousColumn - isString, ' ');
				else if (span.m_StartLine > previousLine)
					data.m_Str += std::string(span.m_StartLine - previousLine, '\n') + std::string(span.m_StartColumn - isString, ' ');
				else if (span.m_StartColumn != previousColumn)
					data.m_Str += std::string(span.m_StartColumn - previousColumn - isString, ' ');
			}
			else if (previousSourceID)
			{
				data.m_Str += ' ';
			}

			auto view = token.getView(sources);
			data.m_Spans.emplace_back(data.m_Str.size(), view.size(), span);
			if (token.m_Class == ETokenClass::String)
				data.m_Str += '"';
			data.m_Str += view;
			if (token.m_Class == ETokenClass::String)
				data.m_Str += '"';
			previousSourceID = sourceID;
			previousLine     = span.m_EndLine;
			previousColumn   = span.m_EndColumn + isString;
		}

		return data;
	}

	ECharacterClass s_CharacterClassLUT[128] {
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Tab,
		ECharacterClass::Newline,
		ECharacterClass::Whitespace,
		ECharacterClass::Whitespace,
		ECharacterClass::Whitespace,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Unknown,
		ECharacterClass::Whitespace,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Digit,
		ECharacterClass::Digit,
		ECharacterClass::Digit,
		ECharacterClass::Digit,
		ECharacterClass::Digit,
		ECharacterClass::Digit,
		ECharacterClass::Digit,
		ECharacterClass::Digit,
		ECharacterClass::Digit,
		ECharacterClass::Digit,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::NonDigit,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Symbol,
		ECharacterClass::Unknown
	};
} // namespace Frertex::Tokenizer