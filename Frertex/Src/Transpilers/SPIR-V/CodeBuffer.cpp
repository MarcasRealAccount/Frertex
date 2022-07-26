#include "Frertex/Transpilers/SPIR-V/CodeBuffer.h"
#include "Frertex/Utils/Profiler.h"

namespace Frertex::Transpilers::SPIRV
{
	void CodeBuffer::pushOpSource(std::uint32_t language, std::uint32_t version, std::uint32_t fileID)
	{
		PROFILE_FUNC;

		std::uint32_t wordCount = 0x0003 + !!fileID;
		m_Code.emplace_back(0x0003 + (wordCount << 16));
		m_Code.emplace_back(language);
		m_Code.emplace_back(version);
		if (fileID)
			m_Code.emplace_back(fileID);
	}

	void CodeBuffer::pushOpMemoryModel(EAddressingMode addessingMode, EMemoryModel memoryModel)
	{
		PROFILE_FUNC;

		m_Code.emplace_back(0x0003'000E);
		m_Code.emplace_back(static_cast<std::uint32_t>(addessingMode));
		m_Code.emplace_back(static_cast<std::uint32_t>(memoryModel));
	}

	void CodeBuffer::pushOpEntryPoint(EExecutionModel executionModel, std::uint32_t entrypoint, std::string_view name, const std::vector<std::uint32_t>& interface)
	{
		PROFILE_FUNC;

		std::uint32_t wordCount = static_cast<std::uint32_t>(0x0004 + name.size() / 4 + interface.size());
		m_Code.reserve(m_Code.size() + wordCount);
		m_Code.emplace_back(0x000F + (wordCount << 16));
		m_Code.emplace_back(static_cast<std::uint32_t>(executionModel));
		m_Code.emplace_back(entrypoint);
		for (std::size_t i = 0; i < name.size(); i += 4)
		{
			std::uint32_t value = name[i] |
			                      name[i + 1] << 8 |
			                      name[i + 2] << 16 |
			                      name[i + 3] << 24;
			m_Code.emplace_back(value);
		}
		std::uint32_t residualWord = 0;
		for (std::size_t i = name.size() - ((name.size() + 4) / 4) * 4, j = 0; i < name.size(); ++i, ++j)
			residualWord |= name[i] << (j * 8);
		m_Code.emplace_back(residualWord);
		m_Code.insert(m_Code.end(), interface.begin(), interface.end());
	}

	void CodeBuffer::pushOpCapability(ECapability capability)
	{
		PROFILE_FUNC;

		m_Code.emplace_back(0x0002'0011);
		m_Code.emplace_back(static_cast<std::uint32_t>(capability));
	}

	void CodeBuffer::pushOpTypeVoid(std::uint32_t result)
	{
		PROFILE_FUNC;

		m_Code.emplace_back(0x0002'0013);
		m_Code.emplace_back(result);
	}

	void CodeBuffer::pushOpTypeBool(std::uint32_t result)
	{
		PROFILE_FUNC;

		m_Code.emplace_back(0x0002'0014);
		m_Code.emplace_back(result);
	}

	void CodeBuffer::pushOpTypeInt(std::uint32_t result, std::uint32_t width, std::uint32_t signedness)
	{
		PROFILE_FUNC;

		m_Code.emplace_back(0x0004'0015);
		m_Code.emplace_back(result);
		m_Code.emplace_back(width);
		m_Code.emplace_back(signedness);
	}

	void CodeBuffer::pushOpTypeFloat(std::uint32_t result, std::uint32_t width)
	{
		PROFILE_FUNC;

		m_Code.emplace_back(0x0003'0016);
		m_Code.emplace_back(result);
		m_Code.emplace_back(width);
	}

	void CodeBuffer::pushOpTypeVector(std::uint32_t result, std::uint32_t componentType, std::uint32_t componentCount)
	{
		PROFILE_FUNC;

		m_Code.emplace_back(0x0004'0017);
		m_Code.emplace_back(result);
		m_Code.emplace_back(componentType);
		m_Code.emplace_back(componentCount);
	}

	void CodeBuffer::pushOpTypeMatrix(std::uint32_t result, std::uint32_t columnType, std::uint32_t columnCount)
	{
		PROFILE_FUNC;

		m_Code.emplace_back(0x0004'0018);
		m_Code.emplace_back(result);
		m_Code.emplace_back(columnType);
		m_Code.emplace_back(columnCount);
	}

	void CodeBuffer::pushOpTypeImage(std::uint32_t result, std::uint32_t sampledType, EDim dim, std::uint32_t depth, std::uint32_t arrayed, std::uint32_t ms, std::uint32_t sampled, std::uint32_t format, EAccessQualifier accessQualifier)
	{
		PROFILE_FUNC;

		m_Code.emplace_back(0x0009'0019 + ((accessQualifier != EAccessQualifier::None) << 16));
		m_Code.emplace_back(result);
		m_Code.emplace_back(sampledType);
		m_Code.emplace_back(static_cast<std::uint32_t>(dim));
		m_Code.emplace_back(depth);
		m_Code.emplace_back(arrayed);
		m_Code.emplace_back(ms);
		m_Code.emplace_back(sampled);
		m_Code.emplace_back(format);
		if (accessQualifier != EAccessQualifier::None)
			m_Code.emplace_back(static_cast<std::uint32_t>(accessQualifier));
	}

	void CodeBuffer::pushOpTypeSampler(std::uint32_t result)
	{
		PROFILE_FUNC;

		m_Code.emplace_back(0x0002'001A);
		m_Code.emplace_back(result);
	}

	void CodeBuffer::pushOpTypeSampledImage(std::uint32_t result, std::uint32_t imageType)
	{
		PROFILE_FUNC;

		m_Code.emplace_back(0x0003'001B);
		m_Code.emplace_back(result);
		m_Code.emplace_back(imageType);
	}

	void CodeBuffer::pushOpTypeArray(std::uint32_t result, std::uint32_t elementType, std::uint32_t length)
	{
		PROFILE_FUNC;

		m_Code.emplace_back(0x0004'001C);
		m_Code.emplace_back(result);
		m_Code.emplace_back(elementType);
		m_Code.emplace_back(length);
	}

	void CodeBuffer::pushOpTypeRuntimeArray(std::uint32_t result, std::uint32_t elementType)
	{
		PROFILE_FUNC;

		m_Code.emplace_back(0x0003'001D);
		m_Code.emplace_back(result);
		m_Code.emplace_back(elementType);
	}

	void CodeBuffer::pushOpTypeStruct(std::uint32_t result, const std::vector<std::uint32_t>& memberTypes)
	{
		PROFILE_FUNC;

		m_Code.emplace_back(static_cast<std::uint32_t>(0x0002'001E + (memberTypes.size() << 16)));
		m_Code.emplace_back(result);
		m_Code.insert(m_Code.end(), memberTypes.begin(), memberTypes.end());
	}

	void CodeBuffer::pushOpTypeOpaque(std::uint32_t result, std::string_view name)
	{
		PROFILE_FUNC;

		std::uint32_t wordCount = static_cast<std::uint32_t>(0x0003 + name.size() / 4);
		m_Code.reserve(m_Code.size() + wordCount);
		m_Code.emplace_back(0x001F | (wordCount << 16));
		m_Code.emplace_back(result);
		for (std::size_t i = 0; i < name.size(); i += 4)
		{
			std::uint32_t value = name[i] |
			                      name[i + 1] << 8 |
			                      name[i + 2] << 16 |
			                      name[i + 3] << 24;
			m_Code.emplace_back(value);
		}
		std::uint32_t residualWord = 0;
		for (std::size_t i = name.size() - (name.size() / 4) * 4, j = 0; i < name.size(); ++i, ++j)
			residualWord |= name[i] << (j * 8);
		m_Code.emplace_back(residualWord);
	}

	void CodeBuffer::pushOpTypePointer(std::uint32_t result, EStorageClass storageClass, std::uint32_t type)
	{
		PROFILE_FUNC;

		m_Code.emplace_back(0x0004'0020);
		m_Code.emplace_back(result);
		m_Code.emplace_back(static_cast<std::uint32_t>(storageClass));
		m_Code.emplace_back(type);
	}

	void CodeBuffer::pushOpTypeFunction(std::uint32_t result, std::uint32_t returnType, const std::vector<std::uint32_t>& parameterTypes)
	{
		PROFILE_FUNC;

		m_Code.emplace_back(0x0003'0021 + (parameterTypes.size() << 16));
		m_Code.emplace_back(result);
		m_Code.emplace_back(returnType);
		m_Code.insert(m_Code.end(), parameterTypes.begin(), parameterTypes.end());
	}

	void CodeBuffer::pushOpTypeEvent(std::uint32_t result)
	{
		PROFILE_FUNC;

		m_Code.emplace_back(0x0002'0022);
		m_Code.emplace_back(result);
	}

	void CodeBuffer::pushOpTypeDeviceEvent(std::uint32_t result)
	{
		PROFILE_FUNC;

		m_Code.emplace_back(0x0002'0023);
		m_Code.emplace_back(result);
	}

	void CodeBuffer::pushOpFunction(std::uint32_t returnType, std::uint32_t result, EFunctionControl functionControl, std::uint32_t functionType)
	{
		PROFILE_FUNC;

		m_Code.emplace_back(0x0005'0036);
		m_Code.emplace_back(returnType);
		m_Code.emplace_back(result);
		m_Code.emplace_back(static_cast<std::uint32_t>(functionControl));
		m_Code.emplace_back(functionType);
	}

	void CodeBuffer::pushOpFunctionParameter(std::uint32_t resultType, std::uint32_t result)
	{
		PROFILE_FUNC;

		m_Code.emplace_back(0x0003'0037);
		m_Code.emplace_back(resultType);
		m_Code.emplace_back(result);
	}

	void CodeBuffer::pushOpFunctionEnd()
	{
		PROFILE_FUNC;

		m_Code.emplace_back(0x0001'0038);
	}

	void CodeBuffer::pushOpVariable(std::uint32_t resultType, std::uint32_t result, EStorageClass storageClass, std::uint32_t initializer)
	{
		PROFILE_FUNC;

		m_Code.emplace_back(0x0004'003B + (!!initializer << 16));
		m_Code.emplace_back(resultType);
		m_Code.emplace_back(result);
		m_Code.emplace_back(static_cast<std::uint32_t>(storageClass));
		if (initializer)
			m_Code.emplace_back(initializer);
	}

	void CodeBuffer::pushOpDecorate(std::uint32_t target, EDecoration decoration, const std::vector<std::uint32_t>& literals)
	{
		PROFILE_FUNC;

		m_Code.emplace_back(static_cast<std::uint32_t>(0x0003'0047 + (literals.size() << 16)));
		m_Code.emplace_back(target);
		m_Code.emplace_back(static_cast<std::uint32_t>(decoration));
		m_Code.insert(m_Code.end(), literals.begin(), literals.end());
	}

	void CodeBuffer::pushOpLabel(std::uint32_t result)
	{
		PROFILE_FUNC;

		m_Code.emplace_back(0x0002'00F8);
		m_Code.emplace_back(result);
	}

	void CodeBuffer::pushOpReturn()
	{
		PROFILE_FUNC;

		m_Code.emplace_back(0x0001'00FD);
	}

	void CodeBuffer::pushOpReturnValue(std::uint32_t value)
	{
		PROFILE_FUNC;

		m_Code.emplace_back(0x0002'00FE);
		m_Code.emplace_back(value);
	}

	void CodeBuffer::pushOpUnreachable()
	{
		PROFILE_FUNC;

		m_Code.emplace_back(0x0001'00FF);
	}

	void CodeBuffer::pushCodeBuffer(const CodeBuffer& buffer)
	{
		PROFILE_FUNC;

		m_Code.insert(m_Code.end(), buffer.m_Code.begin(), buffer.m_Code.end());
	}
} // namespace Frertex::Transpilers::SPIRV