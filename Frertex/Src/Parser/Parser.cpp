#include "Frertex/Parser/Parser.h"

namespace Frertex::Parser
{
	State::State(std::string_view source)
		: m_Source(source) {}

	AST::Node State::Parse(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		auto result = ParseDeclarations(tokens);
		return result.Node;
	}

	void State::ReportError(Utils::View<Tokenizer::Token> tokens, std::size_t point, std::string message)
	{
	}

	std::string_view State::GetSource(Tokenizer::Token token)
	{
		return m_Source.substr(token.Start, token.Length);
	}

	bool State::TestToken(Tokenizer::Token token, Tokenizer::ETokenClass clazz, std::string_view str)
	{
		return token.Class == clazz && GetSource(token) == str;
	}

	ParseResult State::ParseDeclarations(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return { 0, { AST::EType::Declarations } };

		std::size_t            usedTokens = 0;
		std::vector<AST::Node> nodes;

		auto itr = tokens.begin();
		auto end = tokens.end();
		while (itr != end)
		{
			auto result = ParseDeclaration({ itr, end });
			if (result.UsedTokens == 0)
			{
				ReportError({ itr, end }, itr->Start, "Expected declaration");
				break;
			}

			usedTokens += result.UsedTokens;
			nodes.emplace_back(result.Node);
			itr += result.UsedTokens;
		}

		return {
			.UsedTokens = usedTokens,
			.Node       = {
						   .Type     = AST::EType::Declarations,
						   .Token    = {},
						   .Children = std::move(nodes)}
		};
	}

	ParseResult State::ParseDeclaration(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		auto result = ParseFunctionDeclaration(tokens);
		if (result.UsedTokens > 0)
			return result;

		return {};
	}

	ParseResult State::ParseFunctionDeclaration(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		std::size_t            usedTokens = 0;
		std::vector<AST::Node> nodes;

		auto result = ParseAttributes(tokens);
		usedTokens  += result.UsedTokens;
		nodes.emplace_back(std::move(result.Node));

		result = ParseTypename({ tokens.begin() + usedTokens, tokens.end() });
		if (result.UsedTokens == 0)
			return {};
		usedTokens += result.UsedTokens;
		nodes.emplace_back(std::move(result.Node));

		result = ParseIdentifier({ tokens.begin() + usedTokens, tokens.end() });
		if (result.UsedTokens == 0)
			return {};
		usedTokens += result.UsedTokens;
		nodes.emplace_back(std::move(result.Node));

		result = ParseParameters({ tokens.begin() + usedTokens, tokens.end() });
		if (result.UsedTokens == 0)
			return {};
		usedTokens += result.UsedTokens;
		nodes.emplace_back(std::move(result.Node));

		result = ParseCompoundStatement({ tokens.begin() + usedTokens, tokens.end() });
		if (result.UsedTokens == 0)
			return {};
		usedTokens += result.UsedTokens;
		nodes.emplace_back(std::move(result.Node));

		return {
			.UsedTokens = usedTokens,
			.Node       = {
						   .Type     = AST::EType::FunctionDeclaration,
						   .Token    = nodes[2].Token,
						   .Children = std::move(nodes)}
		};
	}

	ParseResult State::ParseStatements(Utils::View<Tokenizer::Token> tokens, bool parseFull)
	{
		if (tokens.empty())
			return { 0, { AST::EType::Statements } };

		std::size_t            usedTokens = 0;
		std::vector<AST::Node> nodes;

		auto itr = tokens.begin();
		auto end = tokens.end();
		while (itr != end)
		{
			auto result = ParseStatement({ itr, end });
			if (result.UsedTokens == 0)
			{
				if (parseFull)
					ReportError({ itr, end }, itr->Start, "Expected statement");
				break;
			}

			usedTokens += result.UsedTokens;
			nodes.emplace_back(result.Node);
			itr += result.UsedTokens;
		}

		return {
			.UsedTokens = usedTokens,
			.Node       = {
						   .Type     = AST::EType::Statements,
						   .Token    = {},
						   .Children = std::move(nodes)}
		};
	}

	ParseResult State::ParseStatement(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		auto result = ParseEmptyStatement(tokens);
		if (result.UsedTokens > 0)
			return result;

		result = ParseCompoundStatement(tokens);
		if (result.UsedTokens > 0)
			return result;

		result = ParseDeclaration(tokens);
		if (result.UsedTokens > 0)
			return result;

		return {};
	}

	ParseResult State::ParseEmptyStatement(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		auto token = tokens[0];
		if (!TestToken(token, Tokenizer::ETokenClass::Symbol, ";"))
			return {};

		return {
			.UsedTokens = 1,
			.Node       = {
						   .Type     = AST::EType::EmptyStatement,
						   .Token    = token,
						   .Children = {}}
		};
	}

	ParseResult State::ParseCompoundStatement(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		if (!TestToken(tokens[0], Tokenizer::ETokenClass::Symbol, "{"))
			return {};

		std::size_t end   = 1;
		std::size_t depth = 1;
		while (depth > 0)
		{
			auto token = tokens[end];
			if (TestToken(token, Tokenizer::ETokenClass::Symbol, "{"))
				++depth;
			else if (TestToken(token, Tokenizer::ETokenClass::Symbol, "}"))
				--depth;
			if (++end == tokens.size())
				return {};
		}

		auto result = ParseStatements({ tokens.begin() + 1, tokens.begin() + end }, true);

		return {
			.UsedTokens = end,
			.Node       = {
						   .Type     = AST::EType::CompoundStatement,
						   .Token    = {},
						   .Children = { std::move(result.Node) }}
		};
	}

	ParseResult State::ParseParameters(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return { 0, { AST::EType::Parameters } };

		if (!TestToken(tokens[0], Tokenizer::ETokenClass::Symbol, "("))
			return { 0, { AST::EType::Parameters } };

		std::size_t end   = 1;
		std::size_t depth = 1;
		while (depth > 0)
		{
			auto token = tokens[end];
			if (TestToken(token, Tokenizer::ETokenClass::Symbol, "("))
				++depth;
			else if (TestToken(token, Tokenizer::ETokenClass::Symbol, ")"))
				--depth;
			if (++end == tokens.size())
				return { 0, { AST::EType::Parameters } };
		}

		std::size_t            usedTokens = 0;
		std::vector<AST::Node> nodes;

		auto itr    = tokens.begin() + 1;
		auto endItr = tokens.begin() + end - 1;
		while (itr != endItr)
		{
			auto result = ParseParameter({ itr, endItr });
			if (result.UsedTokens == 0)
			{
				ReportError({ itr, endItr }, itr->Start, "Expected parameter");
				break;
			}

			usedTokens += result.UsedTokens;
			nodes.emplace_back(std::move(result.Node));
			itr += result.UsedTokens;

			if (!TestToken(*itr, Tokenizer::ETokenClass::Symbol, ","))
			{
				ReportError({ itr, endItr }, itr->Start, "Expected ',' followed by parameter");
				break;
			}
			++itr;
		}

		return {
			.UsedTokens = end,
			.Node       = {
						   .Type     = AST::EType::Parameters,
						   .Token    = {},
						   .Children = std::move(nodes)}
		};
	}

	ParseResult State::ParseParameter(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		std::size_t            usedTokens = 0;
		std::vector<AST::Node> nodes;

		auto result = ParseAttributes(tokens);
		usedTokens  += result.UsedTokens;
		nodes.emplace_back(std::move(result.Node));

		result     = ParseTypeQualifiers({ tokens.begin() + usedTokens, tokens.end() });
		usedTokens += result.UsedTokens;
		nodes.emplace_back(std::move(result.Node));

		result = ParseTypename({ tokens.begin() + usedTokens, tokens.end() });
		if (result.UsedTokens == 0)
			return {};
		usedTokens += result.UsedTokens;
		nodes.emplace_back(std::move(result.Node));

		result = ParseIdentifier({ tokens.begin() + usedTokens, tokens.end() });
		if (result.UsedTokens == 0)
			return {};
		usedTokens += result.UsedTokens;
		nodes.emplace_back(std::move(result.Node));

		return {
			.UsedTokens = usedTokens,
			.Node       = {
						   .Type     = AST::EType::Parameter,
						   .Token    = nodes[3].Token,
						   .Children = std::move(nodes)}
		};
	}

	ParseResult State::ParseArguments(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return { 0, { AST::EType::Arguments } };

		if (!TestToken(tokens[0], Tokenizer::ETokenClass::Symbol, "("))
			return { 0, { AST::EType::Arguments } };

		std::size_t end   = 1;
		std::size_t depth = 1;
		while (depth > 0)
		{
			auto token = tokens[end];
			if (TestToken(token, Tokenizer::ETokenClass::Symbol, "("))
				++depth;
			else if (TestToken(token, Tokenizer::ETokenClass::Symbol, ")"))
				--depth;
			if (++end == tokens.size())
				return { 0, { AST::EType::Arguments } };
		}

		std::size_t            usedTokens = 0;
		std::vector<AST::Node> nodes;

		auto itr    = tokens.begin() + 1;
		auto endItr = tokens.begin() + end - 1;
		while (itr != endItr)
		{
			auto result = ParseArgument({ itr, endItr });
			if (result.UsedTokens == 0)
			{
				ReportError({ itr, endItr }, itr->Start, "Expected argument");
				break;
			}

			usedTokens += result.UsedTokens;
			nodes.emplace_back(std::move(result.Node));
			itr += result.UsedTokens;

			if (!TestToken(*itr, Tokenizer::ETokenClass::Symbol, ","))
			{
				ReportError({ itr, endItr }, itr->Start, "Expected ',' followed by argument");
				break;
			}
			++itr;
		}

		return {
			.UsedTokens = end,
			.Node       = {
						   .Type     = AST::EType::Arguments,
						   .Token    = {},
						   .Children = std::move(nodes)}
		};
	}

	ParseResult State::ParseArgument(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		auto result = ParseLiteral(tokens);
		if (result.UsedTokens == 0)
			return {};

		return {
			.UsedTokens = result.UsedTokens,
			.Node       = {
						   .Type     = AST::EType::Argument,
						   .Token    = result.Node.Token,
						   .Children = { std::move(result.Node) }}
		};
	}

	ParseResult State::ParseAttributes(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return { 0, { AST::EType::Attributes } };

		if (!TestToken(tokens[0], Tokenizer::ETokenClass::Symbol, "[["))
			return { 0, { AST::EType::Attributes } };

		std::size_t end   = 1;
		std::size_t depth = 1;
		while (depth > 0)
		{
			auto token = tokens[end];
			if (TestToken(token, Tokenizer::ETokenClass::Symbol, "[["))
				++depth;
			else if (TestToken(token, Tokenizer::ETokenClass::Symbol, "]]"))
				--depth;
			if (++end == tokens.size())
				return { 0, { AST::EType::Attributes } };
		}

		std::size_t            usedTokens = 0;
		std::vector<AST::Node> nodes;

		auto itr    = tokens.begin() + 1;
		auto endItr = tokens.begin() + end - 1;
		while (itr != endItr)
		{
			auto result = ParseAttribute({ itr, endItr });
			if (result.UsedTokens == 0)
			{
				ReportError({ itr, endItr }, itr->Start, "Expected attribute");
				break;
			}

			usedTokens += result.UsedTokens;
			nodes.emplace_back(std::move(result.Node));
			itr += result.UsedTokens;
		}

		return {
			.UsedTokens = end,
			.Node       = {
						   .Type     = AST::EType::Attributes,
						   .Token    = {},
						   .Children = std::move(nodes)}
		};
	}

	ParseResult State::ParseAttribute(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		std::size_t            usedTokens = 0;
		std::vector<AST::Node> nodes;

		auto result = ParseIdentifier(tokens);
		if (result.UsedTokens == 0)
			return {};
		usedTokens += result.UsedTokens;
		nodes.emplace_back(std::move(result.Node));

		result     = ParseArguments(tokens);
		usedTokens += result.UsedTokens;
		nodes.emplace_back(std::move(result.Node));

		return {
			.UsedTokens = usedTokens,
			.Node       = {
						   .Type     = AST::EType::Attribute,
						   .Token    = nodes[0].Token,
						   .Children = std::move(nodes)}
		};
	}

	ParseResult State::ParseTypename(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		std::size_t            usedTokens = 0;
		std::vector<AST::Node> nodes;

		auto itr = tokens.begin();
		auto end = tokens.end();
		while (itr != end)
		{
			auto result = ParseIdentifier({ itr, end });
			if (usedTokens == 0 && result.UsedTokens == 0)
				return {};
			usedTokens += result.UsedTokens;
			nodes.emplace_back(std::move(result.Node));
			itr += result.UsedTokens;
			if (itr == end)
				break;

			if (!TestToken(*itr, Tokenizer::ETokenClass::Symbol, "::"))
				break;
			++itr;
		}

		return {
			.UsedTokens = usedTokens,
			.Node       = {
						   .Type     = AST::EType::Typename,
						   .Token    = nodes.back().Token,
						   .Children = std::move(nodes)}
		};
	}

	ParseResult State::ParseTypeQualifiers(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return { 0, { AST::EType::TypeQualifiers } };

		std::size_t            usedTokens = 0;
		std::vector<AST::Node> nodes;

		auto itr = tokens.begin();
		auto end = tokens.end();
		while (itr != end)
		{
			auto result = ParseTypeQualifier({ itr, end });
			if (result.UsedTokens == 0)
				break;
			usedTokens += result.UsedTokens;
			nodes.emplace_back(std::move(result.Node));
			itr += result.UsedTokens;
		}

		return {
			.UsedTokens = usedTokens,
			.Node       = {
						   .Type     = AST::EType::TypeQualifiers,
						   .Token    = {},
						   .Children = std::move(nodes)}
		};
	}

	ParseResult State::ParseTypeQualifier(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		auto token = tokens[0];
		if (!TestToken(token, Tokenizer::ETokenClass::Identifier, "in") &&
			!TestToken(token, Tokenizer::ETokenClass::Identifier, "out") &&
			!TestToken(token, Tokenizer::ETokenClass::Identifier, "inout"))
			return {};

		return {
			.UsedTokens = 1,
			.Node       = {
						   .Type     = AST::EType::TypeQualifier,
						   .Token    = token,
						   .Children = {}}
		};
	}

	ParseResult State::ParseLiteral(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		auto result = ParseIntegerLiteral(tokens);
		if (result.UsedTokens != 0)
			return result;

		result = ParseFloatLiteral(tokens);
		if (result.UsedTokens != 0)
			return result;

		result = ParseBoolLiteral(tokens);
		if (result.UsedTokens != 0)
			return result;

		return {};
	}

	ParseResult State::ParseIntegerLiteral(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		auto result = ParseBinaryIntegerLiteral(tokens);
		if (result.UsedTokens != 0)
			return result;

		result = ParseOctalIntegerLiteral(tokens);
		if (result.UsedTokens != 0)
			return result;

		result = ParseDecimalIntegerLiteral(tokens);
		if (result.UsedTokens != 0)
			return result;

		result = ParseHexIntegerLiteral(tokens);
		if (result.UsedTokens != 0)
			return result;

		return {};
	}

	ParseResult State::ParseFloatLiteral(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		auto result = ParseDecimalFloatLiteral(tokens);
		if (result.UsedTokens != 0)
			return result;

		result = ParseHexFloatLiteral(tokens);
		if (result.UsedTokens != 0)
			return result;

		return {};
	}

	ParseResult State::ParseBoolLiteral(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		auto token = tokens[0];
		if (!TestToken(token, Tokenizer::ETokenClass::Identifier, "false") &&
			!TestToken(token, Tokenizer::ETokenClass::Identifier, "true"))
			return {};

		return {
			.UsedTokens = 1,
			.Node       = {
						   .Type     = AST::EType::BoolLiteral,
						   .Token    = token,
						   .Children = {}}
		};
	}

	ParseResult State::ParseBinaryIntegerLiteral(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		auto token = tokens[0];
		if (token.Class != Tokenizer::ETokenClass::BinaryInteger)
			return {};

		return {
			.UsedTokens = 1,
			.Node       = {
						   .Type     = AST::EType::IntegerLiteral,
						   .Token    = token,
						   .Children = {}}
		};
	}

	ParseResult State::ParseOctalIntegerLiteral(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		auto token = tokens[0];
		if (token.Class != Tokenizer::ETokenClass::OctalInteger)
			return {};

		return {
			.UsedTokens = 1,
			.Node       = {
						   .Type     = AST::EType::IntegerLiteral,
						   .Token    = token,
						   .Children = {}}
		};
	}

	ParseResult State::ParseDecimalIntegerLiteral(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		auto token = tokens[0];
		if (token.Class != Tokenizer::ETokenClass::DecimalInteger)
			return {};

		return {
			.UsedTokens = 1,
			.Node       = {
						   .Type     = AST::EType::IntegerLiteral,
						   .Token    = token,
						   .Children = {}}
		};
	}

	ParseResult State::ParseHexIntegerLiteral(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		auto token = tokens[0];
		if (token.Class != Tokenizer::ETokenClass::HexInteger)
			return {};

		return {
			.UsedTokens = 1,
			.Node       = {
						   .Type     = AST::EType::IntegerLiteral,
						   .Token    = token,
						   .Children = {}}
		};
	}

	ParseResult State::ParseDecimalFloatLiteral(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		auto token = tokens[0];
		if (token.Class != Tokenizer::ETokenClass::Float)
			return {};

		return {
			.UsedTokens = 1,
			.Node       = {
						   .Type     = AST::EType::FloatLiteral,
						   .Token    = token,
						   .Children = {}}
		};
	}

	ParseResult State::ParseHexFloatLiteral(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		auto token = tokens[0];
		if (token.Class != Tokenizer::ETokenClass::HexFloat)
			return {};

		return {
			.UsedTokens = 1,
			.Node       = {
						   .Type     = AST::EType::FloatLiteral,
						   .Token    = token,
						   .Children = {}}
		};
	}

	ParseResult State::ParseIdentifier(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		if (tokens[0].Class != Tokenizer::ETokenClass::Identifier)
			return {};

		return {
			.UsedTokens = 1,
			.Node       = {
						   .Type     = AST::EType::Identifier,
						   .Token    = tokens[0],
						   .Children = {}}
		};
	}
} // namespace Frertex::Parser