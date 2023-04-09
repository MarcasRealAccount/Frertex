#pragma once

#include "Frertex/Tokenizer/Token.h"
#include "Frertex/Utils/View.h"

#include <cstdint>

#include <string_view>
#include <vector>

namespace Frertex::AST
{
	enum class EType : std::uint16_t
	{
		Unknown = 0,

		Declarations,
		FunctionDeclaration,

		Statements,
		EmptyStatement,
		CompoundStatement,

		Parameters,
		Parameter,
		Arguments,
		Argument,
		Attributes,
		Attribute,

		Typename,
		TypeQualifiers,
		TypeQualifier,

		IntegerLiteral,
		FloatLiteral,
		BoolLiteral,

		Identifier
	};

	std::string_view TypeToString(EType type);

	struct Node
	{
	public:
		EType            Type;
		std::uint16_t    Pad[3]          = { 0, 0, 0 };
		Tokenizer::Token Token           = {};
		std::uint64_t    Parent          = ~0ULL;
		std::uint64_t    Child           = ~0ULL;
		std::uint64_t    NextSibling     = ~0ULL;
		std::uint64_t    PreviousSibling = ~0ULL;
	};

	static constexpr auto S = sizeof(Node);

	struct AST
	{
	public:
		AST();

		std::uint64_t RootNode() const { return m_RootNode; }

		void SetRootNode(std::uint64_t node) { m_RootNode = node; }

		std::uint64_t Alloc(Node&& value = {});
		void          Free(std::uint64_t node);
		void          FreeFull(std::uint64_t node, bool freeSiblings = false);

		void SetParent(std::uint64_t child, std::uint64_t parent);
		void SetSiblings(std::uint64_t first, std::uint64_t second);

		std::uint64_t FindNextAvailableNode() const;
		bool          IsAllocated(std::uint64_t node) const;

		Node& operator[](std::uint64_t node) { return m_Nodes[node]; }

		const Node& operator[](std::uint64_t node) const { return m_Nodes[node]; }

		std::uint64_t Size() const { return m_Size; }

	private:
		std::vector<Node>          m_Nodes;
		std::vector<std::uint64_t> m_AllocationMap;
		std::uint64_t              m_PreviousAllocation;
		std::uint64_t              m_Size;

		std::uint64_t m_RootNode;
	};
} // namespace Frertex::AST