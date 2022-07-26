#pragma once

#include "AST.h"
#include "Message.h"
#include "Token.h"

#include <vector>

namespace Frertex
{
	struct LexResult
	{
		std::size_t m_UsedTokens;
		ASTNode     m_Node;
	};

	class Lexer
	{
	public:
		Lexer(Sources* sources);

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
		LexResult lexTranslationUnit(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		LexResult lexIntegerLiteral(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		LexResult lexFloatLiteral(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		LexResult lexLiteral(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		LexResult lexTypeQualifier(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		LexResult lexTypeQualifiers(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		LexResult lexIdentifier(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		LexResult lexTypename(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		LexResult lexArgument(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		LexResult lexArguments(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		LexResult lexAttribute(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		LexResult lexAttributes(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		LexResult lexParameter(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		LexResult lexParameters(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		LexResult lexBracedInitList(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		LexResult lexInitializerClause(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		LexResult lexAssignmentExpression(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		LexResult lexExpression(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		LexResult lexExpressionStatement(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		LexResult lexCompoundStatement(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		LexResult lexReturnStatement(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		LexResult lexStatement(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		LexResult lexStatements(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		LexResult lexFunctionDeclaration(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		LexResult lexDeclaration(std::vector<Token>& tokens, std::size_t start, std::size_t end);

	private:
		Sources* m_Sources;

		std::vector<Message> m_Messages;
	};
} // namespace Frertex