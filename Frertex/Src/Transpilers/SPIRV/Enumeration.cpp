#include "Frertex/Transpilers/SPIRV/Enumerations.h"

namespace Frertex::Transpilers::SPIRV
{
	bool IsCapabilityImplicit(ECapability a, ECapability b)
	{
		if (a == b)
			return true;

		switch (a)
		{
		case ECapability::Shader: return b == ECapability::Matrix;
		case ECapability::Geometry: return b == ECapability::Shader;
		case ECapability::Tessellation: return b == ECapability::Shader;
		case ECapability::Vector16: return b == ECapability::Kernel;
		case ECapability::Float16Buffer: return b == ECapability::Kernel;
		case ECapability::Int64Atomics: return b == ECapability::Int64;
		case ECapability::ImageReadWrite: return b == ECapability::ImageBasic;
		case ECapability::ImageMipmap: return b == ECapability::ImageBasic;
		case ECapability::Pipes: return b == ECapability::Kernel;
		case ECapability::DeviceEnqueue: return b == ECapability::Kernel;
		case ECapability::LiteralSampler: return b == ECapability::Kernel;
		case ECapability::AtomicStorage: return b == ECapability::Shader;
		case ECapability::TessellationPointSize: return b == ECapability::Tessellation;
		case ECapability::GeometryPointSize: return b == ECapability::Geometry;
		case ECapability::ImageGatherExtended: return b == ECapability::Shader;
		case ECapability::StorageImageMultisample: return b == ECapability::Shader;
		case ECapability::UniformBufferArrayDynamicIndexing: return b == ECapability::Shader;
		case ECapability::SampledImageArrayDynamicIndexing: return b == ECapability::Shader;
		case ECapability::StorageBufferArrayDynamicIndexing: return b == ECapability::Shader;
		case ECapability::StorageImageArrayDynamicIndexing: return b == ECapability::Shader;
		case ECapability::ClipDistance: return b == ECapability::Shader;
		case ECapability::CullDistance: return b == ECapability::Shader;
		case ECapability::ImageRect: return b == ECapability::SampledCubeArray;
		case ECapability::SampledRect: return b == ECapability::Shader;
		case ECapability::GenericPointer: return b == ECapability::Addresses;
		case ECapability::InputAttachment: return b == ECapability::Shader;
		case ECapability::SparseResidency: return b == ECapability::Shader;
		case ECapability::MinLod: return b == ECapability::Shader;
		case ECapability::Image1D: return b == ECapability::Sampled1D;
		case ECapability::SampledCubeArray: return b == ECapability::Shader;
		case ECapability::ImageBuffer: return b == ECapability::SampledBuffer;
		case ECapability::ImageMSArray: return b == ECapability::Shader;
		case ECapability::StorageImageExtendedFormats: return b == ECapability::Shader;
		case ECapability::ImageQuery: return b == ECapability::Shader;
		case ECapability::DerivativeControl: return b == ECapability::Shader;
		case ECapability::InterpolationFunction: return b == ECapability::Shader;
		case ECapability::TransformFeedback: return b == ECapability::Shader;
		case ECapability::GeometryStreams: return b == ECapability::Geometry;
		case ECapability::StorageImageReadWithoutFormat: return b == ECapability::Shader;
		case ECapability::StorageImageWriteWithoutFormat: return b == ECapability::Shader;
		case ECapability::MultiViewport: return b == ECapability::Geometry;
		case ECapability::SubgroupDispatch: return b == ECapability::DeviceEnqueue;
		case ECapability::NamedBarrier: return b == ECapability::Kernel;
		case ECapability::PipeStorage: return b == ECapability::Pipes;
		case ECapability::GroupNonUniformVote: return b == ECapability::GroupNonUniform;
		case ECapability::GroupNonUniformArithmetic: return b == ECapability::GroupNonUniform;
		case ECapability::GroupNonUniformBallot: return b == ECapability::GroupNonUniform;
		case ECapability::GroupNonUniformShuffle: return b == ECapability::GroupNonUniform;
		case ECapability::GroupNonUniformClustered: return b == ECapability::GroupNonUniform;
		case ECapability::GroupNonUniformQuad: return b == ECapability::GroupNonUniform;
		case ECapability::FragmentShadingRateKHR: return b == ECapability::Shader;
		case ECapability::DrawParameters: return b == ECapability::Shader;
		case ECapability::WorkgroupMemoryExplicitLayoutKHR: return b == ECapability::Shader;
		case ECapability::WorkgroupMemoryExplicitLayout8BitAccessKHR: return b == ECapability::WorkgroupMemoryExplicitLayoutKHR;
		case ECapability::WorkgroupMemoryExplicitLayout16BitAccessKHR: return b == ECapability::Shader;
		case ECapability::UniformAndStorageBuffer16BitAccess: return b == ECapability::StorageBuffer16BitAccess;
		case ECapability::MultiView: return b == ECapability::Shader;
		case ECapability::VariablePointersStorageBuffer: return b == ECapability::Shader;
		case ECapability::VariablePointers: return b == ECapability::VariablePointersStorageBuffer;
		case ECapability::UniformAndStorageBuffer8BitAccess: return b == ECapability::StorageBuffer8BitAccess;
		case ECapability::RayQueryProvisionalKHR: return b == ECapability::Shader;
		case ECapability::RayQueryKHR: return b == ECapability::Shader;
		case ECapability::RayTraversalPrimitiveCullingKHR: return b == ECapability::RayQueryKHR || b == ECapability::RayTracingKHR;
		case ECapability::RayTracingKHR: return b == ECapability::Shader;
		case ECapability::Float16ImageAMD: return b == ECapability::Shader;
		case ECapability::ImageGatherBiasLodAMD: return b == ECapability::Shader;
		case ECapability::FragmentMaskAMD: return b == ECapability::Shader;
		case ECapability::StencilExportEXT: return b == ECapability::Shader;
		case ECapability::ImageReadWriteLodAMD: return b == ECapability::Shader;
		case ECapability::Int64ImageEXT: return b == ECapability::Shader;
		case ECapability::ShaderClockKHR: return b == ECapability::Shader;
		case ECapability::SampleMaskOverrideCoverageNV: return b == ECapability::SampleRateShading;
		case ECapability::GeometryShaderPassthroughNV: return b == ECapability::Geometry;
		case ECapability::ShaderViewportIndexLayerEXT: return b == ECapability::MultiView;
		case ECapability::ShaderViewportMaskNV: return b == ECapability::ShaderViewportIndexLayerEXT;
		case ECapability::ShaderStereoViewNV: return b == ECapability::ShaderViewportMaskNV;
		case ECapability::PerViewAttributesNV: return b == ECapability::MultiView;
		case ECapability::FragmentFullyCoveredEXT: return b == ECapability::Shader;
		case ECapability::MeshShadingNV: return b == ECapability::Shader;
		case ECapability::FragmentDensityEXT: return b == ECapability::Shader;
		case ECapability::ShaderNonUniform: return b == ECapability::Shader;
		case ECapability::RuntimeDescriptorArray: return b == ECapability::Shader;
		case ECapability::InputAttachmentArrayDynamicIndexing: return b == ECapability::InputAttachment;
		case ECapability::UniformTexelBufferArrayDynamicIndexing: return b == ECapability::SampledBuffer;
		case ECapability::StorageTexelBufferArrayDynamicIndexing: return b == ECapability::ImageBuffer;
		case ECapability::UniformBufferArrayNonUniformIndexing: return b == ECapability::ShaderNonUniform;
		case ECapability::SampledImageArrayNonUniformIndexing: return b == ECapability::ShaderNonUniform;
		case ECapability::StorageBufferArrayNonUniformIndexing: return b == ECapability::ShaderNonUniform;
		case ECapability::StorageImageArrayNonUniformIndexing: return b == ECapability::ShaderNonUniform;
		case ECapability::InputAttachmentArrayNonUniformIndexing: return b == ECapability::InputAttachment || b == ECapability::ShaderNonUniform;
		case ECapability::UniformTexelBufferArrayNonUniformIndexing: return b == ECapability::SampledBuffer || b == ECapability::ShaderNonUniform;
		case ECapability::StorageTexelBufferArrayNonUniformIndexing: return b == ECapability::ImageBuffer || b == ECapability::ShaderNonUniform;
		case ECapability::RayTracingNV: return b == ECapability::Shader;
		case ECapability::RayTracingMotionBlurNV: return b == ECapability::Shader;
		case ECapability::PhysicalStorageBufferAddresses: return b == ECapability::Shader;
		case ECapability::RayTracingProvisionalKHR: return b == ECapability::Shader;
		case ECapability::CooperativeMatrixNV: return b == ECapability::Shader;
		case ECapability::FragmentShaderSampleInterlockEXT: return b == ECapability::Shader;
		case ECapability::ShaderSMBuiltinsNV: return b == ECapability::Shader;
		case ECapability::FragmentShaderPixelInterlockEXT: return b == ECapability::Shader;
		case ECapability::DemoteToHelperInvocation: return b == ECapability::Shader;
		case ECapability::IntegerFunctions2INTEL: return b == ECapability::Shader;
		case ECapability::VectorComputeINTEL: return b == ECapability::VectorAnyINTEL;
		case ECapability::FPFastMathModeINTEL: return b == ECapability::Kernel;
		case ECapability::DotProductInput4x8Bit: return b == ECapability::Int8;
		case ECapability::GroupNonUniformRotateKHR: return b == ECapability::GroupNonUniform;
		}

		return false;
	}
} // namespace Frertex::Transpilers::SPIRV