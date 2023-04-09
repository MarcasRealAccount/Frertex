#pragma once

#include "Frertex/Tokenizer/Token.h"
#include "Frertex/Utils/View.h"

#include <cstdint>

#include <string_view>
#include <vector>

namespace Frertex::AST
{
	enum class EType : std::uint32_t
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
		EType             Type;
		Tokenizer::Token  Token;
		std::vector<Node> Children;
	};
} // namespace Frertex::AST