#include "Frertex/Compiler.h"
#include "Frertex/Utils/Profiler.h"

#include <cstring>

#include <algorithm>
#include <set>

namespace Frertex
{
	Compiler::Compiler(Sources* sources)
	    : m_Sources(sources) {}

	FIL Compiler::compile(Utils::CopyMovable<AST>&& ast)
	{
		PROFILE_FUNC;

		m_FunctionDefinitions.clear();

		AST input = ast.get();
		if (!input.getRoot())
			return {};

		auto& root = *input.getRoot();

		FIL fil {};

		// Get definitions
		getFunctionDefinitions(root);

		// Compile
		for (auto& declaration : root.getChildren())
		{
			switch (declaration.getType())
			{
			case EASTNodeType::FunctionDeclaration:
				compileFunctionDeclaration(declaration);
				break;
			default:
				break;
			}
		}

		// Link
		std::set<std::uint32_t> usedSourceIDs;
		for (auto& function : m_FunctionDeclarations)
		{
			for (auto& line : function.m_Lines)
			{
				usedSourceIDs.insert(line.m_FileID);
				usedSourceIDs.insert(line.m_SourceID);
			}
		}

		std::unordered_map<std::uint32_t, std::pair<std::uint64_t, std::uint64_t>> filenameLUT;
		for (auto id : usedSourceIDs)
		{
			auto name = m_Sources->getSourceName(id);

			std::uint64_t start  = static_cast<std::uint64_t>(fil.m_Strings.size());
			std::uint64_t length = static_cast<std::uint64_t>(name.size());
			fil.m_Strings.insert(fil.m_Strings.end(), name.begin(), name.end());
			filenameLUT.insert({ id, { start, length } });
		}

		struct LabelLUT
		{
			std::uint64_t              m_LabelID;
			std::vector<std::uint64_t> m_RefIDs;
		};

		std::unordered_map<std::string, LabelLUT> labelLUT;

		struct FunctionLUT
		{
			std::uint64_t m_NameOffset, m_NameLength;
			std::uint64_t m_CodeOffset, m_Length;
			std::uint64_t m_LabelID;
		};

		std::vector<FunctionLUT> functionLUT(m_FunctionDeclarations.size());
		for (std::size_t i = 0; i < m_FunctionDeclarations.size(); ++i)
		{
			auto& function = m_FunctionDeclarations[i];

			std::uint64_t nameStart  = static_cast<std::uint64_t>(fil.m_Strings.size());
			std::uint64_t nameLength = static_cast<std::uint64_t>(function.m_Name.size());
			fil.m_Strings.insert(fil.m_Strings.end(), function.m_Name.begin(), function.m_Name.end());

			std::uint64_t codeStart  = static_cast<std::uint64_t>(fil.m_Code.size());
			std::uint64_t codeLength = static_cast<std::uint64_t>(function.m_Code.size());
			fil.m_Code.insert(fil.m_Code.end(), function.m_Code.begin(), function.m_Code.end());

			std::uint64_t labelID = static_cast<std::uint64_t>(fil.m_Labels.size());
			fil.m_Labels.emplace_back(nameStart, nameLength, codeStart, codeLength);

			std::vector<FILEntrypointParameter> inputs;
			std::vector<FILEntrypointParameter> outputs;
			for (auto& parameter : function.m_Parameters)
			{
				ETypeQualifier qualifier = ETypeQualifier::None;
				for (auto& q : parameter.m_Qualifiers)
				{
					if (qualifier == ETypeQualifier::None)
						qualifier = q;
					else
						qualifier = static_cast<ETypeQualifier>(static_cast<std::uint32_t>(qualifier) | static_cast<std::uint32_t>(q));
				}

				if (qualifier == ETypeQualifier::In)
				{
					if (parameter.m_BuiltinTypeID)
					{
						inputs.emplace_back(0, static_cast<std::uint64_t>(parameter.m_BuiltinTypeID));
					}
					else
					{
						ETypeIDs typeID = getBuiltinTypeID(parameter.m_Typename);
						if (typeID >= ETypeIDs::BuiltinEnd)
						{
							addError({}, {}, "User defined types not supported yet!!!");
							continue;
						}
						inputs.emplace_back(parameter.m_Location, static_cast<std::uint64_t>(typeID));
					}
				}
				else if (qualifier == ETypeQualifier::Out)
				{
					if (parameter.m_BuiltinTypeID)
					{
						outputs.emplace_back(0, static_cast<std::uint64_t>(parameter.m_BuiltinTypeID));
					}
					else
					{
						ETypeIDs typeID = getBuiltinTypeID(parameter.m_Typename);
						if (typeID >= ETypeIDs::BuiltinEnd)
						{
							addError({}, {}, "User defined types not supported yet!!!");
							continue;
						}
						outputs.emplace_back(parameter.m_Location, static_cast<std::uint64_t>(typeID));
					}
				}
				else
				{
					addError({}, {}, "Type qualifier not supported in entrypoints");
					continue;
				}
			}

			for (auto& label : function.m_Labels)
			{
				std::uint64_t labelNameStart  = static_cast<std::uint64_t>(fil.m_Strings.size());
				std::uint64_t labelNameLength = static_cast<std::uint64_t>(function.m_Name.size());
				fil.m_Strings.insert(fil.m_Strings.end(), label.m_Name.begin(), label.m_Name.end());

				std::uint64_t id = static_cast<std::uint64_t>(fil.m_Labels.size());
				fil.m_Labels.emplace_back(labelNameStart, labelNameLength, codeStart + label.m_CodeOffset, label.m_Length);

				auto itr = labelLUT.find(label.m_Name);
				if (itr == labelLUT.end())
				{
					labelLUT.insert_or_assign(label.m_Name, LabelLUT { id, {} });
				}
				else
				{
					itr->second.m_LabelID = id;
				}
			}

			for (auto& ref : function.m_LabelRefs)
			{
				std::uint64_t id = static_cast<std::uint64_t>(fil.m_LabelRefs.size());
				fil.m_LabelRefs.emplace_back(0, codeStart + ref.m_CodeOffset, ref.m_Length);

				auto itr = labelLUT.find(ref.m_Name);
				if (itr == labelLUT.end())
				{
					labelLUT.insert_or_assign(ref.m_Name, LabelLUT { 0, { id } });
				}
				else
				{
					itr->second.m_RefIDs.emplace_back(id);
				}
			}

			for (auto& line : function.m_Lines)
			{
				auto lut = filenameLUT[line.m_FileID];
				fil.m_Lines.emplace_back(codeStart + line.m_CodeOffset, line.m_Line, lut.first, lut.second);
			}

			functionLUT[i] = { nameStart, nameLength, codeStart, codeLength, labelID };
			if (function.m_Type != EEntrypointType::None)
				fil.m_Entrypoints.emplace_back(function.m_Type, labelID, std::move(inputs), std::move(outputs));
		}

		// Resolve Label Refs
		for (auto& [name, lut] : labelLUT)
		{
			auto& label = fil.m_Labels[lut.m_LabelID];
			for (auto ref : lut.m_RefIDs)
			{
				auto& labelRef     = fil.m_LabelRefs[ref];
				labelRef.m_LabelID = lut.m_LabelID;

				std::memcpy(fil.m_Code.data() + labelRef.m_CodeOffset, &label.m_CodeOffset, std::min<std::uint64_t>(8, labelRef.m_Length));
			}
		}

		return fil;
	}

	void Compiler::addWarning(std::uint32_t fileID, std::uint32_t sourceID, std::size_t index, std::size_t length, std::size_t point, Utils::CopyMovable<std::string>&& message)
	{
		PROFILE_FUNC;

		auto span             = m_Sources->getSpan(index, length, sourceID);
		auto pt               = m_Sources->getPoint(point, sourceID);
		span.m_Start.m_FileID = fileID;
		span.m_End.m_FileID   = fileID;
		pt.m_FileID           = fileID;
		m_Messages.emplace_back(EMessageType::Warning, span, pt, message.get());
	}

	void Compiler::addError(std::uint32_t fileID, std::uint32_t sourceID, std::size_t index, std::size_t length, std::size_t point, Utils::CopyMovable<std::string>&& message)
	{
		PROFILE_FUNC;

		auto span             = m_Sources->getSpan(index, length, sourceID);
		auto pt               = m_Sources->getPoint(point, sourceID);
		span.m_Start.m_FileID = fileID;
		span.m_End.m_FileID   = fileID;
		pt.m_FileID           = fileID;
		m_Messages.emplace_back(EMessageType::Error, span, pt, message.get());
	}

	void Compiler::getFunctionDefinitions(ASTNode& root, const std::string& prefix)
	{
		PROFILE_FUNC;

		for (auto& declaration : root.getChildren())
		{
			switch (declaration.getType())
			{
			case EASTNodeType::CompoundStatement:
			{
				getFunctionDefinitions(declaration, prefix);
				break;
			}
			case EASTNodeType::FunctionDeclaration:
			{
				EEntrypointType type = EEntrypointType::None;

				auto& attributes = *declaration.getChild(0);
				for (auto& attribute : attributes.getChildren())
				{
					auto& identifier = *attribute.getChild(0);
					auto  str        = identifier.getToken().getView(*m_Sources);
					if (type == EEntrypointType::None)
					{
						if (str == "VertexShader")
							type = EEntrypointType::VertexShader;
						else if (str == "TessellationControlShader")
							type = EEntrypointType::TessellationControlShader;
						else if (str == "TessellationEvaluationShader")
							type = EEntrypointType::TessellationEvaluationShader;
						else if (str == "GeometryShader")
							type = EEntrypointType::GeometryShader;
						else if (str == "FragmentShader")
							type = EEntrypointType::FragmentShader;
						else if (str == "ComputeShader")
							type = EEntrypointType::ComputeShader;
						else if (str == "RTRayGenShader")
							type = EEntrypointType::RTRayGenShader;
						else if (str == "RTAnyHitShader")
							type = EEntrypointType::RTAnyHitShader;
						else if (str == "RTClosestHitShader")
							type = EEntrypointType::RTClosestHitShader;
						else if (str == "RTMissShader")
							type = EEntrypointType::RTMissShader;
						else if (str == "RTIntersectionShader")
							type = EEntrypointType::RTIntersectionShader;
						else if (str == "RTCallableShader")
							type = EEntrypointType::RTCallableShader;
						else if (str == "NVTaskShader")
							type = EEntrypointType::NVTaskShader;
						else if (str == "NVMeshShader")
							type = EEntrypointType::NVMeshShader;
						else
							addWarning(identifier.getToken(), identifier.getToken().m_Index, "Attribute unused");
					}
					else
					{
						addWarning(identifier.getToken(), identifier.getToken().m_Index, "Attribute unused");
					}
				}

				auto& function = m_FunctionDefinitions.emplace_back(prefix + std::string { declaration.getChild(2)->getToken().getView(*m_Sources) }, type);
				getFunctionDefinitions(*declaration.getChild(4), function.m_Name + "::");
				break;
			}
			default:
				break;
			}
		}
	}

	IntegerLiteral Compiler::getIntegerLiteral(ASTNode& node)
	{
		PROFILE_FUNC;

		if (node.getType() != EASTNodeType::IntegerLiteral)
		{
			addError(node.getToken(), node.getToken().m_Index, "Unexpected integer literal");
			return 0U;
		}

		IntegerLiteral literal  = 0U;
		bool           negative = false;
		std::size_t    radix    = 10;

		auto&       token     = node.getToken();
		auto        str       = token.getView(*m_Sources);
		std::size_t offset    = 0;
		std::size_t end       = str.size();
		std::size_t maxDigits = 0;

		switch (token.m_Class)
		{
		case ETokenClass::Integer:
			if (str[0] == '-')
			{
				negative = true;
				++offset;
			}
			else if (str[0] == '+')
			{
				++offset;
			}
			break;
		case ETokenClass::BinaryInteger:
			if (str[0] != '0' ||
			    (str[1] != 'b' && str[1] != 'B'))
			{
				addError(token, token.m_Index, "Expected binary literal to start with '0b' or '0B'");
				return 0U;
			}

			literal.m_Unsigned = true;
			radix              = 2;
			offset += 2;
			break;
		case ETokenClass::OctalInteger:
			if (str[0] != '0' ||
			    (str[1] != 'o' && str[1] != 'O'))
			{
				addError(token, token.m_Index, "Expected octal literal to start with '0o' or '0O'");
				return 0U;
			}

			literal.m_Unsigned = true;
			radix              = 8;
			offset += 2;
			break;
		case ETokenClass::HexInteger:
			if (str[0] != '0' ||
			    (str[1] != 'x' && str[1] != 'X'))
			{
				addError(token, token.m_Index, "Expected hex literal to start with '0x' or '0X'");
				return 0U;
			}

			literal.m_Unsigned = true;
			radix              = 16;
			offset += 2;
			break;
		default:
			break;
		}

		while (end > offset && std::isdigit(str[end - 1]))
		{
			switch (str[end - 1])
			{
			case 'u':
			case 'U':
				if (literal.m_Unsigned)
				{
					std::size_t point = token.m_Index + end - 1;
					addError(token, point, "Integer literal should not contain 'u' or 'U' multiple times");
					return 0U;
				}

				literal.m_Unsigned = true;
				break;
			case 's':
			case 'S':
				if (literal.m_Size != EIntegerLiteralSize::Int)
				{
					std::size_t point = token.m_Index + end - 1;
					addError(token, point, "Integer literal should not contain sized suffixes multiple times");
					return 0U;
				}

				literal.m_Size = EIntegerLiteralSize::Short;
				break;
			case 'l':
			case 'L':
				if (literal.m_Size != EIntegerLiteralSize::Int)
				{
					std::size_t point = token.m_Index + end - 1;
					addError(token, point, "Integer literal should not contain sized suffixes multiple times");
					return 0U;
				}

				literal.m_Size = EIntegerLiteralSize::Long;
				break;
			default:
			{
				std::size_t point = token.m_Index + end - 1;
				addError(token, point, "Integer literal suffix unrecognized");
			}
			}

			--end;
		}

		switch (literal.m_Size)
		{
		case EIntegerLiteralSize::Byte:
			switch (radix)
			{
			case 2:
				maxDigits = 8;
				break;
			case 8:
				maxDigits = 3;
				break;
			case 10:
				maxDigits = 3;
				break;
			case 16:
				maxDigits = 2;
				break;
			}
			break;
		case EIntegerLiteralSize::Short:
			switch (radix)
			{
			case 2:
				maxDigits = 16;
				break;
			case 8:
				maxDigits = 6;
				break;
			case 10:
				maxDigits = 5;
				break;
			case 16:
				maxDigits = 4;
				break;
			}
			break;
		case EIntegerLiteralSize::Int:
			switch (radix)
			{
			case 2:
				maxDigits = 32;
				break;
			case 8:
				maxDigits = 11;
				break;
			case 10:
				maxDigits = 10;
				break;
			case 16:
				maxDigits = 8;
				break;
			}
			break;
		case EIntegerLiteralSize::Long:
			switch (radix)
			{
			case 2:
				maxDigits = 64;
				break;
			case 8:
				maxDigits = 3;
				break;
			case 10: // 18,446,744,073,709,551,616
				maxDigits = negative ? 19 : 20;
				break;
			case 16:
				maxDigits = 16;
				break;
			}
			break;
		}

		if (end - offset > maxDigits)
		{
			std::size_t point = token.m_Index + offset + maxDigits;
			addError(token, point, "Integer literal contains too many digits");
			return 0U;
		}

		std::uint64_t value = 0;

		std::size_t digit = 0;
		for (; offset < end; ++offset)
		{
			if (digit > maxDigits)
			{
				std::size_t point = token.m_Index + offset;
				addError(token, point, "Integer literal contains too many digits");
				return 0U;
			}

			value *= radix;
			std::uint8_t d = 0;
			char         c = str[offset];
			if (c >= '0' && c <= '9')
			{
				d = c - '0';
			}
			else if (d >= 'A' && c <= 'F')
			{
				d = c - 'A' + 10;
			}
			else if (d >= 'a' && c <= 'f')
			{
				d = c - 'a' + 10;
			}
			else
			{
				std::size_t point = token.m_Index + offset;
				addError(token, point, "Integer literal contains unrecognized digit");
				return 0U;
			}

			switch (radix)
			{
			case 2:
				if (d > 1)
				{
					std::size_t point = token.m_Index + offset;
					addError(token, point, "Integer literal contains digit outside current radix 2");
					return 0U;
				}
				break;
			case 8:
				if (d > 7)
				{
					std::size_t point = token.m_Index + offset;
					addError(token, point, "Integer literal contains digit outside current radix 8");
					return 0U;
				}
				break;
			case 10:
				if (d > 10)
				{
					std::size_t point = token.m_Index + offset;
					addError(token, point, "Integer literal contains digit outside current radix 10");
					return 0U;
				}
				break;
			}
			++digit;
		}

		if (negative)
			value = ~value + 1;

		switch (literal.m_Size)
		{
		case EIntegerLiteralSize::Byte:
			literal.m_Byte = static_cast<std::uint8_t>(value);
			break;
		case EIntegerLiteralSize::Short:
			literal.m_Short = static_cast<std::uint16_t>(value);
			break;
		case EIntegerLiteralSize::Int:
			literal.m_Int = static_cast<std::uint32_t>(value);
			break;
		case EIntegerLiteralSize::Long:
			literal.m_Long = static_cast<std::uint64_t>(value);
			break;
		}

		return literal;
	}

	FloatLiteral Compiler::getFloatLiteral([[maybe_unused]] ASTNode& node)
	{
		PROFILE_FUNC;

		return 0.0f;
	}

	void Compiler::compileFunctionDeclaration(ASTNode& node, const std::string& prefix)
	{
		PROFILE_FUNC;

		EEntrypointType type = EEntrypointType::None;

		std::vector<FunctionParameter> parameters;

		{
			auto& attributes = *node.getChild(0);
			for (auto& attribute : attributes.getChildren())
			{
				auto& identifier = *attribute.getChild(0);
				auto  str        = identifier.getToken().getView(*m_Sources);
				if (type == EEntrypointType::None)
				{
					if (str == "VertexShader")
						type = EEntrypointType::VertexShader;
					else if (str == "TessellationControlShader")
						type = EEntrypointType::TessellationControlShader;
					else if (str == "TessellationEvaluationShader")
						type = EEntrypointType::TessellationEvaluationShader;
					else if (str == "GeometryShader")
						type = EEntrypointType::GeometryShader;
					else if (str == "FragmentShader")
						type = EEntrypointType::FragmentShader;
					else if (str == "ComputeShader")
						type = EEntrypointType::ComputeShader;
					else if (str == "RTRayGenShader")
						type = EEntrypointType::RTRayGenShader;
					else if (str == "RTAnyHitShader")
						type = EEntrypointType::RTAnyHitShader;
					else if (str == "RTClosestHitShader")
						type = EEntrypointType::RTClosestHitShader;
					else if (str == "RTMissShader")
						type = EEntrypointType::RTMissShader;
					else if (str == "RTIntersectionShader")
						type = EEntrypointType::RTIntersectionShader;
					else if (str == "RTCallableShader")
						type = EEntrypointType::RTCallableShader;
					else if (str == "NVTaskShader")
						type = EEntrypointType::NVTaskShader;
					else if (str == "NVMeshShader")
						type = EEntrypointType::NVMeshShader;
				}
			}
		}

		{
			std::uint64_t currentLocation = 0;
			auto&         arguments       = *node.getChild(3);
			for (auto& argument : arguments.getChildren())
			{
				auto& parameter = parameters.emplace_back(std::vector<ETypeQualifier> {}, 0, "", currentLocation++);

				bool givenLocation = false;

				auto& attributes = *argument.getChild(0);
				for (auto& attribute : attributes.getChildren())
				{
					auto& identifier = *attribute.getChild(0);
					auto  str        = identifier.getToken().getView(*m_Sources);
					if (!parameter.m_BuiltinTypeID)
					{
						if (str == "Position")
							parameter.m_BuiltinTypeID = static_cast<std::uint32_t>(ETypeIDs::BuiltinPosition);
						else if (str == "PointSize")
							parameter.m_BuiltinTypeID = static_cast<std::uint32_t>(ETypeIDs::BuiltinPointSize);
					}
					else if (!givenLocation && str == "Location")
					{
						auto&          value   = *attribute.getChild(1);
						IntegerLiteral literal = getIntegerLiteral(value);
						currentLocation        = (parameter.m_Location = literal.getULong()) + 1;
						givenLocation          = true;
					}
				}

				auto& typeName       = *argument.getChild(1);
				auto& typeQualifiers = *typeName.getChild(0);
				for (auto& typeQualifier : typeQualifiers.getChildren())
				{
					auto str = typeQualifier.getToken().getView(*m_Sources);
					if (str == "in")
						parameter.m_Qualifiers.emplace_back(ETypeQualifier::In);
					else if (str == "out")
						parameter.m_Qualifiers.emplace_back(ETypeQualifier::Out);
					else if (str == "inout")
						parameter.m_Qualifiers.emplace_back(ETypeQualifier::InOut);
				}

				parameter.m_Typename = typeName.getChild(1)->getToken().getView(*m_Sources);
				if (parameter.m_BuiltinTypeID)
				{
					auto builtinTypeID = getBuiltinTypeID(parameter.m_Typename);
					switch (parameter.m_BuiltinTypeID)
					{
					case static_cast<std::uint32_t>(ETypeIDs::BuiltinPosition):
						if (builtinTypeID != ETypeIDs::Float4)
						{
							auto& typenameToken = typeName.getChild(1)->getToken();
							addError(typenameToken, typenameToken.m_Index, "[[Position]] attribute requires an 'out float4' type parameter");
						}
						break;
					case static_cast<std::uint32_t>(ETypeIDs::BuiltinPointSize):
						if (builtinTypeID != ETypeIDs::Float)
						{
							auto& typenameToken = typeName.getChild(1)->getToken();
							addError(typenameToken, typenameToken.m_Index, "[[PointSize]] attribute requires an 'out float' type parameter");
						}
						break;
					}
				}
			}
		}

		std::string name = prefix + std::string { node.getChild(2)->getToken().getView(*m_Sources) };
		CodeBuffer  code;
		compileStatements(*node.getChild(4), code, name + "::");
		m_FunctionDeclarations.emplace_back(std::move(name), type, std::move(parameters), code.getLabels(), code.getLabelRefs(), code.getCode(), code.getLines());
	}

	void Compiler::compileAssignmentExpression(ASTNode& node, [[maybe_unused]] CodeBuffer& buffer)
	{
		PROFILE_FUNC;

		// TODO(MarcasRealAccount): Implement assignment expressions

		addWarning(node.getToken(), node.getToken().m_Index, "How compile assignment expression???");
	}

	void Compiler::compileExpression(ASTNode& node, CodeBuffer& buffer)
	{
		PROFILE_FUNC;

		switch (node.getType())
		{
		case EASTNodeType::AssignmentExpression:
			compileAssignmentExpression(node, buffer);
			break;
		default:
			addError(node.getToken(), node.getToken().m_Index, "Unexpected expression");
			break;
		}
	}

	void Compiler::compileStatements(ASTNode& root, CodeBuffer& buffer, const std::string& prefix)
	{
		PROFILE_FUNC;

		for (auto& statement : root.getChildren())
		{
			switch (statement.getType())
			{
			case EASTNodeType::ExpressionStatement:
				compileExpression(*statement.getChild(0), buffer);
				break;
			case EASTNodeType::ReturnStatement:
				buffer.pushRet();
				break;
			case EASTNodeType::FunctionDeclaration:
				compileFunctionDeclaration(*statement.getChild(4), prefix);
				break;
			default:
				addError(statement.getToken(), statement.getToken().m_Index, "Unexpected statement");
				break;
			}
		}
	}

	ETypeIDs Compiler::getBuiltinTypeID(std::string_view type) const
	{
		PROFILE_FUNC;

		if (type == "void")
			return ETypeIDs::Void;

		ETypeIDs    baseType = ETypeIDs::Void;
		std::size_t offset   = 0;

		if (type.starts_with("bool"))
		{
			baseType = ETypeIDs::Bool;
			offset += 4;
		}
		else if (type.starts_with("byte"))
		{
			baseType = ETypeIDs::Byte;
			offset += 4;
		}
		else if (type.starts_with("ubyte"))
		{
			baseType = ETypeIDs::UByte;
			offset += 5;
		}
		else if (type.starts_with("short"))
		{
			baseType = ETypeIDs::Short;
			offset += 5;
		}
		else if (type.starts_with("ushort"))
		{
			baseType = ETypeIDs::UShort;
			offset += 6;
		}
		else if (type.starts_with("int"))
		{
			baseType = ETypeIDs::Int;
			offset += 3;
		}
		else if (type.starts_with("uint"))
		{
			baseType = ETypeIDs::UInt;
			offset += 4;
		}
		else if (type.starts_with("long"))
		{
			baseType = ETypeIDs::Long;
			offset += 4;
		}
		else if (type.starts_with("ulong"))
		{
			baseType = ETypeIDs::ULong;
			offset += 5;
		}
		else if (type.starts_with("half"))
		{
			baseType = ETypeIDs::Half;
			offset += 4;
		}
		else if (type.starts_with("float"))
		{
			baseType = ETypeIDs::Float;
			offset += 5;
		}
		else if (type.starts_with("double"))
		{
			baseType = ETypeIDs::Double;
			offset += 6;
		}
		else
		{
			return ETypeIDs::BuiltinEnd;
		}

		std::uint64_t rows = 0, columns = 0;
		if (type.size() - offset > 0 && type[offset] >= '1' && type[offset] <= '4')
		{
			rows = type[offset] - '1';
			++offset;
			if (type.size() - offset > 1 && type[offset] == 'x' && type[offset + 1] >= '1' && type[offset + 2] <= '4')
			{
				columns = type[offset + 1] - '1';
				offset += 2;
			}
		}

		std::uint64_t baseTypeId = static_cast<std::uint64_t>(baseType);

		return static_cast<ETypeIDs>((baseTypeId & ~0xFFFF) | (baseTypeId & 0xFFFF) * (rows + 1) * (columns + 1) | (rows << 20) | (columns << 22));
	}
} // namespace Frertex