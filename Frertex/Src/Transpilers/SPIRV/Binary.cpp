#include "Frertex/Transpilers/SPIRV/Binary.h"
#include "Frertex/Transpilers/SPIRV/CodeBuffer.h"
#include "Frertex/Utils/Buffer.h"
#include "Frertex/Utils/Profiler.h"

#include <fstream>

namespace Frertex::Transpilers::SPIRV
{
	std::vector<std::uint8_t> Binary::toBinary() const
	{
		PROFILE_FUNC;

		Utils::Buffer buffer;

		buffer.pushU32(0x07230203); // Magic number
		buffer.pushU32(0x00010600); // Version 1.6
		buffer.pushU32(0x00000000); // TODO(MarcasRealAccount): Register magic number https://github.com/KhronosGroup/SPIRV-Headers
		buffer.pushU32(m_IDBound);
		buffer.pushU32(0x00000000); // Reserved

		CodeBuffer headerCode;
		for (auto capability : m_Capabilities)
			headerCode.OpCapability(capability);

		for (auto& extension : m_Extensions)
			headerCode.OpExtension(extension);

		for (auto& instructions : m_ExtInstImports)
			headerCode.OpExtInstImport(instructions.first, instructions.second);

		headerCode.OpMemoryModel(m_AddressingMode, m_MemoryModel);
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