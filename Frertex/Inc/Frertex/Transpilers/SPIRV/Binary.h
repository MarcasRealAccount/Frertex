#pragma once

#include "CodeBuffer.h"
#include "Enumerations.h"

#include <filesystem>
#include <unordered_map>
#include <vector>

namespace Frertex::Transpilers::SPIRV
{
	struct Binary
	{
	public:
		void requireExtension(std::string_view extension) { m_Extensions.emplace_back(std::string { extension }); }
		void importInstructions(std::string_view instructions) { m_ExtInstImports.emplace_back(reserveID(instructions), std::string { instructions }); }
		void requireCapability(ECapability capability)
		{
			m_Capabilities.emplace_back(capability);
			removeImplicitCapabilities(1);
		}
		void requireCapabilities(const CodeBuffer& codeBuffer);
		void removeImplicitCapabilities(std::size_t addedCapabilities = ~0ULL);

		std::vector<std::uint8_t> toBinary() const;

		void fromBinary(std::vector<std::uint8_t>& rawBinary);

		std::uint32_t reserveID(std::string_view name)
		{
			if (name.empty())
				return m_IDBound++;

			auto itr = std::find_if(m_NamedIDs.begin(),
			                        m_NamedIDs.end(),
			                        [name](const std::pair<std::string, std::uint32_t>& a) -> bool
			                        {
				                        return a.first == name;
			                        });
			if (itr == m_NamedIDs.end())
				itr = m_NamedIDs.insert_or_assign(std::string { name }, m_IDBound++).first;
			return itr->second;
		}

	public:
		std::vector<std::string>                           m_Extensions;
		std::vector<std::pair<std::uint32_t, std::string>> m_ExtInstImports;
		std::vector<ECapability>                           m_Capabilities;

		EAddressingMode m_AddressingMode = EAddressingMode::Logical;
		EMemoryModel    m_MemoryModel    = EMemoryModel::Vulkan;

		std::uint32_t m_IDBound = 1;

		std::unordered_map<std::string, std::uint32_t> m_NamedIDs;
	};

	void WriteToFile(const Binary& binary, const std::filesystem::path& filepath);
	void ReadFromFile(Binary& binary, const std::filesystem::path& filepath);
} // namespace Frertex::Transpilers::SPIRV