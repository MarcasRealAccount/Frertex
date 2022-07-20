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
				fil.m_Entrypoints.emplace_back(function.m_Type, labelID);
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

		std::string name = prefix + node.getChild(2)->getToken().m_Str;
		CodeBuffer  code;
		compileStatements(*node.getChild(4), code, name + "::");
		m_FunctionDeclarations.emplace_back(std::move(name), type, code.getLabels(), code.getLabelRefs(), code.getCode(), code.getLines());
	}

	void Compiler::compileAssignmentExpression(ASTNode& node, CodeBuffer& buffer)
	{
		PROFILE_FUNC;

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
} // namespace Frertex
