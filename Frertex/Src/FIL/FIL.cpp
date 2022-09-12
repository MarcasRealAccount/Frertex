#include "Frertex/FIL/FIL.h"
#include "Frertex/Utils/Profiler.h"
#include "Frertex/Utils/Version.h"

#include <fstream>

namespace Frertex::FIL
{
	const Entrypoint* Binary::getEntrypoint(std::uint64_t id) const
	{
		PROFILE_FUNC;

		return id < m_Entrypoints.size() ? &m_Entrypoints[id] : nullptr;
	}

	const Function* Binary::getFunction(std::uint64_t id) const
	{
		PROFILE_FUNC;

		return id < m_Functions.size() ? &m_Functions[id] : nullptr;
	}

	std::string_view Binary::getFunctionName(const Function* function) const
	{
		PROFILE_FUNC;

		if (!function)
			return {};

		const char* start = reinterpret_cast<const char*>(m_Strings.data() + function->m_NameOffset);
		return { start, start + function->m_NameLength };
	}

	const Type* Binary::getType(std::uint64_t id) const
	{
		PROFILE_FUNC;

		return id < m_Types.size() ? &m_Types[id] : nullptr;
	}

	std::vector<std::uint8_t> Binary::toBinary() const
	{
		PROFILE_FUNC;

		Utils::Buffer binary;
		binary.pushU32(0x4C49'4600); // Magic "\0FIL"
		binary.pushU32(Utils::Version { 1, 0, 0 });

		binary.pushU64(m_Entrypoints.size());
		for (auto& entrypoint : m_Entrypoints)
		{
			binary.pushU32(static_cast<std::uint32_t>(entrypoint.m_Type));
			binary.align64(); // Padding
			binary.pushU64(entrypoint.m_CodeOffset);
			binary.pushU64(entrypoint.m_CodeLength);
			binary.pushU64(entrypoint.m_NameOffset);
			binary.pushU64(entrypoint.m_NameLength);
			binary.pushU64(static_cast<std::uint64_t>(entrypoint.m_Inputs.size()));
			for (auto& parameter : entrypoint.m_Inputs)
			{
				binary.pushU64(parameter.m_Location);
				binary.pushU64(parameter.m_TypeID);
			}
			binary.pushU64(static_cast<std::uint64_t>(entrypoint.m_Outputs.size()));
			for (auto& parameter : entrypoint.m_Outputs)
			{
				binary.pushU64(parameter.m_Location);
				binary.pushU64(parameter.m_TypeID);
			}
		}

		binary.pushU64(m_Functions.size());
		for (auto& function : m_Functions)
		{
			binary.pushU64(function.m_CodeOffset);
			binary.pushU64(function.m_CodeLength);
			binary.pushU64(function.m_NameOffset);
			binary.pushU64(function.m_NameLength);
			binary.pushU64(function.m_ReturnTypeID);
			binary.pushU64(function.m_Parameters.size());
			for (auto& parameter : function.m_Parameters)
			{
				binary.pushU64(parameter.m_Qualifiers.size());
				for (auto qualifier : parameter.m_Qualifiers)
					binary.pushU32(static_cast<std::uint32_t>(qualifier));
				binary.align64(); // Padding
				binary.pushU64(parameter.m_TypeID);
			}
		}

		binary.pushU64(m_Strings.size());
		binary.pushU8s(m_Strings.begin(), m_Strings.end());

		binary.pushU64(m_Types.size());
		for ([[maybe_unused]] auto& type : m_Types)
		{
			// TODO(MarcasRealAccount): Implement FIL Types
		}

		binary.pushU64(m_Code.size());
		binary.pushU32s(m_Code.begin(), m_Code.end());

		return binary.get();
	}

	void Binary::fromBinary(std::vector<std::uint8_t>& rawBinary)
	{
		PROFILE_FUNC;

		m_Entrypoints.clear();
		m_Functions.clear();
		m_Strings.clear();
		m_Types.clear();
		m_Code.clear();

		Utils::Buffer binary { rawBinary };
		if (binary.popU32() != 0x4C49'4600) // Magic number
			return;
		Utils::Version version { binary.popU32() };
		switch (static_cast<Utils::Version::SubVersions>(version).major)
		{
		case 1:
			fromBinary1(binary);
			break;
		default:
			return;
		}
	}

	void Binary::fromBinary1(Utils::Buffer& binary)
	{
		m_Entrypoints.resize(binary.popU64());
		for (auto& entrypoint : m_Entrypoints)
		{
			entrypoint.m_Type = static_cast<EEntrypointType>(binary.popU32());
			binary.align64(); // Padding
			entrypoint.m_CodeOffset = binary.popU64();
			entrypoint.m_CodeLength = binary.popU64();
			entrypoint.m_NameOffset = binary.popU64();
			entrypoint.m_NameLength = binary.popU64();
			entrypoint.m_Inputs.resize(binary.popU64());
			for (auto& parameter : entrypoint.m_Inputs)
			{
				parameter.m_Location = binary.popU64();
				parameter.m_TypeID   = binary.popU64();
			}
			entrypoint.m_Outputs.resize(binary.popU64());
			for (auto& parameter : entrypoint.m_Outputs)
			{
				parameter.m_Location = binary.popU64();
				parameter.m_TypeID   = binary.popU64();
			}
		}

		m_Functions.resize(binary.popU64());
		for (auto& function : m_Functions)
		{
			function.m_CodeOffset   = binary.popU64();
			function.m_CodeLength   = binary.popU64();
			function.m_NameOffset   = binary.popU64();
			function.m_NameLength   = binary.popU64();
			function.m_ReturnTypeID = binary.popU64();
			function.m_Parameters.resize(binary.popU64());
			for (auto& parameter : function.m_Parameters)
			{
				parameter.m_Qualifiers.resize(binary.popU64());
				for (auto& qualifier : parameter.m_Qualifiers)
					qualifier = static_cast<ETypeQualifier>(binary.popU32());
				binary.align64(); // Padding
				parameter.m_TypeID = binary.popU64();
			}
		}

		m_Strings.resize(binary.popU64());
		binary.popU8s(m_Strings.begin(), m_Strings.end());

		m_Types.resize(binary.popU64());
		for ([[maybe_unused]] auto& types : m_Types)
		{
			// TODO(MarcasRealAccount): Implement FIL Types
		}

		m_Code.resize(binary.popU64());
		binary.popU32s(m_Code.begin(), m_Code.end());
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
} // namespace Frertex::FIL