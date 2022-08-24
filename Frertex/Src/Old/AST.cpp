#include "Frertex/AST.h"
#include "Frertex/Utils/Profiler.h"

namespace Frertex::AST
{
	std::string_view TypeToString(EType type)
	{
		PROFILE_FUNC;

		switch (type)
		{
		case EType::Unknown: return "Unknown";
		case EType::IntegerLiteral: return "IntegerLiteral";
		case EType::FloatLiteral: return "FloatLiteral";
		case EType::BoolLiteral: return "BoolLiteral";
		case EType::TranslationUnit: return "TranslationUnit";
		case EType::Operator: return "Operator";
		case EType::TypeQualifier: return "TypeQualifier";
		case EType::TypeQualifiers: return "TypeQualifiers";
		case EType::Identifier: return "Identifier";
		case EType::Typename: return "Typename";
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
		}

		return "Unknown";
	}

	void Result::setRoot(Utils::CopyMovable<Node>&& node)
	{
		PROFILE_FUNC;

		m_Root = node.get();
	}
} // namespace Frertex::AST