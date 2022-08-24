#include "Frertex/Compiler/Compiler.h"
#include "Frertex/Utils/Profiler.h"

namespace Frertex::Compiler
{
	State::State(Source::Sources* sources, Message::MessageHandler messageHandler, Utils::CopyMovable<std::vector<Tokenizer::Token>>&& tokens)
	    : m_Sources(sources), m_MessageHandler(messageHandler), m_Tokens(tokens.get()) {}

	FIL::Binary State::compile(Utils::CopyMovable<AST::Node>&& ast)
	{
		PROFILE_FUNC;

		m_AST = ast.get();

		FIL::Binary binary;

		return binary;
	}

	void State::reportMessage(Message::EMessageType type, std::size_t startToken, std::size_t endToken, Utils::CopyMovable<std::vector<Source::SourceSpan>>&& spans, Source::SourcePoint point, Utils::CopyMovable<std::string>&& message)
	{
		PROFILE_FUNC;

		m_MessageHandler(m_Sources, m_Tokens, { type, startToken, endToken + 1, spans.get(), point, message.get() });
	}
} // namespace Frertex::Compiler