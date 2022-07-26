#include "Frertex/Lexer.h"
#include "Frertex/Utils/Profiler.h"

namespace Frertex
{
	Lexer::Lexer(Sources* sources)
	    : m_Sources(sources) {}

	AST Lexer::lex(Utils::CopyMovable<std::vector<Token>>&& tokens)
	{
		PROFILE_FUNC;

		std::vector<Token> input = tokens.get();

		AST ast;
		if (!input.empty())
		{
			auto result = lexTranslationUnit(input, 0, input.size() - 1);
			ast.setRoot(std::move(result.m_Node));
		}
		return ast;
	}

	void Lexer::addWarning(std::uint32_t fileID, std::uint32_t sourceID, std::size_t index, std::size_t length, std::size_t point, Utils::CopyMovable<std::string>&& message)
	{
		PROFILE_FUNC;

		auto span             = m_Sources->getSpan(index, length, sourceID);
		auto pt               = m_Sources->getPoint(point, sourceID);
		span.m_Start.m_FileID = fileID;
		span.m_End.m_FileID   = fileID;
		pt.m_FileID           = fileID;
		m_Messages.emplace_back(EMessageType::Warning, span, pt, message.get());
	}

	void Lexer::addError(std::uint32_t fileID, std::uint32_t sourceID, std::size_t index, std::size_t length, std::size_t point, Utils::CopyMovable<std::string>&& message)
	{
		PROFILE_FUNC;

		auto span             = m_Sources->getSpan(index, length, sourceID);
		auto pt               = m_Sources->getPoint(point, sourceID);
		span.m_Start.m_FileID = fileID;
		span.m_End.m_FileID   = fileID;
		pt.m_FileID           = fileID;
		m_Messages.emplace_back(EMessageType::Error, span, pt, message.get());
	}

	LexResult Lexer::lexTranslationUnit(std::vector<Token>& tokens, std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

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

	LexResult Lexer::lexIntegerLiteral(std::vector<Token>& tokens, std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		Token& token = tokens[start];
		if (token.m_Class != ETokenClass::Integer &&
		    token.m_Class != ETokenClass::BinaryInteger &&
		    token.m_Class != ETokenClass::OctalInteger &&
		    token.m_Class != ETokenClass::HexInteger)
			return {};

		return { 1, { EASTNodeType::IntegerLiteral, std::move(token) } };
	}

	LexResult Lexer::lexFloatLiteral(std::vector<Token>& tokens, std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		Token& token = tokens[start];
		if (token.m_Class != ETokenClass::Float &&
		    token.m_Class != ETokenClass::HexFloat)
			return {};

		return { 1, { EASTNodeType::FloatLiteral, std::move(token) } };
	}

	LexResult Lexer::lexLiteral(std::vector<Token>& tokens, std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		auto result = lexIntegerLiteral(tokens, start, end);
		if (result.m_UsedTokens != 0)
			return result;

		result = lexFloatLiteral(tokens, start, end);
		if (result.m_UsedTokens != 0)
			return result;

		return {};
	}

	LexResult Lexer::lexTypeQualifier(std::vector<Token>& tokens, std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		Token& token = tokens[start];
		if (token.m_Class != ETokenClass::Identifier)
			return {};

		auto str         = token.getView(*m_Sources);
		bool isQualifier = false;
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

	LexResult Lexer::lexArgument(std::vector<Token>& tokens, std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		std::size_t          usedTokens = 0;
		std::vector<ASTNode> nodes;

		auto result = lexLiteral(tokens, start + usedTokens, end);
		if (result.m_UsedTokens == 0)
			return {};
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		auto token = nodes[0].getToken();

		return { usedTokens, { EASTNodeType::Argument, std::move(token), std::move(nodes) } };
	}

	LexResult Lexer::lexArguments(std::vector<Token>& tokens, std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return { 0, { EASTNodeType::Arguments } };

		std::size_t          usedTokens = 0;
		std::vector<ASTNode> nodes;

		{
			// Argument opening
			auto& token = tokens[start + usedTokens];
			if (token.m_Class != ETokenClass::Symbol || token.getView(*m_Sources) != "(")
				return { 0, { EASTNodeType::Arguments } };
			++usedTokens;
		}
		std::size_t argumentsEnd = start + usedTokens;
		{
			// Closing
			std::uint64_t count = 1;
			while (argumentsEnd <= end)
			{
				Token& token = tokens[argumentsEnd];
				if (token.m_Class == ETokenClass::Symbol)
				{
					if (token.getView(*m_Sources) == "(")
						++count;
					else if (token.getView(*m_Sources) == ")")
						--count;
					++argumentsEnd;
				}
				else
				{
					++argumentsEnd;
					continue;
				}
				if (count == 0)
					break;
			}
			if (count != 0)
				return { 0, { EASTNodeType::Arguments } };
		}

		while (start + usedTokens < argumentsEnd - 1)
		{
			auto result = lexArgument(tokens, start + usedTokens, argumentsEnd - 1);
			if (result.m_UsedTokens == 0)
				break;
			usedTokens += result.m_UsedTokens;
			nodes.emplace_back(std::move(result.m_Node));
			if (start + usedTokens > end)
				break;

			auto& token = tokens[start + usedTokens];
			if (token.m_Class != ETokenClass::Symbol || token.getView(*m_Sources) != ",")
				break;
			++usedTokens;
		}

		return { argumentsEnd - start, { EASTNodeType::Arguments, Token {}, std::move(nodes) } };
	}

	LexResult Lexer::lexAttribute(std::vector<Token>& tokens, std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		std::size_t          usedTokens = 0;
		std::vector<ASTNode> nodes;

		auto result = lexIdentifier(tokens, start + usedTokens, end);
		if (result.m_UsedTokens == 0)
			return {};
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		result = lexArguments(tokens, start + usedTokens, end);
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		auto identifierToken = nodes[0].getToken();

		return { usedTokens, { EASTNodeType::Attribute, std::move(identifierToken), std::move(nodes) } };
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
			if (a.m_Class != ETokenClass::Symbol || a.getView(*m_Sources) != "[")
				return { 0, { EASTNodeType::Attributes } };
			++usedTokens;
			Token& b = tokens[start + usedTokens];
			if (b.m_Class != ETokenClass::Symbol || b.getView(*m_Sources) != "[")
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
				if (token.m_Class != ETokenClass::Symbol || token.getView(*m_Sources) != "]")
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
				break;
			usedTokens += result.m_UsedTokens;
			nodes.emplace_back(std::move(result.m_Node));

			auto& token = tokens[start + usedTokens];
			if (token.m_Class != ETokenClass::Symbol || token.getView(*m_Sources) != ",")
				break;
			++usedTokens;
		}

		return { attributesEnd - start, { EASTNodeType::Attributes, Token {}, std::move(nodes) } };
	}

	LexResult Lexer::lexParameter(std::vector<Token>& tokens, std::size_t start, std::size_t end)
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

		return { usedTokens, { EASTNodeType::Parameter, std::move(identifierToken), std::move(nodes) } };
	}

	LexResult Lexer::lexParameters(std::vector<Token>& tokens, std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return { 0, { EASTNodeType::Parameters } };

		std::size_t          usedTokens = 0;
		std::vector<ASTNode> nodes;

		{
			// Argument opening
			auto& token = tokens[start + usedTokens];
			if (token.m_Class != ETokenClass::Symbol || token.getView(*m_Sources) != "(")
				return { 0, { EASTNodeType::Parameters } };
			++usedTokens;
		}
		std::size_t parametersEnd = start + usedTokens;
		{
			// Closing
			std::uint64_t count = 1;
			while (parametersEnd <= end)
			{
				Token& token = tokens[parametersEnd];
				if (token.m_Class == ETokenClass::Symbol)
				{
					if (token.getView(*m_Sources) == "(")
						++count;
					else if (token.getView(*m_Sources) == ")")
						--count;
					++parametersEnd;
				}
				else
				{
					++parametersEnd;
					continue;
				}
				if (count == 0)
					break;
			}
			if (count != 0)
				return { 0, { EASTNodeType::Parameters } };
		}

		while (start + usedTokens < parametersEnd - 1)
		{
			auto result = lexParameter(tokens, start + usedTokens, parametersEnd - 1);
			if (result.m_UsedTokens == 0)
				break;
			usedTokens += result.m_UsedTokens;
			nodes.emplace_back(std::move(result.m_Node));
			if (start + usedTokens > end)
				break;

			auto& token = tokens[start + usedTokens];
			if (token.m_Class != ETokenClass::Symbol || token.getView(*m_Sources) != ",")
				break;
			++usedTokens;
		}

		return { parametersEnd - start, { EASTNodeType::Parameters, Token {}, std::move(nodes) } };
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
			if (token.m_Class != ETokenClass::Symbol && token.getView(*m_Sources) != "{")
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
					if (token.getView(*m_Sources) == "{")
						++count;
					else if (token.getView(*m_Sources) == "}")
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
			if (token.m_Class != ETokenClass::Symbol || token.getView(*m_Sources) != ",")
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
			if (token.getView(*m_Sources) == "=")
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
		if (token.m_Class != ETokenClass::Symbol || token.getView(*m_Sources) != ";")
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
			if (token.m_Class != ETokenClass::Symbol && token.getView(*m_Sources) != "{")
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
					if (token.getView(*m_Sources) == "{")
						++count;
					else if (token.getView(*m_Sources) == "}")
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
			if (token.m_Class != ETokenClass::Identifier || token.getView(*m_Sources) != "return")
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
			if (token.m_Class != ETokenClass::Symbol || token.getView(*m_Sources) != ";")
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

		result = lexParameters(tokens, start + usedTokens, end);
		if (result.m_UsedTokens == 0)
			return {};
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		if (start + usedTokens > end)
			return {};

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
