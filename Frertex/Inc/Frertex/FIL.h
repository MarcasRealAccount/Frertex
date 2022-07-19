#pragma once

#include "Utils/Utils.h"

#include <cstdint>

#include <filesystem>
#include <string>
#include <vector>

namespace Frertex
{
	enum class EEntrypointType : std::uint32_t
	{
		None                         = 0x0000,
		VertexShader                 = 0x0001,
		TessellationControlShader    = 0x0002,
		TessellationEvaluationShader = 0x0004,
		GeometryShader               = 0x0008,
		FragmentShader               = 0x0010,
		ComputeShader                = 0x0020,
		RTRayGenShader               = 0x0100,
		RTAnyHitShader               = 0x0200,
		RTClosestHitShader           = 0x0400,
		RTMissShader                 = 0x0800,
		RTIntersectionShader         = 0x1000,
		RTCallableShader             = 0x2000,
		NVTaskShader                 = 0x0040,
		NVMeshShader                 = 0x0080
	};

	struct FunctionDefinition
	{
	public:
		FunctionDefinition(Utils::CopyMovable<std::string>&& name, EEntrypointType type)
		    : m_Name(name.get()), m_Type(type) {}

		std::string     m_Name;
		EEntrypointType m_Type;
	};

	struct LabelInfo
	{
	public:
		std::string   m_Name;
		std::uint64_t m_CodeOffset, m_Length;
	};

	struct LabelRefInfo
	{
	public:
		std::string   m_Name;
		std::uint64_t m_CodeOffset, m_Length;
	};

	struct LineInfo
	{
	public:
		std::uint64_t m_CodeOffset;
		std::uint64_t m_Line, m_File;
	};

	struct FunctionDeclaration : FunctionDefinition
	{
	public:
		FunctionDeclaration(Utils::CopyMovable<std::string>&& name, EEntrypointType type, Utils::CopyMovable<std::vector<LabelInfo>>&& labels, Utils::CopyMovable<std::vector<LabelRefInfo>>&& labelRefs, Utils::CopyMovable<std::vector<std::uint32_t>>&& code, Utils::CopyMovable<std::vector<LineInfo>>&& lines)
		    : FunctionDefinition(name.get(), type), m_Labels(labels.get()), m_LabelRefs(labelRefs.get()), m_Code(code.get()), m_Lines(lines.get()) {}

		std::vector<LabelInfo>     m_Labels;
		std::vector<LabelRefInfo>  m_LabelRefs;
		std::vector<std::uint32_t> m_Code;

		std::vector<LineInfo> m_Lines;
	};

	struct FILEntrypoint
	{
	public:
		EEntrypointType m_Type;
		std::uint64_t   m_LabelID;
	};

	struct FILLabel
	{
	public:
		std::uint64_t m_NameOffset, m_NameLength;
		std::uint64_t m_CodeOffset, m_Length;
	};

	struct FILLabelRef
	{
	public:
		std::uint64_t m_LabelID;
		std::uint64_t m_CodeOffset, m_Length;
	};

	struct FILLine
	{
	public:
		std::uint64_t m_CodeOffset;
		std::uint64_t m_Line;
		std::uint64_t m_FilenameOffset, m_FilenameLength;
	};

	struct FIL
	{
	public:
		std::vector<FILEntrypoint> m_Entrypoints;
		std::vector<std::uint8_t>  m_Strings;
		std::vector<FILLabel>      m_Labels;
		std::vector<FILLabelRef>   m_LabelRefs;
		std::vector<std::uint32_t> m_Code;

		std::vector<FILLine> m_Lines;
	};

	std::vector<std::uint8_t> GetFILBinary(const FIL& fil);

	void WriteFILToFile(const std::filesystem::path& file, const FIL& fil);

	struct CodeBuffer
	{
	public:
		void pushNOP();
		void pushRet();

		void pushLabel();
		void pushLabelRef();
		void pushLine();

		std::vector<std::uint32_t>&& getCode() { return std::move(m_Code); }
		std::vector<LabelInfo>&&     getLabels() { return std::move(m_Labels); }
		std::vector<LabelRefInfo>&&  getLabelRefs() { return std::move(m_LabelRefs); }
		std::vector<LineInfo>&&      getLines() { return std::move(m_Lines); }

	private:
		std::vector<std::uint32_t> m_Code;
		std::vector<LabelInfo>     m_Labels;
		std::vector<LabelRefInfo>  m_LabelRefs;
		std::vector<LineInfo>      m_Lines;
	};
} // namespace Frertex