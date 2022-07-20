#include "Frertex/Lexer.h"
#include "Frertex/Utils/Profiler.h"

namespace Frertex
{
	AST Lexer::lex(Utils::CopyMovable<std::vector<Token>>&& tokens)
	{
		PROFILE_FUNC;

		std::vector<Token> input = tokens.get();

		AST  ast;
		auto result = lexTranslationUnit(input, 0, input.size() - 1);
		ast.setRoot(std::move(result.m_Node));
		return ast;
	}

	void Lexer::addWarning(SourceSpan span, SourcePoint point, Utils::CopyMovable<std::string>&& message)
	{
		PROFILE_FUNC;

		m_Messages.emplace_back(EMessageType::Warning, span, point, message.get());
	}

	void Lexer::addError(SourceSpan span, SourcePoint point, Utils::CopyMovable<std::string>&& message)
	{
		PROFILE_FUNC;

		m_Messages.emplace_back(EMessageType::Error, span, point, message.get());
	}

	LexResult Lexer::lexTranslationUnit(std::vector<Token>& tokens, std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		std::size_t          usedTokens = 0;
		std::vector<ASTNode> nodes;

		while (start + usedTokens <= end)
		{
			auto result = lexDeclaration(tokens, start + usedTokens, end);
			if (result.m_UsedTokens == 0)
				break;
			usedTokens += result.m_UsedTokens;
			nodes.emplace_back(std::move(result.m_Node));
		}

		return { usedTokens, { EASTNodeType::TranslationUnit, Token {}, std::move(nodes) } };
	}

	LexResult Lexer::lexTypeQualifier(std::vector<Token>& tokens, std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		Token& token = tokens[start];
		if (token.m_Class != ETokenClass::Identifier)
			return {};

		auto& str         = token.m_Str;
		bool  isQualifier = false;
		if (str == "in" ||
		    str == "out" ||
		    str == "inout")
			isQualifier = true;

		if (isQualifier)
			return { 1, { EASTNodeType::TypeQualifier, Token { token } } };
		else
			return {};
	}

	LexResult Lexer::lexTypeQualifiers(std::vector<Token>& tokens, std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return { 0, { EASTNodeType::TypeQualifiers } };

		std::size_t          usedTokens = 0;
		std::vector<ASTNode> nodes;

		while (true)
		{
			auto result = lexTypeQualifier(tokens, start + usedTokens, end);
			if (result.m_UsedTokens == 0)
				break;
			usedTokens += result.m_UsedTokens;
			nodes.emplace_back(std::move(result.m_Node));
		}

		return { usedTokens, { EASTNodeType::TypeQualifiers, Token {}, std::move(nodes) } };
	}

	LexResult Lexer::lexIdentifier(std::vector<Token>& tokens, std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		Token& token = tokens[start];
		if (token.m_Class != ETokenClass::Identifier)
			return {};
		return { 1, { EASTNodeType::Identifier, Token { token } } };
	}

	LexResult Lexer::lexTypename(std::vector<Token>& tokens, std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		std::size_t          usedTokens = 0;
		std::vector<ASTNode> nodes;

		auto result = lexTypeQualifiers(tokens, start + usedTokens, end);
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		result = lexIdentifier(tokens, start + usedTokens, end);
		if (result.m_UsedTokens == 0)
			return {};
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		auto identifierToken = nodes[1].getToken();

		return { usedTokens, { EASTNodeType::Typename, std::move(identifierToken), std::move(nodes) } };
	}

	LexResult Lexer::lexAttribute(std::vector<Token>& tokens, std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		auto result = lexIdentifier(tokens, start, end);
		if (result.m_UsedTokens == 0)
			return {};

		auto identifierToken = result.m_Node.getToken();

		return { result.m_UsedTokens, { EASTNodeType::Attribute, std::move(identifierToken), { std::move(result.m_Node) } } };
	}

	LexResult Lexer::lexAttributes(std::vector<Token>& tokens, std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return { 0, { EASTNodeType::Attributes } };

		std::size_t          usedTokens = 0;
		std::vector<ASTNode> nodes;

		{
			// Opening
			Token& a = tokens[start + usedTokens];
			if (a.m_Class != ETokenClass::Symbol || a.m_Str != "[")
				return { 0, { EASTNodeType::Attributes } };
			++usedTokens;
			Token& b = tokens[start + usedTokens];
			if (b.m_Class != ETokenClass::Symbol || b.m_Str != "[")
				return { 0, { EASTNodeType::Attributes } };
			++usedTokens;
		}
		std::size_t attributesEnd = start + usedTokens;
		{
			// Closing
			std::uint64_t count = 0;
			while (attributesEnd <= end)
			{
				Token& token = tokens[attributesEnd];
				if (token.m_Class != ETokenClass::Symbol || token.m_Str != "]")
				{
					count = 0;
					++attributesEnd;
					continue;
				}
				++count;
				++attributesEnd;
				if (count >= 2)
					break;
			}

			if (attributesEnd >= end)
				return { 0, { EASTNodeType::Attributes } };
		}

		while (start + usedTokens < attributesEnd - 2)
		{
			auto result = lexAttribute(tokens, start + usedTokens, attributesEnd - 2);
			if (result.m_UsedTokens == 0)
				return { 0, { EASTNodeType::Attributes } };
			usedTokens += result.m_UsedTokens;
			nodes.emplace_back(std::move(result.m_Node));
		}

		return { attributesEnd - start, { EASTNodeType::Attributes, Token {}, std::move(nodes) } };
	}

	LexResult Lexer::lexArgument(std::vector<Token>& tokens, std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		std::size_t          usedTokens = 0;
		std::vector<ASTNode> nodes;

		auto result = lexAttributes(tokens, start + usedTokens, end);
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		result = lexTypename(tokens, start + usedTokens, end);
		if (result.m_UsedTokens == 0)
			return {};
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		result = lexIdentifier(tokens, start + usedTokens, end);
		if (result.m_UsedTokens == 0)
			return {};
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		auto identifierToken = nodes[2].getToken();

		return { usedTokens, { EASTNodeType::Argument, std::move(identifierToken), std::move(nodes) } };
	}

	LexResult Lexer::lexArguments(std::vector<Token>& tokens, std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		std::size_t          usedTokens = 0;
		std::vector<ASTNode> nodes;

		while (true)
		{
			auto result = lexArgument(tokens, start + usedTokens, end);
			if (result.m_UsedTokens == 0)
				break;
			usedTokens += result.m_UsedTokens;
			nodes.emplace_back(std::move(result.m_Node));
			if (start + usedTokens > end)
				break;

			auto& token = tokens[start + usedTokens];
			if (token.m_Class != ETokenClass::Symbol || token.m_Str != ",")
				break;
			++usedTokens;
		}

		return { usedTokens, { EASTNodeType::Arguments, Token {}, std::move(nodes) } };
	}

	LexResult Lexer::lexBracedInitList(std::vector<Token>& tokens, std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		std::size_t          usedTokens = 0;
		std::vector<ASTNode> nodes;

		{
			// Opening
			Token& token = tokens[start + usedTokens];
			if (token.m_Class != ETokenClass::Symbol && token.m_Str != "{")
				return {};
			++usedTokens;
		}
		std::size_t listEnd = start + usedTokens;
		{
			// Closing
			std::uint64_t count = 1;
			while (listEnd < end)
			{
				Token& token = tokens[listEnd];
				if (token.m_Class == ETokenClass::Symbol)
				{
					if (token.m_Str == "{")
						++count;
					else if (token.m_Str == "}")
						--count;
					++listEnd;
				}
				else
				{
					++listEnd;
					continue;
				}
				if (count == 0)
					break;
			}
			if (count != 0)
				return {};
		}

		while (start + usedTokens < listEnd - 1)
		{
			auto result = lexInitializerClause(tokens, start + usedTokens, listEnd - 1);
			if (result.m_UsedTokens == 0)
				break;
			usedTokens += result.m_UsedTokens;
			nodes.emplace_back(std::move(result.m_Node));

			auto& token = tokens[start + usedTokens];
			if (token.m_Class != ETokenClass::Symbol || token.m_Str != ",")
				break;
			++usedTokens;
		}

		return { listEnd - start, { EASTNodeType::BracedInitList, tokens[start], std::move(nodes) } };
	}

	LexResult Lexer::lexInitializerClause(std::vector<Token>& tokens, std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		auto result = lexAssignmentExpression(tokens, start, end);
		if (result.m_UsedTokens != 0)
			return result;

		return lexBracedInitList(tokens, start, end);
	}

	LexResult Lexer::lexAssignmentExpression(std::vector<Token>& tokens, std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		std::size_t          usedTokens = 0;
		std::vector<ASTNode> nodes;

		auto result = lexIdentifier(tokens, start + usedTokens, end);
		if (result.m_UsedTokens == 0 ||
		    start + usedTokens > end)
			return {};
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(result.m_Node);

		std::size_t operatorIndex = start + usedTokens;
		{
			auto& token = tokens[start + usedTokens];
			if (token.m_Class != ETokenClass::Symbol)
				return {};

			bool assignment = false;
			if (token.m_Str == "=")
				assignment = true;

			if (!assignment)
				return {};

			++usedTokens;
			nodes.emplace_back(EASTNodeType::Operator, token, std::vector<ASTNode> {});
		}

		result = lexInitializerClause(tokens, start + usedTokens, end);
		if (result.m_UsedTokens == 0)
			return {};
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		return { usedTokens, { EASTNodeType::AssignmentExpression, tokens[operatorIndex], std::move(nodes) } };
	}

	LexResult Lexer::lexExpression(std::vector<Token>& tokens, std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		auto result = lexAssignmentExpression(tokens, start, end);
		if (result.m_UsedTokens != 0)
			return result;

		return {};
	}

	LexResult Lexer::lexExpressionStatement(std::vector<Token>& tokens, std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		std::size_t          usedTokens = 0;
		std::vector<ASTNode> nodes;

		auto result = lexExpression(tokens, start + usedTokens, end);
		if (result.m_UsedTokens == 0)
			return {};
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		auto& token = tokens[start + usedTokens];
		if (token.m_Class != ETokenClass::Symbol || token.m_Str != ";")
			return {};
		++usedTokens;

		return { usedTokens, { EASTNodeType::ExpressionStatement, tokens[start + usedTokens - 1], std::move(nodes) } };
	}

	LexResult Lexer::lexCompoundStatement(std::vector<Token>& tokens, std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		std::size_t          usedTokens = 0;
		std::vector<ASTNode> nodes;

		{
			// Opening
			Token& token = tokens[start + usedTokens];
			if (token.m_Class != ETokenClass::Symbol && token.m_Str != "{")
				return {};
			++usedTokens;
		}
		std::size_t blockEnd = start + usedTokens;
		{
			// Closing
			std::uint64_t count = 1;
			while (blockEnd <= end)
			{
				Token& token = tokens[blockEnd];
				if (token.m_Class == ETokenClass::Symbol)
				{
					if (token.m_Str == "{")
						++count;
					else if (token.m_Str == "}")
						--count;
					++blockEnd;
				}
				else
				{
					++blockEnd;
					continue;
				}
				if (count == 0)
					break;
			}
			if (count != 0)
				return {};
		}

		while (start + usedTokens < blockEnd - 1)
		{
			auto result = lexStatement(tokens, start + usedTokens, blockEnd - 1);
			if (result.m_UsedTokens == 0)
				return {};
			usedTokens += result.m_UsedTokens;
			nodes.emplace_back(std::move(result.m_Node));
		}

		return { blockEnd - start, { EASTNodeType::CompoundStatement, tokens[start], std::move(nodes) } };
	}

	LexResult Lexer::lexReturnStatement(std::vector<Token>& tokens, std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		std::size_t          usedTokens = 0;
		std::vector<ASTNode> nodes;

		{
			auto& token = tokens[start + usedTokens];
			if (token.m_Class != ETokenClass::Identifier || token.m_Str != "return")
				return {};
			++usedTokens;
		}

		do
		{
			auto result = lexExpression(tokens, start, end);
			if (result.m_UsedTokens != 0)
			{
				usedTokens += result.m_UsedTokens;
				nodes.emplace_back(result.m_Node);
				break;
			}

			result = lexBracedInitList(tokens, start, end);
			if (result.m_UsedTokens != 0)
			{
				usedTokens += result.m_UsedTokens;
				nodes.emplace_back(result.m_Node);
				break;
			}
		} while (false);

		{
			auto& token = tokens[start + usedTokens];
			if (token.m_Class != ETokenClass::Symbol || token.m_Str != ";")
				return {};
			++usedTokens;
		}

		return { usedTokens, { EASTNodeType::ReturnStatement, tokens[start], std::move(nodes) } };
	}

	LexResult Lexer::lexStatement(std::vector<Token>& tokens, std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		auto result = lexExpressionStatement(tokens, start, end);
		if (result.m_UsedTokens != 0)
			return result;

		result = lexCompoundStatement(tokens, start, end);
		if (result.m_UsedTokens != 0)
			return result;

		result = lexReturnStatement(tokens, start, end);
		if (result.m_UsedTokens != 0)
			return result;

		result = lexDeclaration(tokens, start, end);
		if (result.m_UsedTokens != 0)
			return result;

		return {};
	}

	LexResult Lexer::lexStatements(std::vector<Token>& tokens, std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return { 0, { EASTNodeType::Statements } };

		std::size_t          usedTokens = 0;
		std::vector<ASTNode> nodes;

		while (start + usedTokens < end)
		{
			auto result = lexStatement(tokens, start + usedTokens, end);
			if (result.m_UsedTokens == 0)
				break;
			usedTokens += result.m_UsedTokens;
			nodes.emplace_back(std::move(result.m_Node));
		}

		return { usedTokens, { EASTNodeType::Statements, Token {}, std::move(nodes) } };
	}

	LexResult Lexer::lexFunctionDeclaration(std::vector<Token>& tokens, std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		std::size_t          usedTokens = 0;
		std::vector<ASTNode> nodes;

		auto result = lexAttributes(tokens, start + usedTokens, end);
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		result = lexTypename(tokens, start + usedTokens, end);
		if (result.m_UsedTokens == 0)
			return {};
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		result = lexIdentifier(tokens, start + usedTokens, end);
		if (result.m_UsedTokens == 0)
			return {};
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		if (start + usedTokens > end)
			return {};

		{
			// Argument opening
			auto& token = tokens[start + usedTokens];
			if (token.m_Class != ETokenClass::Symbol || token.m_Str != "(")
				return {};
			++usedTokens;
		}

		result = lexArguments(tokens, start + usedTokens, end);
		if (result.m_UsedTokens == 0)
			return {};
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		if (start + usedTokens > end)
			return {};

		{
			// Argument closing
			auto& token = tokens[start + usedTokens];
			if (token.m_Class != ETokenClass::Symbol || token.m_Str != ")")
				return {};
			++usedTokens;
		}

		result = lexCompoundStatement(tokens, start + usedTokens, end);
		if (result.m_UsedTokens == 0)
			return {};
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		auto identifierToken = nodes[2].getToken();

		return { usedTokens, { EASTNodeType::FunctionDeclaration, std::move(identifierToken), std::move(nodes) } };
	}

	LexResult Lexer::lexDeclaration(std::vector<Token>& tokens, std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		auto result = lexFunctionDeclaration(tokens, start, end);
		if (result.m_UsedTokens != 0)
			return result;

		return {};
	}
} // namespace Frertex
