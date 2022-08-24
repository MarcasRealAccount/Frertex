#pragma once

#include "Token.h"

#include <vector>

namespace Frertex::AST
{
	enum class EType : std::uint32_t
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

	std::string_view TypeToString(EType type);

	struct Node
	{
	public:
		Node()
		    : m_Type(EType::Unknown) {}
		Node(EType type, Utils::CopyMovable<Token>&& token = Token {}, Utils::CopyMovable<std::vector<Node>>&& children = std::vector<Node> {})
		    : m_Type(type), m_Token(token.get()), m_Children(children.get()) {}

		auto        getType() const { return m_Type; }
		auto&       getToken() { return m_Token; }
		auto&       getToken() const { return m_Token; }
		Node*       getChild(std::size_t index) { return index <= m_Children.size() ? &m_Children[index] : nullptr; }
		const Node* getChild(std::size_t index) const { return index <= m_Children.size() ? &m_Children[index] : nullptr; }
		auto&       getChildren() { return m_Children; }
		auto&       getChildren() const { return m_Children; }

	private:
		EType             m_Type;
		Token             m_Token;
		std::vector<Node> m_Children;
	};

	struct Result
	{
	public:
		void setRoot(Utils::CopyMovable<Node>&& node);

		auto& getRoot() const { return m_Root; }

	private:
		Node m_Root;
	};
} // namespace Frertex::AST