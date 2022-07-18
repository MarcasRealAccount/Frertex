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
		TypeQualifier,
		TypeQualifiers,
		Identifier,
		Typename,
		Attribute,
		Attributes,
		Argument,
		Arguments,
		StatementBlock,
		Function
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
		LexResult lexTranslationUnit(std::vector<Token>& tokens, std::size_t offset);
		LexResult lexTypeQualifier(std::vector<Token>& tokens, std::size_t offset);
		LexResult lexTypeQualifiers(std::vector<Token>& tokens, std::size_t offset);
		LexResult lexIdentifier(std::vector<Token>& tokens, std::size_t offset);
		LexResult lexTypename(std::vector<Token>& tokens, std::size_t offset);
		LexResult lexAttribute(std::vector<Token>& tokens, std::size_t offset);
		LexResult lexAttributes(std::vector<Token>& tokens, std::size_t offset);
		LexResult lexArgument(std::vector<Token>& tokens, std::size_t offset);
		LexResult lexArguments(std::vector<Token>& tokens, std::size_t offset);
		LexResult lexStatementBlock(std::vector<Token>& tokens, std::size_t offset);
		LexResult lexFunction(std::vector<Token>& tokens, std::size_t offset);

	private:
		std::vector<Message> m_Messages;
	};
} // namespace Frertex