#pragma once

#include "Message.h"
#include "Token.h"

#include <vector>

namespace Frertex
{
	enum class EASTNodeType : std::uint32_t
	{
		Unknown = 0,
		TranslationUnit,
		Operator,
		TypeQualifier,
		TypeQualifiers,
		Identifier,
		Typename,
		Attribute,
		Attributes,
		Argument,
		Arguments,
		BracedInitList,
		AssignmentExpression,
		ExpressionStatement,
		CompoundStatement,
		ReturnStatement,
		Statement,
		Statements,
		FunctionDeclaration,
		Declaration
	};

	std::string_view ASTNodeTypeToString(EASTNodeType type);

	struct ASTNode
	{
	public:
		ASTNode()
		    : m_Type(EASTNodeType::Unknown) {}
		ASTNode(EASTNodeType type, Utils::CopyMovable<Token>&& token = Token {}, Utils::CopyMovable<std::vector<ASTNode>>&& children = std::vector<ASTNode> {})
		    : m_Type(type), m_Token(token.get()), m_Children(children.get()) {}

		auto           getType() const { return m_Type; }
		auto&          getToken() { return m_Token; }
		auto&          getToken() const { return m_Token; }
		ASTNode*       getChild(std::size_t index) { return index <= m_Children.size() ? &m_Children[index] : nullptr; }
		const ASTNode* getChild(std::size_t index) const { return index <= m_Children.size() ? &m_Children[index] : nullptr; }
		auto&          getChildren() { return m_Children; }
		auto&          getChildren() const { return m_Children; }

	private:
		EASTNodeType         m_Type;
		Token                m_Token;
		std::vector<ASTNode> m_Children;
	};

	struct AST
	{
	public:
		void setRoot(Utils::CopyMovable<ASTNode>&& node);

		auto getRoot() const { return m_Root; }

	private:
		ASTNode* m_Root = nullptr;
	};

	struct LexResult
	{
		std::size_t m_UsedTokens;
		ASTNode     m_Node;
	};

	class Lexer
	{
	public:
		AST lex(Utils::CopyMovable<std::vector<Token>>&& tokens);

		void addWarning(SourceSpan span, SourcePoint point, Utils::CopyMovable<std::string>&& message);
		void addError(SourceSpan span, SourcePoint point, Utils::CopyMovable<std::string>&& message);

		auto& getMessages() const { return m_Messages; }

	private:
		LexResult lexTranslationUnit(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		LexResult lexTypeQualifier(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		LexResult lexTypeQualifiers(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		LexResult lexIdentifier(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		LexResult lexTypename(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		LexResult lexAttribute(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		LexResult lexAttributes(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		LexResult lexArgument(std::vector<Token>& tokens, std::size_t start, std::size_t end);
		LexResult lexArguments(std::vector<Token>& tokens, std::size_t start, std::size_t end);
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
		std::vector<Message> m_Messages;
	};
} // namespace Frertex