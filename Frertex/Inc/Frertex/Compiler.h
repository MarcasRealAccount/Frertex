#pragma once

#include "AST.h"
#include "FIL.h"
#include "Message.h"
#include "Utils/Utils.h"

namespace Frertex
{
	class Compiler
	{
	public:
		Compiler(Utils::CopyMovable<std::vector<std::string>>&& includedFilenames);

		FIL compile(Utils::CopyMovable<AST>&& ast);

		void addWarning(SourceSpan span, SourcePoint point, Utils::CopyMovable<std::string>&& message);
		void addError(SourceSpan span, SourcePoint point, Utils::CopyMovable<std::string>&& message);

		auto& getMessages() const { return m_Messages; }
		auto& getIncludedFilenames() const { return m_IncludedFilenames; }

	private:
		void getFunctionDefinitions(ASTNode& root, const std::string& prefix = "");

		void compileFunctionDeclaration(ASTNode& node, const std::string& prefix = "");
		void compileAssignmentExpression(ASTNode& node, CodeBuffer& buffer);
		void compileExpression(ASTNode& node, CodeBuffer& buffer);
		void compileStatements(ASTNode& root, CodeBuffer& buffer, const std::string& prefix = "");

		ETypeIDs getBuiltinTypeID(std::string_view type) const;

	private:
		std::vector<Message> m_Messages;

		std::vector<std::string> m_IncludedFilenames;

		std::vector<FunctionDefinition>  m_FunctionDefinitions;
		std::vector<FunctionDeclaration> m_FunctionDeclarations;
	};
} // namespace Frertex