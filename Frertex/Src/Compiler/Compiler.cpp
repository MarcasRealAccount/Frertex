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

		getFunctionDefinitions();

		FIL::Binary binary;

		return binary;
	}

	void State::getFunctionDefinitions()
	{
		for (auto& node : m_AST.getChildren())
		{
			if (node.getType() == AST::EType::FunctionDeclaration)
			{
				auto                 attributes = node.getChild(0);
				FIL::EEntrypointType type       = FIL::EEntrypointType::None;
				for (auto& attribute : attributes->getChildren())
				{
					type = FIL::StringToEntrypointType(attribute.getToken().getView(m_Sources));
					if (type != FIL::EEntrypointType::None)
						break;
				}

				m_FunctionDefinitions.emplace_back(node.getChild(2)->getToken().getView(m_Sources), type, node.getChild(4));
			}
		}
	}

	void State::compileFunctionDeclaration(AST::Node* node)
	{
	}

	void State::compileAssignmentExpression(AST::Node* node)
	{
	}

	void State::compileExpression(AST::Node* node)
	{
	}

	void State::compileStatements(AST::Node* root)
	{
	}

	void State::pushScope()
	{
	}

	void State::popScope()
	{
	}

	void State::reportMessage(Message::EMessageType type, std::size_t startToken, std::size_t endToken, Utils::CopyMovable<std::vector<Source::SourceSpan>>&& spans, Source::SourcePoint point, Utils::CopyMovable<std::string>&& message)
	{
		PROFILE_FUNC;

		m_MessageHandler(m_Sources, m_Tokens, { type, startToken, endToken + 1, spans.get(), point, message.get() });
	}
} // namespace Frertex::Compiler