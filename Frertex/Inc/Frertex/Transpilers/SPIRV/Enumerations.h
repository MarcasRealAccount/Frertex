#pragma once

#include <cstdint>

namespace Frertex::Transpilers::SPIRV
{
	enum class ESourceLanguage : std::uint32_t
	{
		Unknown = 0,
		ESSL,
		GLSL,
		OpenCL_C,
		OpenCL_Cpp,
		HLSL,
		CppForOpenCL,
		SYCL
	};

	enum class EExecutionModel : std::uint32_t
	{
		Vertex = 0,
		TessellationControl,
		TessellationEvaluation,
		Geometry,
		Fragment,
		GLCompute,
		Kernel,
		TaskNV           = 5267,
		TaskEXT          = 5267,
		MeshNV           = 5268,
		MeshEXT          = 5268,
		RayGenerationNV  = 5313,
		RayGenerationKHR = 5313,
		IntersectionNV   = 5314,
		IntersectionKHR  = 5314,
		AnyHitNV         = 5315,
		AnyHitKHR        = 5315,
		ClosestHitNV     = 5316,
		ClosestHitKHR    = 5316,
		MissNV           = 5317,
		MissKHR          = 5317,
		CallableNV       = 5318,
		CallableKHR      = 5318
	};

	enum class EAddressingMode : std::uint32_t
	{
		Logical = 0,
		Physical32,
		Physical64,
		PhysicalStorageBuffer64    = 5348,
		PhysicalStorageBuffer64EXT = 5348
	};

	enum class EMemoryModel : std::uint32_t
	{
		Simple = 0,
		GLSL450,
		OpenCL,
		Vulkan,
		VulkanKHR = 3
	};

	enum class EExecutionMode : std::uint32_t
	{
		Invocations = 0,
		SpacingEqual,
		SpacingFractionalEven,
		SpacingFractionalOdd,
		VertexOrderCw,
		VertexOrderCcw,
		PixelCenterInteger,
		OriginUpperLeft,
		OriginLowerLeft,
		EarlyFragmentTests,
		PointMode,
		Xfb,
		DepthReplacing,
		DepthGreater = 14,
		DepthLess,
		DepthUnchanged,
		LocalSize,
		LocalSizeHint,
		InputPoints,
		InputLines,
		InputLinesAdjacency,
		Triangles,
		InputTrianglesAdjacency,
		Quads,
		Isolines,
		OutputVertices,
		OutputPoints,
		OutputLineStrip,
		OutputTriangleStrip,
		VecTypeHint,
		ContractionOff,
		Initializer = 33,
		Finalizer,
		SubgroupSize,
		SubgroupsPerWorkgroup,
		SubgroupsPerWorkgroupId,
		LocalSizeId,
		LocalSizeHintId,
		SubgroupUniformControlFlowKHR = 4421,
		PostDepthCoverage             = 4446,
		DenormPreserve                = 4459,
		DenormFlushToZero,
		SignedZeroInfNanPreserve,
		RoundingModeRTE,
		RoundingModeRTZ,
		EarlyAndLateFragmenttestsAMD = 4017,
		StencilRefReplacingEXT       = 4027,
		StencilRefUnchangedFrontAMD  = 5079,
		StencilRefGreaterFrontAMD,
		StencilRefLessFrontAMD,
		StencilRefUnchangedBackAMD,
		StencilRefGreaterBackAMD,
		StencilRefLessBackAMD,
		OutputLinesNV = 5269,
		OutputPrimitivesNV,
		DerivativeGroupQuadsNV = 5289,
		DerivativeGroupLinearNV,
		OutputTrianglesNV        = 5298,
		PixelInterlockOrderedEXT = 5366,
		PixelInterlockUnorderedEXT,
		SampleInterlockOrderedEXT,
		SampleInterlockUnorderedEXT,
		ShadingRateInterlockOrderedEXT,
		ShadingRateInterlockUnorderedEXT,
		SharedLocalMemorySizeINTEL = 5618,
		RoundingModeRTPINTEL       = 5620,
		RoundingModeRTNINTEL,
		FloatingPointModeALTINTEL,
		FloatingPointModeIEEEINTEL,
		MaxWorkgroupSizeINTEL = 5893,
		MaxWorkDimINTEL,
		NoGlobalOffsetINTEL,
		NumSIMDWorkitemsINTEL,
		SchedulerTargetFmaxMhzINTEL = 5903,
		NamedBarrierCountINTEL      = 6417
	};

	enum class EStorageClass : std::uint32_t
	{
		UniformConstant = 0,
		Input,
		Uniform,
		Output,
		Workgroup,
		CrossWorkgroup,
		Private,
		Function,
		Generic,
		PushConstant,
		AtomicCounter,
		Image,
		StorageBuffer,
		CallableDataNV           = 5328,
		CallableDataKHR          = 5328,
		IncomingCallableDataNV   = 5329,
		IncomingCallableDataKHR  = 5329,
		RayPayloadNV             = 5338,
		RayPayloadKHR            = 5338,
		HitAttributeNV           = 5339,
		HitAttributeKHR          = 5339,
		IncomingRayPayloadNV     = 5342,
		IncomingRayPayloadKHR    = 5342,
		ShaderRecordBufferNV     = 5343,
		ShaderRecordBufferKHR    = 5343,
		PhysicalStorageBuffer    = 5349,
		PhysicalStorageBufferEXT = 5349,
		CodeSectionINTEL         = 5605,
		DeviceOnlyINTEL          = 5936,
		HostOnlyINTEL
	};

	enum class EDim : std::uint32_t
	{
		_1D = 0,
		_2D,
		_3D,
		Cube,
		Rect,
		Buffer,
		SubpassData
	};

	enum class ESamplerAddressingMode : std::uint32_t
	{
		None = 0,
		ClampToEdge,
		Clamp,
		Repeat,
		RepeatMirrored
	};

	enum class ESamplerFilterMode : std::uint32_t
	{
		Nearest = 0,
		Linear
	};

	enum class EImageFormat : std::uint32_t
	{
		Unknown = 0,
		Rgba32f,
		Rgba16f,
		R32f,
		Rgba8,
		Rgba8Snorm,
		Rg32f,
		Rg16f,
		R11fG11fB10f,
		R16f,
		Rgba16,
		Rgb10A2,
		Rg16,
		Rg8,
		R16,
		R8,
		Rgba16Snorm,
		Rg16Snorm,
		Rg8Snorm,
		R16Snorm,
		R8Snorm,
		Rgba32i,
		Rgba16i,
		Rgba8i,
		R32i,
		Rg32i,
		Rg16i,
		Rg8i,
		R16i,
		R8i,
		Rgba32ui,
		Rgba16ui,
		Rgba8ui,
		R32ui,
		Rgb10a2ui,
		Rg32ui,
		Rg16ui,
		Rg8ui,
		R16ui,
		R8ui,
		R64ui,
		R64i
	};

	enum class EImageChannelOrder : std::uint32_t
	{
		R = 0,
		A,
		RG,
		RA,
		RGB,
		RGBA,
		BGRA,
		ARGB,
		Intensity,
		Limunance,
		Rx,
		RGx,
		RGBx,
		Depth,
		DepthStencil,
		sRGB,
		sRGBx,
		sRGBA,
		sBGRA,
		ABGR
	};

	enum class EImageChannelDataType : std::uint32_t
	{
		SnormInt8,
		SnormInt16,
		UnormInt8,
		UnormInt16,
		UnormShort565,
		UnormShort555,
		UnormInt101010,
		SignedInt8,
		SignedInt16,
		SignedInt32,
		UnsignedInt8,
		UnsignedInt16,
		UnsignedInt32,
		HalfFloat,
		Float,
		UnormInt24,
		UnormInt101010_2
	};

	enum class EImageOperands : std::uint32_t
	{
		None                  = 0x00000,
		Bias                  = 0x00001,
		Lod                   = 0x00002,
		Grad                  = 0x00004,
		ConstOffset           = 0x00008,
		Offset                = 0x00010,
		ConstOffsets          = 0x00020,
		Sample                = 0x00040,
		MinLod                = 0x00080,
		MakeTexelAvailable    = 0x00100,
		MakeTexelAvailableKHR = 0x00100,
		MakeTexelVisible      = 0x00200,
		MakeTexelVisibleKHR   = 0x00200,
		NonPrivateTexel       = 0x00400,
		NonPrivateTexelKHR    = 0x00400,
		VolatileTexel         = 0x00800,
		VolatileTexelKHR      = 0x00800,
		SignExtend            = 0x01000,
		ZeroExtend            = 0x02000,
		Nontemporal           = 0x04000,
		Offsets               = 0x10000
	};

	enum class EFPFastMathMode : std::uint32_t
	{
		None                   = 0x00000,
		NotNaN                 = 0x00001,
		NotInf                 = 0x00002,
		NSZ                    = 0x00004,
		AllowRecip             = 0x00008,
		Fast                   = 0x00010,
		AllowContractFastINTEL = 0x10000,
		AllowReassocINTEL      = 0x20000
	};

	enum class EFPRoundingMode : std::uint32_t
	{
		RTE = 0,
		RTZ,
		RTP,
		RTN
	};

	enum class ELinkageType : std::uint32_t
	{
		Export = 0,
		Import,
		LinkOnceODR
	};

	enum class EAccessQualifier : std::uint32_t
	{
		ReadOnly = 0,
		WriteOnly,
		ReadWrite
	};

	enum class EFunctionParameterAttribute : std::uint32_t
	{
		Zext = 0,
		Sext,
		ByVal,
		Sret,
		NoAlias,
		NoCapture,
		NoWrite,
		NoReadWrite
	};

	enum class EDecoration : std::uint32_t
	{
		RelaxedPrecision = 0,
		SpecId,
		Block,
		BufferBlock,
		RowMajor,
		ColMajor,
		ArrayStride,
		MatrixStride,
		GLSLShared,
		GLSLPacked,
		CPacked,
		Builtin,
		NoPerspective = 13,
		Flat,
		Patch,
		Centroid,
		Sample,
		Invariant,
		Restrict,
		Aliased,
		Volatile,
		Constant,
		Coherent,
		NonWritable,
		NonReadable,
		Uniform,
		UniformId,
		SaturatedConversion,
		Stream,
		Location,
		Component,
		Index,
		Binding,
		DescriptorSet,
		Offset,
		XfbBuffer,
		XfbStride,
		FuncParamAttr,
		FPRoundingMode,
		FPFastMathMode,
		LinkageAttributes,
		NoContraction,
		InputAttachmentIndex,
		Alignment,
		MaxByteOffset,
		AlignmentId,
		MaxByteOffsetId,
		NoSignedWrap = 4469,
		NoUnsignedWrap,
		ExplicitInterpAMD           = 4999,
		OverrideCoverageNV          = 5248,
		PassthroughNV               = 5250,
		ViewportRelativeNV          = 5252,
		SecondaryViewportRelativeNV = 5256,
		PerPrimitiveNV              = 5271,
		PerViewNV,
		PerTaskNV,
		PerVertexKHR       = 5285,
		PerVertexNV        = 5285,
		NonUniform         = 5300,
		NonUniformEXT      = 5300,
		RestrictPointer    = 5355,
		RestrictPointerEXT = 5355,
		AliasedPointer     = 5356,
		AliasedPointerEXT  = 5356,
		BindlessSamplerNV  = 5398,
		BindlessImageNV,
		BoundSamplerNV,
		BoundImageNV,
		SIMTCallINTEL             = 5599,
		ReferencedIndirectlyINTEL = 5602,
		ClobberINTEL              = 5607,
		SideEffectsINTEL,
		VectorComputeVariableINTEL = 5624,
		FuncParamIOKindINTEL,
		VectorComputeFunctionINTEL,
		StackCallINTEL,
		GlobalVariableOffsetINTEL,
		CounterBuffer           = 5634,
		HlslCounterBufferGOOGLE = 5634,
		UserSemantic            = 5635,
		HlslSemanticGOOGLE      = 5635,
		UserTypeGOOGLE,
		FunctionRoundingModeINTEL = 5822,
		FunctionDenormModeINTEL,
		RegisterINTEL = 5825,
		MemoryINTEL,
		NumbanksINTEL,
		BankwidthINTEL,
		MaxPrivateCopiesINTEL,
		SinglepumpINTEL,
		DoublepumpINTEL,
		MaxReplicatesINTEL,
		SimpleDualPortINTEL,
		MergeINTEL,
		BankBitsINTEL,
		ForcePow2DepthINTEL,
		BurstCoalesceINTEL = 5899,
		CacheSizeINTEL     = 5900,
		DontStaticallyCoalesceINTEL,
		PrefetchINTEL,
		StallEnableINTEL         = 5905,
		FuseLoopsInFunctionINTEL = 5907,
		AliasScopeINTEL          = 5914,
		NoAliasINTEL,
		BufferLocationINTEL                = 5921,
		IOPipeStorageINTEL                 = 5944,
		FunctionFloatingPOintModeINTEL     = 6080,
		SingleElementVectorINTEL           = 6085,
		VectorComputeCallableFunctionINTEL = 6087,
		MediaBlockIOINTEL                  = 6140
	};

	enum class EBuiltin : std::uint32_t
	{
		Position = 0,
		PointSize,
		ClipDistance = 3,
		CullDistance,
		VertexId,
		InstanceId,
		PrimitiveId,
		InvocationId,
		Layer,
		ViewportIndex,
		TessLevelOuter,
		TessLevelInner,
		TessCoord,
		PatchVertices,
		FragCoord,
		PointCoord,
		FrontFacing,
		SampleId,
		SamplePosition,
		SampleMask,
		FragDepth = 22,
		HelperInvocation,
		NumWorkgroups,
		WorkgroupSize,
		WorkgroupId,
		LocalInvocationId,
		GlobalinvocationId,
		LocalInvocationIndex,
		WorkDim,
		GlobalSize,
		EnqueuedWorkgroupSize,
		GlobalOffset,
		GlobalLinearId,
		SubgroupSize = 36,
		SubgroupMaxSize,
		NumSubgroups,
		NumEnqueuedSubgroups,
		SubgroupId,
		SubgroupLocalInvocationId,
		VertexIndex,
		InstanceIndex,
		SubgroupEqMask    = 4416,
		SubgroupEqMaskKHR = 4416,
		SubgroupGeMask    = 4417,
		SubgroupGeMaskKHR = 4417,
		SubgroupGtMask    = 4418,
		SubgroupGtMaskKHR = 4418,
		SubgroupLeMask    = 4419,
		SubgroupLeMaskKHR = 4419,
		SubgroupLtMask    = 4420,
		SubgroupLtMaskKHR = 4420,
		BaseVertex        = 4424,
		BaseInstance,
		DrawIndex,
		PrimitiveShadingRateKHR = 4432,
		DeviceIndex             = 4438,
		ViewIndex               = 4440,
		ShadingRateKHR          = 4444,
		BaryCoordNoPerspAMD     = 4992,
		BaryCoordNoPerspCentroidAMD,
		BaryCoordNoPerspSampleAMD,
		BaryCoordSmoothAMD,
		BaryCoordSmoothCentroidAMD,
		BaryCoordSmoothSampleAMD,
		BaryCoordPullModelAMD,
		FragStencilRefEXT   = 5014,
		ViewportMaskNV      = 5253,
		SecondaryPositionNV = 5257,
		SecondaryViewportMaskNV,
		PositionPerViewNV = 5261,
		ViewportMaskPerViewNV,
		FullyCoveredEXT  = 5264,
		TaskCountNV      = 5274,
		PrimitiveCountNV = 5275,
		PrimitiveIndicesNV,
		ClipDistancePerViewNV,
		CullDistancePerViewNV,
		LayerPerViewNV,
		MeshViewCountNV,
		MeshViewIndicesNV,
		BaryCoordKHR           = 5286,
		BaryCoordNV            = 5286,
		BaryCoordNoPerspKHR    = 5287,
		BaryCoordNoPerspNV     = 5287,
		FragSizeEXT            = 5292,
		FragmentSizeNV         = 5292,
		FragInvocationCountEXT = 5293,
		InvocationsPerPixelNV  = 5293,
		LaunchIdNV             = 5319,
		LaunchIdKHR            = 5319,
		LaunchSizeNV           = 5320,
		LaunchSizeKHR          = 5320,
		WorldRayOriginNV       = 5321,
		WorldRayOriginKHR      = 5321,
		WorldRayDirectionNV    = 5322,
		WorldRayDirectionKHR   = 5322,
		ObjectRayOriginNV      = 5323,
		ObjectRayOriginKHR     = 5323,
		ObjectRayDirectionNV   = 5324,
		ObjectRayDirectionKHR  = 5324,
		RayTMinNV              = 5325,
		RayTMinKHR             = 5325,
		RayTMaxNV              = 5326,
		RayTMaxKHR             = 5326,
		InstanceCustomIndexNV  = 5327,
		InstanceCustomIndexKHR = 5327,
		ObjectToWorldNV        = 5330,
		ObjectToWorldKHR       = 5330,
		WorldToObjectNV        = 5331,
		WorldToObjectKHR       = 5331,
		HitTNV                 = 5332,
		HitKindNV              = 5333,
		HitKindKHR             = 5333,
		CurrentRayTimeNV       = 5334,
		IncomingRayFlagsNV     = 5351,
		IncomingRayFlagsKHR    = 5351,
		WarpsPerSMNV           = 5374,
		SMCountNV,
		WarpIDNV,
		SMIDNV,
		CullMaskKHR = 6021
	};

	enum class ESelectionControl : std::uint32_t
	{
		None        = 0x0,
		Flatten     = 0x1,
		DontFlatten = 0x2
	};

	enum class ELoopControl : std::uint32_t
	{
		None                      = 0x000000,
		Unroll                    = 0x000001,
		DontUnroll                = 0x000002,
		DependencyInfinite        = 0x000004,
		DependencyLength          = 0x000008,
		MinIterations             = 0x000010,
		MaxIterations             = 0x000020,
		IterationMultiple         = 0x000040,
		PeelCount                 = 0x000080,
		PartialCount              = 0x000100,
		InitiationIntervalINTEL   = 0x010000,
		MaxConcurrencyINTEL       = 0x020000,
		DependencyArrayINTEL      = 0x040000,
		PipelineEnableINTEL       = 0x080000,
		LoopCoalesceINTEL         = 0x100000,
		MaxInterleavingINTEL      = 0x200000,
		SpeculatedIterationsINTEL = 0x400000,
		NoFusionINTEL             = 0x800000
	};

	enum class EFunctionControl : std::uint32_t
	{
		None         = 0x00000,
		Inline       = 0x00001,
		DontInline   = 0x00002,
		Pure         = 0x00004,
		Const        = 0x00008,
		OptNoneINTEL = 0x10000
	};

	enum class EMemorySemantics : std::uint32_t
	{
		None                   = 0x0000,
		Acquire                = 0x0002,
		Release                = 0x0004,
		AcquireRelease         = 0x0008,
		SequentiallyConsistent = 0x0010,
		UniformMemory          = 0x0040,
		SubgroupMemory         = 0x0080,
		WorkgroupMemory        = 0x0100,
		CrossWorkgroupMemory   = 0x0200,
		AtomicCounterMemory    = 0x0400,
		ImageMemory            = 0x0800,
		OutputMemory           = 0x1000,
		OutputMemoryKHR        = 0x1000,
		MakeAvailable          = 0x2000,
		MakeAvailableKHR       = 0x2000,
		MakeVisible            = 0x4000,
		MakeVisibleKHR         = 0x4000,
		Volatile               = 0x8000
	};

	enum class EMemoryOperands : std::uint32_t
	{
		None                    = 0x00000,
		Volatile                = 0x00001,
		Aligned                 = 0x00002,
		Nontemporal             = 0x00004,
		MakePointerAvailable    = 0x00008,
		MakePointerAvailableKHR = 0x00008,
		MakePointerVisible      = 0x00010,
		MakePointerVisibleKHR   = 0x00010,
		MakePrivatePointer      = 0x00020,
		MakePrivatePointerKHR   = 0x00020,
		AliasScopeINTELMask     = 0x10000,
		NoAliasINTELMask        = 0x20000,
	};

	enum class EScope : std::uint32_t
	{
		CrossDevice = 0,
		Device,
		Workgroup,
		Subgroup,
		Invocation,
		QueueFamily,
		QueueFamilyKHR = 5,
		ShaderCallKHR
	};

	enum class EGroupOperation : std::uint32_t
	{
		Reduce = 0,
		InclusiveScan,
		ExcluseScan,
		ClusteredReduce,
		PartitionedReduceNV = 6,
		PartitionedInclusiveScanNV,
		PartitionedExclusiveScanNV,
	};

	enum class EKernelEnqueueFlags : std::uint32_t
	{
		NoWait = 0,
		WaitKernel,
		WaitWorkGroup
	};

	enum class KernelProfilingInfo : std::uint32_t
	{
		None        = 0x0,
		CmdExecTime = 0x1
	};

	enum class ECapability : std::uint32_t
	{
		Matrix = 0,
		Shader,
		Geometry,
		Tessellation,
		Addresses,
		Linkage,
		Kernel,
		Vector16,
		Float16Buffer,
		Float16,
		Float64,
		Int64,
		Int64Atomics,
		ImageBasic,
		ImageReadWrite,
		ImageMipmap,
		Pipes = 17,
		Groups,
		DeviceEnqueue,
		LiteralSampler,
		AtomicStorage,
		Int16,
		TessellationPointSize,
		GeometryPointSize,
		ImageGatherExtended,
		StorageImageMultisample = 27,
		UniformBufferArrayDynamicIndexing,
		SampledImageArrayDynamicIndexing,
		StorageBufferArrayDynamicIndexing,
		StorageImageArrayDynamicIndexing,
		ClipDistance,
		CullDistance,
		ImageCubeArray,
		SampleRateShading,
		ImageRect,
		SampledRect,
		GenericPointer,
		Int8,
		InputAttachment,
		SparseResidency,
		MinLod,
		Sampled1D,
		Image1D,
		SampledCubeArray,
		SampledBuffer,
		ImageBuffer,
		ImageMSArray,
		StorageImageExtendedFormats,
		ImageQuery,
		DerivativeControl,
		InterpolationFunction,
		TransformFeedback,
		GeometryStreams,
		StorageImageReadWithoutFormat,
		StorageImageWriteWithoutFormat,
		MultiViewport,
		SubgroupDispatch,
		NamedBarrier,
		PipeStorage,
		GroupNonUniform,
		GroupNonUniformVote,
		GroupNonUniformArithmetic,
		GroupNonUniformBallot,
		GroupNonUniformShuffle,
		GroupNonUniformShuffleRelative,
		GroupNonUniformClustered,
		GroupNonUniformQuad,
		ShaderLayer,
		ShaderViewportIndex,
		UniformDecoration,
		FragmentShadingRateKHR           = 4422,
		SubgroupBallotKHR                = 4423,
		DrawParameters                   = 4427,
		WorkgroupMemoryExplicitLayoutKHR = 4428,
		WorkgroupMemoryExplicitLayout8BitAccessKHR,
		WorkgroupMemoryExplicitLayout16BitAccessKHR,
		SubgroupVoteKHR,
		StorageBuffer16BitAccess           = 4433,
		StorageUniformbufferBlock16        = 4433,
		UniformAndStorageBuffer16BitAccess = 4434,
		StorageUniform16                   = 4434,
		StoragePushConstant16,
		StorageInputOutput16,
		DeviceGroup,
		MultiView                     = 4439,
		VariablePointersStorageBuffer = 4441,
		VariablePointers,
		AtomicStorageOps            = 4445,
		SampleMaskPostDepthCoverage = 4447,
		StorageBuffer8BitAccess     = 4448,
		UniformAndStorageBuffer8BitAccess,
		StoragePushConstant8,
		DenormPreserve = 4464,
		DenormFlushToZero,
		SignedZeroInfNanPreserve,
		RoundingModeRTE,
		RoundingModeRTZ,
		RayQueryProvisionalKHR = 4471,
		RayQueryKHR,
		RayTraversalPrimitiveCullingKHR = 4478,
		RayTracingKHR,
		Float16ImageAMD = 5008,
		ImageGatherBiasLodAMD,
		FragmentMaskAMD,
		StencilExportEXT     = 5013,
		ImageReadWriteLodAMD = 5015,
		Int64ImageEXT,
		ShaderClockKHR               = 5055,
		SampleMaskOverrideCoverageNV = 5249,
		GeometryShaderPassthroughNV  = 5251,
		ShaderViewportIndexLayerEXT  = 5254,
		ShaderViewportIndexLayerNV   = 5254,
		ShaderViewportMaskNV         = 5255,
		ShaderStereoViewNV           = 5259,
		PerViewAttributesNV,
		FragmentFullyCoveredEXT = 5265,
		MeshShadingNV,
		ImageFootprintNV                             = 5282,
		FragmentBarycentricKHR                       = 5284,
		FragmentBarycentricNV                        = 5284,
		ComputeDerivativeGroupQuadsNV                = 5288,
		FragmentDensityEXT                           = 5291,
		ShadingRateNV                                = 5291,
		GroupNonUniformPartitionedNV                 = 5297,
		ShaderNonUniform                             = 5301,
		ShaderNonUniformEXT                          = 5301,
		RuntimeDescriptorArray                       = 5302,
		RuntimeDescriptorArrayEXT                    = 5302,
		InputAttachmentArrayDynamicIndexing          = 5303,
		InputAttachmentArrayDynamicIndexingEXT       = 5303,
		UniformTexelBufferArrayDynamicIndexing       = 5304,
		UniformTexelBufferArrayDynamicIndexingEXT    = 5304,
		StorageTexelBufferArrayDynamicIndexing       = 5305,
		StorageTexelBufferArrayDynamicIndexingEXT    = 5305,
		UniformBufferArrayNonUniformIndexing         = 5306,
		UniformBufferArrayNonUniformIndexingEXT      = 5306,
		SampledImageArrayNonUniformIndexing          = 5307,
		SampledImageArrayNonUniformIndexingEXT       = 5307,
		StorageBufferArrayNonUniformIndexing         = 5308,
		StorageBufferArrayNonUniformIndexingEXT      = 5308,
		StorageImageArrayNonUniformIndexing          = 5309,
		StorageImageArrayNonUniformIndexingEXT       = 5309,
		InputAttachmentArrayNonUniformIndexing       = 5310,
		InputAttachmentArrayNonUniformIndexingEXT    = 5310,
		UniformTexelBufferArrayNonUniformIndexing    = 5311,
		UniformTexelBufferArrayNonUniformIndexingEXT = 5311,
		StorageTexelBufferArrayNonUniformIndexing    = 5312,
		StorageTexelBufferArrayNonUniformIndexingEXT = 5312,
		RayTracingNV                                 = 5340,
		RayTracingMotionBlurNV,
		VulkanMemoryModel                     = 5345,
		VulkanMemoryModelKHR                  = 5345,
		VulkanMemoryModelDeviceScope          = 5346,
		VulkanMemoryModelDeviceScopeKHR       = 5346,
		PhysicalStorageBufferAddresses        = 5347,
		PhysicalStorageBufferAddressesEXT     = 5347,
		ComputeDerivativeGroupLinearNV        = 5350,
		RayTracingProvisionalKHR              = 5353,
		CooperativeMatrixNV                   = 5357,
		FragmentShaderSampleInterlockEXT      = 5363,
		FragmentShaderShadingRateInterlockEXT = 5372,
		ShaderSMBuiltinsNV,
		FragmentShaderPixelInterlockEXT = 5378,
		DemoteToHelperInvocation        = 5379,
		DemoteToHelperInvocationEXT     = 5379,
		BindlessTextureNV               = 5390,
		SubgroupShuffleINTEL            = 5568,
		SubgroupBufferBlockIOINTEL,
		SubgroupImageBlockIOIntel,
		SubgroupImageMediaBlockIOIntel = 5579,
		RoundToInfinityINTEL           = 5582,
		FloatingPointModeINTEL,
		IntegerFunctions2INTEL,
		FunctionPointersINTEL = 5603,
		IndirectReferencesINTEL,
		AsmINTEL               = 5606,
		AtomicFloat32MinMaxEXT = 5612,
		AtomicFloat64MinMaxEXT,
		AtomicFloat16MinMaxEXT = 5616,
		VectorComputeINTEL,
		VectorAnyINTEL                   = 5619,
		ExpectAssumeKHR                  = 5629,
		SubgroupAvcMotionEstimationINTEL = 5696,
		SubgroupAvcMotionEstimationIntraINTEL,
		SubgroupAvcMotionEstimationChromaINTEL,
		VariableLengthArrayINTEL        = 5817,
		FunctionFloatControlINTEL       = 5821,
		FPGBAMemoryAttributesINTEL      = 5824,
		FPFastMathModeINTEL             = 5837,
		ArbitraryPrecisionIntegersINTEL = 5844,
		AribtraryPrecisionFloatingPointINTEL,
		UnstructuredLoopControlsINTEL = 5886,
		FPGALoopControlsINTEL         = 5888,
		KernelAttributesINTEL         = 5892,
		FPGAKernelAttributesINTEL     = 5897,
		FPGAClusterAttributesINTEL    = 5904,
		LoopFuseINTEL                 = 5906,
		MemoryAccessAliasingINTEL,
		FPGABufferLocationINTEL           = 5920,
		ArbitraryPrecisionFixedPointINTEL = 5922,
		USMStorageClassesINTEL            = 5935,
		IOPipesINTEL                      = 5943,
		BlockingPipesINTEL                = 5945,
		FPGARegINTEL                      = 5948,
		DotProductInputAll                = 6016,
		DotProductInputAllKHR             = 6016,
		DotProductInput4x8Bit             = 6017,
		DotProductInput4x8BitKHR          = 6017,
		DotProductInput4x8BitPacked       = 6018,
		DotProductInput4x8BitPackedKHR    = 6018,
		DotProduct                        = 6019,
		DotProductKHR                     = 6019,
		RayCullMaskKHR,
		BitInstructions = 6025,
		GroupNonUniformRotateKHR,
		AtomicFloat32AddEXT = 6033,
		AtomicFloat64AddEXT,
		LongConstantCompositeINTEL = 6089,
		OptNoneINTEL               = 6094,
		AtomicFloat16AddEXT,
		DebugInfoModuleINTEL      = 6114,
		SplitBarrierINTEL         = 6141,
		GroupUniformArithmeticKHR = 6400
	};

	bool IsCapabilityImplicit(ECapability a, ECapability b);

	enum class EReservedRayFlags : std::uint32_t
	{
		None                        = 0x000,
		OpaqueKHR                   = 0x001,
		NoOpaqueKHR                 = 0x002,
		TerminateOnFirstHitKHR      = 0x004,
		SkipClosestHitShaderKHR     = 0x008,
		CullBackFacingTrianglesKHR  = 0x010,
		CullFrontFacingTrianglesKHR = 0x020,
		CullOpaqueKHR               = 0x040,
		CullNoOpaqueKHR             = 0x080,
		SkipTrianglesKHR            = 0x100,
		SkipAAABsKHR                = 0x200
	};

	enum class EReservedRayQueryIntersection : std::uint32_t
	{
		RayQueryCandidateIntersectionKHR = 0,
		RayQueryCommittedIntersectionKHR
	};

	enum class EReservedRayQueryCommittedType : std::uint32_t
	{
		RayQueryCommittedIntersectionNoneKHR = 0,
		RayQueryCommittedIntersectionTriangleKHR,
		RayQueryCommittedIntersectionGeneratedKHR
	};

	enum class EReservedRayQueryCandidateType : std::uint32_t
	{
		RayQueryCandidateIntersectionTriangleKHR = 0,
		RayQueryCandidateIntersectionAABBKHR     = 0,
	};

	enum class EReservedFragmentShadingRate : std::uint32_t
	{
		None              = 0x0,
		Vertical2Pixels   = 0x1,
		Vertical4Pixels   = 0x2,
		Horizontal2Pixels = 0x4,
		Horizontal4Pixels = 0x8
	};

	enum class EReservedFPDenormMode : std::uint32_t
	{
		Preserve = 0,
		FlushToZero
	};

	enum class EReservedFPOperationMode : std::uint32_t
	{
		IEEE = 0,
		ALT
	};

	enum class EQuantizationMode : std::uint32_t
	{
		TRN = 0,
		TRN_ZERO,
		RND,
		RND_ZERO,
		RND_INF,
		RND_MIN_INF,
		RND_CONV,
		RND_CONV_ODD
	};

	enum class EOverflowMode : std::uint32_t
	{
		WRAP = 0,
		SAT,
		SAT_ZERO,
		SAT_SYM
	};

	enum class EPackedVectorFormat : std::uint32_t
	{
		PackedVectorFormat4x8Bit    = 0,
		PackedVectorFormat4x8BitKHR = 0
	};
} // namespace Frertex::Transpilers::SPIRV