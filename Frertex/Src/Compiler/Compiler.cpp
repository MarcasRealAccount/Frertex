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
		m_FunctionDefinitions.clear();
		m_FunctionDeclarations.clear();
		m_Types.clear();
		m_Variables.clear();
		m_Scopes = {};

		pushScope();

		// Get definitions
		getFunctionDefinitions(&m_AST);

		FIL::Binary binary;

		return binary;
	}

	void State::getFunctionDefinitions(AST::Node* root)
	{
		for (auto& node : root->getChildren())
		{
			switch (node.getType())
			{
			case AST::EType::CompoundStatement:
				getFunctionDefinitions(&node);
				break;
			case AST::EType::FunctionDeclaration:
			{
				auto                 attributes = node.getChild(0);
				FIL::EEntrypointType type       = FIL::EEntrypointType::None;
				for (auto& attribute : attributes->getChildren())
				{
					if (type == FIL::EEntrypointType::None)
					{
						type = FIL::StringToEntrypointType(attribute.getToken().getView(m_Sources));
						if (type == FIL::EEntrypointType::None)
							reportWarning(attribute.getToken().m_Index,
							              attribute.getToken().m_Index,
							              { attribute.getToken().getSpan(m_Sources) },
							              attribute.getToken().getStart(m_Sources),
							              "Attribute unused");
					}
					else
						reportWarning(attribute.getToken().m_Index,
						              attribute.getToken().m_Index,
						              { attribute.getToken().getSpan(m_Sources) },
						              attribute.getToken().getStart(m_Sources),
						              "Attribute unused");
				}

				auto& currentNamespace     = m_Scopes.top().m_CurrentNamespace;
				auto  functionCompoundNode = node.getChild(4);
				auto& function             = m_FunctionDefinitions.emplace_back(currentNamespace, node.getChild(2)->getToken().getView(m_Sources), type, functionCompoundNode);

				auto newNamespace = currentNamespace.empty() ? function.m_Name : (currentNamespace + "::" + function.m_Name);

				pushScope();
				m_Scopes.top().m_CurrentNamespace = newNamespace;

				getFunctionDefinitions(functionCompoundNode);

				popScope();
				break;
			}
			default:
				break;
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
		m_Scopes.emplace();
	}

	void State::popScope()
	{
		m_Scopes.pop();
	}

	void State::reportMessage(Message::EMessageType type, std::size_t startToken, std::size_t endToken, Utils::CopyMovable<std::vector<Source::SourceSpan>>&& spans, Source::SourcePoint point, Utils::CopyMovable<std::string>&& message)
	{
		PROFILE_FUNC;

		m_MessageHandler(m_Sources, m_Tokens, { type, startToken, endToken + 1, spans.get(), point, message.get() });
	}
} // namespace Frertex::Compiler