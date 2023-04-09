#include "Frertex/AST/AST.h"

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
		case EType::TypeQualifiers: return "TypeQualifiers";
		case EType::TypeQualifier: return "TypeQualifier";
		case EType::IntegerLiteral: return "IntegerLiteral";
		case EType::FloatLiteral: return "FloatLiteral";
		case EType::BoolLiteral: return "BoolLiteral";
		case EType::Identifier: return "Identifier";
		}
		return "Unknown";
	}
} // namespace Frertex::AST