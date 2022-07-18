#include "Frertex/Lexer.h"

namespace Frertex
{
	std::string_view ASTNodeTypeToString(EASTNodeType type)
	{
		switch (type)
		{
		case EASTNodeType::Unknown: return "Unknown";
		case EASTNodeType::TranslationUnit: return "TranslationUnit";
		case EASTNodeType::TypeQualifier: return "TypeQualifier";
		case EASTNodeType::TypeQualifiers: return "TypeQualifiers";
		case EASTNodeType::Identifier: return "Identifier";
		case EASTNodeType::Typename: return "Typename";
		case EASTNodeType::Attribute: return "Attribute";
		case EASTNodeType::Attributes: return "Attributes";
		case EASTNodeType::Argument: return "Argument";
		case EASTNodeType::Arguments: return "Arguments";
		case EASTNodeType::StatementBlock: return "StatementBlock";
		case EASTNodeType::Function: return "Function";
		default: return "Unknown";
		}
	}

	void AST::setRoot(Utils::CopyMovable<ASTNode>&& node)
	{
		m_Root = new ASTNode(node.get());
	}

	AST Lexer::lex(Utils::CopyMovable<std::vector<Token>>&& tokens)
	{
		std::vector<Token> input = tokens.get();

		AST  ast;
		auto result = lexTranslationUnit(input, 0);
		ast.setRoot(std::move(result.m_Node));
		return ast;
	}

	void Lexer::addWarning(SourceSpan span, SourcePoint point, Utils::CopyMovable<std::string>&& message)
	{
		m_Messages.emplace_back(EMessageType::Warning, span, point, message.get());
	}

	void Lexer::addError(SourceSpan span, SourcePoint point, Utils::CopyMovable<std::string>&& message)
	{
		m_Messages.emplace_back(EMessageType::Error, span, point, message.get());
	}

	LexResult Lexer::lexTranslationUnit(std::vector<Token>& tokens, std::size_t offset)
	{
		std::size_t          usedTokens = 0;
		std::vector<ASTNode> nodes;

		while (offset + usedTokens < tokens.size())
		{
			auto result = lexFunction(tokens, offset + usedTokens);
			if (result.m_UsedTokens == 0)
				break;
			usedTokens += result.m_UsedTokens;
			nodes.emplace_back(std::move(result.m_Node));
		}

		return { usedTokens, { EASTNodeType::TranslationUnit, Token {}, std::move(nodes) } };
	}

	LexResult Lexer::lexTypeQualifier(std::vector<Token>& tokens, std::size_t offset)
	{
		Token& token = tokens[offset];
		if (token.m_Class != ETokenClass::Identifier)
			return { 0, {} };

		auto& str         = token.m_Str;
		bool  isQualifier = false;
		if (str == "in" ||
		    str == "out" ||
		    str == "inout")
			isQualifier = true;

		if (isQualifier)
			return { 1, { EASTNodeType::TypeQualifier, token } };
		else
			return { 0, {} };
	}

	LexResult Lexer::lexTypeQualifiers(std::vector<Token>& tokens, std::size_t offset)
	{
		std::size_t          usedTokens = 0;
		std::vector<ASTNode> nodes;

		while (true)
		{
			auto result = lexTypeQualifier(tokens, offset + usedTokens);
			if (result.m_UsedTokens == 0)
				break;
			usedTokens += result.m_UsedTokens;
			nodes.emplace_back(std::move(result.m_Node));
		}

		return { usedTokens, { EASTNodeType::TypeQualifiers, Token {}, std::move(nodes) } };
	}

	LexResult Lexer::lexIdentifier(std::vector<Token>& tokens, std::size_t offset)
	{
		Token& token = tokens[offset];
		if (token.m_Class != ETokenClass::Identifier)
			return { 0, {} };
		return { 1, { EASTNodeType::Identifier, token } };
	}

	LexResult Lexer::lexTypename(std::vector<Token>& tokens, std::size_t offset)
	{
		std::size_t          usedTokens = 0;
		std::vector<ASTNode> nodes;

		auto result = lexTypeQualifiers(tokens, offset + usedTokens);
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		result = lexIdentifier(tokens, offset + usedTokens);
		if (result.m_UsedTokens == 0)
			return { 0, {} };
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		return { usedTokens, { EASTNodeType::Typename, Token {}, std::move(nodes) } };
	}

	LexResult Lexer::lexAttribute(std::vector<Token>& tokens, std::size_t offset)
	{
		auto result = lexIdentifier(tokens, offset);
		if (result.m_UsedTokens == 0)
			return { 0, {} };
		return { result.m_UsedTokens, { EASTNodeType::Attribute, Token {}, { std::move(result.m_Node) } } };
	}

	LexResult Lexer::lexAttributes(std::vector<Token>& tokens, std::size_t offset)
	{
		std::size_t          usedTokens = 0;
		std::vector<ASTNode> nodes;

		{
			// Opening
			Token& a = tokens[offset + usedTokens];
			if (a.m_Class != ETokenClass::Symbol || a.m_Str != "[")
				return { 0, { EASTNodeType::Attributes } };
			++usedTokens;
			Token& b = tokens[offset + usedTokens];
			if (b.m_Class != ETokenClass::Symbol || b.m_Str != "[")
				return { 0, { EASTNodeType::Attributes } };
			++usedTokens;
		}
		std::size_t attributesEnd = offset + usedTokens;
		{
			// Closing
			std::uint64_t count = 0;
			while (attributesEnd < tokens.size())
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

			if (attributesEnd >= tokens.size() - 1)
				return { 0, { EASTNodeType::Attributes } };
		}

		while (offset + usedTokens < attributesEnd - 2)
		{
			auto result = lexAttribute(tokens, offset + usedTokens);
			if (result.m_UsedTokens == 0)
				return { 0, { EASTNodeType::Attributes } };
			usedTokens += result.m_UsedTokens;
			nodes.emplace_back(std::move(result.m_Node));
		}

		return { attributesEnd - offset, { EASTNodeType::Attributes, Token {}, std::move(nodes) } };
	}

	LexResult Lexer::lexArgument(std::vector<Token>& tokens, std::size_t offset)
	{
		std::size_t          usedTokens = 0;
		std::vector<ASTNode> nodes;

		auto result = lexAttributes(tokens, offset + usedTokens);
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		result = lexTypename(tokens, offset + usedTokens);
		if (result.m_UsedTokens == 0)
			return { 0, {} };
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		result = lexIdentifier(tokens, offset + usedTokens);
		if (result.m_UsedTokens == 0)
			return { 0, {} };
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		return { usedTokens, { EASTNodeType::Argument, Token {}, std::move(nodes) } };
	}

	LexResult Lexer::lexArguments(std::vector<Token>& tokens, std::size_t offset)
	{
		std::size_t          usedTokens = 0;
		std::vector<ASTNode> nodes;

		while (true)
		{
			auto result = lexArgument(tokens, offset + usedTokens);
			if (result.m_UsedTokens == 0)
				break;
			usedTokens += result.m_UsedTokens;
			nodes.emplace_back(std::move(result.m_Node));

			auto& token = tokens[offset + usedTokens];
			if (token.m_Class != ETokenClass::Symbol || token.m_Str != ",")
				break;
			++usedTokens;
		}

		return { usedTokens, { EASTNodeType::Arguments, Token {}, std::move(nodes) } };
	}

	LexResult Lexer::lexStatementBlock(std::vector<Token>& tokens, std::size_t offset)
	{
		std::size_t          usedTokens = 0;
		std::vector<ASTNode> nodes;

		{
			// Opening
			Token& token = tokens[offset + usedTokens];
			if (token.m_Class != ETokenClass::Symbol && token.m_Str != "{")
				return { 0, {} };
			++usedTokens;
		}
		std::size_t blockEnd = offset + usedTokens;
		{
			// Closing
			std::uint64_t count = 1;
			while (blockEnd < tokens.size())
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
				return { 0, {} };
		}

		//while (offset + usedTokens < blockEnd)
		//{
		//	auto result = lexStatement(tokens, offset + usedTokens);
		//	if (result.m_UsedTokens == 0)
		//		return { 0, {} };
		//	usedTokens += result.m_UsedTokens;
		//	nodes.emplace_back(std::move(result.m_Node));
		//}

		return { blockEnd - offset, { EASTNodeType::StatementBlock, Token {}, std::move(nodes) } };
	}

	LexResult Lexer::lexFunction(std::vector<Token>& tokens, std::size_t offset)
	{
		std::size_t          usedTokens = 0;
		std::vector<ASTNode> nodes;

		auto result = lexAttributes(tokens, offset + usedTokens);
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		result = lexTypename(tokens, offset + usedTokens);
		if (result.m_UsedTokens == 0)
			return { 0, {} };
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		result = lexIdentifier(tokens, offset + usedTokens);
		if (result.m_UsedTokens == 0)
			return { 0, {} };
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		{
			// Argument opening
			auto& token = tokens[offset + usedTokens];
			if (token.m_Class != ETokenClass::Symbol || token.m_Str != "(")
				return { 0, {} };
			++usedTokens;
		}

		result = lexArguments(tokens, offset + usedTokens);
		if (result.m_UsedTokens == 0)
			return { 0, {} };
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		{
			// Argument closing
			auto& token = tokens[offset + usedTokens];
			if (token.m_Class != ETokenClass::Symbol || token.m_Str != ")")
				return { 0, {} };
			++usedTokens;
		}

		result = lexStatementBlock(tokens, offset + usedTokens);
		if (result.m_UsedTokens == 0)
			return { 0, {} };
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		return { usedTokens, { EASTNodeType::Function, Token {}, std::move(nodes) } };
	}
} // namespace Frertex