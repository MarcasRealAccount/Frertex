#pragma once

#include "Enumerations.h"
#include "Frertex/Utils/Buffer.h"

#include <cstdint>

#include <string_view>
#include <vector>

namespace Frertex::Transpilers::SPIRV
{
	struct CodeBuffer
	{
	public:
		void OpExtension(std::string_view name);
		void OpExtInstImport(std::uint32_t resultID, std::string_view name);
		void OpExtInst(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t set, std::uint32_t instruction, const std::vector<std::uint32_t>& operandIDs);

		void OpMemoryModel(EAddressingMode addressingModel, EMemoryModel memoryModel);
		void OpEntryPoint(EExecutionModel executionModel, std::uint32_t entryPoint, std::string_view name, const std::vector<std::uint32_t>& interface);
		void OpExecutionMode(std::uint32_t entryPoint, EExecutionMode executionMode, const std::vector<std::uint32_t>& operands);
		void OpCapability(ECapability capability);
		void OpExecutionModeId(std::uint32_t entryPoint, EExecutionMode executionMode, const std::vector<std::uint32_t>& operandIDs);

		auto                       begin() { return m_Buffer.begin(); }
		auto                       end() { return m_Buffer.end(); }
		auto                       begin() const { return m_Buffer.begin(); }
		auto                       end() const { return m_Buffer.end(); }
		auto                       cbegin() const { return m_Buffer.cbegin(); }
		auto                       cend() const { return m_Buffer.cend(); }
		auto                       rbegin() { return m_Buffer.rbegin(); }
		auto                       rend() { return m_Buffer.rend(); }
		auto                       rbegin() const { return m_Buffer.rbegin(); }
		auto                       rend() const { return m_Buffer.rend(); }
		auto                       crbegin() const { return m_Buffer.crbegin(); }
		auto                       crend() const { return m_Buffer.crend(); }
		std::vector<std::uint32_t> get() { return std::move(m_Buffer); }

	private:
		void          pushLiteralString(std::string_view name);
		std::uint64_t literalStringLength(std::string_view name);

	private:
		std::vector<std::uint32_t> m_Buffer;
	};
} // namespace Frertex::Transpilers::SPIRV