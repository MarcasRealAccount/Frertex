#pragma once

#include <cstdint>

#include <string_view>

namespace Frertex::FIL
{
	enum class EEntrypointType : std::uint32_t
	{
		None                         = 0x0000,
		VertexShader                 = 0x0001,
		TessellationControlShader    = 0x0002,
		TessellationEvaluationShader = 0x0004,
		GeometryShader               = 0x0008,
		FragmentShader               = 0x0010,
		ComputeShader                = 0x0020,
		RTRayGenShader               = 0x0100,
		RTAnyHitShader               = 0x0200,
		RTClosestHitShader           = 0x0400,
		RTMissShader                 = 0x0800,
		RTIntersectionShader         = 0x1000,
		RTCallableShader             = 0x2000,
		NVTaskShader                 = 0x0040,
		NVMeshShader                 = 0x0080
	};

	std::string_view EntrypointTypeToString(EEntrypointType type);
	EEntrypointType  StringToEntrypointType(std::string_view view);
} // namespace Frertex::FIL