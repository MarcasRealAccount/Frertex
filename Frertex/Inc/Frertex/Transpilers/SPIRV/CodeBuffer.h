#pragma once

#include "Enumerations.h"
#include "Frertex/Utils/Buffer.h"

#include <cstdint>

#include <set>
#include <string_view>
#include <vector>

namespace Frertex::Transpilers::SPIRV
{
	struct CodeBuffer
	{
	public:
		void OpNop();
		void OpUndef(std::uint32_t resultType, std::uint32_t resultID);
		void OpSizeOf(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer);
		void OpAssumeTrueKHR(std::uint32_t condition);
		void OpExpectKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t value, std::uint32_t expectedValue);

		void OpSourceContinued(std::string_view continuedSource);
		void OpSource(ESourceLanguage sourceLanguage, std::uint32_t version, std::uint32_t file, std::string_view source);
		void OpSourceExtension(std::string_view extension);
		void OpName(std::uint32_t target, std::string_view name);
		void OpMemberName(std::uint32_t type, std::uint32_t member, std::string_view name);
		void OpString(std::uint32_t resultID, std::string_view string);
		void OpLine(std::uint32_t file, std::uint32_t line, std::uint32_t column);
		void OpNoLine();
		void OpModuleProcessed(std::string_view process);

		void OpDecorate(std::uint32_t target, EDecoration decoration, const std::vector<std::uint32_t>& operands);
		void OpMemberDecorate(std::uint32_t structureType, std::uint32_t member, EDecoration decoration, const std::vector<std::uint32_t>& operands);
		// DEPRECATED: void OpDecorationGroup(std::uint32_t resultID)
		// DEPRECATED: void OpGroupDecorate(std::uint32_t group, const std::vector<std::uint32_t>& targets)
		// DEPRECATED: void OpGroupMemberDecorate(std::uint32_t group, const std::vector<std::uint32_t>& targets)
		void OpDecorateId(std::uint32_t target, EDecoration decoration, const std::vector<std::uint32_t>& operands);
		void OpDecorateString(std::uint32_t target, EDecoration decoration, const std::vector<std::string_view>& operands);
		void OpMemberDecorateString(std::uint32_t structureType, std::uint32_t member, EDecoration decoration, const std::vector<std::string_view>& operands);

		void OpExtension(std::string_view name);
		void OpExtInstImport(std::uint32_t resultID, std::string_view name);
		void OpExtInst(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t set, std::uint32_t instruction, const std::vector<std::uint32_t>& operandIDs);

		void OpMemoryModel(EAddressingMode addressingModel, EMemoryModel memoryModel);
		void OpEntryPoint(EExecutionModel executionModel, std::uint32_t entryPoint, std::string_view name, const std::vector<std::uint32_t>& interface);
		void OpExecutionMode(std::uint32_t entryPoint, EExecutionMode executionMode, const std::vector<std::uint32_t>& operands);
		void OpCapability(ECapability capability);
		void OpExecutionModeId(std::uint32_t entryPoint, EExecutionMode executionMode, const std::vector<std::uint32_t>& operandIDs);

		void OpTypeVoid(std::uint32_t resultID);
		void OpTypeBool(std::uint32_t resultID);
		void OpTypeInt(std::uint32_t resultID, std::uint32_t width, std::uint32_t signedness);
		void OpTypeFloat(std::uint32_t resultID, std::uint32_t width);
		void OpTypeVector(std::uint32_t resultID, std::uint32_t componentType, std::uint32_t componentCount);
		void OpTypeMatrix(std::uint32_t resultID, std::uint32_t columnType, std::uint32_t columnCount);
		void OpTypeImage(std::uint32_t resultID, std::uint32_t sampledType, EDim dim, std::uint32_t depth, std::uint32_t arrayed, std::uint32_t ms, std::uint32_t sampled, EImageFormat imageFormat);
		void OpTypeImage(std::uint32_t resultID, std::uint32_t sampledType, EDim dim, std::uint32_t depth, std::uint32_t arrayed, std::uint32_t ms, std::uint32_t sampled, EImageFormat imageFormat, EAccessQualifier accessQualifier);
		void OpTypeSampler(std::uint32_t resultID);
		void OpTypeSampledImage(std::uint32_t resultID, std::uint32_t imageType);
		void OpTypeArray(std::uint32_t resultID, std::uint32_t elementType, std::uint32_t length);
		void OpTypeRuntimeArray(std::uint32_t resultID, std::uint32_t elementType);
		void OpTypeStruct(std::uint32_t resultID, const std::vector<std::uint32_t>& memberTypes);
		void OpTypeOpaque(std::uint32_t resultID, std::string_view name);
		void OpTypePointer(std::uint32_t resultID, EStorageClass storageClass, std::uint32_t type);
		void OpTypeFunction(std::uint32_t resultID, std::uint32_t returnType, const std::vector<std::uint32_t>& parameterTypes);
		void OpTypeEvent(std::uint32_t resultID);
		void OpTypeDeviceEvent(std::uint32_t resultID);
		void OpTypeReserveId(std::uint32_t resultID);
		void OpTypeQueue(std::uint32_t resultID);
		void OpTypePipe(std::uint32_t resultID, EAccessQualifier accessQualifier);
		void OpTypeForwardPointer(std::uint32_t pointerType, EStorageClass storageClass);
		void OpTypePipeStorage(std::uint32_t resultID);
		void OpTypeNamedBarrier(std::uint32_t resultID);
		void OpTypeBufferSurfaceINTEL(std::uint32_t resultID, EAccessQualifier accessQualifier);
		void OpTypeStructContinuedINTEL(const std::vector<std::uint32_t>& memberTypes);

		void OpConstantTrue(std::uint32_t resultType, std::uint32_t resultID);
		void OpConstantFalse(std::uint32_t resultType, std::uint32_t resultID);
		void OpConstant(std::uint32_t resultType, std::uint32_t resultID, const std::vector<std::uint32_t>& value);
		void OpConstantComposite(std::uint32_t resultType, std::uint32_t resultID, const std::vector<std::uint32_t>& constituents);
		void OpConstantSampler(std::uint32_t resultType, std::uint32_t resultID, ESamplerAddressingMode addressingMode, std::uint32_t param, ESamplerFilterMode filterMode);
		void OpConstantNull(std::uint32_t resultType, std::uint32_t resultID);
		void OpSpecConstantTrue(std::uint32_t resultType, std::uint32_t resultID);
		void OpSpecConstantFalse(std::uint32_t resultType, std::uint32_t resultID);
		void OpSpecConstant(std::uint32_t resultType, std::uint32_t resultID, const std::vector<std::uint32_t>& value);
		void OpSpecConstantComposite(std::uint32_t resultType, std::uint32_t resultID, const std::vector<std::uint32_t>& constituents);
		void OpSpecConstantOp(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opcode, const std::vector<std::uint32_t>& operands);
		void OpConstantCompositeContinuedINTEL(const std::vector<std::uint32_t>& constituents);
		void OpSpecConstantCompositeContinuedINTEL(const std::vector<std::uint32_t>& constituents);

		void OpVariable(std::uint32_t resultType, std::uint32_t resultID, EStorageClass storageClass, std::uint32_t initializer);
		void OpImageTexelPointer(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t coordinate, std::uint32_t sample);
		void OpLoad(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, EMemoryOperands memoryOperands = EMemoryOperands::None);
		void OpStore(std::uint32_t pointer, std::uint32_t object, EMemoryOperands memoryOperands = EMemoryOperands::None);
		void OpCopyMemory(std::uint32_t target, std::uint32_t source, EMemoryOperands targetMemoryOperands = EMemoryOperands::None, EMemoryOperands sourceMemoryOperands = EMemoryOperands::None);
		void OpCopyMemorySized(std::uint32_t target, std::uint32_t source, std::uint32_t size, EMemoryOperands targetMemoryOperands = EMemoryOperands::None, EMemoryOperands sourceMemoryOperands = EMemoryOperands::None);
		void OpAccessChain(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t base, const std::vector<std::uint32_t>& indices);
		void OpInBoundsAccessChain(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t base, const std::vector<std::uint32_t>& indices);
		void OpPtrAccessChain(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t base, std::uint32_t element, const std::vector<std::uint32_t>& indices);
		void OpArrayLength(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t structure, std::uint32_t lastMember);
		void OpGenericPtrMemSemantics(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer);
		void OpInBoundsPtrAccessChain(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t base, std::uint32_t element, const std::vector<std::uint32_t>& indices);
		void OpPtrEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpPtrNotEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpPtrDiff(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);

		void OpFunction(std::uint32_t resultType, std::uint32_t resultID, EFunctionControl functionControl, std::uint32_t functionType);
		void OpFunctionParameter(std::uint32_t resultType, std::uint32_t resultID);
		void OpFunctionEnd();
		void OpFunctionCall(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t function, const std::vector<std::uint32_t>& arguments);

		void OpSampledImage(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t image, std::uint32_t sampler);
		void OpImageSampleImplicitLod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands);
		void OpImageSampleExplicitLot(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, EImageOperands imageOperands, std::uint32_t operand1, const std::vector<std::uint32_t>& operands);
		void OpImageSampleDrefImplicitLod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, std::uint32_t Dref, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands);
		void OpImageSampleDrefExplicitLod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, std::uint32_t Dref, EImageOperands imageOperands, std::uint32_t operand1, const std::vector<std::uint32_t>& operands);
		void OpImageSampleProjImplicitLod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands);
		void OpImageSampleProjExplicitLod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, EImageOperands imageOperands, std::uint32_t operand1, const std::vector<std::uint32_t>& operands);
		void OpImageSampleProjDrefImplicitLod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, std::uint32_t Dref, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands);
		void OpImageSampleProjDrefExplicitLod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, std::uint32_t Dref, EImageOperands imageOperands, std::uint32_t operand1, const std::vector<std::uint32_t>& operands);
		void OpImageFetch(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t image, std::uint32_t coordinate, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands);
		void OpImageGather(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t image, std::uint32_t coordinate, std::uint32_t component, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands);
		void OpImageDrefGather(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t image, std::uint32_t coordinate, std::uint32_t Dref, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands);
		void OpImageRead(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t image, std::uint32_t coordinate, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands);
		void OpImageWrite(std::uint32_t image, std::uint32_t coordinate, std::uint32_t texel, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands);
		void OpImage(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage);
		void OpImageQueryFormat(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t image);
		void OpImageQueryOrder(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t image);
		void OpImageQuerySizeLod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t image, std::uint32_t levelOfDetail);
		void OpImageQuerySize(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t image);
		void OpImageQueryLod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate);
		void OpImageQueryLevels(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t image);
		void OpImageQuerySamples(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t image);
		void OpImageSparseSampleImplicitLod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands);
		void OpImageSparseSampleExplicitLod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, EImageOperands imageOperands, std::uint32_t operand1, const std::vector<std::uint32_t>& operands);
		void OpImageSparseSampleDrefImplicitLod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, std::uint32_t Dref, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands);
		void OpImageSparseSampleDrefExplicitLod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, std::uint32_t Dref, EImageOperands imageOperands, std::uint32_t operand1, const std::vector<std::uint32_t>& operands);
		void OpImageSparseSampleProjImplicitLod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands);
		void OpImageSparseSampleProjExplicitLod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, EImageOperands imageOperands, std::uint32_t operand1, const std::vector<std::uint32_t>& operands);
		void OpImageSparseSampleProjDrefImplicitLod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, std::uint32_t Dref, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands);
		void OpImageSparseSampleProjDrefExplicitLod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, std::uint32_t Dref, EImageOperands imageOperands, std::uint32_t operand1, const std::vector<std::uint32_t>& operands);
		void OpImageSparseFetch(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t image, std::uint32_t coordinate, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands);
		void OpImageSparseGather(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, std::uint32_t component, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands);
		void OpImageSparseDrefGather(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, std::uint32_t Dref, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands);
		void OpImageSparseTexelsResident(std::uint32_t resultType, std::uint32_t resultId, std::uint32_t residentCode);
		void OpImageSparseRead(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t image, std::uint32_t coordinate, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands);
		void OpImageSampleFootprintNV(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, std::uint32_t granularity, std::uint32_t coarse, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands);

		void OpConvertFToU(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t floatValue);
		void OpConvertFToS(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t floatValue);
		void OpConvertSToF(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t signedValue);
		void OpConvertUToF(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t unsignedValue);
		void OpUConvert(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t unsignedValue);
		void OpSConvert(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t signedValue);
		void OpFConvert(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t floatValue);
		void OpQuantizeToF16(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t value);
		void OpConvertPtrToU(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer);
		void OpSatConvertSToU(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t signedValue);
		void OpSatConvertUToS(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t unsignedValue);
		void OpConvertUToPtr(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t integerValue);
		void OpPtrCastToGeneric(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer);
		void OpGenericCastToPtr(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer);
		void OpGenericCastToPtrExplicit(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, EStorageClass storageClass);
		void OpBitcast(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand);

		void OpVectorExtractDynamic(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t vector, std::uint32_t index);
		void OpVectorInsertDynamic(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t vector, std::uint32_t component, std::uint32_t index);
		void OpVectorShuffle(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t vector1, std::uint32_t vector2, const std::vector<std::uint32_t>& components);
		void OpCompositeConstruct(std::uint32_t resultType, std::uint32_t resultID, const std::vector<std::uint32_t>& constituents);
		void OpCompositeExtract(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t composite, const std::vector<std::uint32_t>& indices);
		void OpCompositeInsert(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t object, std::uint32_t composite, const std::vector<std::uint32_t>& indices);
		void OpCopyObject(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand);
		void OpTranspose(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t matrix);
		void OpCopyLogical(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand);

		void OpSNegate(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand);
		void OpFNegate(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand);
		void OpIAdd(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpFAdd(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpISub(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpFSub(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpIMul(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpFMul(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpUDiv(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpSDiv(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpFDiv(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpUMod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpSRem(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpSMod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpFRem(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpFMod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpVectorTimesScalar(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t vector, std::uint32_t scalar);
		void OpMatrixTimesScalar(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t matrix, std::uint32_t scalar);
		void OpVectorTimesMatrix(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t vector, std::uint32_t matrix);
		void OpMatrixTimesVector(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t matrix, std::uint32_t vector);
		void OpMatrixTimesMatrix(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t matrix1, std::uint32_t matrix2);
		void OpOuterProduct(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t vector1, std::uint32_t vector2);
		void OpDot(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t vector1, std::uint32_t vector2);
		void OpIAddCarry(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpISubBorrow(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpUMulExtended(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpSMulExtended(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpSDot(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t vector1, std::uint32_t vector2, EPackedVectorFormat packedVectorFormat);
		void OpUDot(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t vector1, std::uint32_t vector2, EPackedVectorFormat packedVectorFormat);
		void OpSUDot(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t vector1, std::uint32_t vector2, EPackedVectorFormat packedVectorFormat);
		void OpSDotAccSat(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t vector1, std::uint32_t vector2, std::uint32_t accumulator, EPackedVectorFormat packedVectorFormat);
		void OpUDotAccSat(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t vector1, std::uint32_t vector2, std::uint32_t accumulator, EPackedVectorFormat packedVectorFormat);
		void OpSUDotAccSat(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t vector1, std::uint32_t vector2, std::uint32_t accumulator, EPackedVectorFormat packedVectorFormat);

		void OpShiftRightLogical(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t base, std::uint32_t shift);
		void OpShiftRightArithmetic(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t base, std::uint32_t shift);
		void OpShiftLeftLogical(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t base, std::uint32_t shift);
		void OpShiftLeftArithmetic(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t base, std::uint32_t shift);
		void OpBitwiseOr(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpBitwiseXor(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpBitwiseAnd(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpNot(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand);
		void OpBitFieldInsert(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t base, std::uint32_t insert, std::uint32_t offset, std::uint32_t count);
		void OpBitFieldSExtract(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t base, std::uint32_t offset, std::uint32_t count);
		void OpBitFieldUExtract(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t base, std::uint32_t offset, std::uint32_t count);
		void OpBitReverse(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t base);
		void OpBitCount(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t base);

		void OpAny(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t vector);
		void OpAll(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t vector);
		void OpIsNan(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t x);
		void OpIsInf(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t x);
		void OpIsFinite(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t x);
		void OpIsNormal(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t x);
		void OpSignBitSet(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t x);
		void OpLessOrGreater(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t x, std::uint32_t y);
		void OpOrdered(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t x, std::uint32_t y);
		void OpUnordered(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t x, std::uint32_t y);
		void OpLogicalEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpLogicalNotEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpLogicalOr(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpLogicalAnd(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpLogicalNot(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand);
		void OpSelect(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t condition, std::uint32_t object1, std::uint32_t object2);
		void OpIEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpINotEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpUGreaterThan(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpSGreaterThan(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpUGreaterThanEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpSGreaterThanEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpULessThan(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpSLessThan(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpULessThanEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpSLessThanEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpFOrdEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpFUnordEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpFOrdNotEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpFUnordNotEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpFOrdLessThan(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpFUnordLessThan(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpFOrdGreaterThan(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpFUnordGreaterThan(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpFOrdLessThanEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpFUnordLessThanEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpFOrdGreaterThanEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);
		void OpFUnordGreaterThanEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2);

		void OpDPdx(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t P);
		void OpDPdy(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t P);
		void OpFWidth(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t P);
		void OpDPdxFine(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t P);
		void OpDPdyFine(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t P);
		void OpFWidthFine(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t P);
		void OpDPdxCoarse(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t P);
		void OpDPdyCoarse(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t P);
		void OpFWidthCoarse(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t P);

		void OpPhi(std::uint32_t resultType, std::uint32_t resultID, const std::vector<std::pair<std::uint32_t, std::uint32_t>>& variableParentPairs);
		void OpLoopMerge(std::uint32_t mergeBlock, std::uint32_t continueTarget, ELoopControl loopControl, const std::vector<std::uint32_t>& loopControlParameters);
		void OpSelectionMerge(std::uint32_t mergeBlock, ESelectionControl selectionControl);
		void OpLabel(std::uint32_t resultID);
		void OpBranch(std::uint32_t targetLabel);
		void OpBranchConditional(std::uint32_t condition, std::uint32_t trueLabel, std::uint32_t falseLabel, const std::vector<std::uint32_t>& branchWeights);
		void OpSwitch(std::uint32_t selector, std::uint32_t def, const std::vector<std::pair<std::uint32_t, std::uint32_t>>& target);
		// DEPRECATED: void OpKill();
		void OpReturn();
		void OpReturnValue(std::uint32_t value);
		void OpUnreachable();
		void OpLifetimeStart(std::uint32_t pointer, std::uint32_t size);
		void OpLifetimeStop(std::uint32_t pointer, std::uint32_t size);
		void OpTerminateInvocation();
		void OpDemoteToHelperInvocation();

		void OpAtomicLoad(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics);
		void OpAtomicStore(std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics, std::uint32_t value);
		void OpAtomicExchange(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics, std::uint32_t value);
		void OpAtomicCompareExchange(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semanticsEqual, std::uint32_t semanticsUnequal, std::uint32_t value, std::uint32_t comparator);
		void OpAtomicCompareExchangeWeak(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semanticsEqual, std::uint32_t semanticsUnequal, std::uint32_t value, std::uint32_t comparator);
		void OpAtomicIIncrement(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics);
		void OpAtomicIDecrement(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics);
		void OpAtomicIAdd(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics, std::uint32_t value);
		void OpAtomicISub(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics, std::uint32_t value);
		void OpAtomicSMin(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics, std::uint32_t value);
		void OpAtomicUMin(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics, std::uint32_t value);
		void OpAtomicSMax(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics, std::uint32_t value);
		void OpAtomicUMax(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics, std::uint32_t value);
		void OpAtomicAnd(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics, std::uint32_t value);
		void OpAtomicOr(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics, std::uint32_t value);
		void OpAtomicXor(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics, std::uint32_t value);
		void OpAtomicFlagTestAndSet(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics);
		void OpAtomicFlagClear(std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics);
		void OpAtomicFMinEXT(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics, std::uint32_t value);
		void OpAtomicFMaxEXT(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics, std::uint32_t value);
		void OpAtomicFAddEXT(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics, std::uint32_t value);

		void OpEmitVertex();
		void OpEndPrimitive();
		void OpEmitStreamVertex(std::uint32_t stream);
		void OpEndStreamPrimitive(std::uint32_t stream);

		void OpControlBarrier(std::uint32_t execution, std::uint32_t memory, std::uint32_t semantics);
		void OpMemoryBarrier(std::uint32_t memory, std::uint32_t semantics);
		void OpNamedBarrierInitialize(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t subgroupCount);
		void OpMemoryNamedBarrier(std::uint32_t namedBarrier, std::uint32_t memory, std::uint32_t semantics);
		void OpControlBarrierArriveINTEL(std::uint32_t execution, std::uint32_t memory, std::uint32_t semantics);
		void OpControlBarrierWaitINTEL(std::uint32_t execution, std::uint32_t memory, std::uint32_t semantics);

		void OpGroupAsyncCopy(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t destination, std::uint32_t source, std::uint32_t numElements, std::uint32_t stride, std::uint32_t event);
		void OpGroupWaitEvents(std::uint32_t execution, std::uint32_t numEvents, std::uint32_t eventsList);
		void OpGroupAll(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t predicate);
		void OpGroupAny(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t predicate);
		void OpGroupBroadcast(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t value, std::uint32_t localID);
		void OpGroupIAdd(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x);
		void OpGroupFAdd(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x);
		void OpGroupFMin(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x);
		void OpGroupUMin(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x);
		void OpGroupSMin(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x);
		void OpGroupFMax(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x);
		void OpGroupUMax(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x);
		void OpGroupSMax(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x);
		void OpSubgroupBallotKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t value);
		void OpSubgroupFirstInvocationKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t value);
		void OpSubgroupAllKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t predicate);
		void OpSubgroupAnyKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t predicate);
		void OpSubgroupAllEqualKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t predicate);
		void OpGroupNonUniformRotateKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t value, std::uint32_t delta, std::uint32_t clusterSize);
		void OpSubgroupReadInvocationKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t value, std::uint32_t index);
		void OpGroupIAddNonUniformAMD(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x);
		void OpGroupFAddNonUniformAMD(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x);
		void OpGroupFMinNonUniformAMD(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x);
		void OpGroupUMinNonUniformAMD(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x);
		void OpGroupSMinNonUniformAMD(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x);
		void OpGroupFMaxNonUniformAMD(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x);
		void OpGroupUMaxNonUniformAMD(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x);
		void OpGroupSMaxNonUniformAMD(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x);
		void OpSubgroupSuffleINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t data, std::uint32_t invocationID);
		void OpSubgroupSuffleDownINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t current, std::uint32_t next, std::uint32_t data);
		void OpSubgroupSuffleUpINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t current, std::uint32_t next, std::uint32_t data);
		void OpSubgroupSuffleXorINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t data, std::uint32_t value);
		void OpSubgroupBlockReadINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t ptr);
		void OpSubgroupBlockWriteINTEL(std::uint32_t ptr, std::uint32_t data);
		void OpSubgroupImageBlockReadINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t image, std::uint32_t coordinate);
		void OpSubgroupImageBlockWriteINTEL(std::uint32_t image, std::uint32_t coordinate, std::uint32_t data);
		void OpSubgroupImageMediaBlockReadINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t image, std::uint32_t coordinate, std::uint32_t width, std::uint32_t height);
		void OpSubgroupImageMediaBlockWriteINTEL(std::uint32_t image, std::uint32_t coordinate, std::uint32_t width, std::uint32_t height, std::uint32_t data);
		void OpGroupIMulKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x);
		void OpGroupFMulKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x);
		void OpGroupBitwiseAndKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x);
		void OpGroupBitwiseOrKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x);
		void OpGroupBitwiseXorKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x);
		void OpGroupLogicalAndKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x);
		void OpGroupLogicalOrKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x);
		void OpGroupLogicalXorKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x);

		void OpEnqueueMarker(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t queue, std::uint32_t numEvents, std::uint32_t waitEvents, std::uint32_t retEvent);
		void OpEnqueueKernel(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t queue, std::uint32_t flags, std::uint32_t ndRange, std::uint32_t numEvents, std::uint32_t waitEvents, std::uint32_t retEvent, std::uint32_t invoke, std::uint32_t param, std::uint32_t paramSize, std::uint32_t paramAlign, const std::vector<std::uint32_t>& localSizes);
		void OpGetKernelINDrangeSubGroupCount(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t ndRange, std::uint32_t invoke, std::uint32_t param, std::uint32_t paramSize, std::uint32_t paramAlign);
		void OpGetKernelINDrangeMaxSubGroupSize(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t ndRange, std::uint32_t invoke, std::uint32_t param, std::uint32_t paramSize, std::uint32_t paramAlign);
		void OpGetKernelWorkGroupSize(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t invoke, std::uint32_t param, std::uint32_t paramSize, std::uint32_t paramAlign);
		void OpGetKernelPreferredWorkGroupSizeMultiple(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t invoke, std::uint32_t param, std::uint32_t paramSize, std::uint32_t paramAlign);
		void OpRetainEvent(std::uint32_t event);
		void OpReleaseEvent(std::uint32_t event);
		void OpCreateUserEvent(std::uint32_t resultType, std::uint32_t resultID);
		void OpIsValidEvent(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t event);
		void OpSetUserEventStatus(std::uint32_t event, std::uint32_t status);
		void OpCaptureEventProfilingInfo(std::uint32_t event, std::uint32_t profilingInfo, std::uint32_t value);
		void OpGetDefaultQueue(std::uint32_t resultType, std::uint32_t resultID);
		void OpBuildNDRange(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t globalWorkSize, std::uint32_t localWorkSize, std::uint32_t globalWorkOffset);
		void OpGetKernelLocalSizeForSubgroupCount(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t subgroupCount, std::uint32_t invoke, std::uint32_t param, std::uint32_t paramSize, std::uint32_t paramAlign);
		void OpGetKernelMaxNumSubgroups(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t invoke, std::uint32_t param, std::uint32_t paramSize, std::uint32_t paramAlign);

		void OpReadPipe(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pipe, std::uint32_t pointer, std::uint32_t packetSize, std::uint32_t packetAlignment);
		void OpWritePipe(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pipe, std::uint32_t pointer, std::uint32_t packetSize, std::uint32_t packetAlignment);
		void OpReservedReadPipe(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pipe, std::uint32_t reserveID, std::uint32_t index, std::uint32_t pointer, std::uint32_t packetSize, std::uint32_t packetAlignment);
		void OpReservedWritePipe(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pipe, std::uint32_t reserveID, std::uint32_t index, std::uint32_t pointer, std::uint32_t packetSize, std::uint32_t packetAlignment);
		void OpReserveReadPipePackets(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pipe, std::uint32_t numPackets, std::uint32_t packetSize, std::uint32_t packetAlignment);
		void OpReserveWritePipePackets(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pipe, std::uint32_t numPackets, std::uint32_t packetSize, std::uint32_t packetAlignment);
		void OpCommitReadPipe(std::uint32_t pipe, std::uint32_t reserveID, std::uint32_t packetSize, std::uint32_t packetAlignment);
		void OpCommitWritePipe(std::uint32_t pipe, std::uint32_t reserveID, std::uint32_t packetSize, std::uint32_t packetAlignment);
		void OpIsValidReserveId(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t reserveID);
		void OpGetNumPipePackets(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pipe, std::uint32_t packetSize, std::uint32_t packetAlignment);
		void OpGetMaxPipePackets(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pipe, std::uint32_t packetSize, std::uint32_t packetAlignment);
		void OpGroupReserveReadPipePackets(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t pipe, std::uint32_t numPackets, std::uint32_t packetSize, std::uint32_t packetAlignment);
		void OpGroupReserveWritePipePackets(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t pipe, std::uint32_t numPackets, std::uint32_t packetSize, std::uint32_t packetAlignment);
		void OpGroupCommitReadPipe(std::uint32_t execution, std::uint32_t pipe, std::uint32_t reserveID, std::uint32_t packetSize, std::uint32_t packetAlignment);
		void OpGroupCommitWritePipe(std::uint32_t execution, std::uint32_t pipe, std::uint32_t reserveID, std::uint32_t packetSize, std::uint32_t packetAlignment);
		void OpConstantPipeStorage(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t packetSize, std::uint32_t packetAlignment, std::uint32_t capacity);
		void OpCreatePipeFromPipeStorage(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pipeStorage);
		void OpReadPipeBlockingINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t packetSize, std::uint32_t packetAlignment);
		void OpWritePipeBlockingINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t packetSize, std::uint32_t packetAlignment);

		void OpGroupNonUniformElect(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution);
		void OpGroupNonUniformAll(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t predicate);
		void OpGroupNonUniformAny(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t predicate);
		void OpGroupNonUniformAllEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t value);
		void OpGroupNonUniformBroadcast(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t value, std::uint32_t id);
		void OpGroupNonUniformBroadcastFirst(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t value);
		void OpGroupNonUniformBallot(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t value);
		void OpGroupNonUniformInveseBallot(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t value);
		void OpGroupNonUniformBallotBitExtract(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t value, std::uint32_t index);
		void OpGroupNonUniformBallotBitCount(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t value);
		void OpGroupNonUniformBallotFindLSB(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t value);
		void OpGroupNonUniformBallotFindMSB(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t value);
		void OpGroupNonUniformShuffle(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t value, std::uint32_t id);
		void OpGroupNonUniformShuffleXor(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t value, std::uint32_t mask);
		void OpGroupNonUniformShuffleUp(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t value, std::uint32_t delta);
		void OpGroupNonUniformShuffleDown(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t value, std::uint32_t delta);
		void OpGroupNonUniformIAdd(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t value, std::uint32_t clusterSize);
		void OpGroupNonUniformFAdd(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t value, std::uint32_t clusterSize);
		void OpGroupNonUniformIMul(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t value, std::uint32_t clusterSize);
		void OpGroupNonUniformFMul(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t value, std::uint32_t clusterSize);
		void OpGroupNonUniformSMin(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t value, std::uint32_t clusterSize);
		void OpGroupNonUniformUMin(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t value, std::uint32_t clusterSize);
		void OpGroupNonUniformFMin(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t value, std::uint32_t clusterSize);
		void OpGroupNonUniformSMax(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t value, std::uint32_t clusterSize);
		void OpGroupNonUniformUMax(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t value, std::uint32_t clusterSize);
		void OpGroupNonUniformFMax(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t value, std::uint32_t clusterSize);
		void OpGroupNonUniformBitwiseAnd(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t value, std::uint32_t clusterSize);
		void OpGroupNonUniformBitwiseOr(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t value, std::uint32_t clusterSize);
		void OpGroupNonUniformBitwiseXor(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t value, std::uint32_t clusterSize);
		void OpGroupNonUniformLogicalAnd(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t value, std::uint32_t clusterSize);
		void OpGroupNonUniformLogicalOr(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t value, std::uint32_t clusterSize);
		void OpGroupNonUniformLogicalXor(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t value, std::uint32_t clusterSize);
		void OpGroupNonUniformQuadBroadcast(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t value, std::uint32_t index);
		void OpGroupNonUniformQuadSwap(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t value, std::uint32_t direction);
		void OpGroupNonUniformPartitionNV(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t value);

		void OpTraceRayKHR(std::uint32_t accel, std::uint32_t rayFlags, std::uint32_t cullMask, std::uint32_t SBTOffset, std::uint32_t SBTStride, std::uint32_t missIndex, std::uint32_t rayOrigin, std::uint32_t rayTmin, std::uint32_t rayDirection, std::uint32_t rayTmax, std::uint32_t rayPayload);
		void OpExecuteCallableKHR(std::uint32_t SBTIndex, std::uint32_t callableData);
		void OpConvertUToAccelerationStructureKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t accel);
		void OpIgnoreIntersectionKHR();
		void OpTerminateRayKHR();
		void OpTypeRayQueryKHR(std::uint32_t resultID);
		void OpRayQueryGenerateIntersectionKHR(std::uint32_t rayQuery, std::uint32_t hit);
		void OpRayQueryConfirmIntersectionKHR(std::uint32_t rayQuery);
		void OpRayQueryProceedKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQuery);
		void OpRayQueryGetIntersectionTypeKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQuery, std::uint32_t intersection);
		void OpFragmentMaskFetchAMD(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t image, std::uint32_t coordinate);
		void OpFragmentFetchAMD(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t image, std::uint32_t coordinate, std::uint32_t fragmentIndex);
		void OpReadClockKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t scope);
		void OpWritePackedPrimitiveIndices4x8NV(std::uint32_t indexOffset, std::uint32_t packedIndices);
		void OpReportIntersectionKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t hit, std::uint32_t hitKind);
		void OpIgnoreIntersectionNV();
		void OpTerminateRayNV();
		void OpTraceNV(std::uint32_t accel, std::uint32_t rayFlags, std::uint32_t cullMask, std::uint32_t SBTOffset, std::uint32_t SBTStride, std::uint32_t missIndex, std::uint32_t rayOrigin, std::uint32_t rayTmin, std::uint32_t rayDirection, std::uint32_t rayTmax, std::uint32_t rayPayload);
		void OpTraceMotionNV(std::uint32_t accel, std::uint32_t rayFlags, std::uint32_t cullMask, std::uint32_t SBTOffset, std::uint32_t SBTStride, std::uint32_t missIndex, std::uint32_t rayOrigin, std::uint32_t rayTmin, std::uint32_t rayDirection, std::uint32_t rayTmax, std::uint32_t time, std::uint32_t rayPayload);
		void OpTraceRayMotionNV(std::uint32_t accel, std::uint32_t rayFlags, std::uint32_t cullMask, std::uint32_t SBTOffset, std::uint32_t SBTStride, std::uint32_t missIndex, std::uint32_t rayOrigin, std::uint32_t rayTmin, std::uint32_t rayDirection, std::uint32_t rayTmax, std::uint32_t time, std::uint32_t rayPayload);
		void OpTypeAccelerationStructureKHR(std::uint32_t resultID);
		void OpExecuteCallableNV(std::uint32_t SBTIndex, std::uint32_t callableDataID);
		void OpTypeCooperativeMatrixNV(std::uint32_t resultID, std::uint32_t componentType, std::uint32_t execution, std::uint32_t rows, std::uint32_t columns);
		void OpCooperativeMatrixLoadNV(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t stride, std::uint32_t columnMajor, EMemoryOperands memoryOperands);
		void OpCooperativeMatrixStoreNV(std::uint32_t pointer, std::uint32_t object, std::uint32_t stride, std::uint32_t columnMajor, EMemoryOperands memoryOperands);
		void OpCooperativeMatrixMulAddNV(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t a, std::uint32_t b, std::uint32_t c);
		void OpCooperativeMatrixLengthNV(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t type);
		void OpBeginInvocationInterlockEXT();
		void OpEndInvocationInterlockEXT();
		void OpIsHelperInvocationEXT(std::uint32_t resultType, std::uint32_t resultID);
		void OpConvertUToImageNV(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand);
		void OpConvertUToSamplerNV(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand);
		void OpConvertImageToUNV(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand);
		void OpConvertSamplerToUNV(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand);
		void OpConvertUToSampledImageNV(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand);
		void OpConvertSampledImageToUNV(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand);
		void OpSamplerImageAddressingModeNV(std::uint32_t bitWidth);
		void OpUCountLeadingZerosINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand);
		void OpUCountTrailingZerosINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand);
		void OpAbsISubINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand1, std::uint32_t operand2);
		void OpAbsUSubINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand1, std::uint32_t operand2);
		void OpIAddSatINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand1, std::uint32_t operand2);
		void OpUAddSatINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand1, std::uint32_t operand2);
		void OpIAverageINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand1, std::uint32_t operand2);
		void OpUAverageINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand1, std::uint32_t operand2);
		void OpIAverageRoundedINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand1, std::uint32_t operand2);
		void OpUAverageRoundedINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand1, std::uint32_t operand2);
		void OpISubSatINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand1, std::uint32_t operand2);
		void OpUSubSatINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand1, std::uint32_t operand2);
		void OpIMul32x16INTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand1, std::uint32_t operand2);
		void OpUMul32x16INTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand1, std::uint32_t operand2);
		void OpLoopControlINTEL(const std::vector<std::uint32_t>& loopControlParameters);
		void OpFPGARegINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t result, std::uint32_t input);
		void OpRayQueryGetRayTMinKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQeuery);
		void OpRayQueryGetRayFlagsKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQeuery);
		void OpRayQueryGetIntersectionKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQeuery, std::uint32_t intersection);
		void OpRayQueryGetIntersectionInstanceCustomIndexKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQeuery, std::uint32_t intersection);
		void OpRayQueryGetIntersectionInstanceIdKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQeuery, std::uint32_t intersection);
		void OpRayQueryGetIntersectionInstanceShaderBindingTableRecordOffsetKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQeuery, std::uint32_t intersection);
		void OpRayQueryGetIntersectionGeometryIndexKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQeuery, std::uint32_t intersection);
		void OpRayQueryGetIntersectionPrimitiveIndexKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQeuery, std::uint32_t intersection);
		void OpRayQueryGetIntersectionBarycentricsKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQeuery, std::uint32_t intersection);
		void OpRayQueryGetIntersectionFrontFaceKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQeuery, std::uint32_t intersection);
		void OpRayQueryGetIntersectionCandidateAABBOpaqueKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQeuery);
		void OpRayQueryGetIntersectionObjectRayDirectionKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQeuery, std::uint32_t intersection);
		void OpRayQueryGetIntersectionObjectRayOriginKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQeuery, std::uint32_t intersection);
		void OpRayQueryGetWorldRayDirectionKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQeuery);
		void OpRayQueryGetWorldRayOriginKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQeuery);
		void OpRayQueryGetIntersectionObjectToWorldKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQeuery, std::uint32_t intersection);
		void OpRayQueryGetIntersectionWorldToObjectKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQeuery, std::uint32_t intersection);

		auto begin() { return m_Buffer.begin(); }
		auto end() { return m_Buffer.end(); }
		auto begin() const { return m_Buffer.begin(); }
		auto end() const { return m_Buffer.end(); }
		auto cbegin() const { return m_Buffer.cbegin(); }
		auto cend() const { return m_Buffer.cend(); }
		auto rbegin() { return m_Buffer.rbegin(); }
		auto rend() { return m_Buffer.rend(); }
		auto rbegin() const { return m_Buffer.rbegin(); }
		auto rend() const { return m_Buffer.rend(); }
		auto crbegin() const { return m_Buffer.crbegin(); }
		auto crend() const { return m_Buffer.crend(); }

		std::vector<std::uint32_t> get() { return std::move(m_Buffer); }
		auto&                      getCapabilities() const { return m_Capabilities; }

	private:
		void          pushLiteralString(std::string_view name);
		std::uint64_t literalStringLength(std::string_view name);

		void requireCapability(ECapability capability) { m_Capabilities.insert(capability); }

	private:
		std::vector<std::uint32_t> m_Buffer;
		std::set<ECapability>      m_Capabilities;
	};
} // namespace Frertex::Transpilers::SPIRV