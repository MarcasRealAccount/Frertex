#include "Frertex/Parser/Parser.h"
#include "Frertex/Utils/Profiler.h"

namespace Frertex::Parser
{
	State::State(Source::Sources* sources, Message::MessageHandler messageHandler)
	    : m_Sources(sources), m_MessageHandler(messageHandler) {}

	AST::Node State::parse(Utils::CopyMovable<std::vector<Tokenizer::Token>>&& tokens)
	{
		PROFILE_FUNC;

		m_Tokens = tokens.get();

		if (!m_Tokens.empty())
		{
			auto result = parseTranslationUnit(0, m_Tokens.size() - 1);
			return result.m_Node;
		}
		return {};
	}

	ParseResult State::parseIntegerLiteral(std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		auto& token = m_Tokens[start];
		if (token.m_Class != Tokenizer::ETokenClass::Integer &&
		    token.m_Class != Tokenizer::ETokenClass::BinaryInteger &&
		    token.m_Class != Tokenizer::ETokenClass::OctalInteger &&
		    token.m_Class != Tokenizer::ETokenClass::HexInteger)
			return {};

		return { 1, { AST::EType::IntegerLiteral, token } };
	}

	ParseResult State::parseFloatLiteral(std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		auto& token = m_Tokens[start];
		if (token.m_Class != Tokenizer::ETokenClass::Float &&
		    token.m_Class != Tokenizer::ETokenClass::HexFloat)
			return {};

		return { 1, { AST::EType::FloatLiteral, token } };
	}

	ParseResult State::parseBoolLiteral(std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		auto& token = m_Tokens[start];
		if (token.m_Class != Tokenizer::ETokenClass::Identifier)
			return {};

		auto view   = token.getView(m_Sources);
		bool isBool = false;
		if (view == "false" ||
		    view == "true")
			isBool = true;

		if (!isBool)
			return {};

		return { 1, { AST::EType::BoolLiteral, token } };
	}

	ParseResult State::parseLiteral(std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		auto result = parseIntegerLiteral(start, end);
		if (result.m_UsedTokens != 0)
			return result;

		result = parseFloatLiteral(start, end);
		if (result.m_UsedTokens != 0)
			return result;

		result = parseBoolLiteral(start, end);
		if (result.m_UsedTokens != 0)
			return result;

		return {};
	}

	ParseResult State::parsePreUnaryOperator(std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		std::size_t            usedTokens = 0;
		std::vector<AST::Node> nodes;

		auto& token = m_Tokens[start];
		if (Tokenizer::GetBaseTokenClass(token.m_Class) != Tokenizer::ETokenClass::LastSymbol)
			return {};

		auto str        = token.getView(m_Sources);
		bool isOperator = false;
		if (str == "!" ||
		    str == "~" ||
		    str == "*" ||
		    str == "&")
		{
			isOperator = true;
			++usedTokens;
			nodes.emplace_back(AST::EType::Operator, token);
		}
		else if (str == "+" ||
		         str == "-")
		{
			if (token.m_Class == Tokenizer::ETokenClass::FirstSymbol)
			{
				++usedTokens;
				auto& b = m_Tokens[start + usedTokens];
				if (Tokenizer::GetBaseTokenClass(b.m_Class) != Tokenizer::ETokenClass::LastSymbol)
					return {};

				if (b.getView(m_Sources) != str)
					return {};

				isOperator = true;
				++usedTokens;
				nodes.emplace_back(AST::EType::Operator, token);
				nodes.emplace_back(AST::EType::Operator, b);
			}
			else
			{
				isOperator = true;
				++usedTokens;
				nodes.emplace_back(AST::EType::Operator, token);
			}
		}

		if (!isOperator)
			return {};

		return { usedTokens, { AST::EType::Operator, Tokenizer::Token {}, std::move(nodes) } };
	}

	ParseResult State::parsePostUnaryOperator(std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		std::size_t            usedTokens = 0;
		std::vector<AST::Node> nodes;

		auto& token = m_Tokens[start];
		if (Tokenizer::GetBaseTokenClass(token.m_Class) != Tokenizer::ETokenClass::LastSymbol)
			return {};

		auto str        = token.getView(m_Sources);
		bool isOperator = false;
		if (str == ".")
		{
			isOperator = true;
			++usedTokens;
			nodes.emplace_back(AST::EType::Operator, token);
		}
		else if (str == "+")
		{
			if (token.m_Class != Tokenizer::ETokenClass::FirstSymbol)
				return {};

			++usedTokens;
			auto& b = m_Tokens[start + usedTokens];
			if (Tokenizer::GetBaseTokenClass(b.m_Class) != Tokenizer::ETokenClass::LastSymbol)
				return {};

			if (b.getView(m_Sources) != str)
				return {};

			isOperator = true;
			++usedTokens;
			nodes.emplace_back(AST::EType::Operator, token);
			nodes.emplace_back(AST::EType::Operator, b);
		}
		else if (str == "-")
		{
			if (token.m_Class != Tokenizer::ETokenClass::FirstSymbol)
				return {};

			++usedTokens;
			auto& b = m_Tokens[start + usedTokens];
			if (Tokenizer::GetBaseTokenClass(b.m_Class) != Tokenizer::ETokenClass::LastSymbol)
				return {};

			auto view = b.getView(m_Sources);
			if (view == "-" ||
			    view == ">")
				isOperator = true;

			if (!isOperator)
				return {};

			++usedTokens;
			nodes.emplace_back(AST::EType::Operator, token);
			nodes.emplace_back(AST::EType::Operator, b);
		}

		if (!isOperator)
			return {};

		return { usedTokens, { AST::EType::Operator, Tokenizer::Token {}, std::move(nodes) } };
	}

	ParseResult State::parseBinaryOperator(std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		std::size_t            usedTokens = 0;
		std::vector<AST::Node> nodes;

		auto& token = m_Tokens[start];
		if (Tokenizer::GetBaseTokenClass(token.m_Class) != Tokenizer::ETokenClass::LastSymbol)
			return {};

		auto str        = token.getView(m_Sources);
		bool isOperator = false;
		if (str == "+" ||
		    str == "-" ||
		    str == "*" ||
		    str == "/" ||
		    str == "%" ||
		    str == "&" ||
		    str == "|" ||
		    str == "^")
		{
			isOperator = true;
			++usedTokens;
			nodes.emplace_back(AST::EType::Operator, token);
		}
		else if (str == "<" ||
		         str == ">")
		{
			if (token.m_Class != Tokenizer::ETokenClass::FirstSymbol)
				return {};

			++usedTokens;
			auto& b = m_Tokens[start + usedTokens];
			if (Tokenizer::GetBaseTokenClass(b.m_Class) != Tokenizer::ETokenClass::LastSymbol)
				return {};

			if (b.getView(m_Sources) != str)
				return {};

			isOperator = true;
			++usedTokens;
			nodes.emplace_back(AST::EType::Operator, token);
			nodes.emplace_back(AST::EType::Operator, b);
		}

		if (!isOperator)
			return {};

		return { usedTokens, { AST::EType::Operator, Tokenizer::Token {}, std::move(nodes) } };
	}

	ParseResult State::parseAssignmentOperator(std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		std::size_t            usedTokens = 0;
		std::vector<AST::Node> nodes;

		auto& microOp = m_Tokens[start + usedTokens];
		if (microOp.m_Class == Tokenizer::ETokenClass::FirstSymbol)
		{
			auto str = microOp.getView(m_Sources);
			if (str == "+" ||
			    str == "-" ||
			    str == "*" ||
			    str == "/" ||
			    str == "%" ||
			    str == "&" ||
			    str == "|" ||
			    str == "^")
			{
				++usedTokens;
				nodes.emplace_back(AST::EType::Operator, microOp);
			}
			else if (str == "<" ||
			         str == ">")
			{
				++usedTokens;
				auto& b = m_Tokens[start + usedTokens];
				if (Tokenizer::GetBaseTokenClass(b.m_Class) != Tokenizer::ETokenClass::LastSymbol)
					return {};

				if (b.getView(m_Sources) != str)
					return {};

				++usedTokens;
				nodes.emplace_back(AST::EType::Operator, microOp);
				nodes.emplace_back(AST::EType::Operator, b);
			}
		}

		auto& token = m_Tokens[start + usedTokens];
		if (Tokenizer::GetBaseTokenClass(token.m_Class) != Tokenizer::ETokenClass::LastSymbol ||
		    token.getView(m_Sources) != "=")
			return {};
		++usedTokens;

		return { usedTokens, { AST::EType::Operator, token, std::move(nodes) } };
	}

	ParseResult State::parseTypeQualifier(std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		auto& token = m_Tokens[start];
		if (token.m_Class != Tokenizer::ETokenClass::Identifier)
			return {};

		auto str         = token.getView(m_Sources);
		bool isQualifier = false;
		if (str == "in" ||
		    str == "out" ||
		    str == "inout")
			isQualifier = true;

		if (isQualifier)
			return { 1, { AST::EType::TypeQualifier, token } };
		else
			return {};
	}

	ParseResult State::parseTypeQualifiers(std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return { 0, { AST::EType::TypeQualifiers } };

		std::size_t            usedTokens = 0;
		std::vector<AST::Node> nodes;

		while (true)
		{
			auto result = parseTypeQualifier(start + usedTokens, end);
			if (result.m_UsedTokens == 0)
				break;
			usedTokens += result.m_UsedTokens;
			nodes.emplace_back(std::move(result.m_Node));
		}

		return { usedTokens, { AST::EType::TypeQualifiers, Tokenizer::Token {}, std::move(nodes) } };
	}

	ParseResult State::parseIdentifier(std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		auto& token = m_Tokens[start];
		if (token.m_Class != Tokenizer::ETokenClass::Identifier)
			return {};

		return { 1, { AST::EType::Identifier, token } };
	}

	ParseResult State::parseTypename(std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		std::size_t            usedTokens = 0;
		std::vector<AST::Node> nodes;

		auto result = parseIdentifier(start + usedTokens, end);
		if (result.m_UsedTokens == 0)
			return {};
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		auto identifierToken = nodes[0].getToken();

		return { usedTokens, { AST::EType::Typename, std::move(identifierToken), std::move(nodes) } };
	}

	ParseResult State::parseParameterTypename(std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		std::size_t            usedTokens = 0;
		std::vector<AST::Node> nodes;

		auto result = parseTypeQualifiers(start + usedTokens, end);
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		result = parseIdentifier(start + usedTokens, end);
		if (result.m_UsedTokens == 0)
			return {};
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		auto identifierToken = nodes[1].getToken();

		return { usedTokens, { AST::EType::Typename, std::move(identifierToken), std::move(nodes) } };
	}

	ParseResult State::parseArgument(std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		std::size_t            usedTokens = 0;
		std::vector<AST::Node> nodes;

		auto result = parseLiteral(start + usedTokens, end);
		if (result.m_UsedTokens == 0)
			return {};
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		auto token = nodes[0].getToken();

		return { usedTokens, { AST::EType::Argument, std::move(token), std::move(nodes) } };
	}

	ParseResult State::parseArguments(std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return { 0, { AST::EType::Arguments } };

		std::size_t            usedTokens = 0;
		std::vector<AST::Node> nodes;

		{
			// Opening
			auto& token = m_Tokens[start + usedTokens];
			if (Tokenizer::GetBaseTokenClass(token.m_Class) != Tokenizer::ETokenClass::LastSymbol ||
			    token.getView(m_Sources) != "(")
				return { 0, { AST::EType::Arguments } };
			++usedTokens;
		}
		std::size_t argumentsEnd = start + usedTokens;
		{
			// Closing
			std::uint64_t indents = 1;
			while (indents > 0 && argumentsEnd <= end)
			{
				auto& token = m_Tokens[argumentsEnd];
				if (Tokenizer::GetBaseTokenClass(token.m_Class) == Tokenizer::ETokenClass::LastSymbol)
				{
					auto view = token.getView(m_Sources);
					if (view == "(")
						++indents;
					else if (view == ")")
						--indents;
				}
				++argumentsEnd;
			}
			if (indents > 0)
				return { 0, { AST::EType::Arguments } };
		}

		while (start + usedTokens < argumentsEnd - 1)
		{
			auto result = parseArgument(start + usedTokens, argumentsEnd - 1);
			if (result.m_UsedTokens == 0)
				break;
			usedTokens += result.m_UsedTokens;
			nodes.emplace_back(std::move(result.m_Node));

			auto& token = m_Tokens[start + usedTokens];
			if (Tokenizer::GetBaseTokenClass(token.m_Class) != Tokenizer::ETokenClass::LastSymbol ||
			    token.getView(m_Sources) != ",")
				break;
			++usedTokens;
		}

		if (start + usedTokens < argumentsEnd - 1)
			reportError(start, argumentsEnd, std::vector<Source::SourceSpan> {}, m_Tokens[start + usedTokens].getEnd(m_Sources), "Expected argument");

		return { usedTokens, { AST::EType::Arguments, Tokenizer::Token {}, std::move(nodes) } };
	}

	ParseResult State::parseAttribute(std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		std::size_t            usedTokens = 0;
		std::vector<AST::Node> nodes;

		auto result = parseIdentifier(start + usedTokens, end);
		if (result.m_UsedTokens == 0)
			return {};
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		result = parseArguments(start + usedTokens, end);
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		auto identifierToken = nodes[0].getToken();

		return { usedTokens, { AST::EType::Attribute, std::move(identifierToken), std::move(nodes) } };
	}

	ParseResult State::parseAttributes(std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return { 0, { AST::EType::Attributes } };

		std::size_t            usedTokens = 0;
		std::vector<AST::Node> nodes;

		{
			// Opening
			auto& a = m_Tokens[start + usedTokens];
			if (a.m_Class != Tokenizer::ETokenClass::FirstSymbol ||
			    a.getView(m_Sources) != "[")
				return { 0, { AST::EType::Attributes } };
			++usedTokens;
			auto& b = m_Tokens[start + usedTokens];
			if (Tokenizer::GetBaseTokenClass(b.m_Class) != Tokenizer::ETokenClass::LastSymbol ||
			    b.getView(m_Sources) != "[")
				return { 0, { AST::EType::Attributes } };
			++usedTokens;
		}
		std::size_t attributesEnd = start + usedTokens;
		{
			// Closing
			std::uint64_t indents = 1;
			while (indents > 0 && attributesEnd <= end)
			{
				std::size_t state = 0;
				auto&       a     = m_Tokens[attributesEnd];
				if (a.m_Class == Tokenizer::ETokenClass::FirstSymbol)
				{
					auto view = a.getView(m_Sources);
					if (view == "[")
						state = 1;
					else if (view == "]")
						state = 2;
				}
				++attributesEnd;
				if (state == 0)
					continue;

				state   = 0;
				auto& b = m_Tokens[attributesEnd];
				if (Tokenizer::GetBaseTokenClass(b.m_Class) == Tokenizer::ETokenClass::LastSymbol)
				{
					auto view = b.getView(m_Sources);
					if (view == "[")
						state = 1;
					else if (view == "]")
						state = 2;
				}
				++attributesEnd;

				switch (state)
				{
				case 1:
					++indents;
					break;
				case 2:
					--indents;
					break;
				}
			}
			if (indents > 0)
				return { 0, { AST::EType::Attributes } };
		}

		while (start + usedTokens < attributesEnd - 2)
		{
			auto result = parseAttribute(start + usedTokens, attributesEnd - 1);
			if (result.m_UsedTokens == 0)
				break;
			usedTokens += result.m_UsedTokens;
			nodes.emplace_back(std::move(result.m_Node));

			auto& token = m_Tokens[start + usedTokens];
			if (Tokenizer::GetBaseTokenClass(token.m_Class) != Tokenizer::ETokenClass::LastSymbol ||
			    token.getView(m_Sources) != ",")
				break;
			++usedTokens;
		}

		if (start + usedTokens < attributesEnd - 2)
			reportError(start, attributesEnd, std::vector<Source::SourceSpan> {}, m_Tokens[start + usedTokens].getEnd(m_Sources), "Expected attribute");

		return { attributesEnd - start, { AST::EType::Attributes, Tokenizer::Token {}, std::move(nodes) } };
	}

	ParseResult State::parseParameter(std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		std::size_t            usedTokens = 0;
		std::vector<AST::Node> nodes;

		auto result = parseAttributes(start + usedTokens, end);
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		result = parseParameterTypename(start + usedTokens, end);
		if (result.m_UsedTokens == 0)
			return {};
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		result = parseIdentifier(start + usedTokens, end);
		if (result.m_UsedTokens == 0)
			return {};
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		auto identifierToken = nodes[2].getToken();

		return { usedTokens, { AST::EType::Parameter, std::move(identifierToken), std::move(nodes) } };
	}

	ParseResult State::parseParameters(std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return { 0, { AST::EType::Parameters } };

		std::size_t            usedTokens = 0;
		std::vector<AST::Node> nodes;

		{
			// Opening
			auto& token = m_Tokens[start + usedTokens];
			if (Tokenizer::GetBaseTokenClass(token.m_Class) != Tokenizer::ETokenClass::LastSymbol ||
			    token.getView(m_Sources) != "(")
				return { 0, { AST::EType::Parameters } };
			++usedTokens;
		}
		std::size_t parametersEnd = start + usedTokens;
		{
			// Closing
			std::uint64_t indents = 1;
			while (indents > 0 && parametersEnd <= end)
			{
				auto& token = m_Tokens[parametersEnd];
				if (Tokenizer::GetBaseTokenClass(token.m_Class) == Tokenizer::ETokenClass::LastSymbol)
				{
					auto view = token.getView(m_Sources);
					if (view == "(")
						++indents;
					else if (view == ")")
						--indents;
				}
				++parametersEnd;
			}
			if (indents > 0)
				return { 0, { AST::EType::Parameters } };
		}

		while (start + usedTokens < parametersEnd - 1)
		{
			auto result = parseParameter(start + usedTokens, parametersEnd - 1);
			if (result.m_UsedTokens == 0)
				break;
			usedTokens += result.m_UsedTokens;
			nodes.emplace_back(std::move(result.m_Node));

			auto& token = m_Tokens[start + usedTokens];
			if (Tokenizer::GetBaseTokenClass(token.m_Class) != Tokenizer::ETokenClass::LastSymbol ||
			    token.getView(m_Sources) != ",")
				break;
			++usedTokens;
		}

		if (start + usedTokens < parametersEnd - 1)
			reportError(start, parametersEnd, std::vector<Source::SourceSpan> {}, m_Tokens[start + usedTokens].getEnd(m_Sources), "Expected parameter");

		return { parametersEnd - start, { AST::EType::Parameters, Tokenizer::Token {}, std::move(nodes) } };
	}

	ParseResult State::parseBracedInitList(std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		std::size_t            usedTokens = 0;
		std::vector<AST::Node> nodes;

		{
			// Opening
			auto& token = m_Tokens[start + usedTokens];
			if (Tokenizer::GetBaseTokenClass(token.m_Class) != Tokenizer::ETokenClass::LastSymbol ||
			    token.getView(m_Sources) != "{")
				return {};
			++usedTokens;
		}
		std::size_t listEnd = start + usedTokens;
		{
			// Closing
			std::uint64_t indents = 1;
			while (indents > 0 && listEnd <= end)
			{
				auto& token = m_Tokens[listEnd];
				if (Tokenizer::GetBaseTokenClass(token.m_Class) == Tokenizer::ETokenClass::LastSymbol)
				{
					auto view = token.getView(m_Sources);
					if (view == "{")
						++indents;
					else if (view == "}")
						--indents;
				}
				++listEnd;
			}
			if (indents > 0)
				return {};
		}

		while (start + usedTokens < listEnd - 1)
		{
			auto result = parseInitializerClause(start + usedTokens, listEnd - 1);
			if (result.m_UsedTokens == 0)
				break;
			usedTokens += result.m_UsedTokens;
			nodes.emplace_back(std::move(result.m_Node));

			auto& token = m_Tokens[start + usedTokens];
			if (Tokenizer::GetBaseTokenClass(token.m_Class) != Tokenizer::ETokenClass::LastSymbol ||
			    token.getView(m_Sources) != ";")
				break;
			++usedTokens;
		}

		if (start + usedTokens < listEnd - 1)
			reportError(start, listEnd, std::vector<Source::SourceSpan> {}, m_Tokens[start + usedTokens].getEnd(m_Sources), "Expected initializer clause");

		return { listEnd - start, { AST::EType::BracedInitList, m_Tokens[start], std::move(nodes) } };
	}

	ParseResult State::parseInitializerClause(std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		auto result = parseAssignmentExpression(start, end);
		if (result.m_UsedTokens != 0)
			return result;

		result = parseBracedInitList(start, end);
		if (result.m_UsedTokens != 0)
			return result;

		return {};
	}

	ParseResult State::parseAssignmentExpression(std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		std::size_t            usedTokens = 0;
		std::vector<AST::Node> nodes;

		auto result = parseIdentifier(start + usedTokens, end);
		if (result.m_UsedTokens == 0)
			return {};
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		std::size_t operatorIndex = start + usedTokens;

		result = parseAssignmentOperator(start + usedTokens, end);
		if (result.m_UsedTokens == 0)
			return {};
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		result = parseInitializerClause(start + usedTokens, end);
		if (result.m_UsedTokens == 0)
			return {};
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		return { usedTokens, { AST::EType::AssignmentExpression, m_Tokens[operatorIndex], std::move(nodes) } };
	}

	ParseResult State::parseExpression(std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		auto result = parseAssignmentExpression(start, end);
		if (result.m_UsedTokens != 0)
			return result;

		return {};
	}

	ParseResult State::parseEmptyStatement(std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		auto& token = m_Tokens[start];
		if (Tokenizer::GetBaseTokenClass(token.m_Class) != Tokenizer::ETokenClass::LastSymbol ||
		    token.getView(m_Sources) != ";")
			return {};

		return { 1, { AST::EType::EmptyStatement, m_Tokens[start] } };
	}

	ParseResult State::parseExpressionStatement(std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		std::size_t            usedTokens = 0;
		std::vector<AST::Node> nodes;

		auto result = parseExpression(start + usedTokens, end);
		if (result.m_UsedTokens == 0)
			return {};
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		auto& token = m_Tokens[start + usedTokens];
		if (Tokenizer::GetBaseTokenClass(token.m_Class) != Tokenizer::ETokenClass::LastSymbol ||
		    token.getView(m_Sources) != ";")
			return {};
		++usedTokens;

		return { usedTokens, { AST::EType::ExpressionStatement, m_Tokens[start + usedTokens - 1], std::move(nodes) } };
	}

	ParseResult State::parseCompoundStatement(std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		std::size_t            usedTokens = 0;
		std::vector<AST::Node> nodes;

		{
			// Opening
			auto& token = m_Tokens[start + usedTokens];
			if (Tokenizer::GetBaseTokenClass(token.m_Class) != Tokenizer::ETokenClass::LastSymbol ||
			    token.getView(m_Sources) != "{")
				return {};
			++usedTokens;
		}
		std::size_t blockEnd = start + usedTokens;
		{
			// Closing
			std::uint64_t indents = 1;
			while (indents > 0 && blockEnd <= end)
			{
				auto& token = m_Tokens[blockEnd];
				if (Tokenizer::GetBaseTokenClass(token.m_Class) == Tokenizer::ETokenClass::LastSymbol)
				{
					auto view = token.getView(m_Sources);
					if (view == "{")
						++indents;
					else if (view == "}")
						--indents;
				}
				++blockEnd;
			}
			if (indents > 0)
				return {};
		}

		auto result = parseStatements(start + usedTokens, blockEnd - 1);
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		if ((start + usedTokens) != blockEnd - 1)
		{
			std::vector<Source::SourceSpan> spans;
			for (std::size_t i = start + usedTokens + 1; i < blockEnd; ++i)
				spans.emplace_back(m_Tokens[i].getSpan(m_Sources));

			reportError(start + usedTokens, blockEnd, std::move(spans), m_Tokens[start + usedTokens].getEnd(m_Sources), "Failed to parse tokens for compound statement");
		}

		return { blockEnd - start, { AST::EType::CompoundStatement, Tokenizer::Token {}, std::move(nodes) } };
	}

	ParseResult State::parseReturnStatement(std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		std::size_t            usedTokens = 0;
		std::vector<AST::Node> nodes;

		{
			auto& token = m_Tokens[start + usedTokens];
			if (token.m_Class != Tokenizer::ETokenClass::Identifier ||
			    token.getView(m_Sources) != "return")
				return {};
			++usedTokens;
		}

		do
		{
			auto result = parseExpression(start + usedTokens, end);
			if (result.m_UsedTokens != 0)
			{
				usedTokens += result.m_UsedTokens;
				nodes.emplace_back(std::move(result.m_Node));
				break;
			}

			result = parseBracedInitList(start + usedTokens, end);
			if (result.m_UsedTokens != 0)
			{
				usedTokens += result.m_UsedTokens;
				nodes.emplace_back(std::move(result.m_Node));
				break;
			}
		} while (false);

		{
			auto& token = m_Tokens[start + usedTokens];
			if (Tokenizer::GetBaseTokenClass(token.m_Class) != Tokenizer::ETokenClass::LastSymbol ||
			    token.getView(m_Sources) != ";")
				return {};
			++usedTokens;
		}

		return { usedTokens, { AST::EType::ReturnStatement, m_Tokens[start], std::move(nodes) } };
	}

	ParseResult State::parseStatement(std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		auto result = parseEmptyStatement(start, end);
		if (result.m_UsedTokens != 0)
			return result;

		result = parseExpressionStatement(start, end);
		if (result.m_UsedTokens != 0)
			return result;

		result = parseCompoundStatement(start, end);
		if (result.m_UsedTokens != 0)
			return result;

		result = parseReturnStatement(start, end);
		if (result.m_UsedTokens != 0)
			return result;

		result = parseDeclaration(start, end);
		if (result.m_UsedTokens != 0)
			return result;

		return {};
	}

	ParseResult State::parseStatements(std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return { 0, { AST::EType::Statements } };

		std::size_t            usedTokens = 0;
		std::vector<AST::Node> nodes;

		while (start + usedTokens < end)
		{
			auto result = parseStatement(start + usedTokens, end);
			if (result.m_UsedTokens == 0)
				break;
			usedTokens += result.m_UsedTokens;
			nodes.emplace_back(std::move(result.m_Node));
		}

		return { usedTokens, { AST::EType::Statements, Tokenizer::Token {}, std::move(nodes) } };
	}

	ParseResult State::parseFunctionDeclaration(std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		std::size_t            usedTokens = 0;
		std::vector<AST::Node> nodes;

		auto result = parseAttributes(start + usedTokens, end);
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		result = parseTypename(start + usedTokens, end);
		if (result.m_UsedTokens == 0)
			return {};
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		result = parseIdentifier(start + usedTokens, end);
		if (result.m_UsedTokens == 0)
			return {};
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		result = parseParameters(start + usedTokens, end);
		if (result.m_UsedTokens == 0)
			return {};
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		result = parseCompoundStatement(start + usedTokens, end);
		if (result.m_UsedTokens == 0)
			return {};
		usedTokens += result.m_UsedTokens;
		nodes.emplace_back(std::move(result.m_Node));

		auto identifierToken = nodes[2].getToken();
		return { usedTokens, { AST::EType::FunctionDeclaration, std::move(identifierToken), std::move(nodes) } };
	}

	ParseResult State::parseDeclaration(std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		auto result = parseFunctionDeclaration(start, end);
		if (result.m_UsedTokens != 0)
			return result;

		return {};
	}

	ParseResult State::parseTranslationUnit(std::size_t start, std::size_t end)
	{
		PROFILE_FUNC;

		if (start > end)
			return {};

		std::size_t            usedTokens = 0;
		std::vector<AST::Node> nodes;

		while (start + usedTokens <= end)
		{
			auto result = parseDeclaration(start + usedTokens, end);
			if (result.m_UsedTokens == 0)
			{
				if (start + usedTokens <= end)
					reportError(start + usedTokens, start + usedTokens, std::vector<Source::SourceSpan> {}, m_Tokens[start + usedTokens].getStart(m_Sources), "Expected declaration");
				break;
			}

			usedTokens += result.m_UsedTokens;
			nodes.emplace_back(std::move(result.m_Node));
		}

		return { usedTokens, { AST::EType::TranslationUnit, Tokenizer::Token {}, std::move(nodes) } };
	}

	void State::reportMessage(Message::EMessageType type, std::size_t startToken, std::size_t endToken, Utils::CopyMovable<std::vector<Source::SourceSpan>>&& spans, Source::SourcePoint point, Utils::CopyMovable<std::string>&& message)
	{
		PROFILE_FUNC;

		m_MessageHandler(m_Sources, m_Tokens, { type, startToken, endToken + 1, spans.get(), point, message.get() });
	}
} // namespace Frertex::Parser