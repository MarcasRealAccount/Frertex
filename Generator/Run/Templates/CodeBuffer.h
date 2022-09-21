#pragma once

#include "Enumerations.h"

#include <cstdint>

#include <optional>
#include <set>
#include <string_view>
#include <vector>

namespace Frertex::Transpilers::SPIRV
{
	struct CodeBuffer
	{
	public:
		$$FOREACH:INSTRUCTION=INSTRUCTIONS$$
		$$TEMPLATE:"Instruction.h",NAME=INSTRUCTION.NAME,ARGUMENTS=INSTRUCTION.ARGUMENTS,OPCODE=INSTRUCTION.OPCODE$$
		$$END$$

		auto begin() { return m_Buffer.begin(); }
		auto end() { return m_Buffer.end(); }
		auto begin() const { return m_Buffer.begin(); }
		auto end() const { return m_Buffer.end(); }
		auto cbegin() const { return m_Buffer.cbegin(); }
		auto cend() const { return m_Buffer.cend(); }
		auto rbegin() { return m_Buffer.rbegin(); }
		auto rend() { return m_Buffer.rend(); }
		auto rbegin() const { return m_Buffer.rbegin(); }
		auto rend() const { return m_Buffer.rend(); }
		auto crbegin() const { return m_Buffer.crbegin(); }
		auto crend() const { return m_Buffer.crend(); }

		auto& getBuffer() const { return m_Buffer; }
		auto& getCapabilities() const { return m_Capabilities; }
		auto& getExtensions() const { return m_Extensions; }

	private:
		void          pushLiteralString(std::string_view name);
		std::uint64_t literalStringLength(std::string_view name);

		void requireCapability(ECapability capability) { m_Capabilities.insert(capability); }
		void requireExtension(std::string_view extension) { m_Extensions.insert(std::string { extension }); }

	private:
		std::vector<std::uint32_t> m_Buffer;
		std::set<ECapability>      m_Capabilities;
		std::set<std::string>      m_Extensions;
	};
} // namespace Frertex::Transpilers::SPIRV