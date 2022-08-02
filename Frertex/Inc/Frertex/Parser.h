#pragma once

#include "AST.h"
#include "Message.h"
#include "Token.h"

#include <vector>

namespace Frertex
{
	struct ParseResult
	{
		std::size_t m_UsedTokens;
		ASTNode     m_Node;
	};

	class Parser
	{
	public:
		Parser(Sources* sources);

		AST lex(Utils::CopyMovable<std::vector<Token>>&& tokens);

		void addWarning(std::uint32_t fileID, std::uint32_t sourceID, std::size_t index, std::size_t length, std::size_t point, Utils::CopyMovable<std::string>&& message);
		void addError(std::uint32_t fileID, std::uint32_t sourceID, std::size_t index, std::size_t length, std::size_t point, Utils::CopyMovable<std::string>&& message);
		void addWarning(const Token& token, std::size_t point, Utils::CopyMovable<std::string>&& message)
		{
			addWarning(token.m_FileID, token.m_SourceID, token.m_Index, token.m_Length, point, message.get());
		}
		void addError(const Token& token, std::size_t point, Utils::CopyMovable<std::string>&& message)
		{
			addError(token.m_FileID, token.m_SourceID, token.m_Index, token.m_Length, point, message.get());
		}

		auto& getMessages() const { return m_Messages; }

	private:
		ParseResult lexTranslationUnit(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		ParseResult lexIntegerLiteral(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		ParseResult lexFloatLiteral(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		ParseResult lexBoolLiteral(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		ParseResult lexLiteral(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		ParseResult lexTypeQualifier(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		ParseResult lexTypeQualifiers(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		ParseResult lexIdentifier(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		ParseResult lexTypename(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		ParseResult lexArgument(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		ParseResult lexArguments(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		ParseResult lexAttribute(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		ParseResult lexAttributes(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		ParseResult lexParameter(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		ParseResult lexParameters(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		ParseResult lexBracedInitList(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		ParseResult lexInitializerClause(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		ParseResult lexAssignmentExpression(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		ParseResult lexExpression(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		ParseResult lexEmptyStatement(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		ParseResult lexExpressionStatement(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		ParseResult lexCompoundStatement(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		ParseResult lexReturnStatement(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		ParseResult lexStatement(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		ParseResult lexStatements(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		ParseResult lexFunctionDeclaration(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		ParseResult lexDeclaration(std::vector<Token>& tokens, std::size_t start, std::size_t end);

	private:
		Sources* m_Sources;

		std::vector<Message> m_Messages;
	};
} // namespace Frertex