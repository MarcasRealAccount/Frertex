#include "Frertex/Transpilers/SPIRV/Enumerations.h"

namespace Frertex::Transpilers::SPIRV
{
	void RequireCapExtImageOperands([[maybe_unused]] EImageOperands value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case EImageOperands::Bias:
			capabilities.insert(ECapability::Shader);
			break;
		case EImageOperands::Offset:
			capabilities.insert(ECapability::ImageGatherExtended);
			break;
		case EImageOperands::ConstOffsets:
			capabilities.insert(ECapability::ImageGatherExtended);
			break;
		case EImageOperands::MinLod:
			capabilities.insert(ECapability::MinLod);
			break;
		case EImageOperands::MakeTexelAvailable:
			capabilities.insert(ECapability::VulkanMemoryModel);
			break;
		case EImageOperands::MakeTexelVisible:
			capabilities.insert(ECapability::VulkanMemoryModel);
			break;
		case EImageOperands::NonPrivateTexel:
			capabilities.insert(ECapability::VulkanMemoryModel);
			break;
		case EImageOperands::VolatileTexel:
			capabilities.insert(ECapability::VulkanMemoryModel);
			break;
		default: break;
		}
	}

	void RequireCapExtFPFastMathMode([[maybe_unused]] EFPFastMathMode value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case EFPFastMathMode::AllowContractFastINTEL:
			capabilities.insert(ECapability::FPFastMathModeINTEL);
			break;
		case EFPFastMathMode::AllowReassocINTEL:
			capabilities.insert(ECapability::FPFastMathModeINTEL);
			break;
		default: break;
		}
	}

	void RequireCapExtSelectionControl([[maybe_unused]] ESelectionControl value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
	}

	void RequireCapExtLoopControl([[maybe_unused]] ELoopControl value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case ELoopControl::InitiationIntervalINTEL:
			capabilities.insert(ECapability::FPGALoopControlsINTEL);
			extensions.insert("SPV_INTEL_fpga_loop_controls");
			break;
		case ELoopControl::MaxConcurrencyINTEL:
			capabilities.insert(ECapability::FPGALoopControlsINTEL);
			extensions.insert("SPV_INTEL_fpga_loop_controls");
			break;
		case ELoopControl::DependencyArrayINTEL:
			capabilities.insert(ECapability::FPGALoopControlsINTEL);
			extensions.insert("SPV_INTEL_fpga_loop_controls");
			break;
		case ELoopControl::PipelineEnableINTEL:
			capabilities.insert(ECapability::FPGALoopControlsINTEL);
			extensions.insert("SPV_INTEL_fpga_loop_controls");
			break;
		case ELoopControl::LoopCoalesceINTEL:
			capabilities.insert(ECapability::FPGALoopControlsINTEL);
			extensions.insert("SPV_INTEL_fpga_loop_controls");
			break;
		case ELoopControl::MaxInterleavingINTEL:
			capabilities.insert(ECapability::FPGALoopControlsINTEL);
			extensions.insert("SPV_INTEL_fpga_loop_controls");
			break;
		case ELoopControl::SpeculatedIterationsINTEL:
			capabilities.insert(ECapability::FPGALoopControlsINTEL);
			extensions.insert("SPV_INTEL_fpga_loop_controls");
			break;
		case ELoopControl::NoFusionINTEL:
			capabilities.insert(ECapability::FPGALoopControlsINTEL);
			extensions.insert("SPV_INTEL_fpga_loop_controls");
			break;
		default: break;
		}
	}

	void RequireCapExtFunctionControl([[maybe_unused]] EFunctionControl value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case EFunctionControl::OptNoneINTEL:
			capabilities.insert(ECapability::OptNoneINTEL);
			break;
		default: break;
		}
	}

	void RequireCapExtMemorySemantics([[maybe_unused]] EMemorySemantics value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case EMemorySemantics::UniformMemory:
			capabilities.insert(ECapability::Shader);
			break;
		case EMemorySemantics::AtomicCounterMemory:
			capabilities.insert(ECapability::AtomicStorage);
			break;
		case EMemorySemantics::OutputMemory:
			capabilities.insert(ECapability::VulkanMemoryModel);
			break;
		case EMemorySemantics::MakeAvailable:
			capabilities.insert(ECapability::VulkanMemoryModel);
			break;
		case EMemorySemantics::MakeVisible:
			capabilities.insert(ECapability::VulkanMemoryModel);
			break;
		case EMemorySemantics::Volatile:
			capabilities.insert(ECapability::VulkanMemoryModel);
			extensions.insert("SPV_KHR_vulkan_memory_model");
			break;
		default: break;
		}
	}

	void RequireCapExtMemoryAccess([[maybe_unused]] EMemoryAccess value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case EMemoryAccess::MakePointerAvailable:
			capabilities.insert(ECapability::VulkanMemoryModel);
			break;
		case EMemoryAccess::MakePointerVisible:
			capabilities.insert(ECapability::VulkanMemoryModel);
			break;
		case EMemoryAccess::NonPrivatePointer:
			capabilities.insert(ECapability::VulkanMemoryModel);
			break;
		case EMemoryAccess::AliasScopeINTELMask:
			capabilities.insert(ECapability::MemoryAccessAliasingINTEL);
			extensions.insert("SPV_INTEL_memory_access_aliasing");
			break;
		case EMemoryAccess::NoAliasINTELMask:
			capabilities.insert(ECapability::MemoryAccessAliasingINTEL);
			extensions.insert("SPV_INTEL_memory_access_aliasing");
			break;
		default: break;
		}
	}

	void RequireCapExtKernelProfilingInfo([[maybe_unused]] EKernelProfilingInfo value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case EKernelProfilingInfo::CmdExecTime:
			capabilities.insert(ECapability::Kernel);
			break;
		default: break;
		}
	}

	void RequireCapExtRayFlags([[maybe_unused]] ERayFlags value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case ERayFlags::NoneKHR:
			capabilities.insert(ECapability::RayQueryKHR);
			capabilities.insert(ECapability::RayTracingKHR);
			break;
		case ERayFlags::OpaqueKHR:
			capabilities.insert(ECapability::RayQueryKHR);
			capabilities.insert(ECapability::RayTracingKHR);
			break;
		case ERayFlags::NoOpaqueKHR:
			capabilities.insert(ECapability::RayQueryKHR);
			capabilities.insert(ECapability::RayTracingKHR);
			break;
		case ERayFlags::TerminateOnFirstHitKHR:
			capabilities.insert(ECapability::RayQueryKHR);
			capabilities.insert(ECapability::RayTracingKHR);
			break;
		case ERayFlags::SkipClosestHitShaderKHR:
			capabilities.insert(ECapability::RayQueryKHR);
			capabilities.insert(ECapability::RayTracingKHR);
			break;
		case ERayFlags::CullBackFacingTrianglesKHR:
			capabilities.insert(ECapability::RayQueryKHR);
			capabilities.insert(ECapability::RayTracingKHR);
			break;
		case ERayFlags::CullFrontFacingTrianglesKHR:
			capabilities.insert(ECapability::RayQueryKHR);
			capabilities.insert(ECapability::RayTracingKHR);
			break;
		case ERayFlags::CullOpaqueKHR:
			capabilities.insert(ECapability::RayQueryKHR);
			capabilities.insert(ECapability::RayTracingKHR);
			break;
		case ERayFlags::CullNoOpaqueKHR:
			capabilities.insert(ECapability::RayQueryKHR);
			capabilities.insert(ECapability::RayTracingKHR);
			break;
		case ERayFlags::SkipTrianglesKHR:
			capabilities.insert(ECapability::RayTraversalPrimitiveCullingKHR);
			break;
		case ERayFlags::SkipAABBsKHR:
			capabilities.insert(ECapability::RayTraversalPrimitiveCullingKHR);
			break;
		default: break;
		}
	}

	void RequireCapExtFragmentShadingRate([[maybe_unused]] EFragmentShadingRate value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case EFragmentShadingRate::Vertical2Pixels:
			capabilities.insert(ECapability::FragmentShadingRateKHR);
			break;
		case EFragmentShadingRate::Vertical4Pixels:
			capabilities.insert(ECapability::FragmentShadingRateKHR);
			break;
		case EFragmentShadingRate::Horizontal2Pixels:
			capabilities.insert(ECapability::FragmentShadingRateKHR);
			break;
		case EFragmentShadingRate::Horizontal4Pixels:
			capabilities.insert(ECapability::FragmentShadingRateKHR);
			break;
		default: break;
		}
	}

	void RequireCapExtSourceLanguage([[maybe_unused]] ESourceLanguage value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
	}

	void RequireCapExtExecutionModel([[maybe_unused]] EExecutionModel value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case EExecutionModel::Vertex:
			capabilities.insert(ECapability::Shader);
			break;
		case EExecutionModel::TessellationControl:
			capabilities.insert(ECapability::Tessellation);
			break;
		case EExecutionModel::TessellationEvaluation:
			capabilities.insert(ECapability::Tessellation);
			break;
		case EExecutionModel::Geometry:
			capabilities.insert(ECapability::Geometry);
			break;
		case EExecutionModel::Fragment:
			capabilities.insert(ECapability::Shader);
			break;
		case EExecutionModel::GLCompute:
			capabilities.insert(ECapability::Shader);
			break;
		case EExecutionModel::Kernel:
			capabilities.insert(ECapability::Kernel);
			break;
		case EExecutionModel::TaskNV:
			capabilities.insert(ECapability::MeshShadingNV);
			break;
		case EExecutionModel::MeshNV:
			capabilities.insert(ECapability::MeshShadingNV);
			break;
		case EExecutionModel::TaskEXT:
			capabilities.insert(ECapability::MeshShadingEXT);
			break;
		case EExecutionModel::MeshEXT:
			capabilities.insert(ECapability::MeshShadingEXT);
			break;
		default: break;
		}
	}

	void RequireCapExtAddressingModel([[maybe_unused]] EAddressingModel value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case EAddressingModel::Physical32:
			capabilities.insert(ECapability::Addresses);
			break;
		case EAddressingModel::Physical64:
			capabilities.insert(ECapability::Addresses);
			break;
		case EAddressingModel::PhysicalStorageBuffer64:
			capabilities.insert(ECapability::PhysicalStorageBufferAddresses);
			extensions.insert("SPV_EXT_physical_storage_buffer");
			extensions.insert("SPV_KHR_physical_storage_buffer");
			break;
		default: break;
		}
	}

	void RequireCapExtMemoryModel([[maybe_unused]] EMemoryModel value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case EMemoryModel::Simple:
			capabilities.insert(ECapability::Shader);
			break;
		case EMemoryModel::GLSL450:
			capabilities.insert(ECapability::Shader);
			break;
		case EMemoryModel::OpenCL:
			capabilities.insert(ECapability::Kernel);
			break;
		case EMemoryModel::Vulkan:
			capabilities.insert(ECapability::VulkanMemoryModel);
			break;
		default: break;
		}
	}

	void RequireCapExtExecutionMode([[maybe_unused]] EExecutionMode value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case EExecutionMode::Invocations:
			capabilities.insert(ECapability::Geometry);
			break;
		case EExecutionMode::SpacingEqual:
			capabilities.insert(ECapability::Tessellation);
			break;
		case EExecutionMode::SpacingFractionalEven:
			capabilities.insert(ECapability::Tessellation);
			break;
		case EExecutionMode::SpacingFractionalOdd:
			capabilities.insert(ECapability::Tessellation);
			break;
		case EExecutionMode::VertexOrderCw:
			capabilities.insert(ECapability::Tessellation);
			break;
		case EExecutionMode::VertexOrderCcw:
			capabilities.insert(ECapability::Tessellation);
			break;
		case EExecutionMode::PixelCenterInteger:
			capabilities.insert(ECapability::Shader);
			break;
		case EExecutionMode::OriginUpperLeft:
			capabilities.insert(ECapability::Shader);
			break;
		case EExecutionMode::OriginLowerLeft:
			capabilities.insert(ECapability::Shader);
			break;
		case EExecutionMode::EarlyFragmentTests:
			capabilities.insert(ECapability::Shader);
			break;
		case EExecutionMode::PointMode:
			capabilities.insert(ECapability::Tessellation);
			break;
		case EExecutionMode::Xfb:
			capabilities.insert(ECapability::TransformFeedback);
			break;
		case EExecutionMode::DepthReplacing:
			capabilities.insert(ECapability::Shader);
			break;
		case EExecutionMode::DepthGreater:
			capabilities.insert(ECapability::Shader);
			break;
		case EExecutionMode::DepthLess:
			capabilities.insert(ECapability::Shader);
			break;
		case EExecutionMode::DepthUnchanged:
			capabilities.insert(ECapability::Shader);
			break;
		case EExecutionMode::LocalSizeHint:
			capabilities.insert(ECapability::Kernel);
			break;
		case EExecutionMode::InputPoints:
			capabilities.insert(ECapability::Geometry);
			break;
		case EExecutionMode::InputLines:
			capabilities.insert(ECapability::Geometry);
			break;
		case EExecutionMode::InputLinesAdjacency:
			capabilities.insert(ECapability::Geometry);
			break;
		case EExecutionMode::Triangles:
			capabilities.insert(ECapability::Geometry);
			capabilities.insert(ECapability::Tessellation);
			break;
		case EExecutionMode::InputTrianglesAdjacency:
			capabilities.insert(ECapability::Geometry);
			break;
		case EExecutionMode::Quads:
			capabilities.insert(ECapability::Tessellation);
			break;
		case EExecutionMode::Isolines:
			capabilities.insert(ECapability::Tessellation);
			break;
		case EExecutionMode::OutputVertices:
			capabilities.insert(ECapability::Geometry);
			capabilities.insert(ECapability::Tessellation);
			capabilities.insert(ECapability::MeshShadingNV);
			capabilities.insert(ECapability::MeshShadingEXT);
			break;
		case EExecutionMode::OutputPoints:
			capabilities.insert(ECapability::Geometry);
			capabilities.insert(ECapability::MeshShadingNV);
			capabilities.insert(ECapability::MeshShadingEXT);
			break;
		case EExecutionMode::OutputLineStrip:
			capabilities.insert(ECapability::Geometry);
			break;
		case EExecutionMode::OutputTriangleStrip:
			capabilities.insert(ECapability::Geometry);
			break;
		case EExecutionMode::VecTypeHint:
			capabilities.insert(ECapability::Kernel);
			break;
		case EExecutionMode::ContractionOff:
			capabilities.insert(ECapability::Kernel);
			break;
		case EExecutionMode::Initializer:
			capabilities.insert(ECapability::Kernel);
			break;
		case EExecutionMode::Finalizer:
			capabilities.insert(ECapability::Kernel);
			break;
		case EExecutionMode::SubgroupSize:
			capabilities.insert(ECapability::SubgroupDispatch);
			break;
		case EExecutionMode::SubgroupsPerWorkgroup:
			capabilities.insert(ECapability::SubgroupDispatch);
			break;
		case EExecutionMode::SubgroupsPerWorkgroupId:
			capabilities.insert(ECapability::SubgroupDispatch);
			break;
		case EExecutionMode::LocalSizeHintId:
			capabilities.insert(ECapability::Kernel);
			break;
		case EExecutionMode::SubgroupUniformControlFlowKHR:
			capabilities.insert(ECapability::Shader);
			extensions.insert("SPV_KHR_subgroup_uniform_control_flow");
			break;
		case EExecutionMode::PostDepthCoverage:
			capabilities.insert(ECapability::SampleMaskPostDepthCoverage);
			extensions.insert("SPV_KHR_post_depth_coverage");
			break;
		case EExecutionMode::DenormPreserve:
			capabilities.insert(ECapability::DenormPreserve);
			extensions.insert("SPV_KHR_float_controls");
			break;
		case EExecutionMode::DenormFlushToZero:
			capabilities.insert(ECapability::DenormFlushToZero);
			extensions.insert("SPV_KHR_float_controls");
			break;
		case EExecutionMode::SignedZeroInfNanPreserve:
			capabilities.insert(ECapability::SignedZeroInfNanPreserve);
			extensions.insert("SPV_KHR_float_controls");
			break;
		case EExecutionMode::RoundingModeRTE:
			capabilities.insert(ECapability::RoundingModeRTE);
			extensions.insert("SPV_KHR_float_controls");
			break;
		case EExecutionMode::RoundingModeRTZ:
			capabilities.insert(ECapability::RoundingModeRTZ);
			extensions.insert("SPV_KHR_float_controls");
			break;
		case EExecutionMode::EarlyAndLateFragmentTestsAMD:
			capabilities.insert(ECapability::Shader);
			extensions.insert("SPV_AMD_shader_early_and_late_fragment_tests");
			break;
		case EExecutionMode::StencilRefReplacingEXT:
			capabilities.insert(ECapability::StencilExportEXT);
			extensions.insert("SPV_EXT_shader_stencil_export");
			break;
		case EExecutionMode::StencilRefUnchangedFrontAMD:
			capabilities.insert(ECapability::StencilExportEXT);
			extensions.insert("SPV_AMD_shader_early_and_late_fragment_tests");
			extensions.insert("SPV_EXT_shader_stencil_export");
			break;
		case EExecutionMode::StencilRefGreaterFrontAMD:
			capabilities.insert(ECapability::StencilExportEXT);
			extensions.insert("SPV_AMD_shader_early_and_late_fragment_tests");
			extensions.insert("SPV_EXT_shader_stencil_export");
			break;
		case EExecutionMode::StencilRefLessFrontAMD:
			capabilities.insert(ECapability::StencilExportEXT);
			extensions.insert("SPV_AMD_shader_early_and_late_fragment_tests");
			extensions.insert("SPV_EXT_shader_stencil_export");
			break;
		case EExecutionMode::StencilRefUnchangedBackAMD:
			capabilities.insert(ECapability::StencilExportEXT);
			extensions.insert("SPV_AMD_shader_early_and_late_fragment_tests");
			extensions.insert("SPV_EXT_shader_stencil_export");
			break;
		case EExecutionMode::StencilRefGreaterBackAMD:
			capabilities.insert(ECapability::StencilExportEXT);
			extensions.insert("SPV_AMD_shader_early_and_late_fragment_tests");
			extensions.insert("SPV_EXT_shader_stencil_export");
			break;
		case EExecutionMode::StencilRefLessBackAMD:
			capabilities.insert(ECapability::StencilExportEXT);
			extensions.insert("SPV_AMD_shader_early_and_late_fragment_tests");
			extensions.insert("SPV_EXT_shader_stencil_export");
			break;
		case EExecutionMode::DerivativeGroupQuadsNV:
			capabilities.insert(ECapability::ComputeDerivativeGroupQuadsNV);
			extensions.insert("SPV_NV_compute_shader_derivatives");
			break;
		case EExecutionMode::DerivativeGroupLinearNV:
			capabilities.insert(ECapability::ComputeDerivativeGroupLinearNV);
			extensions.insert("SPV_NV_compute_shader_derivatives");
			break;
		case EExecutionMode::PixelInterlockOrderedEXT:
			capabilities.insert(ECapability::FragmentShaderPixelInterlockEXT);
			extensions.insert("SPV_EXT_fragment_shader_interlock");
			break;
		case EExecutionMode::PixelInterlockUnorderedEXT:
			capabilities.insert(ECapability::FragmentShaderPixelInterlockEXT);
			extensions.insert("SPV_EXT_fragment_shader_interlock");
			break;
		case EExecutionMode::SampleInterlockOrderedEXT:
			capabilities.insert(ECapability::FragmentShaderSampleInterlockEXT);
			extensions.insert("SPV_EXT_fragment_shader_interlock");
			break;
		case EExecutionMode::SampleInterlockUnorderedEXT:
			capabilities.insert(ECapability::FragmentShaderSampleInterlockEXT);
			extensions.insert("SPV_EXT_fragment_shader_interlock");
			break;
		case EExecutionMode::ShadingRateInterlockOrderedEXT:
			capabilities.insert(ECapability::FragmentShaderShadingRateInterlockEXT);
			extensions.insert("SPV_EXT_fragment_shader_interlock");
			break;
		case EExecutionMode::ShadingRateInterlockUnorderedEXT:
			capabilities.insert(ECapability::FragmentShaderShadingRateInterlockEXT);
			extensions.insert("SPV_EXT_fragment_shader_interlock");
			break;
		case EExecutionMode::SharedLocalMemorySizeINTEL:
			capabilities.insert(ECapability::VectorComputeINTEL);
			break;
		case EExecutionMode::RoundingModeRTPINTEL:
			capabilities.insert(ECapability::RoundToInfinityINTEL);
			break;
		case EExecutionMode::RoundingModeRTNINTEL:
			capabilities.insert(ECapability::RoundToInfinityINTEL);
			break;
		case EExecutionMode::FloatingPointModeALTINTEL:
			capabilities.insert(ECapability::RoundToInfinityINTEL);
			break;
		case EExecutionMode::FloatingPointModeIEEEINTEL:
			capabilities.insert(ECapability::RoundToInfinityINTEL);
			break;
		case EExecutionMode::MaxWorkgroupSizeINTEL:
			capabilities.insert(ECapability::KernelAttributesINTEL);
			extensions.insert("SPV_INTEL_kernel_attributes");
			break;
		case EExecutionMode::MaxWorkDimINTEL:
			capabilities.insert(ECapability::KernelAttributesINTEL);
			extensions.insert("SPV_INTEL_kernel_attributes");
			break;
		case EExecutionMode::NoGlobalOffsetINTEL:
			capabilities.insert(ECapability::KernelAttributesINTEL);
			extensions.insert("SPV_INTEL_kernel_attributes");
			break;
		case EExecutionMode::NumSIMDWorkitemsINTEL:
			capabilities.insert(ECapability::FPGAKernelAttributesINTEL);
			extensions.insert("SPV_INTEL_kernel_attributes");
			break;
		case EExecutionMode::SchedulerTargetFmaxMhzINTEL:
			capabilities.insert(ECapability::FPGAKernelAttributesINTEL);
			break;
		case EExecutionMode::NamedBarrierCountINTEL:
			capabilities.insert(ECapability::VectorComputeINTEL);
			break;
		default: break;
		}
	}

	void RequireCapExtStorageClass([[maybe_unused]] EStorageClass value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case EStorageClass::Uniform:
			capabilities.insert(ECapability::Shader);
			break;
		case EStorageClass::Output:
			capabilities.insert(ECapability::Shader);
			break;
		case EStorageClass::Private:
			capabilities.insert(ECapability::Shader);
			capabilities.insert(ECapability::VectorComputeINTEL);
			break;
		case EStorageClass::Generic:
			capabilities.insert(ECapability::GenericPointer);
			break;
		case EStorageClass::PushConstant:
			capabilities.insert(ECapability::Shader);
			break;
		case EStorageClass::AtomicCounter:
			capabilities.insert(ECapability::AtomicStorage);
			break;
		case EStorageClass::StorageBuffer:
			capabilities.insert(ECapability::Shader);
			extensions.insert("SPV_KHR_storage_buffer_storage_class");
			extensions.insert("SPV_KHR_variable_pointers");
			break;
		case EStorageClass::PhysicalStorageBuffer:
			capabilities.insert(ECapability::PhysicalStorageBufferAddresses);
			extensions.insert("SPV_EXT_physical_storage_buffer");
			extensions.insert("SPV_KHR_physical_storage_buffer");
			break;
		case EStorageClass::TaskPayloadWorkgroupEXT:
			capabilities.insert(ECapability::MeshShadingEXT);
			extensions.insert("SPV_EXT_mesh_shader");
			break;
		case EStorageClass::CodeSectionINTEL:
			capabilities.insert(ECapability::FunctionPointersINTEL);
			extensions.insert("SPV_INTEL_function_pointers");
			break;
		case EStorageClass::DeviceOnlyINTEL:
			capabilities.insert(ECapability::USMStorageClassesINTEL);
			extensions.insert("SPV_INTEL_usm_storage_classes");
			break;
		case EStorageClass::HostOnlyINTEL:
			capabilities.insert(ECapability::USMStorageClassesINTEL);
			extensions.insert("SPV_INTEL_usm_storage_classes");
			break;
		default: break;
		}
	}

	void RequireCapExtDim([[maybe_unused]] EDim value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case EDim::_1D:
			capabilities.insert(ECapability::Sampled1D);
			capabilities.insert(ECapability::Image1D);
			break;
		case EDim::_2D:
			capabilities.insert(ECapability::Shader);
			capabilities.insert(ECapability::Kernel);
			capabilities.insert(ECapability::ImageMSArray);
			break;
		case EDim::Cube:
			capabilities.insert(ECapability::Shader);
			capabilities.insert(ECapability::ImageCubeArray);
			break;
		case EDim::Rect:
			capabilities.insert(ECapability::SampledRect);
			capabilities.insert(ECapability::ImageRect);
			break;
		case EDim::Buffer:
			capabilities.insert(ECapability::SampledBuffer);
			capabilities.insert(ECapability::ImageBuffer);
			break;
		case EDim::SubpassData:
			capabilities.insert(ECapability::InputAttachment);
			break;
		default: break;
		}
	}

	void RequireCapExtSamplerAddressingMode([[maybe_unused]] ESamplerAddressingMode value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case ESamplerAddressingMode::None:
			capabilities.insert(ECapability::Kernel);
			break;
		case ESamplerAddressingMode::ClampToEdge:
			capabilities.insert(ECapability::Kernel);
			break;
		case ESamplerAddressingMode::Clamp:
			capabilities.insert(ECapability::Kernel);
			break;
		case ESamplerAddressingMode::Repeat:
			capabilities.insert(ECapability::Kernel);
			break;
		case ESamplerAddressingMode::RepeatMirrored:
			capabilities.insert(ECapability::Kernel);
			break;
		default: break;
		}
	}

	void RequireCapExtSamplerFilterMode([[maybe_unused]] ESamplerFilterMode value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case ESamplerFilterMode::Nearest:
			capabilities.insert(ECapability::Kernel);
			break;
		case ESamplerFilterMode::Linear:
			capabilities.insert(ECapability::Kernel);
			break;
		default: break;
		}
	}

	void RequireCapExtImageFormat([[maybe_unused]] EImageFormat value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case EImageFormat::Rgba32f:
			capabilities.insert(ECapability::Shader);
			break;
		case EImageFormat::Rgba16f:
			capabilities.insert(ECapability::Shader);
			break;
		case EImageFormat::R32f:
			capabilities.insert(ECapability::Shader);
			break;
		case EImageFormat::Rgba8:
			capabilities.insert(ECapability::Shader);
			break;
		case EImageFormat::Rgba8Snorm:
			capabilities.insert(ECapability::Shader);
			break;
		case EImageFormat::Rg32f:
			capabilities.insert(ECapability::StorageImageExtendedFormats);
			break;
		case EImageFormat::Rg16f:
			capabilities.insert(ECapability::StorageImageExtendedFormats);
			break;
		case EImageFormat::R11fG11fB10f:
			capabilities.insert(ECapability::StorageImageExtendedFormats);
			break;
		case EImageFormat::R16f:
			capabilities.insert(ECapability::StorageImageExtendedFormats);
			break;
		case EImageFormat::Rgba16:
			capabilities.insert(ECapability::StorageImageExtendedFormats);
			break;
		case EImageFormat::Rgb10A2:
			capabilities.insert(ECapability::StorageImageExtendedFormats);
			break;
		case EImageFormat::Rg16:
			capabilities.insert(ECapability::StorageImageExtendedFormats);
			break;
		case EImageFormat::Rg8:
			capabilities.insert(ECapability::StorageImageExtendedFormats);
			break;
		case EImageFormat::R16:
			capabilities.insert(ECapability::StorageImageExtendedFormats);
			break;
		case EImageFormat::R8:
			capabilities.insert(ECapability::StorageImageExtendedFormats);
			break;
		case EImageFormat::Rgba16Snorm:
			capabilities.insert(ECapability::StorageImageExtendedFormats);
			break;
		case EImageFormat::Rg16Snorm:
			capabilities.insert(ECapability::StorageImageExtendedFormats);
			break;
		case EImageFormat::Rg8Snorm:
			capabilities.insert(ECapability::StorageImageExtendedFormats);
			break;
		case EImageFormat::R16Snorm:
			capabilities.insert(ECapability::StorageImageExtendedFormats);
			break;
		case EImageFormat::R8Snorm:
			capabilities.insert(ECapability::StorageImageExtendedFormats);
			break;
		case EImageFormat::Rgba32i:
			capabilities.insert(ECapability::Shader);
			break;
		case EImageFormat::Rgba16i:
			capabilities.insert(ECapability::Shader);
			break;
		case EImageFormat::Rgba8i:
			capabilities.insert(ECapability::Shader);
			break;
		case EImageFormat::R32i:
			capabilities.insert(ECapability::Shader);
			break;
		case EImageFormat::Rg32i:
			capabilities.insert(ECapability::StorageImageExtendedFormats);
			break;
		case EImageFormat::Rg16i:
			capabilities.insert(ECapability::StorageImageExtendedFormats);
			break;
		case EImageFormat::Rg8i:
			capabilities.insert(ECapability::StorageImageExtendedFormats);
			break;
		case EImageFormat::R16i:
			capabilities.insert(ECapability::StorageImageExtendedFormats);
			break;
		case EImageFormat::R8i:
			capabilities.insert(ECapability::StorageImageExtendedFormats);
			break;
		case EImageFormat::Rgba32ui:
			capabilities.insert(ECapability::Shader);
			break;
		case EImageFormat::Rgba16ui:
			capabilities.insert(ECapability::Shader);
			break;
		case EImageFormat::Rgba8ui:
			capabilities.insert(ECapability::Shader);
			break;
		case EImageFormat::R32ui:
			capabilities.insert(ECapability::Shader);
			break;
		case EImageFormat::Rgb10a2ui:
			capabilities.insert(ECapability::StorageImageExtendedFormats);
			break;
		case EImageFormat::Rg32ui:
			capabilities.insert(ECapability::StorageImageExtendedFormats);
			break;
		case EImageFormat::Rg16ui:
			capabilities.insert(ECapability::StorageImageExtendedFormats);
			break;
		case EImageFormat::Rg8ui:
			capabilities.insert(ECapability::StorageImageExtendedFormats);
			break;
		case EImageFormat::R16ui:
			capabilities.insert(ECapability::StorageImageExtendedFormats);
			break;
		case EImageFormat::R8ui:
			capabilities.insert(ECapability::StorageImageExtendedFormats);
			break;
		case EImageFormat::R64ui:
			capabilities.insert(ECapability::Int64ImageEXT);
			break;
		case EImageFormat::R64i:
			capabilities.insert(ECapability::Int64ImageEXT);
			break;
		default: break;
		}
	}

	void RequireCapExtImageChannelOrder([[maybe_unused]] EImageChannelOrder value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case EImageChannelOrder::R:
			capabilities.insert(ECapability::Kernel);
			break;
		case EImageChannelOrder::A:
			capabilities.insert(ECapability::Kernel);
			break;
		case EImageChannelOrder::RG:
			capabilities.insert(ECapability::Kernel);
			break;
		case EImageChannelOrder::RA:
			capabilities.insert(ECapability::Kernel);
			break;
		case EImageChannelOrder::RGB:
			capabilities.insert(ECapability::Kernel);
			break;
		case EImageChannelOrder::RGBA:
			capabilities.insert(ECapability::Kernel);
			break;
		case EImageChannelOrder::BGRA:
			capabilities.insert(ECapability::Kernel);
			break;
		case EImageChannelOrder::ARGB:
			capabilities.insert(ECapability::Kernel);
			break;
		case EImageChannelOrder::Intensity:
			capabilities.insert(ECapability::Kernel);
			break;
		case EImageChannelOrder::Luminance:
			capabilities.insert(ECapability::Kernel);
			break;
		case EImageChannelOrder::Rx:
			capabilities.insert(ECapability::Kernel);
			break;
		case EImageChannelOrder::RGx:
			capabilities.insert(ECapability::Kernel);
			break;
		case EImageChannelOrder::RGBx:
			capabilities.insert(ECapability::Kernel);
			break;
		case EImageChannelOrder::Depth:
			capabilities.insert(ECapability::Kernel);
			break;
		case EImageChannelOrder::DepthStencil:
			capabilities.insert(ECapability::Kernel);
			break;
		case EImageChannelOrder::sRGB:
			capabilities.insert(ECapability::Kernel);
			break;
		case EImageChannelOrder::sRGBx:
			capabilities.insert(ECapability::Kernel);
			break;
		case EImageChannelOrder::sRGBA:
			capabilities.insert(ECapability::Kernel);
			break;
		case EImageChannelOrder::sBGRA:
			capabilities.insert(ECapability::Kernel);
			break;
		case EImageChannelOrder::ABGR:
			capabilities.insert(ECapability::Kernel);
			break;
		default: break;
		}
	}

	void RequireCapExtImageChannelDataType([[maybe_unused]] EImageChannelDataType value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case EImageChannelDataType::SnormInt8:
			capabilities.insert(ECapability::Kernel);
			break;
		case EImageChannelDataType::SnormInt16:
			capabilities.insert(ECapability::Kernel);
			break;
		case EImageChannelDataType::UnormInt8:
			capabilities.insert(ECapability::Kernel);
			break;
		case EImageChannelDataType::UnormInt16:
			capabilities.insert(ECapability::Kernel);
			break;
		case EImageChannelDataType::UnormShort565:
			capabilities.insert(ECapability::Kernel);
			break;
		case EImageChannelDataType::UnormShort555:
			capabilities.insert(ECapability::Kernel);
			break;
		case EImageChannelDataType::UnormInt101010:
			capabilities.insert(ECapability::Kernel);
			break;
		case EImageChannelDataType::SignedInt8:
			capabilities.insert(ECapability::Kernel);
			break;
		case EImageChannelDataType::SignedInt16:
			capabilities.insert(ECapability::Kernel);
			break;
		case EImageChannelDataType::SignedInt32:
			capabilities.insert(ECapability::Kernel);
			break;
		case EImageChannelDataType::UnsignedInt8:
			capabilities.insert(ECapability::Kernel);
			break;
		case EImageChannelDataType::UnsignedInt16:
			capabilities.insert(ECapability::Kernel);
			break;
		case EImageChannelDataType::UnsignedInt32:
			capabilities.insert(ECapability::Kernel);
			break;
		case EImageChannelDataType::HalfFloat:
			capabilities.insert(ECapability::Kernel);
			break;
		case EImageChannelDataType::Float:
			capabilities.insert(ECapability::Kernel);
			break;
		case EImageChannelDataType::UnormInt24:
			capabilities.insert(ECapability::Kernel);
			break;
		case EImageChannelDataType::UnormInt101010_2:
			capabilities.insert(ECapability::Kernel);
			break;
		default: break;
		}
	}

	void RequireCapExtFPRoundingMode([[maybe_unused]] EFPRoundingMode value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
	}

	void RequireCapExtFPDenormMode([[maybe_unused]] EFPDenormMode value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case EFPDenormMode::Preserve:
			capabilities.insert(ECapability::FunctionFloatControlINTEL);
			break;
		case EFPDenormMode::FlushToZero:
			capabilities.insert(ECapability::FunctionFloatControlINTEL);
			break;
		default: break;
		}
	}

	void RequireCapExtQuantizationModes([[maybe_unused]] EQuantizationModes value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case EQuantizationModes::TRN:
			capabilities.insert(ECapability::ArbitraryPrecisionFixedPointINTEL);
			break;
		case EQuantizationModes::TRN_ZERO:
			capabilities.insert(ECapability::ArbitraryPrecisionFixedPointINTEL);
			break;
		case EQuantizationModes::RND:
			capabilities.insert(ECapability::ArbitraryPrecisionFixedPointINTEL);
			break;
		case EQuantizationModes::RND_ZERO:
			capabilities.insert(ECapability::ArbitraryPrecisionFixedPointINTEL);
			break;
		case EQuantizationModes::RND_INF:
			capabilities.insert(ECapability::ArbitraryPrecisionFixedPointINTEL);
			break;
		case EQuantizationModes::RND_MIN_INF:
			capabilities.insert(ECapability::ArbitraryPrecisionFixedPointINTEL);
			break;
		case EQuantizationModes::RND_CONV:
			capabilities.insert(ECapability::ArbitraryPrecisionFixedPointINTEL);
			break;
		case EQuantizationModes::RND_CONV_ODD:
			capabilities.insert(ECapability::ArbitraryPrecisionFixedPointINTEL);
			break;
		default: break;
		}
	}

	void RequireCapExtFPOperationMode([[maybe_unused]] EFPOperationMode value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case EFPOperationMode::IEEE:
			capabilities.insert(ECapability::FunctionFloatControlINTEL);
			break;
		case EFPOperationMode::ALT:
			capabilities.insert(ECapability::FunctionFloatControlINTEL);
			break;
		default: break;
		}
	}

	void RequireCapExtOverflowModes([[maybe_unused]] EOverflowModes value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case EOverflowModes::WRAP:
			capabilities.insert(ECapability::ArbitraryPrecisionFixedPointINTEL);
			break;
		case EOverflowModes::SAT:
			capabilities.insert(ECapability::ArbitraryPrecisionFixedPointINTEL);
			break;
		case EOverflowModes::SAT_ZERO:
			capabilities.insert(ECapability::ArbitraryPrecisionFixedPointINTEL);
			break;
		case EOverflowModes::SAT_SYM:
			capabilities.insert(ECapability::ArbitraryPrecisionFixedPointINTEL);
			break;
		default: break;
		}
	}

	void RequireCapExtLinkageType([[maybe_unused]] ELinkageType value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case ELinkageType::Export:
			capabilities.insert(ECapability::Linkage);
			break;
		case ELinkageType::Import:
			capabilities.insert(ECapability::Linkage);
			break;
		case ELinkageType::LinkOnceODR:
			capabilities.insert(ECapability::Linkage);
			extensions.insert("SPV_KHR_linkonce_odr");
			break;
		default: break;
		}
	}

	void RequireCapExtAccessQualifier([[maybe_unused]] EAccessQualifier value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case EAccessQualifier::ReadOnly:
			capabilities.insert(ECapability::Kernel);
			break;
		case EAccessQualifier::WriteOnly:
			capabilities.insert(ECapability::Kernel);
			break;
		case EAccessQualifier::ReadWrite:
			capabilities.insert(ECapability::Kernel);
			break;
		default: break;
		}
	}

	void RequireCapExtFunctionParameterAttribute([[maybe_unused]] EFunctionParameterAttribute value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case EFunctionParameterAttribute::Zext:
			capabilities.insert(ECapability::Kernel);
			break;
		case EFunctionParameterAttribute::Sext:
			capabilities.insert(ECapability::Kernel);
			break;
		case EFunctionParameterAttribute::ByVal:
			capabilities.insert(ECapability::Kernel);
			break;
		case EFunctionParameterAttribute::Sret:
			capabilities.insert(ECapability::Kernel);
			break;
		case EFunctionParameterAttribute::NoAlias:
			capabilities.insert(ECapability::Kernel);
			break;
		case EFunctionParameterAttribute::NoCapture:
			capabilities.insert(ECapability::Kernel);
			break;
		case EFunctionParameterAttribute::NoWrite:
			capabilities.insert(ECapability::Kernel);
			break;
		case EFunctionParameterAttribute::NoReadWrite:
			capabilities.insert(ECapability::Kernel);
			break;
		default: break;
		}
	}

	void RequireCapExtDecoration([[maybe_unused]] EDecoration value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case EDecoration::RelaxedPrecision:
			capabilities.insert(ECapability::Shader);
			break;
		case EDecoration::SpecId:
			capabilities.insert(ECapability::Shader);
			capabilities.insert(ECapability::Kernel);
			break;
		case EDecoration::Block:
			capabilities.insert(ECapability::Shader);
			break;
		case EDecoration::BufferBlock:
			capabilities.insert(ECapability::Shader);
			break;
		case EDecoration::RowMajor:
			capabilities.insert(ECapability::Matrix);
			break;
		case EDecoration::ColMajor:
			capabilities.insert(ECapability::Matrix);
			break;
		case EDecoration::ArrayStride:
			capabilities.insert(ECapability::Shader);
			break;
		case EDecoration::MatrixStride:
			capabilities.insert(ECapability::Matrix);
			break;
		case EDecoration::GLSLShared:
			capabilities.insert(ECapability::Shader);
			break;
		case EDecoration::GLSLPacked:
			capabilities.insert(ECapability::Shader);
			break;
		case EDecoration::CPacked:
			capabilities.insert(ECapability::Kernel);
			break;
		case EDecoration::NoPerspective:
			capabilities.insert(ECapability::Shader);
			break;
		case EDecoration::Flat:
			capabilities.insert(ECapability::Shader);
			break;
		case EDecoration::Patch:
			capabilities.insert(ECapability::Tessellation);
			break;
		case EDecoration::Centroid:
			capabilities.insert(ECapability::Shader);
			break;
		case EDecoration::Sample:
			capabilities.insert(ECapability::SampleRateShading);
			break;
		case EDecoration::Invariant:
			capabilities.insert(ECapability::Shader);
			break;
		case EDecoration::Constant:
			capabilities.insert(ECapability::Kernel);
			break;
		case EDecoration::Uniform:
			capabilities.insert(ECapability::Shader);
			capabilities.insert(ECapability::UniformDecoration);
			break;
		case EDecoration::UniformId:
			capabilities.insert(ECapability::Shader);
			capabilities.insert(ECapability::UniformDecoration);
			break;
		case EDecoration::SaturatedConversion:
			capabilities.insert(ECapability::Kernel);
			break;
		case EDecoration::Stream:
			capabilities.insert(ECapability::GeometryStreams);
			break;
		case EDecoration::Location:
			capabilities.insert(ECapability::Shader);
			break;
		case EDecoration::Component:
			capabilities.insert(ECapability::Shader);
			break;
		case EDecoration::Index:
			capabilities.insert(ECapability::Shader);
			break;
		case EDecoration::Binding:
			capabilities.insert(ECapability::Shader);
			break;
		case EDecoration::DescriptorSet:
			capabilities.insert(ECapability::Shader);
			break;
		case EDecoration::Offset:
			capabilities.insert(ECapability::Shader);
			break;
		case EDecoration::XfbBuffer:
			capabilities.insert(ECapability::TransformFeedback);
			break;
		case EDecoration::XfbStride:
			capabilities.insert(ECapability::TransformFeedback);
			break;
		case EDecoration::FuncParamAttr:
			capabilities.insert(ECapability::Kernel);
			break;
		case EDecoration::FPFastMathMode:
			capabilities.insert(ECapability::Kernel);
			break;
		case EDecoration::LinkageAttributes:
			capabilities.insert(ECapability::Linkage);
			break;
		case EDecoration::NoContraction:
			capabilities.insert(ECapability::Shader);
			break;
		case EDecoration::InputAttachmentIndex:
			capabilities.insert(ECapability::InputAttachment);
			break;
		case EDecoration::Alignment:
			capabilities.insert(ECapability::Kernel);
			break;
		case EDecoration::MaxByteOffset:
			capabilities.insert(ECapability::Addresses);
			break;
		case EDecoration::AlignmentId:
			capabilities.insert(ECapability::Kernel);
			break;
		case EDecoration::MaxByteOffsetId:
			capabilities.insert(ECapability::Addresses);
			break;
		case EDecoration::NoSignedWrap:
			extensions.insert("SPV_KHR_no_integer_wrap_decoration");
			break;
		case EDecoration::NoUnsignedWrap:
			extensions.insert("SPV_KHR_no_integer_wrap_decoration");
			break;
		case EDecoration::ExplicitInterpAMD:
			extensions.insert("SPV_AMD_shader_explicit_vertex_parameter");
			break;
		case EDecoration::OverrideCoverageNV:
			capabilities.insert(ECapability::SampleMaskOverrideCoverageNV);
			extensions.insert("SPV_NV_sample_mask_override_coverage");
			break;
		case EDecoration::PassthroughNV:
			capabilities.insert(ECapability::GeometryShaderPassthroughNV);
			extensions.insert("SPV_NV_geometry_shader_passthrough");
			break;
		case EDecoration::ViewportRelativeNV:
			capabilities.insert(ECapability::ShaderViewportMaskNV);
			break;
		case EDecoration::SecondaryViewportRelativeNV:
			capabilities.insert(ECapability::ShaderStereoViewNV);
			extensions.insert("SPV_NV_stereo_view_rendering");
			break;
		case EDecoration::PerViewNV:
			capabilities.insert(ECapability::MeshShadingNV);
			extensions.insert("SPV_NV_mesh_shader");
			break;
		case EDecoration::PerTaskNV:
			capabilities.insert(ECapability::MeshShadingNV);
			capabilities.insert(ECapability::MeshShadingEXT);
			extensions.insert("SPV_NV_mesh_shader");
			extensions.insert("SPV_EXT_mesh_shader");
			break;
		case EDecoration::PerVertexKHR:
			capabilities.insert(ECapability::FragmentBarycentricKHR);
			capabilities.insert(ECapability::FragmentBarycentricKHR);
			extensions.insert("SPV_NV_fragment_shader_barycentric");
			extensions.insert("SPV_KHR_fragment_shader_barycentric");
			break;
		case EDecoration::NonUniform:
			capabilities.insert(ECapability::ShaderNonUniform);
			break;
		case EDecoration::RestrictPointer:
			capabilities.insert(ECapability::PhysicalStorageBufferAddresses);
			extensions.insert("SPV_EXT_physical_storage_buffer");
			extensions.insert("SPV_KHR_physical_storage_buffer");
			break;
		case EDecoration::AliasedPointer:
			capabilities.insert(ECapability::PhysicalStorageBufferAddresses);
			extensions.insert("SPV_EXT_physical_storage_buffer");
			extensions.insert("SPV_KHR_physical_storage_buffer");
			break;
		case EDecoration::BindlessSamplerNV:
			capabilities.insert(ECapability::BindlessTextureNV);
			break;
		case EDecoration::BindlessImageNV:
			capabilities.insert(ECapability::BindlessTextureNV);
			break;
		case EDecoration::BoundSamplerNV:
			capabilities.insert(ECapability::BindlessTextureNV);
			break;
		case EDecoration::BoundImageNV:
			capabilities.insert(ECapability::BindlessTextureNV);
			break;
		case EDecoration::SIMTCallINTEL:
			capabilities.insert(ECapability::VectorComputeINTEL);
			break;
		case EDecoration::ReferencedIndirectlyINTEL:
			capabilities.insert(ECapability::IndirectReferencesINTEL);
			extensions.insert("SPV_INTEL_function_pointers");
			break;
		case EDecoration::ClobberINTEL:
			capabilities.insert(ECapability::AsmINTEL);
			break;
		case EDecoration::SideEffectsINTEL:
			capabilities.insert(ECapability::AsmINTEL);
			break;
		case EDecoration::VectorComputeVariableINTEL:
			capabilities.insert(ECapability::VectorComputeINTEL);
			break;
		case EDecoration::FuncParamIOKindINTEL:
			capabilities.insert(ECapability::VectorComputeINTEL);
			break;
		case EDecoration::VectorComputeFunctionINTEL:
			capabilities.insert(ECapability::VectorComputeINTEL);
			break;
		case EDecoration::StackCallINTEL:
			capabilities.insert(ECapability::VectorComputeINTEL);
			break;
		case EDecoration::GlobalVariableOffsetINTEL:
			capabilities.insert(ECapability::VectorComputeINTEL);
			break;
		case EDecoration::UserTypeGOOGLE:
			extensions.insert("SPV_GOOGLE_user_type");
			break;
		case EDecoration::FunctionRoundingModeINTEL:
			capabilities.insert(ECapability::FunctionFloatControlINTEL);
			break;
		case EDecoration::FunctionDenormModeINTEL:
			capabilities.insert(ECapability::FunctionFloatControlINTEL);
			break;
		case EDecoration::RegisterINTEL:
			capabilities.insert(ECapability::FPGAMemoryAttributesINTEL);
			extensions.insert("SPV_INTEL_fpga_memory_attributes");
			break;
		case EDecoration::MemoryINTEL:
			capabilities.insert(ECapability::FPGAMemoryAttributesINTEL);
			extensions.insert("SPV_INTEL_fpga_memory_attributes");
			break;
		case EDecoration::NumbanksINTEL:
			capabilities.insert(ECapability::FPGAMemoryAttributesINTEL);
			extensions.insert("SPV_INTEL_fpga_memory_attributes");
			break;
		case EDecoration::BankwidthINTEL:
			capabilities.insert(ECapability::FPGAMemoryAttributesINTEL);
			extensions.insert("SPV_INTEL_fpga_memory_attributes");
			break;
		case EDecoration::MaxPrivateCopiesINTEL:
			capabilities.insert(ECapability::FPGAMemoryAttributesINTEL);
			extensions.insert("SPV_INTEL_fpga_memory_attributes");
			break;
		case EDecoration::SinglepumpINTEL:
			capabilities.insert(ECapability::FPGAMemoryAttributesINTEL);
			extensions.insert("SPV_INTEL_fpga_memory_attributes");
			break;
		case EDecoration::DoublepumpINTEL:
			capabilities.insert(ECapability::FPGAMemoryAttributesINTEL);
			extensions.insert("SPV_INTEL_fpga_memory_attributes");
			break;
		case EDecoration::MaxReplicatesINTEL:
			capabilities.insert(ECapability::FPGAMemoryAttributesINTEL);
			extensions.insert("SPV_INTEL_fpga_memory_attributes");
			break;
		case EDecoration::SimpleDualPortINTEL:
			capabilities.insert(ECapability::FPGAMemoryAttributesINTEL);
			extensions.insert("SPV_INTEL_fpga_memory_attributes");
			break;
		case EDecoration::MergeINTEL:
			capabilities.insert(ECapability::FPGAMemoryAttributesINTEL);
			extensions.insert("SPV_INTEL_fpga_memory_attributes");
			break;
		case EDecoration::BankBitsINTEL:
			capabilities.insert(ECapability::FPGAMemoryAttributesINTEL);
			extensions.insert("SPV_INTEL_fpga_memory_attributes");
			break;
		case EDecoration::ForcePow2DepthINTEL:
			capabilities.insert(ECapability::FPGAMemoryAttributesINTEL);
			extensions.insert("SPV_INTEL_fpga_memory_attributes");
			break;
		case EDecoration::BurstCoalesceINTEL:
			capabilities.insert(ECapability::FPGAMemoryAccessesINTEL);
			break;
		case EDecoration::CacheSizeINTEL:
			capabilities.insert(ECapability::FPGAMemoryAccessesINTEL);
			break;
		case EDecoration::DontStaticallyCoalesceINTEL:
			capabilities.insert(ECapability::FPGAMemoryAccessesINTEL);
			break;
		case EDecoration::PrefetchINTEL:
			capabilities.insert(ECapability::FPGAMemoryAccessesINTEL);
			break;
		case EDecoration::StallEnableINTEL:
			capabilities.insert(ECapability::FPGAClusterAttributesINTEL);
			break;
		case EDecoration::FuseLoopsInFunctionINTEL:
			capabilities.insert(ECapability::LoopFuseINTEL);
			break;
		case EDecoration::AliasScopeINTEL:
			capabilities.insert(ECapability::MemoryAccessAliasingINTEL);
			break;
		case EDecoration::NoAliasINTEL:
			capabilities.insert(ECapability::MemoryAccessAliasingINTEL);
			break;
		case EDecoration::BufferLocationINTEL:
			capabilities.insert(ECapability::FPGABufferLocationINTEL);
			break;
		case EDecoration::IOPipeStorageINTEL:
			capabilities.insert(ECapability::IOPipesINTEL);
			break;
		case EDecoration::FunctionFloatingPointModeINTEL:
			capabilities.insert(ECapability::FunctionFloatControlINTEL);
			break;
		case EDecoration::SingleElementVectorINTEL:
			capabilities.insert(ECapability::VectorComputeINTEL);
			break;
		case EDecoration::VectorComputeCallableFunctionINTEL:
			capabilities.insert(ECapability::VectorComputeINTEL);
			break;
		case EDecoration::MediaBlockIOINTEL:
			capabilities.insert(ECapability::VectorComputeINTEL);
			break;
		default: break;
		}
	}

	void RequireCapExtBuiltIn([[maybe_unused]] EBuiltIn value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case EBuiltIn::Position:
			capabilities.insert(ECapability::Shader);
			break;
		case EBuiltIn::PointSize:
			capabilities.insert(ECapability::Shader);
			break;
		case EBuiltIn::ClipDistance:
			capabilities.insert(ECapability::ClipDistance);
			break;
		case EBuiltIn::CullDistance:
			capabilities.insert(ECapability::CullDistance);
			break;
		case EBuiltIn::VertexId:
			capabilities.insert(ECapability::Shader);
			break;
		case EBuiltIn::InstanceId:
			capabilities.insert(ECapability::Shader);
			break;
		case EBuiltIn::PrimitiveId:
			capabilities.insert(ECapability::Geometry);
			capabilities.insert(ECapability::Tessellation);
			capabilities.insert(ECapability::RayTracingNV);
			capabilities.insert(ECapability::RayTracingKHR);
			capabilities.insert(ECapability::MeshShadingNV);
			capabilities.insert(ECapability::MeshShadingEXT);
			break;
		case EBuiltIn::InvocationId:
			capabilities.insert(ECapability::Geometry);
			capabilities.insert(ECapability::Tessellation);
			break;
		case EBuiltIn::Layer:
			capabilities.insert(ECapability::Geometry);
			capabilities.insert(ECapability::ShaderLayer);
			capabilities.insert(ECapability::ShaderViewportIndexLayerEXT);
			capabilities.insert(ECapability::MeshShadingNV);
			capabilities.insert(ECapability::MeshShadingEXT);
			break;
		case EBuiltIn::ViewportIndex:
			capabilities.insert(ECapability::MultiViewport);
			capabilities.insert(ECapability::ShaderViewportIndex);
			capabilities.insert(ECapability::ShaderViewportIndexLayerEXT);
			capabilities.insert(ECapability::MeshShadingNV);
			capabilities.insert(ECapability::MeshShadingEXT);
			break;
		case EBuiltIn::TessLevelOuter:
			capabilities.insert(ECapability::Tessellation);
			break;
		case EBuiltIn::TessLevelInner:
			capabilities.insert(ECapability::Tessellation);
			break;
		case EBuiltIn::TessCoord:
			capabilities.insert(ECapability::Tessellation);
			break;
		case EBuiltIn::PatchVertices:
			capabilities.insert(ECapability::Tessellation);
			break;
		case EBuiltIn::FragCoord:
			capabilities.insert(ECapability::Shader);
			break;
		case EBuiltIn::PointCoord:
			capabilities.insert(ECapability::Shader);
			break;
		case EBuiltIn::FrontFacing:
			capabilities.insert(ECapability::Shader);
			break;
		case EBuiltIn::SampleId:
			capabilities.insert(ECapability::SampleRateShading);
			break;
		case EBuiltIn::SamplePosition:
			capabilities.insert(ECapability::SampleRateShading);
			break;
		case EBuiltIn::SampleMask:
			capabilities.insert(ECapability::Shader);
			break;
		case EBuiltIn::FragDepth:
			capabilities.insert(ECapability::Shader);
			break;
		case EBuiltIn::HelperInvocation:
			capabilities.insert(ECapability::Shader);
			break;
		case EBuiltIn::WorkDim:
			capabilities.insert(ECapability::Kernel);
			break;
		case EBuiltIn::GlobalSize:
			capabilities.insert(ECapability::Kernel);
			break;
		case EBuiltIn::EnqueuedWorkgroupSize:
			capabilities.insert(ECapability::Kernel);
			break;
		case EBuiltIn::GlobalOffset:
			capabilities.insert(ECapability::Kernel);
			break;
		case EBuiltIn::GlobalLinearId:
			capabilities.insert(ECapability::Kernel);
			break;
		case EBuiltIn::SubgroupSize:
			capabilities.insert(ECapability::Kernel);
			capabilities.insert(ECapability::GroupNonUniform);
			capabilities.insert(ECapability::SubgroupBallotKHR);
			break;
		case EBuiltIn::SubgroupMaxSize:
			capabilities.insert(ECapability::Kernel);
			break;
		case EBuiltIn::NumSubgroups:
			capabilities.insert(ECapability::Kernel);
			capabilities.insert(ECapability::GroupNonUniform);
			break;
		case EBuiltIn::NumEnqueuedSubgroups:
			capabilities.insert(ECapability::Kernel);
			break;
		case EBuiltIn::SubgroupId:
			capabilities.insert(ECapability::Kernel);
			capabilities.insert(ECapability::GroupNonUniform);
			break;
		case EBuiltIn::SubgroupLocalInvocationId:
			capabilities.insert(ECapability::Kernel);
			capabilities.insert(ECapability::GroupNonUniform);
			capabilities.insert(ECapability::SubgroupBallotKHR);
			break;
		case EBuiltIn::VertexIndex:
			capabilities.insert(ECapability::Shader);
			break;
		case EBuiltIn::InstanceIndex:
			capabilities.insert(ECapability::Shader);
			break;
		case EBuiltIn::SubgroupEqMask:
			capabilities.insert(ECapability::SubgroupBallotKHR);
			capabilities.insert(ECapability::GroupNonUniformBallot);
			break;
		case EBuiltIn::SubgroupGeMask:
			capabilities.insert(ECapability::SubgroupBallotKHR);
			capabilities.insert(ECapability::GroupNonUniformBallot);
			break;
		case EBuiltIn::SubgroupGtMask:
			capabilities.insert(ECapability::SubgroupBallotKHR);
			capabilities.insert(ECapability::GroupNonUniformBallot);
			break;
		case EBuiltIn::SubgroupLeMask:
			capabilities.insert(ECapability::SubgroupBallotKHR);
			capabilities.insert(ECapability::GroupNonUniformBallot);
			break;
		case EBuiltIn::SubgroupLtMask:
			capabilities.insert(ECapability::SubgroupBallotKHR);
			capabilities.insert(ECapability::GroupNonUniformBallot);
			break;
		case EBuiltIn::BaseVertex:
			capabilities.insert(ECapability::DrawParameters);
			extensions.insert("SPV_KHR_shader_draw_parameters");
			break;
		case EBuiltIn::BaseInstance:
			capabilities.insert(ECapability::DrawParameters);
			extensions.insert("SPV_KHR_shader_draw_parameters");
			break;
		case EBuiltIn::DrawIndex:
			capabilities.insert(ECapability::DrawParameters);
			capabilities.insert(ECapability::MeshShadingNV);
			capabilities.insert(ECapability::MeshShadingEXT);
			extensions.insert("SPV_KHR_shader_draw_parameters");
			extensions.insert("SPV_NV_mesh_shader");
			extensions.insert("SPV_EXT_mesh_shader");
			break;
		case EBuiltIn::PrimitiveShadingRateKHR:
			capabilities.insert(ECapability::FragmentShadingRateKHR);
			extensions.insert("SPV_KHR_fragment_shading_rate");
			break;
		case EBuiltIn::DeviceIndex:
			capabilities.insert(ECapability::DeviceGroup);
			extensions.insert("SPV_KHR_device_group");
			break;
		case EBuiltIn::ViewIndex:
			capabilities.insert(ECapability::MultiView);
			extensions.insert("SPV_KHR_multiview");
			break;
		case EBuiltIn::ShadingRateKHR:
			capabilities.insert(ECapability::FragmentShadingRateKHR);
			extensions.insert("SPV_KHR_fragment_shading_rate");
			break;
		case EBuiltIn::BaryCoordNoPerspAMD:
			extensions.insert("SPV_AMD_shader_explicit_vertex_parameter");
			break;
		case EBuiltIn::BaryCoordNoPerspCentroidAMD:
			extensions.insert("SPV_AMD_shader_explicit_vertex_parameter");
			break;
		case EBuiltIn::BaryCoordNoPerspSampleAMD:
			extensions.insert("SPV_AMD_shader_explicit_vertex_parameter");
			break;
		case EBuiltIn::BaryCoordSmoothAMD:
			extensions.insert("SPV_AMD_shader_explicit_vertex_parameter");
			break;
		case EBuiltIn::BaryCoordSmoothCentroidAMD:
			extensions.insert("SPV_AMD_shader_explicit_vertex_parameter");
			break;
		case EBuiltIn::BaryCoordSmoothSampleAMD:
			extensions.insert("SPV_AMD_shader_explicit_vertex_parameter");
			break;
		case EBuiltIn::BaryCoordPullModelAMD:
			extensions.insert("SPV_AMD_shader_explicit_vertex_parameter");
			break;
		case EBuiltIn::FragStencilRefEXT:
			capabilities.insert(ECapability::StencilExportEXT);
			extensions.insert("SPV_EXT_shader_stencil_export");
			break;
		case EBuiltIn::ViewportMaskNV:
			capabilities.insert(ECapability::ShaderViewportMaskNV);
			capabilities.insert(ECapability::MeshShadingNV);
			extensions.insert("SPV_NV_viewport_array2");
			extensions.insert("SPV_NV_mesh_shader");
			break;
		case EBuiltIn::SecondaryPositionNV:
			capabilities.insert(ECapability::ShaderStereoViewNV);
			extensions.insert("SPV_NV_stereo_view_rendering");
			break;
		case EBuiltIn::SecondaryViewportMaskNV:
			capabilities.insert(ECapability::ShaderStereoViewNV);
			extensions.insert("SPV_NV_stereo_view_rendering");
			break;
		case EBuiltIn::PositionPerViewNV:
			capabilities.insert(ECapability::PerViewAttributesNV);
			capabilities.insert(ECapability::MeshShadingNV);
			extensions.insert("SPV_NVX_multiview_per_view_attributes");
			extensions.insert("SPV_NV_mesh_shader");
			break;
		case EBuiltIn::ViewportMaskPerViewNV:
			capabilities.insert(ECapability::PerViewAttributesNV);
			capabilities.insert(ECapability::MeshShadingNV);
			extensions.insert("SPV_NVX_multiview_per_view_attributes");
			extensions.insert("SPV_NV_mesh_shader");
			break;
		case EBuiltIn::FullyCoveredEXT:
			capabilities.insert(ECapability::FragmentFullyCoveredEXT);
			extensions.insert("SPV_EXT_fragment_fully_covered");
			break;
		case EBuiltIn::TaskCountNV:
			capabilities.insert(ECapability::MeshShadingNV);
			extensions.insert("SPV_NV_mesh_shader");
			break;
		case EBuiltIn::PrimitiveCountNV:
			capabilities.insert(ECapability::MeshShadingNV);
			extensions.insert("SPV_NV_mesh_shader");
			break;
		case EBuiltIn::PrimitiveIndicesNV:
			capabilities.insert(ECapability::MeshShadingNV);
			extensions.insert("SPV_NV_mesh_shader");
			break;
		case EBuiltIn::ClipDistancePerViewNV:
			capabilities.insert(ECapability::MeshShadingNV);
			extensions.insert("SPV_NV_mesh_shader");
			break;
		case EBuiltIn::CullDistancePerViewNV:
			capabilities.insert(ECapability::MeshShadingNV);
			extensions.insert("SPV_NV_mesh_shader");
			break;
		case EBuiltIn::LayerPerViewNV:
			capabilities.insert(ECapability::MeshShadingNV);
			extensions.insert("SPV_NV_mesh_shader");
			break;
		case EBuiltIn::MeshViewCountNV:
			capabilities.insert(ECapability::MeshShadingNV);
			extensions.insert("SPV_NV_mesh_shader");
			break;
		case EBuiltIn::MeshViewIndicesNV:
			capabilities.insert(ECapability::MeshShadingNV);
			extensions.insert("SPV_NV_mesh_shader");
			break;
		case EBuiltIn::BaryCoordKHR:
			capabilities.insert(ECapability::FragmentBarycentricKHR);
			capabilities.insert(ECapability::FragmentBarycentricKHR);
			extensions.insert("SPV_NV_fragment_shader_barycentric");
			extensions.insert("SPV_KHR_fragment_shader_barycentric");
			break;
		case EBuiltIn::BaryCoordNoPerspKHR:
			capabilities.insert(ECapability::FragmentBarycentricKHR);
			capabilities.insert(ECapability::FragmentBarycentricKHR);
			extensions.insert("SPV_NV_fragment_shader_barycentric");
			extensions.insert("SPV_KHR_fragment_shader_barycentric");
			break;
		case EBuiltIn::FragSizeEXT:
			capabilities.insert(ECapability::FragmentDensityEXT);
			capabilities.insert(ECapability::FragmentDensityEXT);
			extensions.insert("SPV_EXT_fragment_invocation_density");
			extensions.insert("SPV_NV_shading_rate");
			break;
		case EBuiltIn::FragInvocationCountEXT:
			capabilities.insert(ECapability::FragmentDensityEXT);
			capabilities.insert(ECapability::FragmentDensityEXT);
			extensions.insert("SPV_EXT_fragment_invocation_density");
			extensions.insert("SPV_NV_shading_rate");
			break;
		case EBuiltIn::PrimitivePointIndicesEXT:
			capabilities.insert(ECapability::MeshShadingEXT);
			extensions.insert("SPV_EXT_mesh_shader");
			break;
		case EBuiltIn::PrimitiveLineIndicesEXT:
			capabilities.insert(ECapability::MeshShadingEXT);
			extensions.insert("SPV_EXT_mesh_shader");
			break;
		case EBuiltIn::PrimitiveTriangleIndicesEXT:
			capabilities.insert(ECapability::MeshShadingEXT);
			extensions.insert("SPV_EXT_mesh_shader");
			break;
		case EBuiltIn::CullPrimitiveEXT:
			capabilities.insert(ECapability::MeshShadingEXT);
			extensions.insert("SPV_EXT_mesh_shader");
			break;
		case EBuiltIn::HitTNV:
			capabilities.insert(ECapability::RayTracingNV);
			extensions.insert("SPV_NV_ray_tracing");
			break;
		case EBuiltIn::CurrentRayTimeNV:
			capabilities.insert(ECapability::RayTracingMotionBlurNV);
			extensions.insert("SPV_NV_ray_tracing_motion_blur");
			break;
		case EBuiltIn::RayGeometryIndexKHR:
			capabilities.insert(ECapability::RayTracingKHR);
			extensions.insert("SPV_KHR_ray_tracing");
			break;
		case EBuiltIn::WarpsPerSMNV:
			capabilities.insert(ECapability::ShaderSMBuiltinsNV);
			extensions.insert("SPV_NV_shader_sm_builtins");
			break;
		case EBuiltIn::SMCountNV:
			capabilities.insert(ECapability::ShaderSMBuiltinsNV);
			extensions.insert("SPV_NV_shader_sm_builtins");
			break;
		case EBuiltIn::WarpIDNV:
			capabilities.insert(ECapability::ShaderSMBuiltinsNV);
			extensions.insert("SPV_NV_shader_sm_builtins");
			break;
		case EBuiltIn::SMIDNV:
			capabilities.insert(ECapability::ShaderSMBuiltinsNV);
			extensions.insert("SPV_NV_shader_sm_builtins");
			break;
		case EBuiltIn::CullMaskKHR:
			capabilities.insert(ECapability::RayCullMaskKHR);
			extensions.insert("SPV_KHR_ray_cull_mask");
			break;
		default: break;
		}
	}

	void RequireCapExtScope([[maybe_unused]] EScope value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case EScope::QueueFamily:
			capabilities.insert(ECapability::VulkanMemoryModel);
			break;
		case EScope::ShaderCallKHR:
			capabilities.insert(ECapability::RayTracingKHR);
			break;
		default: break;
		}
	}

	void RequireCapExtGroupOperation([[maybe_unused]] EGroupOperation value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case EGroupOperation::Reduce:
			capabilities.insert(ECapability::Kernel);
			capabilities.insert(ECapability::GroupNonUniformArithmetic);
			capabilities.insert(ECapability::GroupNonUniformBallot);
			break;
		case EGroupOperation::InclusiveScan:
			capabilities.insert(ECapability::Kernel);
			capabilities.insert(ECapability::GroupNonUniformArithmetic);
			capabilities.insert(ECapability::GroupNonUniformBallot);
			break;
		case EGroupOperation::ExclusiveScan:
			capabilities.insert(ECapability::Kernel);
			capabilities.insert(ECapability::GroupNonUniformArithmetic);
			capabilities.insert(ECapability::GroupNonUniformBallot);
			break;
		case EGroupOperation::ClusteredReduce:
			capabilities.insert(ECapability::GroupNonUniformClustered);
			break;
		case EGroupOperation::PartitionedReduceNV:
			capabilities.insert(ECapability::GroupNonUniformPartitionedNV);
			extensions.insert("SPV_NV_shader_subgroup_partitioned");
			break;
		case EGroupOperation::PartitionedInclusiveScanNV:
			capabilities.insert(ECapability::GroupNonUniformPartitionedNV);
			extensions.insert("SPV_NV_shader_subgroup_partitioned");
			break;
		case EGroupOperation::PartitionedExclusiveScanNV:
			capabilities.insert(ECapability::GroupNonUniformPartitionedNV);
			extensions.insert("SPV_NV_shader_subgroup_partitioned");
			break;
		default: break;
		}
	}

	void RequireCapExtKernelEnqueueFlags([[maybe_unused]] EKernelEnqueueFlags value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case EKernelEnqueueFlags::NoWait:
			capabilities.insert(ECapability::Kernel);
			break;
		case EKernelEnqueueFlags::WaitKernel:
			capabilities.insert(ECapability::Kernel);
			break;
		case EKernelEnqueueFlags::WaitWorkGroup:
			capabilities.insert(ECapability::Kernel);
			break;
		default: break;
		}
	}

	void RequireCapExtCapability([[maybe_unused]] ECapability value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case ECapability::Shader:
			capabilities.insert(ECapability::Matrix);
			break;
		case ECapability::Geometry:
			capabilities.insert(ECapability::Shader);
			break;
		case ECapability::Tessellation:
			capabilities.insert(ECapability::Shader);
			break;
		case ECapability::Vector16:
			capabilities.insert(ECapability::Kernel);
			break;
		case ECapability::Float16Buffer:
			capabilities.insert(ECapability::Kernel);
			break;
		case ECapability::Int64Atomics:
			capabilities.insert(ECapability::Int64);
			break;
		case ECapability::ImageBasic:
			capabilities.insert(ECapability::Kernel);
			break;
		case ECapability::ImageReadWrite:
			capabilities.insert(ECapability::ImageBasic);
			break;
		case ECapability::ImageMipmap:
			capabilities.insert(ECapability::ImageBasic);
			break;
		case ECapability::Pipes:
			capabilities.insert(ECapability::Kernel);
			break;
		case ECapability::Groups:
			extensions.insert("SPV_AMD_shader_ballot");
			break;
		case ECapability::DeviceEnqueue:
			capabilities.insert(ECapability::Kernel);
			break;
		case ECapability::LiteralSampler:
			capabilities.insert(ECapability::Kernel);
			break;
		case ECapability::AtomicStorage:
			capabilities.insert(ECapability::Shader);
			break;
		case ECapability::TessellationPointSize:
			capabilities.insert(ECapability::Tessellation);
			break;
		case ECapability::GeometryPointSize:
			capabilities.insert(ECapability::Geometry);
			break;
		case ECapability::ImageGatherExtended:
			capabilities.insert(ECapability::Shader);
			break;
		case ECapability::StorageImageMultisample:
			capabilities.insert(ECapability::Shader);
			break;
		case ECapability::UniformBufferArrayDynamicIndexing:
			capabilities.insert(ECapability::Shader);
			break;
		case ECapability::SampledImageArrayDynamicIndexing:
			capabilities.insert(ECapability::Shader);
			break;
		case ECapability::StorageBufferArrayDynamicIndexing:
			capabilities.insert(ECapability::Shader);
			break;
		case ECapability::StorageImageArrayDynamicIndexing:
			capabilities.insert(ECapability::Shader);
			break;
		case ECapability::ClipDistance:
			capabilities.insert(ECapability::Shader);
			break;
		case ECapability::CullDistance:
			capabilities.insert(ECapability::Shader);
			break;
		case ECapability::ImageCubeArray:
			capabilities.insert(ECapability::SampledCubeArray);
			break;
		case ECapability::SampleRateShading:
			capabilities.insert(ECapability::Shader);
			break;
		case ECapability::ImageRect:
			capabilities.insert(ECapability::SampledRect);
			break;
		case ECapability::SampledRect:
			capabilities.insert(ECapability::Shader);
			break;
		case ECapability::GenericPointer:
			capabilities.insert(ECapability::Addresses);
			break;
		case ECapability::InputAttachment:
			capabilities.insert(ECapability::Shader);
			break;
		case ECapability::SparseResidency:
			capabilities.insert(ECapability::Shader);
			break;
		case ECapability::MinLod:
			capabilities.insert(ECapability::Shader);
			break;
		case ECapability::Image1D:
			capabilities.insert(ECapability::Sampled1D);
			break;
		case ECapability::SampledCubeArray:
			capabilities.insert(ECapability::Shader);
			break;
		case ECapability::ImageBuffer:
			capabilities.insert(ECapability::SampledBuffer);
			break;
		case ECapability::ImageMSArray:
			capabilities.insert(ECapability::Shader);
			break;
		case ECapability::StorageImageExtendedFormats:
			capabilities.insert(ECapability::Shader);
			break;
		case ECapability::ImageQuery:
			capabilities.insert(ECapability::Shader);
			break;
		case ECapability::DerivativeControl:
			capabilities.insert(ECapability::Shader);
			break;
		case ECapability::InterpolationFunction:
			capabilities.insert(ECapability::Shader);
			break;
		case ECapability::TransformFeedback:
			capabilities.insert(ECapability::Shader);
			break;
		case ECapability::GeometryStreams:
			capabilities.insert(ECapability::Geometry);
			break;
		case ECapability::StorageImageReadWithoutFormat:
			capabilities.insert(ECapability::Shader);
			break;
		case ECapability::StorageImageWriteWithoutFormat:
			capabilities.insert(ECapability::Shader);
			break;
		case ECapability::MultiViewport:
			capabilities.insert(ECapability::Geometry);
			break;
		case ECapability::SubgroupDispatch:
			capabilities.insert(ECapability::DeviceEnqueue);
			break;
		case ECapability::NamedBarrier:
			capabilities.insert(ECapability::Kernel);
			break;
		case ECapability::PipeStorage:
			capabilities.insert(ECapability::Pipes);
			break;
		case ECapability::GroupNonUniformVote:
			capabilities.insert(ECapability::GroupNonUniform);
			break;
		case ECapability::GroupNonUniformArithmetic:
			capabilities.insert(ECapability::GroupNonUniform);
			break;
		case ECapability::GroupNonUniformBallot:
			capabilities.insert(ECapability::GroupNonUniform);
			break;
		case ECapability::GroupNonUniformShuffle:
			capabilities.insert(ECapability::GroupNonUniform);
			break;
		case ECapability::GroupNonUniformShuffleRelative:
			capabilities.insert(ECapability::GroupNonUniform);
			break;
		case ECapability::GroupNonUniformClustered:
			capabilities.insert(ECapability::GroupNonUniform);
			break;
		case ECapability::GroupNonUniformQuad:
			capabilities.insert(ECapability::GroupNonUniform);
			break;
		case ECapability::FragmentShadingRateKHR:
			capabilities.insert(ECapability::Shader);
			extensions.insert("SPV_KHR_fragment_shading_rate");
			break;
		case ECapability::SubgroupBallotKHR:
			extensions.insert("SPV_KHR_shader_ballot");
			break;
		case ECapability::DrawParameters:
			capabilities.insert(ECapability::Shader);
			extensions.insert("SPV_KHR_shader_draw_parameters");
			break;
		case ECapability::WorkgroupMemoryExplicitLayoutKHR:
			capabilities.insert(ECapability::Shader);
			extensions.insert("SPV_KHR_workgroup_memory_explicit_layout");
			break;
		case ECapability::WorkgroupMemoryExplicitLayout8BitAccessKHR:
			capabilities.insert(ECapability::WorkgroupMemoryExplicitLayoutKHR);
			extensions.insert("SPV_KHR_workgroup_memory_explicit_layout");
			break;
		case ECapability::WorkgroupMemoryExplicitLayout16BitAccessKHR:
			capabilities.insert(ECapability::Shader);
			extensions.insert("SPV_KHR_workgroup_memory_explicit_layout");
			break;
		case ECapability::SubgroupVoteKHR:
			extensions.insert("SPV_KHR_subgroup_vote");
			break;
		case ECapability::StoragePushConstant16:
			extensions.insert("SPV_KHR_16bit_storage");
			break;
		case ECapability::StorageInputOutput16:
			extensions.insert("SPV_KHR_16bit_storage");
			break;
		case ECapability::DeviceGroup:
			extensions.insert("SPV_KHR_device_group");
			break;
		case ECapability::MultiView:
			capabilities.insert(ECapability::Shader);
			extensions.insert("SPV_KHR_multiview");
			break;
		case ECapability::VariablePointersStorageBuffer:
			capabilities.insert(ECapability::Shader);
			extensions.insert("SPV_KHR_variable_pointers");
			break;
		case ECapability::VariablePointers:
			capabilities.insert(ECapability::VariablePointersStorageBuffer);
			extensions.insert("SPV_KHR_variable_pointers");
			break;
		case ECapability::AtomicStorageOps:
			extensions.insert("SPV_KHR_shader_atomic_counter_ops");
			break;
		case ECapability::SampleMaskPostDepthCoverage:
			extensions.insert("SPV_KHR_post_depth_coverage");
			break;
		case ECapability::StorageBuffer8BitAccess:
			extensions.insert("SPV_KHR_8bit_storage");
			break;
		case ECapability::UniformAndStorageBuffer8BitAccess:
			capabilities.insert(ECapability::StorageBuffer8BitAccess);
			extensions.insert("SPV_KHR_8bit_storage");
			break;
		case ECapability::StoragePushConstant8:
			extensions.insert("SPV_KHR_8bit_storage");
			break;
		case ECapability::DenormPreserve:
			extensions.insert("SPV_KHR_float_controls");
			break;
		case ECapability::DenormFlushToZero:
			extensions.insert("SPV_KHR_float_controls");
			break;
		case ECapability::SignedZeroInfNanPreserve:
			extensions.insert("SPV_KHR_float_controls");
			break;
		case ECapability::RoundingModeRTE:
			extensions.insert("SPV_KHR_float_controls");
			break;
		case ECapability::RoundingModeRTZ:
			extensions.insert("SPV_KHR_float_controls");
			break;
		case ECapability::RayQueryProvisionalKHR:
			capabilities.insert(ECapability::Shader);
			extensions.insert("SPV_KHR_ray_query");
			break;
		case ECapability::RayQueryKHR:
			capabilities.insert(ECapability::Shader);
			extensions.insert("SPV_KHR_ray_query");
			break;
		case ECapability::RayTraversalPrimitiveCullingKHR:
			capabilities.insert(ECapability::RayQueryKHR);
			capabilities.insert(ECapability::RayTracingKHR);
			extensions.insert("SPV_KHR_ray_query");
			extensions.insert("SPV_KHR_ray_tracing");
			break;
		case ECapability::RayTracingKHR:
			capabilities.insert(ECapability::Shader);
			extensions.insert("SPV_KHR_ray_tracing");
			break;
		case ECapability::Float16ImageAMD:
			capabilities.insert(ECapability::Shader);
			extensions.insert("SPV_AMD_gpu_shader_half_float_fetch");
			break;
		case ECapability::ImageGatherBiasLodAMD:
			capabilities.insert(ECapability::Shader);
			extensions.insert("SPV_AMD_texture_gather_bias_lod");
			break;
		case ECapability::FragmentMaskAMD:
			capabilities.insert(ECapability::Shader);
			extensions.insert("SPV_AMD_shader_fragment_mask");
			break;
		case ECapability::StencilExportEXT:
			capabilities.insert(ECapability::Shader);
			extensions.insert("SPV_EXT_shader_stencil_export");
			break;
		case ECapability::ImageReadWriteLodAMD:
			capabilities.insert(ECapability::Shader);
			extensions.insert("SPV_AMD_shader_image_load_store_lod");
			break;
		case ECapability::Int64ImageEXT:
			capabilities.insert(ECapability::Shader);
			extensions.insert("SPV_EXT_shader_image_int64");
			break;
		case ECapability::ShaderClockKHR:
			capabilities.insert(ECapability::Shader);
			extensions.insert("SPV_KHR_shader_clock");
			break;
		case ECapability::SampleMaskOverrideCoverageNV:
			capabilities.insert(ECapability::SampleRateShading);
			extensions.insert("SPV_NV_sample_mask_override_coverage");
			break;
		case ECapability::GeometryShaderPassthroughNV:
			capabilities.insert(ECapability::Geometry);
			extensions.insert("SPV_NV_geometry_shader_passthrough");
			break;
		case ECapability::ShaderViewportIndexLayerEXT:
			capabilities.insert(ECapability::MultiViewport);
			extensions.insert("SPV_EXT_shader_viewport_index_layer");
			break;
		case ECapability::ShaderViewportMaskNV:
			capabilities.insert(ECapability::ShaderViewportIndexLayerEXT);
			extensions.insert("SPV_NV_viewport_array2");
			break;
		case ECapability::ShaderStereoViewNV:
			capabilities.insert(ECapability::ShaderViewportMaskNV);
			extensions.insert("SPV_NV_stereo_view_rendering");
			break;
		case ECapability::PerViewAttributesNV:
			capabilities.insert(ECapability::MultiView);
			extensions.insert("SPV_NVX_multiview_per_view_attributes");
			break;
		case ECapability::FragmentFullyCoveredEXT:
			capabilities.insert(ECapability::Shader);
			extensions.insert("SPV_EXT_fragment_fully_covered");
			break;
		case ECapability::MeshShadingNV:
			capabilities.insert(ECapability::Shader);
			extensions.insert("SPV_NV_mesh_shader");
			break;
		case ECapability::ImageFootprintNV:
			extensions.insert("SPV_NV_shader_image_footprint");
			break;
		case ECapability::MeshShadingEXT:
			capabilities.insert(ECapability::Shader);
			extensions.insert("SPV_EXT_mesh_shader");
			break;
		case ECapability::FragmentBarycentricKHR:
			extensions.insert("SPV_NV_fragment_shader_barycentric");
			extensions.insert("SPV_KHR_fragment_shader_barycentric");
			break;
		case ECapability::ComputeDerivativeGroupQuadsNV:
			extensions.insert("SPV_NV_compute_shader_derivatives");
			break;
		case ECapability::FragmentDensityEXT:
			capabilities.insert(ECapability::Shader);
			extensions.insert("SPV_EXT_fragment_invocation_density");
			extensions.insert("SPV_NV_shading_rate");
			break;
		case ECapability::GroupNonUniformPartitionedNV:
			extensions.insert("SPV_NV_shader_subgroup_partitioned");
			break;
		case ECapability::ShaderNonUniform:
			capabilities.insert(ECapability::Shader);
			break;
		case ECapability::RuntimeDescriptorArray:
			capabilities.insert(ECapability::Shader);
			break;
		case ECapability::InputAttachmentArrayDynamicIndexing:
			capabilities.insert(ECapability::InputAttachment);
			break;
		case ECapability::UniformTexelBufferArrayDynamicIndexing:
			capabilities.insert(ECapability::SampledBuffer);
			break;
		case ECapability::StorageTexelBufferArrayDynamicIndexing:
			capabilities.insert(ECapability::ImageBuffer);
			break;
		case ECapability::UniformBufferArrayNonUniformIndexing:
			capabilities.insert(ECapability::ShaderNonUniform);
			break;
		case ECapability::SampledImageArrayNonUniformIndexing:
			capabilities.insert(ECapability::ShaderNonUniform);
			break;
		case ECapability::StorageBufferArrayNonUniformIndexing:
			capabilities.insert(ECapability::ShaderNonUniform);
			break;
		case ECapability::StorageImageArrayNonUniformIndexing:
			capabilities.insert(ECapability::ShaderNonUniform);
			break;
		case ECapability::InputAttachmentArrayNonUniformIndexing:
			capabilities.insert(ECapability::InputAttachment);
			capabilities.insert(ECapability::ShaderNonUniform);
			break;
		case ECapability::UniformTexelBufferArrayNonUniformIndexing:
			capabilities.insert(ECapability::SampledBuffer);
			capabilities.insert(ECapability::ShaderNonUniform);
			break;
		case ECapability::StorageTexelBufferArrayNonUniformIndexing:
			capabilities.insert(ECapability::ImageBuffer);
			capabilities.insert(ECapability::ShaderNonUniform);
			break;
		case ECapability::RayTracingNV:
			capabilities.insert(ECapability::Shader);
			extensions.insert("SPV_NV_ray_tracing");
			break;
		case ECapability::RayTracingMotionBlurNV:
			capabilities.insert(ECapability::Shader);
			extensions.insert("SPV_NV_ray_tracing_motion_blur");
			break;
		case ECapability::PhysicalStorageBufferAddresses:
			capabilities.insert(ECapability::Shader);
			extensions.insert("SPV_EXT_physical_storage_buffer");
			extensions.insert("SPV_KHR_physical_storage_buffer");
			break;
		case ECapability::ComputeDerivativeGroupLinearNV:
			extensions.insert("SPV_NV_compute_shader_derivatives");
			break;
		case ECapability::RayTracingProvisionalKHR:
			capabilities.insert(ECapability::Shader);
			extensions.insert("SPV_KHR_ray_tracing");
			break;
		case ECapability::CooperativeMatrixNV:
			capabilities.insert(ECapability::Shader);
			extensions.insert("SPV_NV_cooperative_matrix");
			break;
		case ECapability::FragmentShaderSampleInterlockEXT:
			capabilities.insert(ECapability::Shader);
			extensions.insert("SPV_EXT_fragment_shader_interlock");
			break;
		case ECapability::FragmentShaderShadingRateInterlockEXT:
			capabilities.insert(ECapability::Shader);
			extensions.insert("SPV_EXT_fragment_shader_interlock");
			break;
		case ECapability::ShaderSMBuiltinsNV:
			capabilities.insert(ECapability::Shader);
			extensions.insert("SPV_NV_shader_sm_builtins");
			break;
		case ECapability::FragmentShaderPixelInterlockEXT:
			capabilities.insert(ECapability::Shader);
			extensions.insert("SPV_EXT_fragment_shader_interlock");
			break;
		case ECapability::DemoteToHelperInvocation:
			capabilities.insert(ECapability::Shader);
			break;
		case ECapability::BindlessTextureNV:
			extensions.insert("SPV_NV_bindless_texture");
			break;
		case ECapability::SubgroupShuffleINTEL:
			extensions.insert("SPV_INTEL_subgroups");
			break;
		case ECapability::SubgroupBufferBlockIOINTEL:
			extensions.insert("SPV_INTEL_subgroups");
			break;
		case ECapability::SubgroupImageBlockIOINTEL:
			extensions.insert("SPV_INTEL_subgroups");
			break;
		case ECapability::SubgroupImageMediaBlockIOINTEL:
			extensions.insert("SPV_INTEL_media_block_io");
			break;
		case ECapability::RoundToInfinityINTEL:
			extensions.insert("SPV_INTEL_float_controls2");
			break;
		case ECapability::FloatingPointModeINTEL:
			extensions.insert("SPV_INTEL_float_controls2");
			break;
		case ECapability::IntegerFunctions2INTEL:
			capabilities.insert(ECapability::Shader);
			extensions.insert("SPV_INTEL_shader_integer_functions2");
			break;
		case ECapability::FunctionPointersINTEL:
			extensions.insert("SPV_INTEL_function_pointers");
			break;
		case ECapability::IndirectReferencesINTEL:
			extensions.insert("SPV_INTEL_function_pointers");
			break;
		case ECapability::AsmINTEL:
			extensions.insert("SPV_INTEL_inline_assembly");
			break;
		case ECapability::AtomicFloat32MinMaxEXT:
			extensions.insert("SPV_EXT_shader_atomic_float_min_max");
			break;
		case ECapability::AtomicFloat64MinMaxEXT:
			extensions.insert("SPV_EXT_shader_atomic_float_min_max");
			break;
		case ECapability::AtomicFloat16MinMaxEXT:
			extensions.insert("SPV_EXT_shader_atomic_float_min_max");
			break;
		case ECapability::VectorComputeINTEL:
			capabilities.insert(ECapability::VectorAnyINTEL);
			extensions.insert("SPV_INTEL_vector_compute");
			break;
		case ECapability::VectorAnyINTEL:
			extensions.insert("SPV_INTEL_vector_compute");
			break;
		case ECapability::ExpectAssumeKHR:
			extensions.insert("SPV_KHR_expect_assume");
			break;
		case ECapability::SubgroupAvcMotionEstimationINTEL:
			extensions.insert("SPV_INTEL_device_side_avc_motion_estimation");
			break;
		case ECapability::SubgroupAvcMotionEstimationIntraINTEL:
			extensions.insert("SPV_INTEL_device_side_avc_motion_estimation");
			break;
		case ECapability::SubgroupAvcMotionEstimationChromaINTEL:
			extensions.insert("SPV_INTEL_device_side_avc_motion_estimation");
			break;
		case ECapability::VariableLengthArrayINTEL:
			extensions.insert("SPV_INTEL_variable_length_array");
			break;
		case ECapability::FunctionFloatControlINTEL:
			extensions.insert("SPV_INTEL_float_controls2");
			break;
		case ECapability::FPGAMemoryAttributesINTEL:
			extensions.insert("SPV_INTEL_fpga_memory_attributes");
			break;
		case ECapability::FPFastMathModeINTEL:
			capabilities.insert(ECapability::Kernel);
			extensions.insert("SPV_INTEL_fp_fast_math_mode");
			break;
		case ECapability::ArbitraryPrecisionIntegersINTEL:
			extensions.insert("SPV_INTEL_arbitrary_precision_integers");
			break;
		case ECapability::ArbitraryPrecisionFloatingPointINTEL:
			extensions.insert("SPV_INTEL_arbitrary_precision_floating_point");
			break;
		case ECapability::UnstructuredLoopControlsINTEL:
			extensions.insert("SPV_INTEL_unstructured_loop_controls");
			break;
		case ECapability::FPGALoopControlsINTEL:
			extensions.insert("SPV_INTEL_fpga_loop_controls");
			break;
		case ECapability::KernelAttributesINTEL:
			extensions.insert("SPV_INTEL_kernel_attributes");
			break;
		case ECapability::FPGAKernelAttributesINTEL:
			extensions.insert("SPV_INTEL_kernel_attributes");
			break;
		case ECapability::FPGAMemoryAccessesINTEL:
			extensions.insert("SPV_INTEL_fpga_memory_accesses");
			break;
		case ECapability::FPGAClusterAttributesINTEL:
			extensions.insert("SPV_INTEL_fpga_cluster_attributes");
			break;
		case ECapability::LoopFuseINTEL:
			extensions.insert("SPV_INTEL_loop_fuse");
			break;
		case ECapability::MemoryAccessAliasingINTEL:
			extensions.insert("SPV_INTEL_memory_access_aliasing");
			break;
		case ECapability::FPGABufferLocationINTEL:
			extensions.insert("SPV_INTEL_fpga_buffer_location");
			break;
		case ECapability::ArbitraryPrecisionFixedPointINTEL:
			extensions.insert("SPV_INTEL_arbitrary_precision_fixed_point");
			break;
		case ECapability::USMStorageClassesINTEL:
			extensions.insert("SPV_INTEL_usm_storage_classes");
			break;
		case ECapability::IOPipesINTEL:
			extensions.insert("SPV_INTEL_io_pipes");
			break;
		case ECapability::BlockingPipesINTEL:
			extensions.insert("SPV_INTEL_blocking_pipes");
			break;
		case ECapability::FPGARegINTEL:
			extensions.insert("SPV_INTEL_fpga_reg");
			break;
		case ECapability::DotProductInput4x8Bit:
			capabilities.insert(ECapability::Int8);
			break;
		case ECapability::RayCullMaskKHR:
			extensions.insert("SPV_KHR_ray_cull_mask");
			break;
		case ECapability::BitInstructions:
			extensions.insert("SPV_KHR_bit_instructions");
			break;
		case ECapability::GroupNonUniformRotateKHR:
			capabilities.insert(ECapability::GroupNonUniform);
			extensions.insert("SPV_KHR_subgroup_rotate");
			break;
		case ECapability::AtomicFloat32AddEXT:
			extensions.insert("SPV_EXT_shader_atomic_float_add");
			break;
		case ECapability::AtomicFloat64AddEXT:
			extensions.insert("SPV_EXT_shader_atomic_float_add");
			break;
		case ECapability::LongConstantCompositeINTEL:
			extensions.insert("SPV_INTEL_long_constant_composite");
			break;
		case ECapability::OptNoneINTEL:
			extensions.insert("SPV_INTEL_optnone");
			break;
		case ECapability::AtomicFloat16AddEXT:
			extensions.insert("SPV_EXT_shader_atomic_float16_add");
			break;
		case ECapability::DebugInfoModuleINTEL:
			extensions.insert("SPV_INTEL_debug_module");
			break;
		case ECapability::SplitBarrierINTEL:
			extensions.insert("SPV_INTEL_split_barrier");
			break;
		case ECapability::GroupUniformArithmeticKHR:
			extensions.insert("SPV_KHR_uniform_group_instructions");
			break;
		default: break;
		}
	}

	void RequireCapExtRayQueryIntersection([[maybe_unused]] ERayQueryIntersection value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case ERayQueryIntersection::RayQueryCandidateIntersectionKHR:
			capabilities.insert(ECapability::RayQueryKHR);
			break;
		case ERayQueryIntersection::RayQueryCommittedIntersectionKHR:
			capabilities.insert(ECapability::RayQueryKHR);
			break;
		default: break;
		}
	}

	void RequireCapExtRayQueryCommittedIntersectionType([[maybe_unused]] ERayQueryCommittedIntersectionType value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case ERayQueryCommittedIntersectionType::RayQueryCommittedIntersectionNoneKHR:
			capabilities.insert(ECapability::RayQueryKHR);
			break;
		case ERayQueryCommittedIntersectionType::RayQueryCommittedIntersectionTriangleKHR:
			capabilities.insert(ECapability::RayQueryKHR);
			break;
		case ERayQueryCommittedIntersectionType::RayQueryCommittedIntersectionGeneratedKHR:
			capabilities.insert(ECapability::RayQueryKHR);
			break;
		default: break;
		}
	}

	void RequireCapExtRayQueryCandidateIntersectionType([[maybe_unused]] ERayQueryCandidateIntersectionType value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
		switch (value)
		{
		case ERayQueryCandidateIntersectionType::RayQueryCandidateIntersectionTriangleKHR:
			capabilities.insert(ECapability::RayQueryKHR);
			break;
		case ERayQueryCandidateIntersectionType::RayQueryCandidateIntersectionAABBKHR:
			capabilities.insert(ECapability::RayQueryKHR);
			break;
		default: break;
		}
	}

	void RequireCapExtPackedVectorFormat([[maybe_unused]] EPackedVectorFormat value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)
	{
	}
} // namespace Frertex::Transpilers::SPIRV