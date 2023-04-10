#include "Frertex/FIL/FIL.h"
#include "Frertex/Utils/Buffer.h"

namespace Frertex::FIL
{
	EEntrypointType EntrypointTypeFromString(std::string_view str)
	{
		if (str == "VertexShader") return EEntrypointType::VertexShader;
		if (str == "TessellationControlShader") return EEntrypointType::TessellationControlShader;
		if (str == "TessellationEvaluationShader") return EEntrypointType::TessellationEvaluationShader;
		if (str == "GeometryShader") return EEntrypointType::GeometryShader;
		if (str == "FragmentShader") return EEntrypointType::FragmentShader;
		if (str == "ComputeShader") return EEntrypointType::ComputeShader;
		if (str == "RayGenShader") return EEntrypointType::RayGenShader;
		if (str == "AnyHitShader") return EEntrypointType::AnyHitShader;
		if (str == "ClosestHitShader") return EEntrypointType::ClosestHitShader;
		if (str == "MissShader") return EEntrypointType::MissShader;
		if (str == "IntersectionShader") return EEntrypointType::IntersectionShader;
		if (str == "CallableShader") return EEntrypointType::CallableShader;
		if (str == "TaskShader") return EEntrypointType::TaskShader;
		if (str == "MeshShader") return EEntrypointType::MeshShader;
		return EEntrypointType::None;
	}

	ETypeQualifier TypeQualifierFromString(std::string_view str)
	{
		if (str == "in") return ETypeQualifier::In;
		if (str == "out") return ETypeQualifier::Out;
		if (str == "inout") return ETypeQualifier::InOut;
		return ETypeQualifier::None;
	}

	static Binary ParseBinary1(Utils::View<std::uint8_t> data, [[maybe_unused]] std::uint16_t minor, [[maybe_unused]] std::uint8_t patch)
	{
		Binary binary {};

		Utils::ReadBuffer buffer { data };

		binary.Entrypoints.resize(buffer.PopU64());
		binary.Functions.resize(buffer.PopU64());
		binary.Strings.resize(buffer.PopU64());
		binary.Code.resize(buffer.PopU64());

		for (std::size_t i = 0; i < binary.Entrypoints.size(); ++i)
		{
			auto& entrypoint = binary.Entrypoints[i];
			entrypoint.Type  = static_cast<EEntrypointType>(buffer.PopU16());
			buffer.AlignU64(); // Padding
			entrypoint.CodeOffset = buffer.PopU64();
			entrypoint.CodeLength = buffer.PopU64();
			entrypoint.NameOffset = buffer.PopU64();
			entrypoint.NameLength = buffer.PopU64();
			entrypoint.Inputs.resize(buffer.PopU64());
			entrypoint.Outputs.resize(buffer.PopU64());
			for (std::size_t j = 0; j < entrypoint.Inputs.size(); ++j)
			{
				auto& input    = entrypoint.Inputs[j];
				input.Location = buffer.PopU64();
				input.TypeID   = buffer.PopU64();
			}
			for (std::size_t j = 0; j < entrypoint.Outputs.size(); ++j)
			{
				auto& output    = entrypoint.Outputs[j];
				output.Location = buffer.PopU64();
				output.TypeID   = buffer.PopU64();
			}
		}

		for (std::size_t i = 0; i < binary.Functions.size(); ++i)
		{
			auto& function      = binary.Functions[i];
			function.CodeOffset = buffer.PopU64();
			function.CodeLength = buffer.PopU64();
			function.NameOffset = buffer.PopU64();
			function.NameLength = buffer.PopU64();
			function.Parameters.resize(buffer.PopU64());
			for (std::size_t j = 0; j < function.Parameters.size(); ++j)
			{
				auto& parameter     = function.Parameters[j];
				parameter.Qualifier = static_cast<ETypeQualifier>(buffer.PopU8());
				buffer.AlignU64();
				parameter.TypeID = buffer.PopU64();
			}
		}

		buffer.PopU8s(binary.Strings);
		buffer.PopU32s(binary.Code);

		return binary;
	}

	Binary ParseBinary(Utils::View<std::uint8_t> data)
	{
		Utils::ReadBuffer buffer { data };
		if (buffer.PopU32() != 0x0046'494C) // Magic "\0FIL"
			return {};
		auto version = buffer.PopU32();
		switch (version >> 24)
		{
		case 1: return ParseBinary1({ data.begin() + buffer.Offset(), data.end() },
									static_cast<std::uint16_t>(version >> 8),
									static_cast<std::uint8_t>(version));
		default: return {};
		}
	}

	std::size_t WriteBinary(const Binary& binary, std::vector<std::uint8_t>& data)
	{
		Utils::WriteBuffer buffer { data };
		// Header
		buffer.PushU32(0x0046'494C); // Magic "\0FIL"
		buffer.PushU32(0x0100'0000); // Version 1.0.0

		buffer.PushU64(binary.Entrypoints.size());
		buffer.PushU64(binary.Functions.size());
		buffer.PushU64(binary.Strings.size());
		buffer.PushU64(binary.Code.size());

		// Data
		for (auto& entrypoint : binary.Entrypoints)
		{
			buffer.PushU16(static_cast<std::uint16_t>(entrypoint.Type));
			buffer.AlignU64(); // Padding
			buffer.PushU64(entrypoint.CodeOffset);
			buffer.PushU64(entrypoint.CodeLength);
			buffer.PushU64(entrypoint.NameOffset);
			buffer.PushU64(entrypoint.NameLength);
			buffer.PushU64(entrypoint.Inputs.size());
			buffer.PushU64(entrypoint.Outputs.size());
			for (auto& parameter : entrypoint.Inputs)
			{
				buffer.PushU64(parameter.Location);
				buffer.PushU64(parameter.TypeID);
			}
			for (auto& parameter : entrypoint.Outputs)
			{
				buffer.PushU64(parameter.Location);
				buffer.PushU64(parameter.TypeID);
			}
		}

		for (auto& function : binary.Functions)
		{
			buffer.PushU64(function.CodeOffset);
			buffer.PushU64(function.CodeLength);
			buffer.PushU64(function.NameOffset);
			buffer.PushU64(function.NameLength);
			buffer.PushU64(function.Parameters.size());
			for (auto& parameter : function.Parameters)
			{
				buffer.PushU8(static_cast<std::uint8_t>(parameter.Qualifier));
				buffer.AlignU64();
				buffer.PushU64(parameter.TypeID);
			}
		}

		buffer.PushU8s(binary.Strings);
		buffer.PushU32s(binary.Code);

		return buffer.Offset();
	}
} // namespace Frertex::FIL