#pragma once

#include "Frertex/Tokenizer/Token.h"
#include "Frertex/Utils/View.h"

#include <cstddef>
#include <cstdint>

#include <concepts>
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

	struct AST
	{
	public:
		AST();

		std::uint64_t GetChild(std::uint64_t node, std::uint64_t index) const;

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

	enum class EWalkerResult
	{
		Continue,
		SkipSiblings,
		SkipChild,
		Stop
	};

	template <class F>
	concept WalkerVisiter =
		requires(F&& f, AST& a, std::uint64_t index, Node& node) {
			{
				f(a, index, node)
				} -> std::same_as<EWalkerResult>;
		};

	template <class F>
	concept ConstWalkerVisiter =
		requires(F&& f, const AST& a, std::uint64_t index, const Node& node) {
			{
				f(a, index, node)
				} -> std::same_as<EWalkerResult>;
		};

	namespace Details
	{
		EWalkerResult VisitNode(AST& ast, std::uint64_t node, std::size_t depth, WalkerVisiter auto& enter, WalkerVisiter auto& exit)
		{
			if (node == ~0ULL)
				return EWalkerResult::Continue;

			EWalkerResult result = enter(ast, node, ast[node]);
			if (result == EWalkerResult::Stop)
				return EWalkerResult::Stop;

			if (result != EWalkerResult::SkipChild)
				if (VisitNode(ast, ast[node].Child, depth + 1, enter, exit) == EWalkerResult::Stop)
					return EWalkerResult::Stop;

			EWalkerResult result2 = exit(ast, node, ast[node]);

			if (depth > 0 && result != EWalkerResult::SkipSiblings)
				if (VisitNode(ast, ast[node].NextSibling, depth, enter, exit) == EWalkerResult::Stop)
					return EWalkerResult::Stop;
			return result2;
		}

		EWalkerResult VisitNode(const AST& ast, std::uint64_t node, std::size_t depth, ConstWalkerVisiter auto& enter, ConstWalkerVisiter auto& exit)
		{
			if (node == ~0ULL)
				return EWalkerResult::Continue;

			EWalkerResult result = enter(ast, node, ast[node]);
			if (result == EWalkerResult::Stop)
				return EWalkerResult::Stop;

			if (result != EWalkerResult::SkipChild)
				if (VisitNode(ast, ast[node].Child, depth + 1, enter, exit) == EWalkerResult::Stop)
					return EWalkerResult::Stop;

			EWalkerResult result2 = exit(ast, node, ast[node]);

			if (depth > 0 && result != EWalkerResult::SkipSiblings)
				if (VisitNode(ast, ast[node].NextSibling, depth, enter, exit) == EWalkerResult::Stop)
					return EWalkerResult::Stop;
			return result2;
		}
	} // namespace Details

	void WalkASTNode(AST& ast, std::uint64_t node, WalkerVisiter auto&& enterNode, WalkerVisiter auto&& exitNode)
	{
		auto enter = std::forward<decltype(enterNode)>(enterNode);
		auto exit  = std::forward<decltype(exitNode)>(exitNode);

		Details::VisitNode(ast, node, 0, enter, exit);
	}

	void WalkASTNode(AST& ast, std::uint64_t node, WalkerVisiter auto&& enterNode)
	{
		WalkASTNode(ast,
					node,
					std::forward<decltype(enterNode)>(enterNode),
					[]([[maybe_unused]] AST& ast, [[maybe_unused]] std::uint64_t index, [[maybe_unused]] Node& node) -> EWalkerResult {
						return EWalkerResult::Continue;
					});
	}

	void WalkASTNode(const AST& ast, std::uint64_t node, ConstWalkerVisiter auto&& enterNode, ConstWalkerVisiter auto&& exitNode)
	{
		auto enter = std::forward<decltype(enterNode)>(enterNode);
		auto exit  = std::forward<decltype(exitNode)>(exitNode);

		Details::VisitNode(ast, node, 0, enter, exit);
	}

	void WalkASTNode(const AST& ast, std::uint64_t node, ConstWalkerVisiter auto&& enterNode)
	{
		WalkASTNode(ast,
					node,
					std::forward<decltype(enterNode)>(enterNode),
					[]([[maybe_unused]] const AST& ast, [[maybe_unused]] std::uint64_t index, [[maybe_unused]] const Node& node) -> EWalkerResult {
						return EWalkerResult::Continue;
					});
	}
} // namespace Frertex::AST