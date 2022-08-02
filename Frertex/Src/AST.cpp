#include "Frertex/AST.h"
#include "Frertex/Utils/Profiler.h"

namespace Frertex
{
	std::string_view ASTNodeTypeToString(EASTNodeType type)
	{
		PROFILE_FUNC;

		switch (type)
		{
		case EASTNodeType::Unknown: return "Unknown";
		case EASTNodeType::IntegerLiteral: return "IntegerLiteral";
		case EASTNodeType::FloatLiteral: return "FloatLiteral";
		case EASTNodeType::BoolLiteral: return "BoolLiteral";
		case EASTNodeType::TranslationUnit: return "TranslationUnit";
		case EASTNodeType::Operator: return "Operator";
		case EASTNodeType::TypeQualifier: return "TypeQualifier";
		case EASTNodeType::TypeQualifiers: return "TypeQualifiers";
		case EASTNodeType::Identifier: return "Identifier";
		case EASTNodeType::Typename: return "Typename";
		case EASTNodeType::Argument: return "Argument";
		case EASTNodeType::Arguments: return "Arguments";
		case EASTNodeType::Attribute: return "Attribute";
		case EASTNodeType::Attributes: return "Attributes";
		case EASTNodeType::Parameter: return "Parameter";
		case EASTNodeType::Parameters: return "Parameters";
		case EASTNodeType::BracedInitList: return "BracedInitList";
		case EASTNodeType::AssignmentExpression: return "AssignmentExpression";
		case EASTNodeType::EmptyStatement: return "EmptyStatement";
		case EASTNodeType::ExpressionStatement: return "ExpressionStatement";
		case EASTNodeType::CompoundStatement: return "CompoundStatement";
		case EASTNodeType::ReturnStatement: return "ReturnStatement";
		case EASTNodeType::Statements: return "Statements";
		case EASTNodeType::FunctionDeclaration: return "FunctionDeclaration";
		}

		return "Unknown";
	}

	void AST::setRoot(Utils::CopyMovable<ASTNode>&& node)
	{
		PROFILE_FUNC;

		m_Root = new ASTNode(node.get());
	}

} // namespace Frertex