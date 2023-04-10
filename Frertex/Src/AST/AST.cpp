#include "Frertex/AST/AST.h"

#include <bit>

namespace Frertex::AST
{
	std::string_view TypeToString(EType type)
	{
		switch (type)
		{
		case EType::Unknown: return "Unknown";
		case EType::Declarations: return "Declarations";
		case EType::FunctionDeclaration: return "FunctionDeclaration";
		case EType::Statements: return "Statements";
		case EType::EmptyStatement: return "EmptyStatement";
		case EType::CompoundStatement: return "CompoundStatement";
		case EType::Parameters: return "Parameters";
		case EType::Parameter: return "Parameter";
		case EType::Arguments: return "Arguments";
		case EType::Argument: return "Argument";
		case EType::Attributes: return "Attributes";
		case EType::Attribute: return "Attribute";
		case EType::Typename: return "Typename";
		case EType::TypeQualifier: return "TypeQualifier";
		case EType::IntegerLiteral: return "IntegerLiteral";
		case EType::FloatLiteral: return "FloatLiteral";
		case EType::BoolLiteral: return "BoolLiteral";
		case EType::Identifier: return "Identifier";
		}
		return "Unknown";
	}

	AST::AST()
		: m_Nodes(64),
		  m_AllocationMap(1),
		  m_PreviousAllocation(0),
		  m_Size(0),
		  m_RootNode(~0ULL)
	{
	}

	std::uint64_t AST::GetChild(std::uint64_t node, std::uint64_t index) const
	{
		if (node >= m_Nodes.size())
			return ~0ULL;

		std::uint64_t curNode = m_Nodes[node].Child;
		for (std::size_t i = 0; curNode < m_Nodes.size() && i < index; ++i)
			curNode = m_Nodes[curNode].NextSibling;
		return curNode < m_Nodes.size() ? curNode : ~0ULL;
	}

	std::uint64_t AST::Alloc(Node&& value)
	{
		if (m_Size == m_Nodes.size())
		{
			m_Nodes.resize(((m_Nodes.size() + (m_Nodes.size() >> 1) + 63) >> 6) << 6);
			m_AllocationMap.resize(m_Nodes.size() >> 6);
		}

		std::uint64_t node = FindNextAvailableNode();
		if (node >= m_Nodes.size())
			return ~0ULL;

		std::uint64_t allocationMapIndex = node >> 6;
		std::uint64_t allocationMapBit   = node & 0x3F;

		m_AllocationMap[allocationMapIndex] = m_AllocationMap[allocationMapIndex] | (1ULL << allocationMapBit);
		m_PreviousAllocation                = node;
		++m_Size;

		new (&m_Nodes[node]) Node { std::move(value) };
		return node;
	}

	void AST::Free(std::uint64_t node)
	{
		if (node >= m_Nodes.size())
			return;

		std::uint64_t allocationMapIndex = node >> 6;
		std::uint64_t allocationMapBit   = node & 0x3F;

		m_AllocationMap[allocationMapIndex] &= ~(1ULL << allocationMapBit);
		if (node < m_PreviousAllocation)
			m_PreviousAllocation = node;
		--m_Size;

		new (&m_Nodes[node]) Node {};
	}

	void AST::FreeFull(std::uint64_t node, bool freeSiblings)
	{
		if (node >= m_Nodes.size())
			return;

		auto& n = m_Nodes[node];
		FreeFull(n.Child, true);
		if (freeSiblings)
			FreeFull(n.NextSibling, true);
		Free(node);
	}

	void AST::SetParent(std::uint64_t child, std::uint64_t parent)
	{
		if (parent < m_Nodes.size())
			m_Nodes[parent].Child = child < m_Nodes.size() ? child : ~0ULL;
		if (child < m_Nodes.size())
			m_Nodes[child].Parent = parent < m_Nodes.size() ? parent : ~0ULL;
	}

	void AST::SetSiblings(std::uint64_t first, std::uint64_t second)
	{
		if (first < m_Nodes.size())
			m_Nodes[first].NextSibling = second < m_Nodes.size() ? second : ~0ULL;
		if (second < m_Nodes.size())
			m_Nodes[second].PreviousSibling = first < m_Nodes.size() ? first : ~0ULL;
	}

	std::uint64_t AST::FindNextAvailableNode() const
	{
		std::uint64_t startIndex = m_PreviousAllocation >> 6;
		std::uint64_t curIndex   = startIndex;
		do {
			std::uint64_t bit = std::countr_one(m_AllocationMap[curIndex]);
			if (bit == 64)
			{
				if (++curIndex >= m_AllocationMap.size())
					curIndex = 0ULL;
				continue;
			}
			return curIndex << 6 | bit;
		}
		while (curIndex != startIndex);
		return ~0ULL;
	}

	bool AST::IsAllocated(std::uint64_t node) const
	{
		std::uint64_t index = node >> 6;
		if (index > m_AllocationMap.size()) return false;
		std::uint64_t bit = node & 0x3F;
		return m_AllocationMap[index] & bit;
	}
} // namespace Frertex::AST