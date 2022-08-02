#pragma once

#include "Token.h"

#include <vector>

namespace Frertex
{
	enum class EASTNodeType : std::uint32_t
	{
		Unknown = 0,
		IntegerLiteral,
		FloatLiteral,
		BoolLiteral,
		TranslationUnit,
		Operator,
		TypeQualifier,
		TypeQualifiers,
		Identifier,
		Typename,
		Argument,
		Arguments,
		Attribute,
		Attributes,
		Parameter,
		Parameters,
		BracedInitList,
		AssignmentExpression,
		EmptyStatement,
		ExpressionStatement,
		CompoundStatement,
		ReturnStatement,
		Statements,
		FunctionDeclaration
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
} // namespace Frertex