#include "Frertex/Transpilers/SPIR-V/Enumerations.h"
#include "Frertex/Utils/Profiler.h"

namespace Frertex::Transpilers::SPIRV
{
	EExecutionModel EntrypointTypeToExecutionModel(EEntrypointType type)
	{
		PROFILE_FUNC;

		switch (type)
		{
		case EEntrypointType::VertexShader: return EExecutionModel::Vertex;
		case EEntrypointType::TessellationControlShader: return EExecutionModel::TessellationControl;
		case EEntrypointType::TessellationEvaluationShader: return EExecutionModel::TessellationEvaluation;
		case EEntrypointType::GeometryShader: return EExecutionModel::Geometry;
		case EEntrypointType::FragmentShader: return EExecutionModel::Fragment;
		case EEntrypointType::ComputeShader: return EExecutionModel::Kernel;
		case EEntrypointType::RTRayGenShader: return EExecutionModel::RayGenerationKHR;
		case EEntrypointType::RTAnyHitShader: return EExecutionModel::AnyHitKHR;
		case EEntrypointType::RTClosestHitShader: return EExecutionModel::ClosestHitKHR;
		case EEntrypointType::RTMissShader: return EExecutionModel::MissKHR;
		case EEntrypointType::RTIntersectionShader: return EExecutionModel::IntersectionKHR;
		case EEntrypointType::RTCallableShader: return EExecutionModel::CallableKHR;
		case EEntrypointType::NVTaskShader: return EExecutionModel::TaskNV;
		case EEntrypointType::NVMeshShader: return EExecutionModel::MeshNV;
		default: return EExecutionModel::Vertex;
		}
	}

	std::string_view StorageClassToString(EStorageClass storageClass)
	{
		PROFILE_FUNC;

		switch (storageClass)
		{
		case EStorageClass::UniformConstant: return "UniformConstant";
		case EStorageClass::Input: return "Input";
		case EStorageClass::Uniform: return "Uniform";
		case EStorageClass::Output: return "Output";
		case EStorageClass::Workgroup: return "Workgroup";
		case EStorageClass::CrossWorkgroup: return "CrossWorkgroup";
		case EStorageClass::Private: return "Private";
		case EStorageClass::Function: return "Function";
		case EStorageClass::Generic: return "Generic";
		case EStorageClass::PushConstant: return "PushConstant";
		case EStorageClass::AtomicCounter: return "AtomicCounter";
		case EStorageClass::Image: return "Image";
		case EStorageClass::StorageBuffer: return "StorageBuffer";
		case EStorageClass::CallableDataKHR: return "CallableDataKHR";
		case EStorageClass::IncomingCallableDataKHR: return "IncomingCallableDataKHR";
		case EStorageClass::RayPaylodKHR: return "RayPaylodKHR";
		case EStorageClass::HitAttributeKHR: return "HitAttributeKHR";
		case EStorageClass::IncomingRayPaylodHitAttributeKHR: return "IncomingRayPaylodHitAttributeKHR";
		case EStorageClass::ShaderRecordBufferKHR: return "ShaderRecordBufferKHR";
		case EStorageClass::PhysicalStorageBufferEXT: return "PhysicalStorageBufferEXT";
		case EStorageClass::CodeSectionINTEL: return "CodeSectionINTEL";
		case EStorageClass::DeviceOnlyINTEL: return "DeviceOnlyINTEL";
		case EStorageClass::HostOnlyINTEL: return "HostOnlyINTEL";
		default: return {};
		}
	}
} // namespace Frertex::Transpilers::SPIRV