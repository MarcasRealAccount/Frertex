#include "Frertex/Compiler/Compiler.h"

namespace Frertex::Compiler
{
	FIL::Binary State::Compile(std::string_view source, const AST::AST& ast)
	{
		m_Source = source;
		m_AST    = &ast;
		m_FunctionDeclarations.clear();

		FindDeclarations();

		FIL::Binary fil;
		return fil;
	}

	void State::ReportMessage(std::uint8_t messageType, Utils::View<std::uint64_t> nodes, std::uint64_t point, std::string_view message)
	{
	}

	void State::ReportWarning(Utils::View<std::uint64_t> nodes, std::uint64_t point, std::string_view message)
	{
	}

	void State::ReportError(Utils::View<std::uint64_t> nodes, std::uint64_t point, std::string_view message)
	{
	}

	std::string_view State::GetSource(const Tokenizer::Token& token)
	{
		return m_Source.substr(token.Start, token.Length);
	}

	void State::FindDeclarations()
	{
		std::vector<std::string_view> currentNamespaceStack;

		AST::WalkASTNode(
			*m_AST,
			m_AST->RootNode(),
			[&](const AST::AST& ast, std::uint64_t index, const AST::Node& node) -> AST::EWalkerResult {
				// Enter node
				switch (node.Type)
				{
				case AST::EType::FunctionDeclaration:
				{
					FIL::EEntrypointType type = FIL::EEntrypointType::None;
					AST::WalkASTNode(
						ast,
						ast.GetChild(index, 0),
						[&](const AST::AST& ast2, std::uint64_t index2, const AST::Node& node2) -> AST::EWalkerResult {
							switch (node2.Type)
							{
							case AST::EType::Attribute:
							{
								if (type != FIL::EEntrypointType::None)
								{
									ReportWarning({ index2 }, node2.Token.Start, "Attribute unused");
									break;
								}
								FIL::EEntrypointType type2 = FIL::EntrypointTypeFromString(GetSource(node2.Token));
								if (type2 == FIL::EEntrypointType::None)
								{
									ReportWarning({ index2 }, node2.Token.Start, "Attribute unused");
									break;
								}
								type = type2;
								break;
							}
							default: return AST::EWalkerResult::Continue;
							}
							return AST::EWalkerResult::SkipChild;
						});

					std::string returnType = GetTypename(ast.GetChild(index, 1));

					std::string_view identifier = GetSource(node.Token);
					std::string      fqn;
					for (auto cn : currentNamespaceStack)
						(fqn += cn) += "::";
					fqn += identifier;

					std::vector<FunctionDeclaration::Parameter> parameters;
					AST::WalkASTNode(
						ast,
						ast.GetChild(index, 3),
						[&](const AST::AST& ast2, std::uint64_t index2, const AST::Node& node2) -> AST::EWalkerResult {
							switch (node2.Type)
							{
							case AST::EType::Parameter:
							{
								std::string location;
								AST::WalkASTNode(
									ast2,
									ast2.GetChild(index2, 0),
									[&](const AST::AST& ast3, std::uint64_t index3, const AST::Node& node3) -> AST::EWalkerResult {
										switch (node3.Type)
										{
										case AST::EType::Attribute:
										{
											if (!location.empty())
											{
												ReportWarning({ index3 }, node3.Token.Start, "Attribute unused");
												break;
											}
											std::string location2 = GetLocation(GetSource(node3.Token));
											if (location2.empty())
											{
												ReportWarning({ index3 }, node3.Token.Start, "Attribute unused");
												break;
											}
											location = location2;
											break;
										}
										default: return AST::EWalkerResult::Continue;
										}
										return AST::EWalkerResult::SkipChild;
									});

								parameters.emplace_back(FunctionDeclaration::Parameter {
									.Qualifier  = FIL::TypeQualifierFromString(GetSource(ast2[ast2.GetChild(index2, 1)].Token)),
									.Type       = GetTypename(ast2.GetChild(index2, 2)),
									.Identifier = std::string { GetSource(node2.Token) },
									.Location   = location });
								return AST::EWalkerResult::SkipChild;
							}
							default: return AST::EWalkerResult::Continue;
							}
						});

					m_FunctionDeclarations.emplace_back(FunctionDeclaration {
						.Node               = index,
						.Type               = type,
						.Identifier         = std::string { identifier },
						.FullyQualifiedName = fqn,
						.ReturnType         = std::string { returnType },
						.Parameters         = std::move(parameters) });
					currentNamespaceStack.emplace_back(identifier);
					return AST::EWalkerResult::Continue;
				}
				case AST::EType::Declarations:
				case AST::EType::CompoundStatement: return AST::EWalkerResult::Continue;
				default: return AST::EWalkerResult::SkipChild;
				}
			},
			[&](const AST::AST& ast, std::uint64_t index, const AST::Node& node) -> AST::EWalkerResult {
				// Exit node
				switch (node.Type)
				{
				case AST::EType::FunctionDeclaration:
					currentNamespaceStack.pop_back();
					break;
				}
				return AST::EWalkerResult::Continue;
			});
	}

	std::string State::GetTypename(std::uint64_t node)
	{
		std::string tpn;
		AST::WalkASTNode(
			*m_AST,
			node,
			[&](const AST::AST& ast, std::uint64_t index, const AST::Node& node2) -> AST::EWalkerResult {
				switch (node2.Type)
				{
				case AST::EType::Identifier:
					if (!tpn.empty())
						tpn += "::";
					tpn += GetSource(node2.Token);
					return AST::EWalkerResult::SkipChild;
				default:
					return AST::EWalkerResult::Continue;
				}
			});
		return tpn;
	}

	std::string State::GetLocation(std::string_view string)
	{
		if (string == "Position") return "Position";
		return {};
	}
} // namespace Frertex::Compiler