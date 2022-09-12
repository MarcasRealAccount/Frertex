#pragma once

#include "Frertex/FIL/CodeBuffer.h"
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

	struct EntrypointDefinition
	{
	public:
		EntrypointDefinition(Utils::CopyMovable<std::string>&& nameSpace, Utils::CopyMovable<std::string>&& name, FIL::EEntrypointType type, AST::Node* root, AST::Node* node)
		    : m_Namespace(nameSpace.get()), m_Name(name.get()), m_Type(type), m_Root(root), m_Node(node) {}

	public:
		std::string          m_Namespace;
		std::string          m_Name;
		FIL::EEntrypointType m_Type;
		AST::Node*           m_Root;
		AST::Node*           m_Node;
	};

	struct EntrypointParameter
	{
	public:
		EntrypointParameter()
		    : m_Typename({}), m_BuiltinType(FIL::ETypeID::Void), m_Location(0) {}

		EntrypointParameter(Typename typeName, FIL::ETypeID builtinType, std::uint64_t location)
		    : m_Typename(typeName), m_BuiltinType(builtinType), m_Location(location) {}

	public:
		Typename      m_Typename;
		FIL::ETypeID  m_BuiltinType;
		std::uint64_t m_Location;
	};

	struct EntrypointDeclaration
	{
	public:
		EntrypointDeclaration(Utils::CopyMovable<std::string>&& nameSpace, Utils::CopyMovable<std::string>&& name, FIL::EEntrypointType type, Utils::CopyMovable<std::vector<EntrypointParameter>>&& parameters, Utils::CopyMovable<std::vector<std::uint32_t>>&& code)
		    : m_Namespace(nameSpace.get()), m_Name(name.get()), m_Type(type), m_Parameters(parameters.get()), m_Code(code.get()) {}

	public:
		std::string                      m_Namespace;
		std::string                      m_Name;
		FIL::EEntrypointType             m_Type;
		std::vector<EntrypointParameter> m_Parameters;
		std::vector<std::uint32_t>       m_Code;
	};

	struct FunctionDefinition
	{
	public:
		FunctionDefinition(Utils::CopyMovable<std::string>&& nameSpace, Utils::CopyMovable<std::string>&& name, AST::Node* root, AST::Node* node)
		    : m_Namespace(nameSpace.get()), m_Name(name.get()), m_Root(root), m_Node(node) {}

	public:
		std::string m_Namespace;
		std::string m_Name;
		AST::Node*  m_Root;
		AST::Node*  m_Node;
	};

	struct FunctionParameter
	{
	public:
		FunctionParameter()
		    : m_Typename({}) {}

		FunctionParameter(Typename typeName)
		    : m_Typename(typeName) {}

	public:
		Typename m_Typename;
	};

	struct FunctionDeclaration
	{
	public:
		FunctionDeclaration(Utils::CopyMovable<std::string>&& nameSpace, Utils::CopyMovable<std::string>&& name, Utils::CopyMovable<std::vector<FunctionParameter>>&& parameters, Utils::CopyMovable<std::vector<std::uint32_t>>&& code)
		    : m_Namespace(nameSpace.get()), m_Name(name.get()), m_Parameters(parameters.get()), m_Code(code.get()) {}

	public:
		std::string                    m_Namespace;
		std::string                    m_Name;
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

		void compileEntrypointDeclaration(EntrypointDefinition& definition, AST::Node* root, AST::Node* node);
		void compileFunctionDeclaration(FunctionDefinition& definition, AST::Node* root, AST::Node* node);
		void compileFunctionBody(FIL::CodeBuffer& codeBuffer, AST::Node* root);
		bool compileStatements(FIL::CodeBuffer& codeBuffer, AST::Node* root);
		void compileReturnStatement(FIL::CodeBuffer& codeBuffer, AST::Node* root);
		void compileExpression(FIL::CodeBuffer& codeBuffer, AST::Node* node);
		void compileAssignmentExpression(FIL::CodeBuffer& codeBuffer, AST::Node* node);

		void pushScope();
		void popScope();

		FIL::ETypeID getTypeID(std::string_view str);

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

		std::vector<EntrypointDefinition>  m_EntrypointDefinitions;
		std::vector<EntrypointDeclaration> m_EntrypointDeclarations;
		std::vector<FunctionDefinition>    m_FunctionDefinitions;
		std::vector<FunctionDeclaration>   m_FunctionDeclarations;

		std::vector<TypeInfo>     m_Types;
		std::vector<VariableInfo> m_Variables;
		std::vector<Scope>        m_Scopes;
	};
} // namespace Frertex::Compiler