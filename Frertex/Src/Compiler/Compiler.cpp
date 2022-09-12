#include "Frertex/Compiler/Compiler.h"
#include "Frertex/Parser/Literal.h"
#include "Frertex/Utils/Profiler.h"

#include <fmt/format.h>

namespace Frertex::Compiler
{
	State::State(Source::Sources* sources, Message::MessageHandler messageHandler, Utils::CopyMovable<std::vector<Tokenizer::Token>>&& tokens)
	    : m_Sources(sources), m_MessageHandler(messageHandler), m_Tokens(tokens.get()) {}

	FIL::Binary State::compile(Utils::CopyMovable<AST::Node>&& ast)
	{
		PROFILE_FUNC;

		m_AST = ast.get();
		m_FunctionDefinitions.clear();
		m_FunctionDeclarations.clear();
		m_Types.clear();
		m_Variables.clear();
		m_Scopes.clear();

		pushScope();

		// Get definitions
		getFunctionDefinitions(&m_AST);

		// Compile
		for (auto& definition : m_EntrypointDefinitions)
			compileEntrypointDeclaration(definition, definition.m_Root, definition.m_Node);
		for (auto& definition : m_FunctionDefinitions)
			compileFunctionDeclaration(definition, definition.m_Root, definition.m_Node);

		// Link
		FIL::Binary binary;

		std::unordered_map<std::string, std::pair<std::uint64_t, std::uint64_t>> stringLUT;

		auto EnsureString = [&stringLUT, &binary](std::string_view str) -> std::pair<std::uint64_t, std::uint64_t>
		{
			auto itr = std::find_if(stringLUT.begin(),
			                        stringLUT.end(),
			                        [str](const std::pair<std::string, std::pair<std::uint64_t, std::uint64_t>>& a) -> bool
			                        {
				                        return a.first == str;
			                        });
			if (itr == stringLUT.end())
			{
				itr = stringLUT.insert_or_assign(std::string { str }, std::pair<std::uint64_t, std::uint64_t> { binary.m_Strings.size(), str.size() }).first;
				binary.m_Strings.insert(binary.m_Strings.end(), str.begin(), str.end());
			}

			return itr->second;
		};

		struct FunctionLUTEntry
		{
		public:
			std::uint64_t m_CodeOffset, m_CodeLength;
			std::uint64_t m_NameOffset, m_NameLength;
		};

		std::vector<FunctionLUTEntry> entrypointLUT(m_EntrypointDeclarations.size());
		std::vector<FunctionLUTEntry> functionLUT(m_FunctionDeclarations.size());
		for (std::size_t i = 0; i < m_EntrypointDeclarations.size(); ++i)
		{
			auto& entrypoint = m_EntrypointDeclarations[i];

			auto name = EnsureString(entrypoint.m_FullName);

			std::uint64_t codeStart  = static_cast<std::uint64_t>(binary.m_Code.size());
			std::uint64_t codeLength = static_cast<std::uint64_t>(entrypoint.m_Code.size());
			binary.m_Code.insert(binary.m_Code.end(), entrypoint.m_Code.begin(), entrypoint.m_Code.end());

			std::vector<FIL::EntrypointParameter> inputs;
			std::vector<FIL::EntrypointParameter> outputs;
			for (auto& parameter : entrypoint.m_Parameters)
			{
				FIL::ETypeQualifier qualifier = FIL::ETypeQualifier::None;
				for (auto& q : parameter.m_Typename.m_Qualifiers)
				{
					if (qualifier == FIL::ETypeQualifier::None)
						qualifier = q;
					else
						qualifier = static_cast<FIL::ETypeQualifier>(static_cast<std::uint32_t>(qualifier) | static_cast<std::uint32_t>(q));
				}

				if (qualifier == FIL::ETypeQualifier::In)
				{
					if (parameter.m_BuiltinType != FIL::ETypeID::Void)
						inputs.emplace_back(parameter.m_Location, static_cast<std::uint64_t>(parameter.m_BuiltinType));
					else
						inputs.emplace_back(parameter.m_Location, static_cast<std::uint64_t>(parameter.m_Typename.m_Typename.m_TypeID));
				}
				else if (qualifier == FIL::ETypeQualifier::Out)
				{
					if (parameter.m_BuiltinType != FIL::ETypeID::Void)
						outputs.emplace_back(parameter.m_Location, static_cast<std::uint64_t>(parameter.m_BuiltinType));
					else
						outputs.emplace_back(parameter.m_Location, static_cast<std::uint64_t>(parameter.m_Typename.m_Typename.m_TypeID));
				}
				else
				{
					reportError(0, 0, std::vector<Source::SourceSpan> {}, {}, fmt::format("Type qualifier '{}' not supported in entrypoints", FIL::TypeQualifierToString(qualifier)));
					continue;
				}
			}

			entrypointLUT[i] = { codeStart, codeLength, name.first, name.second };
			binary.m_Entrypoints.emplace_back(entrypoint.m_Type, codeStart, codeLength, name.first, name.second, std::move(inputs), std::move(outputs));
		}
		for (std::size_t i = 0; i < m_FunctionDeclarations.size(); ++i)
		{
			auto& function = m_FunctionDeclarations[i];

			auto name = EnsureString(function.m_FullName);

			std::uint64_t codeStart  = static_cast<std::uint64_t>(binary.m_Code.size());
			std::uint64_t codeLength = static_cast<std::uint64_t>(function.m_Code.size());
			binary.m_Code.insert(binary.m_Code.end(), function.m_Code.begin(), function.m_Code.end());

			std::vector<FIL::FunctionParameter> parameters(function.m_Parameters.size());
			for (auto& parameter : function.m_Parameters)
				parameters.emplace_back(parameter.m_Typename.m_Qualifiers, static_cast<std::uint64_t>(parameter.m_Typename.m_Typename.m_TypeID));

			functionLUT[i] = { codeStart, codeLength, name.first, name.second };
			binary.m_Functions.emplace_back(codeStart, codeLength, name.first, name.second, static_cast<std::uint64_t>(function.m_ReturnType.m_TypeID), std::move(parameters));
		}

		return binary;
	}

	void State::getFunctionDefinitions(AST::Node* root)
	{
		PROFILE_FUNC;

		for (auto& node : root->getChildren())
		{
			switch (node.getType())
			{
			case AST::EType::CompoundStatement:
				getFunctionDefinitions(&node);
				break;
			case AST::EType::FunctionDeclaration:
			{
				auto                 attributes = node.getChild(0);
				FIL::EEntrypointType type       = FIL::EEntrypointType::None;
				for (auto& attribute : attributes->getChildren())
				{
					if (type == FIL::EEntrypointType::None)
					{
						type = FIL::StringToEntrypointType(attribute.getToken().getView(m_Sources));
						if (type == FIL::EEntrypointType::None)
							reportWarning(attribute.getToken().m_Index,
							              attribute.getToken().m_Index,
							              { attribute.getToken().getSpan(m_Sources) },
							              attribute.getToken().getStart(m_Sources),
							              "Attribute unused");
					}
					else
						reportWarning(attribute.getToken().m_Index,
						              attribute.getToken().m_Index,
						              { attribute.getToken().getSpan(m_Sources) },
						              attribute.getToken().getStart(m_Sources),
						              "Attribute unused");
				}

				auto&       currentNamespace     = m_Scopes.back().m_CurrentNamespace;
				auto        functionCompoundNode = node.getChild(4);
				std::string newNamespace         = currentNamespace.empty() ? std::string {} : currentNamespace + "::";
				if (type != FIL::EEntrypointType::None)
				{
					auto& entrypoint = m_EntrypointDefinitions.emplace_back(currentNamespace, node.getChild(2)->getToken().getView(m_Sources), type, functionCompoundNode, &node);
					newNamespace += entrypoint.m_Name;
				}
				else
				{
					auto& function = m_FunctionDefinitions.emplace_back(currentNamespace, node.getChild(2)->getToken().getView(m_Sources), functionCompoundNode, &node);
					newNamespace += function.m_Name;
				}

				pushScope();
				m_Scopes.back().m_CurrentNamespace = newNamespace;

				getFunctionDefinitions(functionCompoundNode);

				popScope();
				break;
			}
			default:
				break;
			}
		}
	}

	void State::compileEntrypointDeclaration(EntrypointDefinition& definition, AST::Node* root, AST::Node* node)
	{
		PROFILE_FUNC;

		std::vector<EntrypointParameter> parameters;

		std::uint64_t currentInputLocation  = 0;
		std::uint64_t currentOutputLocation = 0;
		auto&         arguments             = *node->getChild(3);
		parameters.reserve(arguments.getChildren().size());
		for (auto& argument : arguments.getChildren())
		{
			ParameterTypename compiledTypeName;
			bool              givenLocation = false;
			std::uint64_t     location      = 0;
			FIL::ETypeID      builtinType   = FIL::ETypeID::Void;

			auto& attributes = *argument.getChild(0);
			for (auto& attribute : attributes.getChildren())
			{
				auto& identifier = *attribute.getChild(0);
				auto  str        = identifier.getToken().getView(m_Sources);
				if (!givenLocation && str == "Location")
				{
					auto&                  value = *attribute.getChild(1);
					Parser::IntegerLiteral lit   = Parser::ParseIntegerLiteral(&value, m_Sources);
					if (lit.m_Size == Parser::IntegerLiteral::ESize::None)
					{
						reportError(value.getToken().m_Index, value.getToken().m_Index, { value.getToken().getSpan(m_Sources) }, value.getToken().getStart(m_Sources), "Failed to parse integer literal whilst reading 'Location' attribute");
						continue;
					}
					location      = lit.m_Value;
					givenLocation = true;
				}
				else if (builtinType == FIL::ETypeID::Void)
				{
					builtinType = FIL::StringToBuiltinTypeID(str);
				}
			}

			auto& typeName       = *argument.getChild(1);
			auto& typeQualifiers = *typeName.getChild(0);
			for (auto& typeQualifier : typeQualifiers.getChildren())
			{
				auto str = typeQualifier.getToken().getView(m_Sources);
				if (str == "in")
				{
					if (builtinType == FIL::ETypeID::Void)
					{
						if (!givenLocation)
							location = currentInputLocation++;
						else
							currentInputLocation = location + 1;
					}
					compiledTypeName.m_Qualifiers.emplace_back(FIL::ETypeQualifier::In);
				}
				else if (str == "out")
				{
					if (builtinType == FIL::ETypeID::Void)
					{
						if (!givenLocation)
							location = currentOutputLocation++;
						else
							currentOutputLocation = location + 1;
					}
					compiledTypeName.m_Qualifiers.emplace_back(FIL::ETypeQualifier::Out);
				}
				else if (str == "inout")
				{
					reportWarning(typeQualifier.getToken().m_Index, typeQualifier.getToken().m_Index, { typeQualifier.getToken().getSpan(m_Sources) }, typeQualifier.getToken().getStart(m_Sources), "Entrypoints can only have 'in' and 'out' argument type qualifiers");
					continue;
				}
			}
			compiledTypeName.m_Typename.m_TypeID = getTypeID(typeName.getToken().getView(m_Sources));
			if (builtinType != FIL::ETypeID::Void &&
			    !FIL::TypeIDIsCorrectForBuiltin(compiledTypeName.m_Typename.m_TypeID, builtinType))
			{
				reportError(typeName.getToken().m_Index,
				            typeName.getToken().m_Index,
				            { typeName.getToken().getSpan(m_Sources) },
				            typeName.getToken().getStart(m_Sources),
				            fmt::format("[[{}]] attribute requires an '{} {}' type parameter",
				                        FIL::TypeIDToString(builtinType),
				                        FIL::TypeQualifierToString(FIL::GetBuiltinTypeQualifier(builtinType)),
				                        FIL::TypeIDToString(compiledTypeName.m_Typename.m_TypeID)));
			}

			parameters.emplace_back(compiledTypeName, builtinType, location);
		}

		pushScope();
		m_Scopes.back().m_CurrentNamespace = definition.m_Namespace;

		FIL::CodeBuffer codeBuffer;
		compileFunctionBody(codeBuffer, root);

		popScope();
		m_EntrypointDeclarations.emplace_back(definition.m_Namespace, definition.m_Name, definition.m_Type, std::move(parameters), codeBuffer.get());
	}

	void State::compileFunctionDeclaration(FunctionDefinition& definition, AST::Node* root, AST::Node* node)
	{
		PROFILE_FUNC;

		std::vector<FunctionParameter> parameters;

		auto& arguments = *node->getChild(3);
		parameters.reserve(arguments.getChildren().size());
		for (auto& argument : arguments.getChildren())
		{
			ParameterTypename compiledTypeName;

			//auto& attributes = *argument.getChild(0);
			//for (auto& attribute : attributes.getChildren())
			//{
			//	auto& identifier = *attribute.getChild(0);
			//	auto  str        = identifier.getToken().getView(m_Sources);
			//}

			auto& typeName       = *argument.getChild(1);
			auto& typeQualifiers = *typeName.getChild(0);
			for (auto& typeQualifier : typeQualifiers.getChildren())
			{
				auto str = typeQualifier.getToken().getView(m_Sources);
				if (str == "in")
					compiledTypeName.m_Qualifiers.emplace_back(FIL::ETypeQualifier::In);
				else if (str == "out")
					compiledTypeName.m_Qualifiers.emplace_back(FIL::ETypeQualifier::Out);
				else if (str == "inout")
					compiledTypeName.m_Qualifiers.emplace_back(FIL::ETypeQualifier::InOut);
			}
			compiledTypeName.m_Typename.m_TypeID = getTypeID(typeName.getToken().getView(m_Sources));

			parameters.emplace_back(compiledTypeName);
		}

		auto&    returnType               = *node->getChild(1);
		auto&    returnTypenameIdentifier = *returnType.getChild(0);
		Typename returnTypename;
		returnTypename.m_TypeID = getTypeID(returnTypenameIdentifier.getToken().getView(m_Sources));

		pushScope();
		m_Scopes.back().m_CurrentNamespace = definition.m_Namespace;

		FIL::CodeBuffer codeBuffer;
		compileFunctionBody(codeBuffer, root);

		popScope();
		m_FunctionDeclarations.emplace_back(definition.m_Namespace, definition.m_Name, returnTypename, std::move(parameters), codeBuffer.get());
	}

	void State::compileFunctionBody(FIL::CodeBuffer& codeBuffer, AST::Node* root)
	{
		PROFILE_FUNC;

		if (!compileStatements(codeBuffer, root->getChild(0)))
			codeBuffer.RET();
	}

	bool State::compileStatements(FIL::CodeBuffer& codeBuffer, AST::Node* root)
	{
		PROFILE_FUNC;

		bool blockDead = false;

		for (auto& statement : root->getChildren())
		{
			if (blockDead)
			{
				reportError(statement.getToken().m_Index,
				            statement.getToken().m_Index,
				            { statement.getToken().getSpan(m_Sources) },
				            statement.getToken().getStart(m_Sources),
				            "Unreachable statement");
				break;
			}

			switch (statement.getType())
			{
			case AST::EType::EmptyStatement:
				break;
			case AST::EType::ExpressionStatement:
				compileExpression(codeBuffer, statement.getChild(0));
				break;
			case AST::EType::CompoundStatement:
				blockDead = compileStatements(codeBuffer, statement.getChild(0));
				break;
			case AST::EType::ReturnStatement:
				compileReturnStatement(codeBuffer, &statement);
				blockDead = true;
				break;
			case AST::EType::FunctionDeclaration:
				break;
			default:
				reportError(statement.getToken().m_Index,
				            statement.getToken().m_Index,
				            { statement.getToken().getSpan(m_Sources) },
				            statement.getToken().getStart(m_Sources),
				            "Unexpected statement");
			}
		}

		return blockDead;
	}

	void State::compileReturnStatement(FIL::CodeBuffer& codeBuffer, AST::Node* root)
	{
		PROFILE_FUNC;

		codeBuffer.RET();
	}

	void State::compileExpression(FIL::CodeBuffer& codeBuffer, AST::Node* node)
	{
		PROFILE_FUNC;

		switch (node->getType())
		{
		case AST::EType::AssignmentExpression:
			compileAssignmentExpression(codeBuffer, node);
			break;
		default:
			reportError(node->getToken().m_Index,
			            node->getToken().m_Index,
			            { node->getToken().getSpan(m_Sources) },
			            node->getToken().getStart(m_Sources),
			            "Unexpected expression");
			break;
		}
	}

	void State::compileAssignmentExpression(FIL::CodeBuffer& codeBuffer, AST::Node* node)
	{
		PROFILE_FUNC;
	}

	void State::pushScope()
	{
		PROFILE_FUNC;

		m_Scopes.emplace_back();
	}

	void State::popScope()
	{
		PROFILE_FUNC;

		m_Scopes.pop_back();
	}

	FIL::ETypeID State::getTypeID(std::string_view str)
	{
		PROFILE_FUNC;

		for (auto itr = m_Scopes.rbegin(); itr != m_Scopes.rend(); ++itr)
		{
			auto itr2 = std::find_if(
			    itr->m_TypeIDs.begin(),
			    itr->m_TypeIDs.end(),
			    [str](const std::pair<const std::string, FIL::ETypeID>& a) -> bool
			    {
				    return a.first == str;
			    });
			if (itr2 != itr->m_TypeIDs.end())
				return itr2->second;
		}

		return FIL::StringToTypeID(str);
	}

	void State::reportMessage(Message::EMessageType type, std::size_t startToken, std::size_t endToken, Utils::CopyMovable<std::vector<Source::SourceSpan>>&& spans, Source::SourcePoint point, Utils::CopyMovable<std::string>&& message)
	{
		PROFILE_FUNC;

		m_MessageHandler(m_Sources, m_Tokens, { type, startToken, endToken + 1, spans.get(), point, message.get() });
	}
} // namespace Frertex::Compiler