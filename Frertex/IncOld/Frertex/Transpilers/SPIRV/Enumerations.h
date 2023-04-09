#pragma once

#include <cstdint>

#include <set>
#include <string>

namespace Frertex::Transpilers::SPIRV
{
	enum class EImageOperands : std::uint32_t
	{
		None = 0x0000'0000,
		Bias = 0x0000'0001,
		Lod = 0x0000'0002,
		Grad = 0x0000'0004,
		ConstOffset = 0x0000'0008,
		Offset = 0x0000'0010,
		ConstOffsets = 0x0000'0020,
		Sample = 0x0000'0040,
		MinLod = 0x0000'0080,
		MakeTexelAvailable = 0x0000'0100,
		MakeTexelVisible = 0x0000'0200,
		NonPrivateTexel = 0x0000'0400,
		VolatileTexel = 0x0000'0800,
		SignExtend = 0x0000'1000,
		ZeroExtend = 0x0000'2000,
		Nontemporal = 0x0000'4000,
		Offsets = 0x0001'0000
	};

	enum class EFPFastMathMode : std::uint32_t
	{
		None = 0x0000'0000,
		NotNaN = 0x0000'0001,
		NotInf = 0x0000'0002,
		NSZ = 0x0000'0004,
		AllowRecip = 0x0000'0008,
		Fast = 0x0000'0010,
		AllowContractFastINTEL = 0x0001'0000,
		AllowReassocINTEL = 0x0002'0000
	};

	enum class ESelectionControl : std::uint32_t
	{
		None = 0x0000'0000,
		Flatten = 0x0000'0001,
		DontFlatten = 0x0000'0002
	};

	enum class ELoopControl : std::uint32_t
	{
		None = 0x0000'0000,
		Unroll = 0x0000'0001,
		DontUnroll = 0x0000'0002,
		DependencyInfinite = 0x0000'0004,
		DependencyLength = 0x0000'0008,
		MinIterations = 0x0000'0010,
		MaxIterations = 0x0000'0020,
		IterationMultiple = 0x0000'0040,
		PeelCount = 0x0000'0080,
		PartialCount = 0x0000'0100,
		InitiationIntervalINTEL = 0x0001'0000,
		MaxConcurrencyINTEL = 0x0002'0000,
		DependencyArrayINTEL = 0x0004'0000,
		PipelineEnableINTEL = 0x0008'0000,
		LoopCoalesceINTEL = 0x0010'0000,
		MaxInterleavingINTEL = 0x0020'0000,
		SpeculatedIterationsINTEL = 0x0040'0000,
		NoFusionINTEL = 0x0080'0000
	};

	enum class EFunctionControl : std::uint32_t
	{
		None = 0x0000'0000,
		Inline = 0x0000'0001,
		DontInline = 0x0000'0002,
		Pure = 0x0000'0004,
		Const = 0x0000'0008,
		OptNoneINTEL = 0x0001'0000
	};

	enum class EMemorySemantics : std::uint32_t
	{
		Acquire = 0x0000'0002,
		Release = 0x0000'0004,
		AcquireRelease = 0x0000'0008,
		SequentiallyConsistent = 0x0000'0010,
		UniformMemory = 0x0000'0040,
		SubgroupMemory = 0x0000'0080,
		WorkgroupMemory = 0x0000'0100,
		CrossWorkgroupMemory = 0x0000'0200,
		AtomicCounterMemory = 0x0000'0400,
		ImageMemory = 0x0000'0800,
		OutputMemory = 0x0000'1000,
		MakeAvailable = 0x0000'2000,
		MakeVisible = 0x0000'4000,
		Volatile = 0x0000'8000
	};

	enum class EMemoryAccess : std::uint32_t
	{
		None = 0x0000'0000,
		Volatile = 0x0000'0001,
		Aligned = 0x0000'0002,
		Nontemporal = 0x0000'0004,
		MakePointerAvailable = 0x0000'0008,
		MakePointerVisible = 0x0000'0010,
		NonPrivatePointer = 0x0000'0020,
		AliasScopeINTELMask = 0x0001'0000,
		NoAliasINTELMask = 0x0002'0000
	};

	enum class EKernelProfilingInfo : std::uint32_t
	{
		None = 0x0000'0000,
		CmdExecTime = 0x0000'0001
	};

	enum class ERayFlags : std::uint32_t
	{
		NoneKHR = 0x0000'0000,
		OpaqueKHR = 0x0000'0001,
		NoOpaqueKHR = 0x0000'0002,
		TerminateOnFirstHitKHR = 0x0000'0004,
		SkipClosestHitShaderKHR = 0x0000'0008,
		CullBackFacingTrianglesKHR = 0x0000'0010,
		CullFrontFacingTrianglesKHR = 0x0000'0020,
		CullOpaqueKHR = 0x0000'0040,
		CullNoOpaqueKHR = 0x0000'0080,
		SkipTrianglesKHR = 0x0000'0100,
		SkipAABBsKHR = 0x0000'0200
	};

	enum class EFragmentShadingRate : std::uint32_t
	{
		Vertical2Pixels = 0x0000'0001,
		Vertical4Pixels = 0x0000'0002,
		Horizontal2Pixels = 0x0000'0004,
		Horizontal4Pixels = 0x0000'0008
	};

	enum class ESourceLanguage : std::uint32_t
	{
		Unknown = 0,
		ESSL = 1,
		GLSL = 2,
		OpenCL_C = 3,
		OpenCL_CPP = 4,
		HLSL = 5,
		CPP_for_OpenCL = 6,
		SYCL = 7
	};

	enum class EExecutionModel : std::uint32_t
	{
		Vertex = 0,
		TessellationControl = 1,
		TessellationEvaluation = 2,
		Geometry = 3,
		Fragment = 4,
		GLCompute = 5,
		Kernel = 6,
		TaskNV = 5267,
		MeshNV = 5268,
		TaskEXT = 5364,
		MeshEXT = 5365
	};

	enum class EAddressingModel : std::uint32_t
	{
		Logical = 0,
		Physical32 = 1,
		Physical64 = 2,
		PhysicalStorageBuffer64 = 5348
	};

	enum class EMemoryModel : std::uint32_t
	{
		Simple = 0,
		GLSL450 = 1,
		OpenCL = 2,
		Vulkan = 3
	};

	enum class EExecutionMode : std::uint32_t
	{
		Invocations = 0,
		SpacingEqual = 1,
		SpacingFractionalEven = 2,
		SpacingFractionalOdd = 3,
		VertexOrderCw = 4,
		VertexOrderCcw = 5,
		PixelCenterInteger = 6,
		OriginUpperLeft = 7,
		OriginLowerLeft = 8,
		EarlyFragmentTests = 9,
		PointMode = 10,
		Xfb = 11,
		DepthReplacing = 12,
		DepthGreater = 14,
		DepthLess = 15,
		DepthUnchanged = 16,
		LocalSize = 17,
		LocalSizeHint = 18,
		InputPoints = 19,
		InputLines = 20,
		InputLinesAdjacency = 21,
		Triangles = 22,
		InputTrianglesAdjacency = 23,
		Quads = 24,
		Isolines = 25,
		OutputVertices = 26,
		OutputPoints = 27,
		OutputLineStrip = 28,
		OutputTriangleStrip = 29,
		VecTypeHint = 30,
		ContractionOff = 31,
		Initializer = 33,
		Finalizer = 34,
		SubgroupSize = 35,
		SubgroupsPerWorkgroup = 36,
		SubgroupsPerWorkgroupId = 37,
		LocalSizeId = 38,
		LocalSizeHintId = 39,
		SubgroupUniformControlFlowKHR = 4421,
		PostDepthCoverage = 4446,
		DenormPreserve = 4459,
		DenormFlushToZero = 4460,
		SignedZeroInfNanPreserve = 4461,
		RoundingModeRTE = 4462,
		RoundingModeRTZ = 4463,
		EarlyAndLateFragmentTestsAMD = 5017,
		StencilRefReplacingEXT = 5027,
		StencilRefUnchangedFrontAMD = 5079,
		StencilRefGreaterFrontAMD = 5080,
		StencilRefLessFrontAMD = 5081,
		StencilRefUnchangedBackAMD = 5082,
		StencilRefGreaterBackAMD = 5083,
		StencilRefLessBackAMD = 5084,
		DerivativeGroupQuadsNV = 5289,
		DerivativeGroupLinearNV = 5290,
		PixelInterlockOrderedEXT = 5366,
		PixelInterlockUnorderedEXT = 5367,
		SampleInterlockOrderedEXT = 5368,
		SampleInterlockUnorderedEXT = 5369,
		ShadingRateInterlockOrderedEXT = 5370,
		ShadingRateInterlockUnorderedEXT = 5371,
		SharedLocalMemorySizeINTEL = 5618,
		RoundingModeRTPINTEL = 5620,
		RoundingModeRTNINTEL = 5621,
		FloatingPointModeALTINTEL = 5622,
		FloatingPointModeIEEEINTEL = 5623,
		MaxWorkgroupSizeINTEL = 5893,
		MaxWorkDimINTEL = 5894,
		NoGlobalOffsetINTEL = 5895,
		NumSIMDWorkitemsINTEL = 5896,
		SchedulerTargetFmaxMhzINTEL = 5903,
		NamedBarrierCountINTEL = 6417
	};

	enum class EStorageClass : std::uint32_t
	{
		UniformConstant = 0,
		Input = 1,
		Uniform = 2,
		Output = 3,
		Workgroup = 4,
		CrossWorkgroup = 5,
		Private = 6,
		Function = 7,
		Generic = 8,
		PushConstant = 9,
		AtomicCounter = 10,
		Image = 11,
		StorageBuffer = 12,
		PhysicalStorageBuffer = 5349,
		TaskPayloadWorkgroupEXT = 5402,
		CodeSectionINTEL = 5605,
		DeviceOnlyINTEL = 5936,
		HostOnlyINTEL = 5937
	};

	enum class EDim : std::uint32_t
	{
		_1D = 0,
		_2D = 1,
		_3D = 2,
		Cube = 3,
		Rect = 4,
		Buffer = 5,
		SubpassData = 6
	};

	enum class ESamplerAddressingMode : std::uint32_t
	{
		None = 0,
		ClampToEdge = 1,
		Clamp = 2,
		Repeat = 3,
		RepeatMirrored = 4
	};

	enum class ESamplerFilterMode : std::uint32_t
	{
		Nearest = 0,
		Linear = 1
	};

	enum class EImageFormat : std::uint32_t
	{
		Unknown = 0,
		Rgba32f = 1,
		Rgba16f = 2,
		R32f = 3,
		Rgba8 = 4,
		Rgba8Snorm = 5,
		Rg32f = 6,
		Rg16f = 7,
		R11fG11fB10f = 8,
		R16f = 9,
		Rgba16 = 10,
		Rgb10A2 = 11,
		Rg16 = 12,
		Rg8 = 13,
		R16 = 14,
		R8 = 15,
		Rgba16Snorm = 16,
		Rg16Snorm = 17,
		Rg8Snorm = 18,
		R16Snorm = 19,
		R8Snorm = 20,
		Rgba32i = 21,
		Rgba16i = 22,
		Rgba8i = 23,
		R32i = 24,
		Rg32i = 25,
		Rg16i = 26,
		Rg8i = 27,
		R16i = 28,
		R8i = 29,
		Rgba32ui = 30,
		Rgba16ui = 31,
		Rgba8ui = 32,
		R32ui = 33,
		Rgb10a2ui = 34,
		Rg32ui = 35,
		Rg16ui = 36,
		Rg8ui = 37,
		R16ui = 38,
		R8ui = 39,
		R64ui = 40,
		R64i = 41
	};

	enum class EImageChannelOrder : std::uint32_t
	{
		R = 0,
		A = 1,
		RG = 2,
		RA = 3,
		RGB = 4,
		RGBA = 5,
		BGRA = 6,
		ARGB = 7,
		Intensity = 8,
		Luminance = 9,
		Rx = 10,
		RGx = 11,
		RGBx = 12,
		Depth = 13,
		DepthStencil = 14,
		sRGB = 15,
		sRGBx = 16,
		sRGBA = 17,
		sBGRA = 18,
		ABGR = 19
	};

	enum class EImageChannelDataType : std::uint32_t
	{
		SnormInt8 = 0,
		SnormInt16 = 1,
		UnormInt8 = 2,
		UnormInt16 = 3,
		UnormShort565 = 4,
		UnormShort555 = 5,
		UnormInt101010 = 6,
		SignedInt8 = 7,
		SignedInt16 = 8,
		SignedInt32 = 9,
		UnsignedInt8 = 10,
		UnsignedInt16 = 11,
		UnsignedInt32 = 12,
		HalfFloat = 13,
		Float = 14,
		UnormInt24 = 15,
		UnormInt101010_2 = 16
	};

	enum class EFPRoundingMode : std::uint32_t
	{
		RTE = 0,
		RTZ = 1,
		RTP = 2,
		RTN = 3
	};

	enum class EFPDenormMode : std::uint32_t
	{
		Preserve = 0,
		FlushToZero = 1
	};

	enum class EQuantizationModes : std::uint32_t
	{
		TRN = 0,
		TRN_ZERO = 1,
		RND = 2,
		RND_ZERO = 3,
		RND_INF = 4,
		RND_MIN_INF = 5,
		RND_CONV = 6,
		RND_CONV_ODD = 7
	};

	enum class EFPOperationMode : std::uint32_t
	{
		IEEE = 0,
		ALT = 1
	};

	enum class EOverflowModes : std::uint32_t
	{
		WRAP = 0,
		SAT = 1,
		SAT_ZERO = 2,
		SAT_SYM = 3
	};

	enum class ELinkageType : std::uint32_t
	{
		Export = 0,
		Import = 1,
		LinkOnceODR = 2
	};

	enum class EAccessQualifier : std::uint32_t
	{
		ReadOnly = 0,
		WriteOnly = 1,
		ReadWrite = 2
	};

	enum class EFunctionParameterAttribute : std::uint32_t
	{
		Zext = 0,
		Sext = 1,
		ByVal = 2,
		Sret = 3,
		NoAlias = 4,
		NoCapture = 5,
		NoWrite = 6,
		NoReadWrite = 7
	};

	enum class EDecoration : std::uint32_t
	{
		RelaxedPrecision = 0,
		SpecId = 1,
		Block = 2,
		BufferBlock = 3,
		RowMajor = 4,
		ColMajor = 5,
		ArrayStride = 6,
		MatrixStride = 7,
		GLSLShared = 8,
		GLSLPacked = 9,
		CPacked = 10,
		BuiltIn = 11,
		NoPerspective = 13,
		Flat = 14,
		Patch = 15,
		Centroid = 16,
		Sample = 17,
		Invariant = 18,
		Restrict = 19,
		Aliased = 20,
		Volatile = 21,
		Constant = 22,
		Coherent = 23,
		NonWritable = 24,
		NonReadable = 25,
		Uniform = 26,
		UniformId = 27,
		SaturatedConversion = 28,
		Stream = 29,
		Location = 30,
		Component = 31,
		Index = 32,
		Binding = 33,
		DescriptorSet = 34,
		Offset = 35,
		XfbBuffer = 36,
		XfbStride = 37,
		FuncParamAttr = 38,
		FPRoundingMode = 39,
		FPFastMathMode = 40,
		LinkageAttributes = 41,
		NoContraction = 42,
		InputAttachmentIndex = 43,
		Alignment = 44,
		MaxByteOffset = 45,
		AlignmentId = 46,
		MaxByteOffsetId = 47,
		NoSignedWrap = 4469,
		NoUnsignedWrap = 4470,
		ExplicitInterpAMD = 4999,
		OverrideCoverageNV = 5248,
		PassthroughNV = 5250,
		ViewportRelativeNV = 5252,
		SecondaryViewportRelativeNV = 5256,
		PerViewNV = 5272,
		PerTaskNV = 5273,
		PerVertexKHR = 5285,
		NonUniform = 5300,
		RestrictPointer = 5355,
		AliasedPointer = 5356,
		BindlessSamplerNV = 5398,
		BindlessImageNV = 5399,
		BoundSamplerNV = 5400,
		BoundImageNV = 5401,
		SIMTCallINTEL = 5599,
		ReferencedIndirectlyINTEL = 5602,
		ClobberINTEL = 5607,
		SideEffectsINTEL = 5608,
		VectorComputeVariableINTEL = 5624,
		FuncParamIOKindINTEL = 5625,
		VectorComputeFunctionINTEL = 5626,
		StackCallINTEL = 5627,
		GlobalVariableOffsetINTEL = 5628,
		UserTypeGOOGLE = 5636,
		FunctionRoundingModeINTEL = 5822,
		FunctionDenormModeINTEL = 5823,
		RegisterINTEL = 5825,
		MemoryINTEL = 5826,
		NumbanksINTEL = 5827,
		BankwidthINTEL = 5828,
		MaxPrivateCopiesINTEL = 5829,
		SinglepumpINTEL = 5830,
		DoublepumpINTEL = 5831,
		MaxReplicatesINTEL = 5832,
		SimpleDualPortINTEL = 5833,
		MergeINTEL = 5834,
		BankBitsINTEL = 5835,
		ForcePow2DepthINTEL = 5836,
		BurstCoalesceINTEL = 5899,
		CacheSizeINTEL = 5900,
		DontStaticallyCoalesceINTEL = 5901,
		PrefetchINTEL = 5902,
		StallEnableINTEL = 5905,
		FuseLoopsInFunctionINTEL = 5907,
		AliasScopeINTEL = 5914,
		NoAliasINTEL = 5915,
		BufferLocationINTEL = 5921,
		IOPipeStorageINTEL = 5944,
		FunctionFloatingPointModeINTEL = 6080,
		SingleElementVectorINTEL = 6085,
		VectorComputeCallableFunctionINTEL = 6087,
		MediaBlockIOINTEL = 6140
	};

	enum class EBuiltIn : std::uint32_t
	{
		Position = 0,
		PointSize = 1,
		ClipDistance = 3,
		CullDistance = 4,
		VertexId = 5,
		InstanceId = 6,
		PrimitiveId = 7,
		InvocationId = 8,
		Layer = 9,
		ViewportIndex = 10,
		TessLevelOuter = 11,
		TessLevelInner = 12,
		TessCoord = 13,
		PatchVertices = 14,
		FragCoord = 15,
		PointCoord = 16,
		FrontFacing = 17,
		SampleId = 18,
		SamplePosition = 19,
		SampleMask = 20,
		FragDepth = 22,
		HelperInvocation = 23,
		NumWorkgroups = 24,
		WorkgroupSize = 25,
		WorkgroupId = 26,
		LocalInvocationId = 27,
		GlobalInvocationId = 28,
		LocalInvocationIndex = 29,
		WorkDim = 30,
		GlobalSize = 31,
		EnqueuedWorkgroupSize = 32,
		GlobalOffset = 33,
		GlobalLinearId = 34,
		SubgroupSize = 36,
		SubgroupMaxSize = 37,
		NumSubgroups = 38,
		NumEnqueuedSubgroups = 39,
		SubgroupId = 40,
		SubgroupLocalInvocationId = 41,
		VertexIndex = 42,
		InstanceIndex = 43,
		SubgroupEqMask = 4416,
		SubgroupGeMask = 4417,
		SubgroupGtMask = 4418,
		SubgroupLeMask = 4419,
		SubgroupLtMask = 4420,
		BaseVertex = 4424,
		BaseInstance = 4425,
		DrawIndex = 4426,
		PrimitiveShadingRateKHR = 4432,
		DeviceIndex = 4438,
		ViewIndex = 4440,
		ShadingRateKHR = 4444,
		BaryCoordNoPerspAMD = 4992,
		BaryCoordNoPerspCentroidAMD = 4993,
		BaryCoordNoPerspSampleAMD = 4994,
		BaryCoordSmoothAMD = 4995,
		BaryCoordSmoothCentroidAMD = 4996,
		BaryCoordSmoothSampleAMD = 4997,
		BaryCoordPullModelAMD = 4998,
		FragStencilRefEXT = 5014,
		ViewportMaskNV = 5253,
		SecondaryPositionNV = 5257,
		SecondaryViewportMaskNV = 5258,
		PositionPerViewNV = 5261,
		ViewportMaskPerViewNV = 5262,
		FullyCoveredEXT = 5264,
		TaskCountNV = 5274,
		PrimitiveCountNV = 5275,
		PrimitiveIndicesNV = 5276,
		ClipDistancePerViewNV = 5277,
		CullDistancePerViewNV = 5278,
		LayerPerViewNV = 5279,
		MeshViewCountNV = 5280,
		MeshViewIndicesNV = 5281,
		BaryCoordKHR = 5286,
		BaryCoordNoPerspKHR = 5287,
		FragSizeEXT = 5292,
		FragInvocationCountEXT = 5293,
		PrimitivePointIndicesEXT = 5294,
		PrimitiveLineIndicesEXT = 5295,
		PrimitiveTriangleIndicesEXT = 5296,
		CullPrimitiveEXT = 5299,
		HitTNV = 5332,
		CurrentRayTimeNV = 5334,
		RayGeometryIndexKHR = 5352,
		WarpsPerSMNV = 5374,
		SMCountNV = 5375,
		WarpIDNV = 5376,
		SMIDNV = 5377,
		CullMaskKHR = 6021
	};

	enum class EScope : std::uint32_t
	{
		CrossDevice = 0,
		Device = 1,
		Workgroup = 2,
		Subgroup = 3,
		Invocation = 4,
		QueueFamily = 5,
		ShaderCallKHR = 6
	};

	enum class EGroupOperation : std::uint32_t
	{
		Reduce = 0,
		InclusiveScan = 1,
		ExclusiveScan = 2,
		ClusteredReduce = 3,
		PartitionedReduceNV = 6,
		PartitionedInclusiveScanNV = 7,
		PartitionedExclusiveScanNV = 8
	};

	enum class EKernelEnqueueFlags : std::uint32_t
	{
		NoWait = 0,
		WaitKernel = 1,
		WaitWorkGroup = 2
	};

	enum class ECapability : std::uint32_t
	{
		Matrix = 0,
		Shader = 1,
		Geometry = 2,
		Tessellation = 3,
		Addresses = 4,
		Linkage = 5,
		Kernel = 6,
		Vector16 = 7,
		Float16Buffer = 8,
		Float16 = 9,
		Float64 = 10,
		Int64 = 11,
		Int64Atomics = 12,
		ImageBasic = 13,
		ImageReadWrite = 14,
		ImageMipmap = 15,
		Pipes = 17,
		Groups = 18,
		DeviceEnqueue = 19,
		LiteralSampler = 20,
		AtomicStorage = 21,
		Int16 = 22,
		TessellationPointSize = 23,
		GeometryPointSize = 24,
		ImageGatherExtended = 25,
		StorageImageMultisample = 27,
		UniformBufferArrayDynamicIndexing = 28,
		SampledImageArrayDynamicIndexing = 29,
		StorageBufferArrayDynamicIndexing = 30,
		StorageImageArrayDynamicIndexing = 31,
		ClipDistance = 32,
		CullDistance = 33,
		ImageCubeArray = 34,
		SampleRateShading = 35,
		ImageRect = 36,
		SampledRect = 37,
		GenericPointer = 38,
		Int8 = 39,
		InputAttachment = 40,
		SparseResidency = 41,
		MinLod = 42,
		Sampled1D = 43,
		Image1D = 44,
		SampledCubeArray = 45,
		SampledBuffer = 46,
		ImageBuffer = 47,
		ImageMSArray = 48,
		StorageImageExtendedFormats = 49,
		ImageQuery = 50,
		DerivativeControl = 51,
		InterpolationFunction = 52,
		TransformFeedback = 53,
		GeometryStreams = 54,
		StorageImageReadWithoutFormat = 55,
		StorageImageWriteWithoutFormat = 56,
		MultiViewport = 57,
		SubgroupDispatch = 58,
		NamedBarrier = 59,
		PipeStorage = 60,
		GroupNonUniform = 61,
		GroupNonUniformVote = 62,
		GroupNonUniformArithmetic = 63,
		GroupNonUniformBallot = 64,
		GroupNonUniformShuffle = 65,
		GroupNonUniformShuffleRelative = 66,
		GroupNonUniformClustered = 67,
		GroupNonUniformQuad = 68,
		ShaderLayer = 69,
		ShaderViewportIndex = 70,
		UniformDecoration = 71,
		FragmentShadingRateKHR = 4422,
		SubgroupBallotKHR = 4423,
		DrawParameters = 4427,
		WorkgroupMemoryExplicitLayoutKHR = 4428,
		WorkgroupMemoryExplicitLayout8BitAccessKHR = 4429,
		WorkgroupMemoryExplicitLayout16BitAccessKHR = 4430,
		SubgroupVoteKHR = 4431,
		StoragePushConstant16 = 4435,
		StorageInputOutput16 = 4436,
		DeviceGroup = 4437,
		MultiView = 4439,
		VariablePointersStorageBuffer = 4441,
		VariablePointers = 4442,
		AtomicStorageOps = 4445,
		SampleMaskPostDepthCoverage = 4447,
		StorageBuffer8BitAccess = 4448,
		UniformAndStorageBuffer8BitAccess = 4449,
		StoragePushConstant8 = 4450,
		DenormPreserve = 4464,
		DenormFlushToZero = 4465,
		SignedZeroInfNanPreserve = 4466,
		RoundingModeRTE = 4467,
		RoundingModeRTZ = 4468,
		RayQueryProvisionalKHR = 4471,
		RayQueryKHR = 4472,
		RayTraversalPrimitiveCullingKHR = 4478,
		RayTracingKHR = 4479,
		Float16ImageAMD = 5008,
		ImageGatherBiasLodAMD = 5009,
		FragmentMaskAMD = 5010,
		StencilExportEXT = 5013,
		ImageReadWriteLodAMD = 5015,
		Int64ImageEXT = 5016,
		ShaderClockKHR = 5055,
		SampleMaskOverrideCoverageNV = 5249,
		GeometryShaderPassthroughNV = 5251,
		ShaderViewportIndexLayerEXT = 5254,
		ShaderViewportMaskNV = 5255,
		ShaderStereoViewNV = 5259,
		PerViewAttributesNV = 5260,
		FragmentFullyCoveredEXT = 5265,
		MeshShadingNV = 5266,
		ImageFootprintNV = 5282,
		MeshShadingEXT = 5283,
		FragmentBarycentricKHR = 5284,
		ComputeDerivativeGroupQuadsNV = 5288,
		FragmentDensityEXT = 5291,
		GroupNonUniformPartitionedNV = 5297,
		ShaderNonUniform = 5301,
		RuntimeDescriptorArray = 5302,
		InputAttachmentArrayDynamicIndexing = 5303,
		UniformTexelBufferArrayDynamicIndexing = 5304,
		StorageTexelBufferArrayDynamicIndexing = 5305,
		UniformBufferArrayNonUniformIndexing = 5306,
		SampledImageArrayNonUniformIndexing = 5307,
		StorageBufferArrayNonUniformIndexing = 5308,
		StorageImageArrayNonUniformIndexing = 5309,
		InputAttachmentArrayNonUniformIndexing = 5310,
		UniformTexelBufferArrayNonUniformIndexing = 5311,
		StorageTexelBufferArrayNonUniformIndexing = 5312,
		RayTracingNV = 5340,
		RayTracingMotionBlurNV = 5341,
		VulkanMemoryModel = 5345,
		VulkanMemoryModelDeviceScope = 5346,
		PhysicalStorageBufferAddresses = 5347,
		ComputeDerivativeGroupLinearNV = 5350,
		RayTracingProvisionalKHR = 5353,
		CooperativeMatrixNV = 5357,
		FragmentShaderSampleInterlockEXT = 5363,
		FragmentShaderShadingRateInterlockEXT = 5372,
		ShaderSMBuiltinsNV = 5373,
		FragmentShaderPixelInterlockEXT = 5378,
		DemoteToHelperInvocation = 5379,
		BindlessTextureNV = 5390,
		SubgroupShuffleINTEL = 5568,
		SubgroupBufferBlockIOINTEL = 5569,
		SubgroupImageBlockIOINTEL = 5570,
		SubgroupImageMediaBlockIOINTEL = 5579,
		RoundToInfinityINTEL = 5582,
		FloatingPointModeINTEL = 5583,
		IntegerFunctions2INTEL = 5584,
		FunctionPointersINTEL = 5603,
		IndirectReferencesINTEL = 5604,
		AsmINTEL = 5606,
		AtomicFloat32MinMaxEXT = 5612,
		AtomicFloat64MinMaxEXT = 5613,
		AtomicFloat16MinMaxEXT = 5616,
		VectorComputeINTEL = 5617,
		VectorAnyINTEL = 5619,
		ExpectAssumeKHR = 5629,
		SubgroupAvcMotionEstimationINTEL = 5696,
		SubgroupAvcMotionEstimationIntraINTEL = 5697,
		SubgroupAvcMotionEstimationChromaINTEL = 5698,
		VariableLengthArrayINTEL = 5817,
		FunctionFloatControlINTEL = 5821,
		FPGAMemoryAttributesINTEL = 5824,
		FPFastMathModeINTEL = 5837,
		ArbitraryPrecisionIntegersINTEL = 5844,
		ArbitraryPrecisionFloatingPointINTEL = 5845,
		UnstructuredLoopControlsINTEL = 5886,
		FPGALoopControlsINTEL = 5888,
		KernelAttributesINTEL = 5892,
		FPGAKernelAttributesINTEL = 5897,
		FPGAMemoryAccessesINTEL = 5898,
		FPGAClusterAttributesINTEL = 5904,
		LoopFuseINTEL = 5906,
		MemoryAccessAliasingINTEL = 5910,
		FPGABufferLocationINTEL = 5920,
		ArbitraryPrecisionFixedPointINTEL = 5922,
		USMStorageClassesINTEL = 5935,
		IOPipesINTEL = 5943,
		BlockingPipesINTEL = 5945,
		FPGARegINTEL = 5948,
		DotProductInputAll = 6016,
		DotProductInput4x8Bit = 6017,
		DotProductInput4x8BitPacked = 6018,
		DotProduct = 6019,
		RayCullMaskKHR = 6020,
		BitInstructions = 6025,
		GroupNonUniformRotateKHR = 6026,
		AtomicFloat32AddEXT = 6033,
		AtomicFloat64AddEXT = 6034,
		LongConstantCompositeINTEL = 6089,
		OptNoneINTEL = 6094,
		AtomicFloat16AddEXT = 6095,
		DebugInfoModuleINTEL = 6114,
		SplitBarrierINTEL = 6141,
		GroupUniformArithmeticKHR = 6400
	};

	enum class ERayQueryIntersection : std::uint32_t
	{
		RayQueryCandidateIntersectionKHR = 0,
		RayQueryCommittedIntersectionKHR = 1
	};

	enum class ERayQueryCommittedIntersectionType : std::uint32_t
	{
		RayQueryCommittedIntersectionNoneKHR = 0,
		RayQueryCommittedIntersectionTriangleKHR = 1,
		RayQueryCommittedIntersectionGeneratedKHR = 2
	};

	enum class ERayQueryCandidateIntersectionType : std::uint32_t
	{
		RayQueryCandidateIntersectionTriangleKHR = 0,
		RayQueryCandidateIntersectionAABBKHR = 1
	};

	enum class EPackedVectorFormat : std::uint32_t
	{
		PackedVectorFormat4x8Bit = 0
	};

	void RequireCapExtImageOperands(EImageOperands value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtFPFastMathMode(EFPFastMathMode value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtSelectionControl(ESelectionControl value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtLoopControl(ELoopControl value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtFunctionControl(EFunctionControl value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtMemorySemantics(EMemorySemantics value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtMemoryAccess(EMemoryAccess value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtKernelProfilingInfo(EKernelProfilingInfo value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtRayFlags(ERayFlags value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtFragmentShadingRate(EFragmentShadingRate value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtSourceLanguage(ESourceLanguage value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtExecutionModel(EExecutionModel value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtAddressingModel(EAddressingModel value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtMemoryModel(EMemoryModel value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtExecutionMode(EExecutionMode value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtStorageClass(EStorageClass value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtDim(EDim value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtSamplerAddressingMode(ESamplerAddressingMode value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtSamplerFilterMode(ESamplerFilterMode value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtImageFormat(EImageFormat value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtImageChannelOrder(EImageChannelOrder value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtImageChannelDataType(EImageChannelDataType value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtFPRoundingMode(EFPRoundingMode value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtFPDenormMode(EFPDenormMode value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtQuantizationModes(EQuantizationModes value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtFPOperationMode(EFPOperationMode value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtOverflowModes(EOverflowModes value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtLinkageType(ELinkageType value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtAccessQualifier(EAccessQualifier value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtFunctionParameterAttribute(EFunctionParameterAttribute value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtDecoration(EDecoration value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtBuiltIn(EBuiltIn value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtScope(EScope value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtGroupOperation(EGroupOperation value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtKernelEnqueueFlags(EKernelEnqueueFlags value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtCapability(ECapability value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtRayQueryIntersection(ERayQueryIntersection value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtRayQueryCommittedIntersectionType(ERayQueryCommittedIntersectionType value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtRayQueryCandidateIntersectionType(ERayQueryCandidateIntersectionType value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
	void RequireCapExtPackedVectorFormat(EPackedVectorFormat value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);
} // namespace Frertex::Transpilers::SPIRV