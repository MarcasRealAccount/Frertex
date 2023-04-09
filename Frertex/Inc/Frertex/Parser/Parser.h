#pragma once

#include "Frertex/AST/AST.h"
#include "Frertex/Utils/View.h"

namespace Frertex::Parser
{
	struct ParseResult
	{
	public:
		operator bool() const { return UsedTokens != 0 && Node != ~0ULL; }

	public:
		std::size_t   UsedTokens = 0;
		std::uint64_t Node       = ~0ULL;
	};

	struct TokenPattern
	{
		Tokenizer::ETokenClass Class;
		std::string_view       String;
	};

	class State
	{
	public:
		AST::AST Parse(std::string_view source, Utils::View<Tokenizer::Token> tokens);

	private:
		void ReportError(Utils::View<Tokenizer::Token> tokens, std::size_t point, std::string message);

		std::string_view GetSource(Tokenizer::Token token);

		bool        TestToken(Tokenizer::Token token, TokenPattern pattern);
		std::size_t FindEndToken(Utils::View<Tokenizer::Token> tokens, std::size_t offset, TokenPattern open, TokenPattern close);

		ParseResult ParseDeclarations(Utils::View<Tokenizer::Token> tokens);
		ParseResult ParseDeclaration(Utils::View<Tokenizer::Token> tokens);
		ParseResult ParseFunctionDeclaration(Utils::View<Tokenizer::Token> tokens);

		ParseResult ParseStatements(Utils::View<Tokenizer::Token> tokens, bool parseFull = false);
		ParseResult ParseStatement(Utils::View<Tokenizer::Token> tokens);
		ParseResult ParseEmptyStatement(Utils::View<Tokenizer::Token> tokens);
		ParseResult ParseCompoundStatement(Utils::View<Tokenizer::Token> tokens);

		ParseResult ParseParameters(Utils::View<Tokenizer::Token> tokens);
		ParseResult ParseParameter(Utils::View<Tokenizer::Token> tokens);
		ParseResult ParseArguments(Utils::View<Tokenizer::Token> tokens);
		ParseResult ParseArgument(Utils::View<Tokenizer::Token> tokens);
		ParseResult ParseAttributes(Utils::View<Tokenizer::Token> tokens);
		ParseResult ParseAttribute(Utils::View<Tokenizer::Token> tokens);

		ParseResult ParseTypename(Utils::View<Tokenizer::Token> tokens);
		ParseResult ParseTypeQualifiers(Utils::View<Tokenizer::Token> tokens);
		ParseResult ParseTypeQualifier(Utils::View<Tokenizer::Token> tokens);

		ParseResult ParseLiteral(Utils::View<Tokenizer::Token> tokens);
		ParseResult ParseIntegerLiteral(Utils::View<Tokenizer::Token> tokens);
		ParseResult ParseFloatLiteral(Utils::View<Tokenizer::Token> tokens);
		ParseResult ParseBoolLiteral(Utils::View<Tokenizer::Token> tokens);

		ParseResult ParseBinaryIntegerLiteral(Utils::View<Tokenizer::Token> tokens);
		ParseResult ParseOctalIntegerLiteral(Utils::View<Tokenizer::Token> tokens);
		ParseResult ParseDecimalIntegerLiteral(Utils::View<Tokenizer::Token> tokens);
		ParseResult ParseHexIntegerLiteral(Utils::View<Tokenizer::Token> tokens);
		ParseResult ParseDecimalFloatLiteral(Utils::View<Tokenizer::Token> tokens);
		ParseResult ParseHexFloatLiteral(Utils::View<Tokenizer::Token> tokens);

		ParseResult ParseIdentifier(Utils::View<Tokenizer::Token> tokens);

	private:
		std::string_view m_Source;

		AST::AST m_AST;
	};
} // namespace Frertex::Parser