#include "Frertex/Parser/AST.h"

namespace Frertex::AST
{
	std::string_view TypeToString(EType type)
	{
		switch (type)
		{
		case EType::Unknown: return "Unknown";
		case EType::IntegerLiteral: return "IntegerLiteral";
		case EType::FloatLiteral: return "FloatLiteral";
		case EType::BoolLiteral: return "BoolLiteral";
		case EType::Operator: return "Operator";
		case EType::TypeQualifier: return "TypeQualifier";
		case EType::TypeQualifiers: return "TypeQualifiers";
		case EType::Identifier: return "Identifier";
		case EType::Typename: return "Typename";
		case EType::FunctionParameterTypename: return "FunctionParameterTypename";
		case EType::Argument: return "Argument";
		case EType::Arguments: return "Arguments";
		case EType::Attribute: return "Attribute";
		case EType::Attributes: return "Attributes";
		case EType::Parameter: return "Parameter";
		case EType::Parameters: return "Parameters";
		case EType::BracedInitList: return "BracedInitList";
		case EType::AssignmentExpression: return "AssignmentExpression";
		case EType::EmptyStatement: return "EmptyStatement";
		case EType::ExpressionStatement: return "ExpressionStatement";
		case EType::CompoundStatement: return "CompoundStatement";
		case EType::ReturnStatement: return "ReturnStatement";
		case EType::Statements: return "Statements";
		case EType::FunctionDeclaration: return "FunctionDeclaration";
		case EType::TranslationUnit: return "TranslationUnit";
		}
		return "?";
	}
} // namespace Frertex::AST