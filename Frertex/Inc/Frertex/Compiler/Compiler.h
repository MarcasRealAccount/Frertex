#pragma once

#include "Frertex/AST/AST.h"
#include "Frertex/FIL/FIL.h"

#include <string>
#include <vector>

namespace Frertex::Compiler
{
	struct FunctionDeclaration
	{
	public:
		struct Parameter
		{
		public:
			FIL::ETypeQualifier Qualifier;
			std::string         Type;
			std::string         Identifier;
			std::string         Location;
		};

	public:
		std::uint64_t Node;

		FIL::EEntrypointType Type;
		std::string          Identifier;
		std::string          FullyQualifiedName;

		std::string            ReturnType;
		std::vector<Parameter> Parameters;
	};

	class State
	{
	public:
		FIL::Binary Compile(std::string_view source, const AST::AST& ast);

	private:
		void ReportMessage(std::uint8_t messageType, Utils::View<std::uint64_t> nodes, std::uint64_t point, std::string_view message);
		void ReportWarning(Utils::View<std::uint64_t> nodes, std::uint64_t point, std::string_view message);
		void ReportError(Utils::View<std::uint64_t> nodes, std::uint64_t point, std::string_view message);

		std::string_view GetSource(const Tokenizer::Token& token);

		void FindDeclarations();

		std::string GetTypename(std::uint64_t node);
		std::string GetLocation(std::string_view string);

	private:
		std::string_view m_Source;
		const AST::AST*  m_AST;

		std::vector<FunctionDeclaration> m_FunctionDeclarations;
	};
} // namespace Frertex::Compiler