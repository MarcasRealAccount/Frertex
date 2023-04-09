#pragma once

#include "AST.h"
#include "Frertex/Message.h"
#include "Frertex/Source.h"
#include "Frertex/Tokenizer/Token.h"
#include "Frertex/Utils/Utils.h"

namespace Frertex::Parser
{
	struct ParseResult
	{
		std::size_t m_UsedTokens = 0;
		AST::Node   m_Node;
	};

	class State
	{
	public:
		State(Source::Sources* sources, Message::MessageHandler messageHandler);

		AST::Node parse(Utils::CopyMovable<std::vector<Tokenizer::Token>>&& tokens);

		auto getSources() const { return m_Sources; }
		auto errored() const { return m_Errored; }

		auto&& moveTokens() { return std::move(m_Tokens); }

	private:
		ParseResult parseIntegerLiteral(std::size_t start, std::size_t end);
		ParseResult parseFloatLiteral(std::size_t start, std::size_t end);
		ParseResult parseBoolLiteral(std::size_t start, std::size_t end);
		ParseResult parseLiteral(std::size_t start, std::size_t end);
		ParseResult parsePreUnaryOperator(std::size_t start, std::size_t end);
		ParseResult parsePostUnaryOperator(std::size_t start, std::size_t end);
		ParseResult parseBinaryOperator(std::size_t start, std::size_t end);
		ParseResult parseAssignmentOperator(std::size_t start, std::size_t end);
		ParseResult parseTypeQualifier(std::size_t start, std::size_t end);
		ParseResult parseTypeQualifiers(std::size_t start, std::size_t end);
		ParseResult parseIdentifier(std::size_t start, std::size_t end);
		ParseResult parseTypename(std::size_t start, std::size_t end);
		ParseResult parseParameterTypename(std::size_t start, std::size_t end);
		ParseResult parseArgument(std::size_t start, std::size_t end);
		ParseResult parseArguments(std::size_t start, std::size_t end);
		ParseResult parseAttribute(std::size_t start, std::size_t end);
		ParseResult parseAttributes(std::size_t start, std::size_t end);
		ParseResult parseParameter(std::size_t start, std::size_t end);
		ParseResult parseParameters(std::size_t start, std::size_t end);
		ParseResult parseBracedInitList(std::size_t start, std::size_t end);
		ParseResult parseInitializerClause(std::size_t start, std::size_t end);
		ParseResult parseAssignmentExpression(std::size_t start, std::size_t end);
		ParseResult parseExpression(std::size_t start, std::size_t end);
		ParseResult parseEmptyStatement(std::size_t start, std::size_t end);
		ParseResult parseExpressionStatement(std::size_t start, std::size_t end);
		ParseResult parseCompoundStatement(std::size_t start, std::size_t end);
		ParseResult parseReturnStatement(std::size_t start, std::size_t end);
		ParseResult parseStatement(std::size_t start, std::size_t end);
		ParseResult parseStatements(std::size_t start, std::size_t end);
		ParseResult parseFunctionDeclaration(std::size_t start, std::size_t end);
		ParseResult parseDeclaration(std::size_t start, std::size_t end);
		ParseResult parseTranslationUnit(std::size_t start, std::size_t end);

		void reportMessage(Message::EMessageType type, std::size_t startToken, std::size_t endToken, Utils::CopyMovable<std::vector<Source::SourceSpan>>&& spans, Source::SourcePoint point, Utils::CopyMovable<std::string>&& message);
		void reportWarning(std::size_t startToken, std::size_t endToken, Utils::CopyMovable<std::vector<Source::SourceSpan>>&& spans, Source::SourcePoint point, Utils::CopyMovable<std::string>&& message) { reportMessage(Message::EMessageType::Warning, startToken, endToken, spans.get(), point, message.get()); }
		void reportError(std::size_t startToken, std::size_t endToken, Utils::CopyMovable<std::vector<Source::SourceSpan>>&& spans, Source::SourcePoint point, Utils::CopyMovable<std::string>&& message)
		{
			m_Errored = true;
			reportMessage(Message::EMessageType::Error, startToken, endToken, spans.get(), point, message.get());
		}

	private:
		Source::Sources* m_Sources;

		Message::MessageHandler m_MessageHandler;
		bool                    m_Errored = false;

		std::vector<Tokenizer::Token> m_Tokens;
	};
} // namespace Frertex::Parser