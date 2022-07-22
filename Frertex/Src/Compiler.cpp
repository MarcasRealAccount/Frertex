#include "Frertex/Compiler.h"
#include "Frertex/Utils/Profiler.h"

#include <cstring>

#include <algorithm>
#include <set>

namespace Frertex
{
	Compiler::Compiler(Utils::CopyMovable<std::vector<std::string>>&& includedFilenames)
	    : m_IncludedFilenames(includedFilenames.get()) {}

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
		std::set<std::uint64_t> usedFilenames;
		for (auto& function : m_FunctionDeclarations)
			for (auto& line : function.m_Lines)
				usedFilenames.insert(line.m_File);

		std::unordered_map<std::uint64_t, std::pair<std::uint64_t, std::uint64_t>> filenameLUT;
		for (auto& file : usedFilenames)
		{
			if (file > m_IncludedFilenames.size())
			{
				filenameLUT.insert({ file, { 0, 0 } });
			}
			else
			{
				auto& filename = m_IncludedFilenames[file];

				std::uint64_t start  = static_cast<std::uint64_t>(fil.m_Strings.size());
				std::uint64_t length = static_cast<std::uint64_t>(filename.size());
				fil.m_Strings.insert(fil.m_Strings.end(), filename.begin(), filename.end());
				filenameLUT.insert({ file, { start, length } });
			}
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
					ETypeIDs typeID = getBuiltinTypeID(parameter.m_Typename);
					if (typeID == ETypeIDs::BuiltinEnd)
					{
						addError({}, {}, "User defined types not supported yet!!!");
						continue;
					}
					inputs.emplace_back(static_cast<std::uint64_t>(typeID));
				}
				else if (qualifier == ETypeQualifier::Out)
				{
					ETypeIDs typeID = getBuiltinTypeID(parameter.m_Typename);
					if (typeID == ETypeIDs::BuiltinEnd)
					{
						addError({}, {}, "User defined types not supported yet!!!");
						continue;
					}
					outputs.emplace_back(static_cast<std::uint64_t>(typeID));
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
				auto lut = filenameLUT[line.m_File];
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

	void Compiler::addWarning(SourceSpan span, SourcePoint point, Utils::CopyMovable<std::string>&& message)
	{
		PROFILE_FUNC;

		m_Messages.emplace_back(EMessageType::Warning, span, point, message.get());
	}

	void Compiler::addError(SourceSpan span, SourcePoint point, Utils::CopyMovable<std::string>&& message)
	{
		PROFILE_FUNC;

		m_Messages.emplace_back(EMessageType::Error, span, point, message.get());
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
					auto& str        = identifier.getToken().m_Str;
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
							addWarning(identifier.getToken().m_Span, identifier.getToken().m_Span.m_Start, "Attribute unused");
					}
					else
					{
						addWarning(identifier.getToken().m_Span, identifier.getToken().m_Span.m_Start, "Attribute unused");
					}
				}

				auto& function = m_FunctionDefinitions.emplace_back(prefix + declaration.getChild(2)->getToken().m_Str, type);
				getFunctionDefinitions(*declaration.getChild(4), function.m_Name + "::");
				break;
			}
			default:
				break;
			}
		}
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
				auto& str        = identifier.getToken().m_Str;
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
			auto& arguments = *node.getChild(3);
			for (auto& argument : arguments.getChildren())
			{
				auto& parameter = parameters.emplace_back(std::vector<ETypeQualifier> {}, 0, "");

				auto& attributes = *argument.getChild(0);
				for (auto& attribute : attributes.getChildren())
				{
					auto& identifier = *attribute.getChild(0);
					auto& str        = identifier.getToken().m_Str;
					if (!parameter.m_BuiltinTypeID)
					{
						if (str == "Position")
							parameter.m_BuiltinTypeID = static_cast<std::uint32_t>(ETypeIDs::BuiltinPosition);
						else if (str == "PointSize")
							parameter.m_BuiltinTypeID = static_cast<std::uint32_t>(ETypeIDs::BuiltinPointSize);
					}
				}

				auto& typeName       = *argument.getChild(1);
				auto& typeQualifiers = *typeName.getChild(0);
				for (auto& typeQualifier : typeQualifiers.getChildren())
				{
					auto& str = typeQualifier.getToken().m_Str;
					if (str == "in")
						parameter.m_Qualifiers.emplace_back(ETypeQualifier::In);
					else if (str == "out")
						parameter.m_Qualifiers.emplace_back(ETypeQualifier::Out);
					else if (str == "inout")
						parameter.m_Qualifiers.emplace_back(ETypeQualifier::InOut);
				}

				parameter.m_Typename = typeName.getChild(1)->getToken().m_Str;
				if (parameter.m_BuiltinTypeID)
				{
					auto builtinTypeID = getBuiltinTypeID(parameter.m_Typename);
					switch (parameter.m_BuiltinTypeID)
					{
					case static_cast<std::uint32_t>(ETypeIDs::BuiltinPosition):
						if (builtinTypeID != ETypeIDs::Float4)
						{
							addError(typeName.getChild(1)->getToken().m_Span, typeName.getChild(1)->getToken().m_Span.m_Start, "[[Position]] attribute requires an 'out float4' type parameter");
						}
						break;
					case static_cast<std::uint32_t>(ETypeIDs::BuiltinPointSize):
						if (builtinTypeID != ETypeIDs::Float)
						{
							addError(typeName.getChild(1)->getToken().m_Span, typeName.getChild(1)->getToken().m_Span.m_Start, "[[PointSize]] attribute requires an 'out float' type parameter");
						}
						break;
					}
				}
			}
		}

		std::string name = prefix + node.getChild(2)->getToken().m_Str;
		CodeBuffer  code;
		compileStatements(*node.getChild(4), code, name + "::");
		m_FunctionDeclarations.emplace_back(std::move(name), type, std::move(parameters), code.getLabels(), code.getLabelRefs(), code.getCode(), code.getLines());
	}

	void Compiler::compileAssignmentExpression(ASTNode& node, [[maybe_unused]] CodeBuffer& buffer)
	{
		PROFILE_FUNC;

		// TODO(MarcasRealAccount): Implement assignment expressions

		addWarning(node.getToken().m_Span, node.getToken().m_Span.m_Start, "How compile assignment expression???");
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
			addError(node.getToken().m_Span, node.getToken().m_Span.m_Start, "Unexpected expression");
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
				addError(statement.getToken().m_Span, statement.getToken().m_Span.m_Start, "Unexpected statement");
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
