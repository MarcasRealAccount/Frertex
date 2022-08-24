#pragma once

#include "Frertex/FIL/FIL.h"
#include "Frertex/Source.h"
#include "Frertex/Tokenizer/Token.h"

#include <cstddef>

#include <string>
#include <vector>

namespace Frertex::Message
{
	enum class EMessageType
	{
		Warning,
		Error
	};

	std::string_view MessageTypeToString(EMessageType type);

	struct Message
	{
	public:
		Message(EMessageType                                          type,
		        std::size_t                                           startToken,
		        std::size_t                                           endToken,
		        Utils::CopyMovable<std::vector<Source::SourceSpan>>&& spans,
		        Source::SourcePoint                                   point,
		        Utils::CopyMovable<std::string>&&                     message)
		    : m_Type(type),
		      m_StartToken(startToken),
		      m_EndToken(endToken),
		      m_Spans(spans.get()),
		      m_Point(point),
		      m_Message(message.get()) {}

		std::string format(const Source::Sources* sources, const std::vector<Tokenizer::Token>& tokens) const;

	public:
		EMessageType                    m_Type;
		std::size_t                     m_StartToken;
		std::size_t                     m_EndToken;
		std::vector<Source::SourceSpan> m_Spans;
		Source::SourcePoint             m_Point;
		std::string                     m_Message;
	};

	struct TranspilerMessage
	{
	public:
		TranspilerMessage(EMessageType                      type,
		                  std::size_t                       startInstruction,
		                  std::size_t                       endInstruction,
		                  Utils::CopyMovable<std::string>&& message)
		    : m_Type(type),
		      m_StartInstruction(startInstruction),
		      m_EndInstruction(endInstruction),
		      m_Message(message.get()) {}

		std::string format(const FIL::Binary& fil) const;

	public:
		EMessageType m_Type;
		std::size_t  m_StartInstruction;
		std::size_t  m_EndInstruction;
		std::string  m_Message;
	};

	using MessageHandler           = void (*)(const Source::Sources* sources, const std::vector<Tokenizer::Token>& tokens, const Message& message);
	using TranspilerMessageHandler = void (*)(const FIL::Binary& fil, const TranspilerMessage& message);
} // namespace Frertex::Message