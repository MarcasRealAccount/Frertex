#pragma once

#include "Frertex/Tokenizer/Token.h"

#include <vector>

namespace Frertex::AST
{
	enum class EType : std::uint32_t
	{
		Unknown = 0,
		IntegerLiteral,
		FloatLiteral,
		BoolLiteral,
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
		FunctionDeclaration,
		TranslationUnit
	};

	std::string_view TypeToString(EType type);

	struct Node
	{
	public:
		Node() : m_Type(EType::Unknown) {}
		Node(EType type, const Tokenizer::Token& token = {}, Utils::CopyMovable<std::vector<Node>>&& children = std::vector<Node> {})
		    : m_Type(type), m_Token(token), m_Children(children.get()) {}

		auto  getType() const { return m_Type; }
		auto& getToken() { return m_Token; }
		auto& getToken() const { return m_Token; }
		auto  getChild(std::size_t index) { return index <= m_Children.size() ? &m_Children[index] : nullptr; }
		auto  getChild(std::size_t index) const { return index <= m_Children.size() ? &m_Children[index] : nullptr; }
		auto& getChildren() { return m_Children; }
		auto& getChildren() const { return m_Children; }

	private:
		EType             m_Type;
		Tokenizer::Token  m_Token;
		std::vector<Node> m_Children;
	};
} // namespace Frertex::AST