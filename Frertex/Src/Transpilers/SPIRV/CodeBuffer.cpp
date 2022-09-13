#include "Frertex/Transpilers/SPIRV/CodeBuffer.h"
#include "Frertex/Utils/Profiler.h"

namespace Frertex::Transpilers::SPIRV
{
	void CodeBuffer::OpExtension(std::string_view name)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0001'000A + (literalStringLength(name) << 16));
		pushLiteralString(name);
	}

	void CodeBuffer::OpExtInstImport(std::uint32_t resultID, std::string_view name)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0002'000B + (literalStringLength(name) << 16));
		m_Buffer.emplace_back(resultID);
		pushLiteralString(name);
	}

	void CodeBuffer::OpExtInst(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t set, std::uint32_t instruction, const std::vector<std::uint32_t>& operandIDs)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0005'000C + static_cast<std::uint32_t>(operandIDs.size() << 16));
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(set);
		m_Buffer.emplace_back(instruction);
		m_Buffer.insert(m_Buffer.end(), operandIDs.begin(), operandIDs.end());
	}

	void CodeBuffer::OpMemoryModel(EAddressingMode addressingModel, EMemoryModel memoryModel)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0003'000E);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(addressingModel));
		m_Buffer.emplace_back(static_cast<std::uint32_t>(memoryModel));
	}

	void CodeBuffer::OpEntryPoint(EExecutionModel executionModel, std::uint32_t entryPoint, std::string_view name, const std::vector<std::uint32_t>& interface)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0003'000F + static_cast<std::uint32_t>((literalStringLength(name) + interface.size()) << 16));
		m_Buffer.emplace_back(static_cast<std::uint32_t>(executionModel));
		m_Buffer.emplace_back(entryPoint);
		pushLiteralString(name);
		m_Buffer.insert(m_Buffer.end(), interface.begin(), interface.end());
	}

	void CodeBuffer::OpExecutionMode(std::uint32_t entryPoint, EExecutionMode executionMode, const std::vector<std::uint32_t>& operands)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0003'0010 + static_cast<std::uint32_t>(operands.size() << 16));
		m_Buffer.emplace_back(entryPoint);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(executionMode));
		m_Buffer.insert(m_Buffer.end(), operands.begin(), operands.end());
	}

	void CodeBuffer::OpCapability(ECapability capability)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0002'0011);
		m_Buffer.emplace_back(capability);
	}

	void CodeBuffer::OpExecutionModeId(std::uint32_t entryPoint, EExecutionMode executionMode, const std::vector<std::uint32_t>& operandIDs)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0003'014B + static_cast<std::uint32_t>(operandIDs.size() << 16));
		m_Buffer.emplace_back(entryPoint);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(executionMode));
		m_Buffer.insert(m_Buffer.end(), operandIDs.begin(), operandIDs.end());
	}

	void CodeBuffer::pushLiteralString(std::string_view name)
	{
		m_Buffer.reserve(m_Buffer.size() + literalStringLength(name));
		std::size_t size = (name.size() >> 2) << 2;
		for (std::size_t i = 0; i < size; i += 4)
		{
			std::uint32_t v = name[i] | (name[i + 1] << 8) | (name[i + 2] << 16) | (name[i + 3] << 24);
			m_Buffer.emplace_back(v);
		}
		std::uint32_t last = 0;
		for (std::size_t i = 0, j = size; j < name.size(); ++i, ++j)
			last += name[j] << (i * 8);
		m_Buffer.emplace_back(last);
	}

	std::uint64_t CodeBuffer::literalStringLength(std::string_view name)
	{
		PROFILE_FUNC;

		return 1 + (name.size() >> 2);
	}
} // namespace Frertex::Transpilers::SPIRV