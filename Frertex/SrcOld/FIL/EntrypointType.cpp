#include "Frertex/FIL/EntrypointType.h"

namespace Frertex::FIL
{
	std::string_view EntrypointTypeToString(EEntrypointType type)
	{
		switch (type)
		{
		case EEntrypointType::None: return "None";
		case EEntrypointType::VertexShader: return "VertexShader";
		case EEntrypointType::TessellationControlShader: return "TessellationControlShader";
		case EEntrypointType::TessellationEvaluationShader: return "TessellationEvaluationShader";
		case EEntrypointType::GeometryShader: return "GeometryShader";
		case EEntrypointType::FragmentShader: return "FragmentShader";
		case EEntrypointType::ComputeShader: return "ComputeShader";
		case EEntrypointType::RTRayGenShader: return "RTRayGenShader";
		case EEntrypointType::RTAnyHitShader: return "RTAnyHitShader";
		case EEntrypointType::RTClosestHitShader: return "RTClosestHitShader";
		case EEntrypointType::RTMissShader: return "RTMissShader";
		case EEntrypointType::RTIntersectionShader: return "RTIntersectionShader";
		case EEntrypointType::RTCallableShader: return "RTCallableShader";
		case EEntrypointType::TaskShader: return "TaskShader";
		case EEntrypointType::MeshShader: return "MeshShader";
		}
		return "?";
	}

	EEntrypointType StringToEntrypointType(std::string_view view)
	{
		if (view == "VertexShader")
			return EEntrypointType::VertexShader;
		else if (view == "TessellationControlShader")
			return EEntrypointType::TessellationControlShader;
		else if (view == "TessellationEvaluationShader")
			return EEntrypointType::TessellationEvaluationShader;
		else if (view == "GeometryShader")
			return EEntrypointType::GeometryShader;
		else if (view == "FragmentShader")
			return EEntrypointType::FragmentShader;
		else if (view == "ComputeShader")
			return EEntrypointType::ComputeShader;
		else if (view == "RTRayGenShader")
			return EEntrypointType::RTRayGenShader;
		else if (view == "RTAnyHitShader")
			return EEntrypointType::RTAnyHitShader;
		else if (view == "RTClosestHitShader")
			return EEntrypointType::RTClosestHitShader;
		else if (view == "RTMissShader")
			return EEntrypointType::RTMissShader;
		else if (view == "RTIntersectionShader")
			return EEntrypointType::RTIntersectionShader;
		else if (view == "RTCallableShader")
			return EEntrypointType::RTCallableShader;
		else if (view == "TaskShader")
			return EEntrypointType::TaskShader;
		else if (view == "MeshShader")
			return EEntrypointType::MeshShader;
		return EEntrypointType::None;
	}
} // namespace Frertex::FIL