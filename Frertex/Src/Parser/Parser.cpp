#include "Frertex/Parser/Parser.h"

namespace Frertex::Parser
{
	AST::AST State::Parse(std::string_view source, Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		m_Source = source;
		m_AST    = AST::AST {};

		auto result = ParseDeclarations(tokens);
		m_AST.SetRootNode(result.Node);

		return std::move(m_AST);
	}

	void State::ReportError(Utils::View<Tokenizer::Token> tokens, std::size_t point, std::string message)
	{
	}

	std::string_view State::GetSource(Tokenizer::Token token)
	{
		return m_Source.substr(token.Start, token.Length);
	}

	bool State::TestToken(Tokenizer::Token token, TokenPattern pattern)
	{
		return token.Class == pattern.Class && GetSource(token) == pattern.String;
	}

	std::size_t State::FindEndToken(Utils::View<Tokenizer::Token> tokens, std::size_t offset, TokenPattern open, TokenPattern close)
	{
		std::size_t depth = 1;
		while (depth > 0)
		{
			auto token = tokens[offset];
			if (TestToken(token, open))
				++depth;
			else if (TestToken(token, close))
				--depth;
			if (++offset > tokens.size())
				return ~0ULL;
		}
		return offset;
	}

	ParseResult State::ParseDeclarations(Utils::View<Tokenizer::Token> tokens)
	{
		std::uint64_t node = m_AST.Alloc({ .Type = AST::EType::Declarations });

		if (tokens.empty()) return { .UsedTokens = 0, .Node = node };

		std::size_t   usedTokens   = 0;
		std::uint64_t firstNode    = ~0ULL;
		std::uint64_t previousNode = ~0ULL;

		auto itr = tokens.begin();
		auto end = tokens.end();
		while (itr != end)
		{
			auto result = ParseDeclaration({ itr, end });
			if (!result)
			{
				ReportError({ itr, end }, itr->Start, "Expected declaration");
				break;
			}

			usedTokens += result.UsedTokens;
			itr        += result.UsedTokens;
			if (firstNode == ~0ULL) firstNode = result.Node;
			if (previousNode != ~0ULL)
				m_AST.SetSiblings(previousNode, result.Node);
			previousNode = result.Node;
		}

		if (firstNode != ~0ULL)
			m_AST.SetParent(firstNode, node);
		return { .UsedTokens = usedTokens, .Node = node };
	}

	ParseResult State::ParseDeclaration(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		auto result = ParseFunctionDeclaration(tokens);
		if (result)
			return result;

		return {};
	}

	ParseResult State::ParseFunctionDeclaration(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		std::uint64_t node         = m_AST.Alloc({ .Type = AST::EType::FunctionDeclaration });
		std::size_t   usedTokens   = 0;
		std::uint64_t previousNode = ~0ULL;

		auto result = ParseAttributes(tokens);
		usedTokens  += result.UsedTokens;
		m_AST.SetParent(result.Node, node);
		previousNode = result.Node;

		result = ParseTypename({ tokens.begin() + usedTokens, tokens.end() });
		if (!result)
		{
			m_AST.FreeFull(node);
			return {};
		}
		usedTokens += result.UsedTokens;
		m_AST.SetSiblings(previousNode, result.Node);
		previousNode = result.Node;

		result = ParseIdentifier({ tokens.begin() + usedTokens, tokens.end() });
		if (!result)
		{
			m_AST.FreeFull(node);
			return {};
		}
		usedTokens += result.UsedTokens;
		m_AST.SetSiblings(previousNode, result.Node);
		previousNode      = result.Node;
		m_AST[node].Token = m_AST[result.Node].Token;

		result = ParseParameters({ tokens.begin() + usedTokens, tokens.end() });
		if (!result)
		{
			m_AST.FreeFull(node);
			return {};
		}
		usedTokens += result.UsedTokens;
		m_AST.SetSiblings(previousNode, result.Node);
		previousNode = result.Node;

		result = ParseCompoundStatement({ tokens.begin() + usedTokens, tokens.end() });
		if (!result)
		{
			m_AST.FreeFull(node);
			return {};
		}
		usedTokens += result.UsedTokens;
		m_AST.SetSiblings(previousNode, result.Node);

		return { .UsedTokens = usedTokens, .Node = node };
	}

	ParseResult State::ParseStatements(Utils::View<Tokenizer::Token> tokens, bool parseFull)
	{
		std::uint64_t node = m_AST.Alloc({ .Type = AST::EType::Statements });

		if (tokens.empty()) return { .UsedTokens = 0, .Node = node };

		std::size_t   usedTokens   = 0;
		std::uint64_t firstNode    = ~0ULL;
		std::uint64_t previousNode = ~0ULL;

		auto itr = tokens.begin();
		auto end = tokens.end();
		while (itr != end)
		{
			auto result = ParseStatement({ itr, end });
			if (!result)
			{
				if (parseFull)
					ReportError({ itr, end }, itr->Start, "Expected statement");
				break;
			}

			usedTokens += result.UsedTokens;
			itr        += result.UsedTokens;
			if (firstNode == ~0ULL) firstNode = result.Node;
			if (previousNode != ~0ULL)
				m_AST.SetSiblings(previousNode, result.Node);
			previousNode = result.Node;
		}

		m_AST.SetParent(firstNode, node);

		return { .UsedTokens = usedTokens, .Node = node };
	}

	ParseResult State::ParseStatement(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		auto result = ParseEmptyStatement(tokens);
		if (result)
			return result;

		result = ParseCompoundStatement(tokens);
		if (result)
			return result;

		result = ParseDeclaration(tokens);
		if (result)
			return result;

		return {};
	}

	ParseResult State::ParseEmptyStatement(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		auto token = tokens[0];
		if (!TestToken(token, { .Class = Tokenizer::ETokenClass::Symbol, .String = ";" }))
			return {};

		return {
			.UsedTokens = 1,
			.Node       = m_AST.Alloc({ .Type  = AST::EType::EmptyStatement,
										.Token = token })
		};
	}

	ParseResult State::ParseCompoundStatement(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		if (!TestToken(tokens[0], { .Class = Tokenizer::ETokenClass::Symbol, .String = "{" }))
			return {};
		std::size_t end = FindEndToken(tokens,
									   1,
									   { .Class = Tokenizer::ETokenClass::Symbol, .String = "{" },
									   { .Class = Tokenizer::ETokenClass::Symbol, .String = "}" });
		if (end == ~0ULL)
			return {};

		auto result = ParseStatements({ tokens.begin() + 1, tokens.begin() + end }, true);

		std::uint64_t node = m_AST.Alloc({ .Type = AST::EType::CompoundStatement });
		m_AST.SetParent(result.Node, node);

		return { .UsedTokens = end, .Node = node };
	}

	ParseResult State::ParseParameters(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		if (!TestToken(tokens[0], { .Class = Tokenizer::ETokenClass::Symbol, .String = "(" }))
			return {};
		std::size_t end = FindEndToken(tokens,
									   1,
									   { .Class = Tokenizer::ETokenClass::Symbol, .String = "(" },
									   { .Class = Tokenizer::ETokenClass::Symbol, .String = ")" });
		if (end == ~0ULL)
			return {};

		std::uint64_t node = m_AST.Alloc({ .Type = AST::EType::Parameters });

		std::size_t   usedTokens   = 0;
		std::uint64_t firstNode    = ~0ULL;
		std::uint64_t previousNode = ~0ULL;

		auto itr    = tokens.begin() + 1;
		auto endItr = tokens.begin() + end - 1;
		while (itr != endItr)
		{
			auto result = ParseParameter({ itr, endItr });
			if (!result)
			{
				ReportError({ itr, endItr }, itr->Start, "Expected parameter");
				break;
			}

			usedTokens += result.UsedTokens;
			itr        += result.UsedTokens;
			if (firstNode == ~0ULL) firstNode = result.Node;
			if (previousNode != ~0ULL)
				m_AST.SetSiblings(previousNode, result.Node);
			previousNode = result.Node;

			if (!TestToken(*itr, { .Class = Tokenizer::ETokenClass::Symbol, .String = "," }))
			{
				ReportError({ itr, endItr }, itr->Start, "Expected ',' followed by parameter");
				break;
			}
			++itr;
		}

		m_AST.SetParent(firstNode, node);

		return { .UsedTokens = end, .Node = node };
	}

	ParseResult State::ParseParameter(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		std::uint64_t node = m_AST.Alloc({ .Type = AST::EType::Parameter });

		std::size_t   usedTokens   = 0;
		std::uint64_t previousNode = ~0ULL;

		auto result = ParseAttributes(tokens);
		usedTokens  += result.UsedTokens;
		m_AST.SetParent(result.Node, node);
		previousNode = result.Node;

		result     = ParseTypeQualifier({ tokens.begin() + usedTokens, tokens.end() });
		usedTokens += result.UsedTokens;
		m_AST.SetSiblings(previousNode, result.Node);
		previousNode = result.Node;

		result = ParseTypename({ tokens.begin() + usedTokens, tokens.end() });
		if (!result)
		{
			m_AST.FreeFull(node);
			return {};
		}
		usedTokens += result.UsedTokens;
		m_AST.SetSiblings(previousNode, result.Node);
		previousNode = result.Node;

		result = ParseIdentifier({ tokens.begin() + usedTokens, tokens.end() });
		if (!result)
		{
			m_AST.FreeFull(node);
			return {};
		}
		usedTokens += result.UsedTokens;
		m_AST.SetSiblings(previousNode, result.Node);
		m_AST[node].Token = m_AST[result.Node].Token;

		return { .UsedTokens = usedTokens, .Node = node };
	}

	ParseResult State::ParseArguments(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		if (!TestToken(tokens[0], { .Class = Tokenizer::ETokenClass::Symbol, .String = "(" }))
			return {};
		std::size_t end = FindEndToken(tokens,
									   1,
									   { .Class = Tokenizer::ETokenClass::Symbol, .String = "(" },
									   { .Class = Tokenizer::ETokenClass::Symbol, .String = "}" });
		if (end == ~0ULL)
			return {};

		std::uint64_t node = m_AST.Alloc({ .Type = AST::EType::Arguments });

		std::size_t   usedTokens   = 0;
		std::uint64_t firstNode    = ~0ULL;
		std::uint64_t previousNode = ~0ULL;

		auto itr    = tokens.begin() + 1;
		auto endItr = tokens.begin() + end - 1;
		while (itr != endItr)
		{
			auto result = ParseArgument({ itr, endItr });
			if (!result)
			{
				ReportError({ itr, endItr }, itr->Start, "Expected argument");
				break;
			}

			usedTokens += result.UsedTokens;
			itr        += result.UsedTokens;
			if (firstNode == ~0ULL) firstNode = result.Node;
			if (previousNode != ~0ULL)
				m_AST.SetSiblings(previousNode, result.Node);
			previousNode = result.Node;

			if (!TestToken(*itr, { .Class = Tokenizer::ETokenClass::Symbol, .String = "," }))
			{
				ReportError({ itr, endItr }, itr->Start, "Expected ',' followed by argument");
				break;
			}
			++itr;
		}

		return { .UsedTokens = end, .Node = node };
	}

	ParseResult State::ParseArgument(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		auto result = ParseLiteral(tokens);
		if (!result)
			return {};

		std::uint64_t node = m_AST.Alloc({ .Type = AST::EType::Argument });
		m_AST.SetParent(result.Node, node);
		m_AST[node].Token = m_AST[result.Node].Token;

		return { .UsedTokens = result.UsedTokens, .Node = node };
	}

	ParseResult State::ParseAttributes(Utils::View<Tokenizer::Token> tokens)
	{
		std::uint64_t node = m_AST.Alloc({ .Type = AST::EType::Attributes });

		if (tokens.empty())
			return { .UsedTokens = 0, .Node = node };

		if (!TestToken(tokens[0], { .Class = Tokenizer::ETokenClass::Symbol, .String = "[[" }))
			return { .UsedTokens = 0, .Node = node };
		std::size_t end = FindEndToken(tokens,
									   1,
									   { .Class = Tokenizer::ETokenClass::Symbol, .String = "[[" },
									   { .Class = Tokenizer::ETokenClass::Symbol, .String = "]]" });
		if (end == ~0ULL)
			return { .UsedTokens = 0, .Node = node };

		std::size_t   usedTokens   = 0;
		std::uint64_t firstNode    = ~0ULL;
		std::uint64_t previousNode = ~0ULL;

		auto itr    = tokens.begin() + 1;
		auto endItr = tokens.begin() + end - 1;
		while (itr != endItr)
		{
			auto result = ParseAttribute({ itr, endItr });
			if (!result)
			{
				ReportError({ itr, endItr }, itr->Start, "Expected attribute");
				break;
			}

			usedTokens += result.UsedTokens;
			itr        += result.UsedTokens;
			if (firstNode == ~0ULL) firstNode = result.Node;
			if (previousNode != ~0ULL)
				m_AST.SetSiblings(previousNode, result.Node);
			previousNode = result.Node;
		}

		m_AST.SetParent(firstNode, node);

		return { .UsedTokens = end, .Node = node };
	}

	ParseResult State::ParseAttribute(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		std::uint64_t node = m_AST.Alloc({ .Type = AST::EType::Attribute });

		std::size_t   usedTokens   = 0;
		std::uint64_t previousNode = ~0ULL;

		auto result = ParseIdentifier(tokens);
		if (!result)
		{
			m_AST.FreeFull(node);
			return {};
		}
		usedTokens += result.UsedTokens;
		m_AST.SetParent(result.Node, node);
		previousNode      = result.Node;
		m_AST[node].Token = m_AST[result.Node].Token;

		result     = ParseArguments(tokens);
		usedTokens += result.UsedTokens;
		m_AST.SetSiblings(previousNode, result.Node);


		return { .UsedTokens = usedTokens, .Node = node };
	}

	ParseResult State::ParseTypename(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		std::uint64_t node = m_AST.Alloc({ .Type = AST::EType::Typename });

		std::size_t   usedTokens   = 0;
		std::uint64_t firstNode    = ~0ULL;
		std::uint64_t previousNode = ~0ULL;

		auto itr = tokens.begin();
		auto end = tokens.end();
		while (itr != end)
		{
			auto result = ParseIdentifier({ itr, end });
			if (!result)
			{
				if (usedTokens)
					break;
				else
					return {};
			}
			usedTokens += result.UsedTokens;
			itr        += result.UsedTokens;
			if (firstNode == ~0ULL) firstNode = result.Node;
			if (previousNode != ~0ULL)
				m_AST.SetSiblings(previousNode, result.Node);
			previousNode = result.Node;
			if (itr == end)
				break;

			if (!TestToken(*itr, { .Class = Tokenizer::ETokenClass::Symbol, .String = "::" }))
				break;
			++itr;
		}

		m_AST.SetParent(firstNode, node);

		return { .UsedTokens = usedTokens, .Node = node };
	}

	ParseResult State::ParseTypeQualifier(Utils::View<Tokenizer::Token> tokens)
	{
		std::uint64_t node = m_AST.Alloc({ .Type = AST::EType::TypeQualifier });

		if (tokens.empty())
			return { .UsedTokens = 0, .Node = node };

		auto token = tokens[0];
		if (!TestToken(token, { .Class = Tokenizer::ETokenClass::Identifier, .String = "in" }) &&
			!TestToken(token, { .Class = Tokenizer::ETokenClass::Identifier, .String = "out" }) &&
			!TestToken(token, { .Class = Tokenizer::ETokenClass::Identifier, .String = "inout" }))
			return {};

		m_AST[node].Token = token;

		return { .UsedTokens = 1, .Node = node };
	}

	ParseResult State::ParseLiteral(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		auto result = ParseIntegerLiteral(tokens);
		if (!result)
			return result;

		result = ParseFloatLiteral(tokens);
		if (!result)
			return result;

		result = ParseBoolLiteral(tokens);
		if (!result)
			return result;

		return {};
	}

	ParseResult State::ParseIntegerLiteral(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		auto result = ParseBinaryIntegerLiteral(tokens);
		if (!result)
			return result;

		result = ParseOctalIntegerLiteral(tokens);
		if (!result)
			return result;

		result = ParseDecimalIntegerLiteral(tokens);
		if (!result)
			return result;

		result = ParseHexIntegerLiteral(tokens);
		if (!result)
			return result;

		return {};
	}

	ParseResult State::ParseFloatLiteral(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		auto result = ParseDecimalFloatLiteral(tokens);
		if (!result)
			return result;

		result = ParseHexFloatLiteral(tokens);
		if (!result)
			return result;

		return {};
	}

	ParseResult State::ParseBoolLiteral(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		auto token = tokens[0];
		if (!TestToken(token, { .Class = Tokenizer::ETokenClass::Identifier, .String = "false" }) &&
			!TestToken(token, { .Class = Tokenizer::ETokenClass::Identifier, .String = "true" }))
			return {};

		std::uint64_t node = m_AST.Alloc({ .Type = AST::EType::BoolLiteral });
		m_AST[node].Token  = token;

		return { .UsedTokens = 1, .Node = node };
	}

	ParseResult State::ParseBinaryIntegerLiteral(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		auto token = tokens[0];
		if (token.Class != Tokenizer::ETokenClass::BinaryInteger)
			return {};

		std::uint64_t node = m_AST.Alloc({ .Type = AST::EType::IntegerLiteral });
		m_AST[node].Token  = token;

		return { .UsedTokens = 1, .Node = node };
	}

	ParseResult State::ParseOctalIntegerLiteral(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		auto token = tokens[0];
		if (token.Class != Tokenizer::ETokenClass::OctalInteger)
			return {};

		std::uint64_t node = m_AST.Alloc({ .Type = AST::EType::IntegerLiteral });
		m_AST[node].Token  = token;

		return { .UsedTokens = 1, .Node = node };
	}

	ParseResult State::ParseDecimalIntegerLiteral(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		auto token = tokens[0];
		if (token.Class != Tokenizer::ETokenClass::DecimalInteger)
			return {};

		std::uint64_t node = m_AST.Alloc({ .Type = AST::EType::IntegerLiteral });
		m_AST[node].Token  = token;

		return { .UsedTokens = 1, .Node = node };
	}

	ParseResult State::ParseHexIntegerLiteral(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		auto token = tokens[0];
		if (token.Class != Tokenizer::ETokenClass::HexInteger)
			return {};

		std::uint64_t node = m_AST.Alloc({ .Type = AST::EType::IntegerLiteral });
		m_AST[node].Token  = token;

		return { .UsedTokens = 1, .Node = node };
	}

	ParseResult State::ParseDecimalFloatLiteral(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		auto token = tokens[0];
		if (token.Class != Tokenizer::ETokenClass::Float)
			return {};

		std::uint64_t node = m_AST.Alloc({ .Type = AST::EType::FloatLiteral });
		m_AST[node].Token  = token;

		return { .UsedTokens = 1, .Node = node };
	}

	ParseResult State::ParseHexFloatLiteral(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		auto token = tokens[0];
		if (token.Class != Tokenizer::ETokenClass::HexFloat)
			return {};

		std::uint64_t node = m_AST.Alloc({ .Type = AST::EType::FloatLiteral });
		m_AST[node].Token  = token;

		return { .UsedTokens = 1, .Node = node };
	}

	ParseResult State::ParseIdentifier(Utils::View<Tokenizer::Token> tokens)
	{
		if (tokens.empty())
			return {};

		if (tokens[0].Class != Tokenizer::ETokenClass::Identifier)
			return {};

		std::uint64_t node = m_AST.Alloc({ .Type = AST::EType::Identifier });
		m_AST[node].Token  = tokens[0];

		return { .UsedTokens = 1, .Node = node };
	}
} // namespace Frertex::Parser