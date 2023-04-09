#include "Frertex/Transpilers/SPIRV/Binary.h"
#include "Frertex/Utils/Buffer.h"
#include "Frertex/Utils/Profiler.h"

#include <fstream>

namespace Frertex::Transpilers::SPIRV
{
	void Binary::require(const CodeBuffer& codeBuffer)
	{
		for (auto capability : codeBuffer.getCapabilities())
			m_Capabilities.insert(capability);

		for (auto& extension : codeBuffer.getExtensions())
			m_Extensions.insert(extension);
	}

	std::vector<std::uint8_t> Binary::toBinary() const
	{
		PROFILE_FUNC;

		Utils::Buffer buffer;

		buffer.pushU32(0x07230203); // Magic number
		buffer.pushU32(0x00010600); // Version 1.6
		buffer.pushU32(0x00000000); // TODO(MarcasRealAccount): Register magic number https://github.com/KhronosGroup/SPIRV-Headers
		buffer.pushU32(m_IDBound);
		buffer.pushU32(0x00000000); // Reserved

		std::set<ECapability> implicitCapabilities;
		std::set<std::string> requiredExtensions;
		std::set<ECapability> capabilities = m_Capabilities;

		RequireCapExtAddressingModel(m_AddressingModel, capabilities, requiredExtensions);
		RequireCapExtMemoryModel(m_MemoryModel, capabilities, requiredExtensions);
		for (auto capability : m_Capabilities)
			RequireCapExtCapability(capability, implicitCapabilities, requiredExtensions);

		for (auto implicitCapability : implicitCapabilities)
			capabilities.erase(implicitCapability);
		for (auto& extension : m_Extensions)
			requiredExtensions.insert(extension);

		CodeBuffer headerCode;
		for (auto capability : capabilities)
			headerCode.OpCapability(capability);

		for (auto& extension : requiredExtensions)
			headerCode.OpExtension(extension);

		for (auto& instructions : m_ExtInstImports)
			headerCode.OpExtInstImport(instructions.first, instructions.second);

		headerCode.OpMemoryModel(m_AddressingModel, m_MemoryModel);
		buffer.pushU32s(headerCode.begin(), headerCode.end());

		return buffer.get();
	}

	void Binary::fromBinary(std::vector<std::uint8_t>& rawBinary)
	{
		PROFILE_FUNC;
	}

	void WriteToFile(const Binary& binary, const std::filesystem::path& filepath)
	{
		PROFILE_FUNC;

		auto bin = binary.toBinary();

		std::ofstream stream { filepath, std::ios::binary };
		if (stream)
		{
			stream.write(reinterpret_cast<const char*>(bin.data()), bin.size());
			stream.close();
		}
	}

	void ReadFromFile(Binary& binary, const std::filesystem::path& filepath)
	{
		PROFILE_FUNC;

		std::ifstream stream { filepath, std::ios::ate };
		if (stream)
		{
			std::vector<std::uint8_t> buffer;
			buffer.resize(stream.tellg());
			stream.seekg(0);
			stream.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
			stream.close();
			binary.fromBinary(buffer);
		}
	}
} // namespace Frertex::Transpilers::SPIRV