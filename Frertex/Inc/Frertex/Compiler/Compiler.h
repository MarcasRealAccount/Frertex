#pragma once

#include "Frertex/FIL/FIL.h"
#include "Frertex/Message.h"
#include "Frertex/Parser/AST.h"
#include "Frertex/Source.h"

#include <stack>

namespace Frertex::Compiler
{
	struct Typename
	{
	public:
		std::vector<FIL::ETypeQualifier> m_Qualifiers;
		FIL::ETypeID                     m_TypeID;
	};

	struct TypeInfo
	{
	};

	struct VariableInfo
	{
	public:
		FIL::ETypeID m_TypeID;
	};

	struct FunctionDefinition
	{
	public:
		FunctionDefinition(Utils::CopyMovable<std::string>&& nameSpace, Utils::CopyMovable<std::string>&& name, FIL::EEntrypointType type, AST::Node* root)
		    : m_Namespace(nameSpace.get()), m_Name(name.get()), m_Type(type), m_Root(root) {}

	public:
		std::string          m_Namespace;
		std::string          m_Name;
		FIL::EEntrypointType m_Type;
		AST::Node*           m_Root;
	};

	struct FunctionParameter
	{
	public:
		FunctionParameter(Typename typeName, std::uint64_t location)
		    : m_Typename(typeName), m_Location(location) {}

	public:
		Typename      m_Typename;
		std::uint64_t m_Location;
	};

	struct FunctionDeclaration
	{
	public:
		FunctionDeclaration(Utils::CopyMovable<std::string>&& nameSpace, Utils::CopyMovable<std::string>&& name, FIL::EEntrypointType type, Utils::CopyMovable<std::vector<FunctionParameter>>&& parameters, Utils::CopyMovable<std::vector<std::uint32_t>>&& code)
		    : m_Namespace(nameSpace.get()), m_Name(name.get()), m_Type(type), m_Parameters(parameters.get()), m_Code(code.get()) {}

	public:
		std::string                    m_Namespace;
		std::string                    m_Name;
		FIL::EEntrypointType           m_Type;
		std::vector<FunctionParameter> m_Parameters;
		std::vector<std::uint32_t>     m_Code;
	};

	struct Scope
	{
	public:
		std::unordered_map<std::string, FIL::ETypeID> m_TypeIDs;
		std::unordered_map<std::string, std::size_t>  m_VariableIDs;

		std::string m_CurrentNamespace;
	};

	class State
	{
	public:
		State(Source::Sources* sources, Message::MessageHandler messageHandler, Utils::CopyMovable<std::vector<Tokenizer::Token>>&& tokens);

		FIL::Binary compile(Utils::CopyMovable<AST::Node>&& ast);

		auto getSources() const { return m_Sources; }
		auto errored() const { return m_Errored; }

	private:
		void getFunctionDefinitions(AST::Node* root);

		void compileFunctionDeclaration(AST::Node* node);
		void compileAssignmentExpression(AST::Node* node);
		void compileExpression(AST::Node* node);
		void compileStatements(AST::Node* root);

		void pushScope();
		void popScope();

		void reportMessage(Message::EMessageType type, std::size_t startToken, std::size_t endToken, Utils::CopyMovable<std::vector<Source::SourceSpan>>&& spans, Source::SourcePoint point, Utils::CopyMovable<std::string>&& message);
		void reportWarning(std::size_t startToken, std::size_t endToken, Utils::CopyMovable<std::vector<Source::SourceSpan>>&& spans, Source::SourcePoint point, Utils::CopyMovable<std::string>&& message) { reportMessage(Message::EMessageType::Warning, startToken, endToken, spans.get(), point, message.get()); }
		void reportError(std::size_t startToken, std::size_t endToken, Utils::CopyMovable<std::vector<Source::SourceSpan>>&& spans, Source::SourcePoint point, Utils::CopyMovable<std::string>&& message)
		{
			m_Errored = true;
			reportMessage(Message::EMessageType::Error, startToken, endToken, spans.get(), point, message.get());
		}

	private:
		Source::Sources* m_Sources;

		Message::MessageHandler m_MessageHandler;
		bool                    m_Errored = false;

		std::vector<Tokenizer::Token> m_Tokens;
		AST::Node                     m_AST;

		std::vector<FunctionDefinition>  m_FunctionDefinitions;
		std::vector<FunctionDeclaration> m_FunctionDeclarations;

		std::vector<TypeInfo> m_Types;

		std::vector<VariableInfo> m_Variables;
		std::stack<Scope>         m_Scopes;
	};
} // namespace Frertex::Compiler