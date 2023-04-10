#pragma once

#include "Frertex/Utils/View.h"

#include <string_view>
#include <vector>

namespace Frertex::FIL
{
	enum class EEntrypointType : std::uint16_t
	{
		None                         = 0x0000,
		VertexShader                 = 0x0001,
		TessellationControlShader    = 0x0002,
		TessellationEvaluationShader = 0x0004,
		GeometryShader               = 0x0008,
		FragmentShader               = 0x0010,
		ComputeShader                = 0x0020,
		RayGenShader                 = 0x0100,
		AnyHitShader                 = 0x0200,
		ClosestHitShader             = 0x0400,
		MissShader                   = 0x0800,
		IntersectionShader           = 0x1000,
		CallableShader               = 0x2000,
		TaskShader                   = 0x4000,
		MeshShader                   = 0x8000
	};

	enum class ETypeQualifier : std::uint8_t
	{
		None  = 0x00,
		In    = 0x01,
		Out   = 0x02,
		InOut = In | Out
	};

	EEntrypointType EntrypointTypeFromString(std::string_view str);
	ETypeQualifier  TypeQualifierFromString(std::string_view str);

	struct EntrypointParameter
	{
	public:
		std::uint64_t Location;
		std::uint64_t TypeID;
	};

	struct Entrypoint
	{
	public:
		EEntrypointType Type;
		std::uint64_t   CodeOffset, CodeLength;
		std::uint64_t   NameOffset, NameLength;

		std::vector<EntrypointParameter> Inputs;
		std::vector<EntrypointParameter> Outputs;
	};

	struct FunctionParameter
	{
	public:
		ETypeQualifier Qualifier;
		std::uint64_t  TypeID;
	};

	struct Function
	{
	public:
		std::uint64_t CodeOffset, CodeLength;
		std::uint64_t NameOffset, NameLength;

		std::uint64_t                  ReturnTypeID;
		std::vector<FunctionParameter> Parameters;
	};

	struct Binary
	{
	public:
		std::vector<Entrypoint>    Entrypoints;
		std::vector<Function>      Functions;
		std::vector<std::uint8_t>  Strings;
		std::vector<std::uint32_t> Code;
	};

	Binary      ParseBinary(Utils::View<std::uint8_t> data);
	std::size_t WriteBinary(const Binary& binary, std::vector<std::uint8_t>& data);
} // namespace Frertex::FIL