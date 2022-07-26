#include "Frertex/FIL.h"
#include "Frertex/Utils/Buffer.h"
#include "Frertex/Utils/Profiler.h"

#include <fstream>

namespace Frertex
{
	std::string_view EntrypointTypeToString(EEntrypointType type)
	{
		switch (type)
		{
		case EEntrypointType::None: return "None";
		case EEntrypointType::VertexShader: return "VertexShader";
		case EEntrypointType::TessellationControlShader: return "TessellationControlShader";
		case EEntrypointType::TessellationEvaluationShader: return "TessellationEvaluationShader";
		case EEntrypointType::GeometryShader: return "GeometryShader";
		case EEntrypointType::FragmentShader: return "FragmentShader";
		case EEntrypointType::ComputeShader: return "ComputeShader";
		case EEntrypointType::RTRayGenShader: return "RTRayGenShader";
		case EEntrypointType::RTAnyHitShader: return "RTAnyHitShader";
		case EEntrypointType::RTClosestHitShader: return "RTClosestHitShader";
		case EEntrypointType::RTMissShader: return "RTMissShader";
		case EEntrypointType::RTIntersectionShader: return "RTIntersectionShader";
		case EEntrypointType::RTCallableShader: return "RTCallableShader";
		case EEntrypointType::NVTaskShader: return "NVTaskShader";
		case EEntrypointType::NVMeshShader: return "NVMeshShader";
		default: return "???";
		}
	}

	std::string TypeIDToString(ETypeIDs type)
	{
		if (type == ETypeIDs::Void)
			return "void";

		if (static_cast<std::uint64_t>(type) < 0x8000'0000)
		{
			std::uint64_t rows    = ((static_cast<std::uint64_t>(type) >> 20) & 3) + 1;
			std::uint64_t columns = ((static_cast<std::uint64_t>(type) >> 22) & 3) + 1;

			std::string   str        = "void";
			std::uint64_t baseTypeID = static_cast<std::uint64_t>(type);

			ETypeIDs baseType = static_cast<ETypeIDs>((baseTypeID & 0x7'0000) | ((baseTypeID & 0xFFFF) / (rows * columns)));
			switch (baseType)
			{
			case ETypeIDs::Bool:
				str = "bool";
				break;
			case ETypeIDs::Byte:
				str = "byte";
				break;
			case ETypeIDs::UByte:
				str = "ubyte";
				break;
			case ETypeIDs::Short:
				str = "short";
				break;
			case ETypeIDs::UShort:
				str = "ushort";
				break;
			case ETypeIDs::Int:
				str = "int";
				break;
			case ETypeIDs::UInt:
				str = "uint";
				break;
			case ETypeIDs::Long:
				str = "long";
				break;
			case ETypeIDs::ULong:
				str = "ulong";
				break;
			case ETypeIDs::Half:
				str = "half";
				break;
			case ETypeIDs::Float:
				str = "float";
				break;
			case ETypeIDs::Double:
				str = "double";
				break;
			default: return {};
			}

			if (rows != 1)
				str += std::to_string(rows);
			if (columns != 1)
				str += "x" + std::to_string(columns);
			return str;
		}
		else if (static_cast<std::uint64_t>(type) < 0x8000'0000'0000'0000)
		{
			switch (type)
			{
			case ETypeIDs::BuiltinPosition: return "BuiltinPosition";
			case ETypeIDs::BuiltinPointSize: return "BuiltinPointSize";
			default: return {};
			}
		}
		else
		{
			return {};
		}
	}

	bool TypeIDIsUserDefined(ETypeIDs type)
	{
		return static_cast<std::uint64_t>(type) >= 0x8000'0000'0000'0000;
	}

	bool TypeIDIsBuiltIn(ETypeIDs type)
	{
		return static_cast<std::uint64_t>(type) >= 0x8000'0000 &&
		       static_cast<std::uint64_t>(type) < 0x8000'0000'0000'0000;
	}

	ETypeIDs TypeIDGetBase(ETypeIDs type)
	{
		std::uint64_t typeID = static_cast<std::uint64_t>(type);
		if (typeID >= 0x8000'0000)
			return type;

		std::uint64_t rows    = (typeID >> 20) & 0b11;
		std::uint64_t columns = (typeID >> 22) & 0b11;
		if (!columns)
		{
			if (!rows)
			{
				// Base type is itself
				return type;
			}
			else
			{
				// Base type is a scalar
				return static_cast<ETypeIDs>(((typeID & 0xFFFF) / (rows + 1)) | (typeID & ~0xF0'FFFF));
			}
		}
		else
		{
			if (!rows)
			{
				// Base type is a scalar
				return static_cast<ETypeIDs>(((typeID & 0xFFFF) / (columns + 1)) | (typeID & ~0xF0'FFFF));
			}
			else
			{
				// Base type is a vector
				return static_cast<ETypeIDs>(((typeID & 0xFFFF) / (columns + 1)) | (typeID & ~0xC0'FFFF));
			}
		}
	}

	std::vector<std::uint8_t> GetFILBinary(const FIL& fil)
	{
		PROFILE_FUNC;

		Utils::Buffer binary;
		binary.pushU32(0x4C49'4600); // Magic "\0FIL"
		binary.pushU32(0x0000'0000); // Version 0.0.0

		binary.pushU64(fil.m_Entrypoints.size());
		for (auto& entrypoint : fil.m_Entrypoints)
		{
			binary.pushU32(static_cast<std::uint32_t>(entrypoint.m_Type));
			binary.pushU32(0); // Reserved
			binary.pushU64(entrypoint.m_LabelID);
			binary.pushU64(entrypoint.m_Inputs.size());
			for (auto& parameter : entrypoint.m_Inputs)
			{
				binary.pushU64(parameter.m_Location);
				binary.pushU64(parameter.m_TypeID);
			}
			binary.pushU64(entrypoint.m_Outputs.size());
			for (auto& parameter : entrypoint.m_Outputs)
			{
				binary.pushU64(parameter.m_Location);
				binary.pushU64(parameter.m_TypeID);
			}
		}

		binary.pushU64(fil.m_Functions.size());
		for (auto& function : fil.m_Functions)
		{
			binary.pushU64(function.m_LabelID);
			binary.pushU64(function.m_ReturnTypeID);
			binary.pushU64(function.m_Parameters.size());
			for (auto& parameter : function.m_Parameters)
			{
				binary.pushU64(parameter.m_Qualifiers.size());
				for (auto qualifier : parameter.m_Qualifiers)
					binary.pushU32(static_cast<std::uint32_t>(qualifier));
				if ((parameter.m_Qualifiers.size() & 1) == 1)
					binary.pushU32(0); // Padding
				binary.pushU64(parameter.m_TypeID);
			}
		}

		binary.pushU64(fil.m_Strings.size());
		binary.pushU8s(fil.m_Strings.begin(), fil.m_Strings.end());

		binary.pushU64(fil.m_Types.size());
		for ([[maybe_unused]] auto& type : fil.m_Types)
		{
			// TODO(MarcasRealAccount): Implement FIL types
		}

		binary.pushU64(fil.m_Labels.size());
		for (auto& label : fil.m_Labels)
		{
			binary.pushU64(label.m_NameOffset);
			binary.pushU64(label.m_NameLength);
			binary.pushU64(label.m_CodeOffset);
			binary.pushU64(label.m_Length);
		}

		binary.pushU64(fil.m_LabelRefs.size());
		for (auto& ref : fil.m_LabelRefs)
		{
			binary.pushU64(ref.m_LabelID);
			binary.pushU64(ref.m_CodeOffset);
			binary.pushU64(ref.m_Length);
		}

		binary.pushU64(fil.m_Code.size());
		binary.pushU32s(fil.m_Code.begin(), fil.m_Code.end());

		binary.pushU64(fil.m_Lines.size());
		for (auto& line : fil.m_Lines)
		{
			binary.pushU64(line.m_CodeOffset);
			binary.pushU64(line.m_Line);
			binary.pushU64(line.m_FilenameOffset);
			binary.pushU64(line.m_FilenameLength);
		}

		return binary.getBuffer();
	}

	void WriteFILToFile(const std::filesystem::path& file, const FIL& fil)
	{
		PROFILE_FUNC;

		auto binary = GetFILBinary(fil);

		std::ofstream stream { file, std::ios::binary };
		if (stream)
		{
			stream.write(reinterpret_cast<const char*>(binary.data()), binary.size());
			stream.close();
		}
	}

	void CodeBuffer::pushNOP()
	{
		PROFILE_FUNC;

		m_Code.emplace_back(0x0001'0000);
	}

	void CodeBuffer::pushRet()
	{
		PROFILE_FUNC;

		m_Code.emplace_back(0x0001'0001);
	}

	void CodeBuffer::pushLabel()
	{
		PROFILE_FUNC;
	}

	void CodeBuffer::pushLabelRef()
	{
		PROFILE_FUNC;
	}

	void CodeBuffer::pushLine()
	{
		PROFILE_FUNC;
	}
} // namespace Frertex