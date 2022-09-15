#pragma once

#include "Enumerations.h"
#include "Frertex/Utils/Buffer.h"

#include <cstdint>

#include <optional>
#include <set>
#include <string_view>
#include <vector>

namespace Frertex::Transpilers::SPIRV
{
	struct CodeBuffer
	{
	public:
		void OpNop(); // 0
		void OpUndef(std::uint32_t resultTypeID, std::uint32_t resultID); // 1
		void OpSourceContinued(std::string_view continuedSource); // 2
		void OpSource(ESourceLanguage sourceLanguage, std::uint32_t version, std::uint32_t fileID = 0, std::string_view source = {}); // 3
		void OpSourceExtension(std::string_view extension); // 4
		void OpName(std::uint32_t targetID, std::string_view name); // 5
		void OpMemberName(std::uint32_t typeID, std::uint32_t member, std::string_view name); // 6
		void OpString(std::uint32_t resultID, std::string_view string); // 7
		void OpLine(std::uint32_t fileID, std::uint32_t line, std::uint32_t column); // 8
		void OpExtension(std::string_view name); // 10
		void OpExtInstImport(std::uint32_t resultID, std::string_view name); // 11
		void OpExtInst(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t setID, std::uint32_t instruction, const std::vector<std::uint32_t>& operandsID = {}); // 12
		void OpMemoryModel(EAddressingModel addressingModel, EMemoryModel memoryModel); // 14
		void OpEntryPoint(EExecutionModel executionModel, std::uint32_t entryPointID, std::string_view name, const std::vector<std::uint32_t>& interfacesID = {}); // 15
		void OpExecutionMode(std::uint32_t entryPointID, EExecutionMode mode); // 16
		void OpCapability(ECapability capability); // 17
		void OpTypeVoid(std::uint32_t resultID); // 19
		void OpTypeBool(std::uint32_t resultID); // 20
		void OpTypeInt(std::uint32_t resultID, std::uint32_t width, std::uint32_t signedness); // 21
		void OpTypeFloat(std::uint32_t resultID, std::uint32_t width); // 22
		void OpTypeVector(std::uint32_t resultID, std::uint32_t componentTypeID, std::uint32_t componentCount); // 23
		void OpTypeMatrix(std::uint32_t resultID, std::uint32_t columnTypeID, std::uint32_t columnCount); // 24
		void OpTypeImage(std::uint32_t resultID, std::uint32_t sampledTypeID, EDim dim, std::uint32_t depth, std::uint32_t arrayed, std::uint32_t mS, std::uint32_t sampled, EImageFormat imageFormat, std::optional<EAccessQualifier> accessQualifier = {}); // 25
		void OpTypeSampler(std::uint32_t resultID); // 26
		void OpTypeSampledImage(std::uint32_t resultID, std::uint32_t imageTypeID); // 27
		void OpTypeArray(std::uint32_t resultID, std::uint32_t elementTypeID, std::uint32_t lengthID); // 28
		void OpTypeRuntimeArray(std::uint32_t resultID, std::uint32_t elementTypeID); // 29
		void OpTypeStruct(std::uint32_t resultID, const std::vector<std::uint32_t>& membersID = {}); // 30
		void OpTypeOpaque(std::uint32_t resultID, std::string_view henameoftheopaquetype); // 31
		void OpTypePointer(std::uint32_t resultID, EStorageClass storageClass, std::uint32_t typeID); // 32
		void OpTypeFunction(std::uint32_t resultID, std::uint32_t returnTypeID, const std::vector<std::uint32_t>& parametersID = {}); // 33
		void OpTypeEvent(std::uint32_t resultID); // 34
		void OpTypeDeviceEvent(std::uint32_t resultID); // 35
		void OpTypeReserveId(std::uint32_t resultID); // 36
		void OpTypeQueue(std::uint32_t resultID); // 37
		void OpTypePipe(std::uint32_t resultID, EAccessQualifier qualifier); // 38
		void OpTypeForwardPointer(std::uint32_t pointerTypeID, EStorageClass storageClass); // 39
		void OpConstantTrue(std::uint32_t resultTypeID, std::uint32_t resultID); // 41
		void OpConstantFalse(std::uint32_t resultTypeID, std::uint32_t resultID); // 42
		void OpConstant(std::uint32_t resultTypeID, std::uint32_t resultID, const std::vector<std::uint32_t>& value); // 43
		void OpConstantComposite(std::uint32_t resultTypeID, std::uint32_t resultID, const std::vector<std::uint32_t>& constituentsID = {}); // 44
		void OpConstantSampler(std::uint32_t resultTypeID, std::uint32_t resultID, ESamplerAddressingMode samplerAddressingMode, std::uint32_t param, ESamplerFilterMode samplerFilterMode); // 45
		void OpConstantNull(std::uint32_t resultTypeID, std::uint32_t resultID); // 46
		void OpSpecConstantTrue(std::uint32_t resultTypeID, std::uint32_t resultID); // 48
		void OpSpecConstantFalse(std::uint32_t resultTypeID, std::uint32_t resultID); // 49
		void OpSpecConstant(std::uint32_t resultTypeID, std::uint32_t resultID, const std::vector<std::uint32_t>& value); // 50
		void OpSpecConstantComposite(std::uint32_t resultTypeID, std::uint32_t resultID, const std::vector<std::uint32_t>& constituentsID = {}); // 51
		void OpSpecConstantOp(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t opcode); // 52
		void OpFunction(std::uint32_t resultTypeID, std::uint32_t resultID, EFunctionControl functionControl, std::uint32_t functionTypeID); // 54
		void OpFunctionParameter(std::uint32_t resultTypeID, std::uint32_t resultID); // 55
		void OpFunctionEnd(); // 56
		void OpFunctionCall(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t functionID, const std::vector<std::uint32_t>& argumentsID = {}); // 57
		void OpVariable(std::uint32_t resultTypeID, std::uint32_t resultID, EStorageClass storageClass, std::uint32_t initializerID = 0); // 59
		void OpImageTexelPointer(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageID, std::uint32_t coordinateID, std::uint32_t sampleID); // 60
		void OpLoad(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, EMemoryAccess memoryAccess = EMemoryAccess::None); // 61
		void OpStore(std::uint32_t pointerID, std::uint32_t objectID, EMemoryAccess memoryAccess = EMemoryAccess::None); // 62
		void OpCopyMemory(std::uint32_t targetID, std::uint32_t sourceID, EMemoryAccess memoryAccess = EMemoryAccess::None, EMemoryAccess memoryAccess0 = EMemoryAccess::None); // 63
		void OpCopyMemorySized(std::uint32_t targetID, std::uint32_t sourceID, std::uint32_t sizeID, EMemoryAccess memoryAccess = EMemoryAccess::None, EMemoryAccess memoryAccess0 = EMemoryAccess::None); // 64
		void OpAccessChain(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t baseID, const std::vector<std::uint32_t>& indexesID = {}); // 65
		void OpInBoundsAccessChain(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t baseID, const std::vector<std::uint32_t>& indexesID = {}); // 66
		void OpPtrAccessChain(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t baseID, std::uint32_t elementID, const std::vector<std::uint32_t>& indexesID = {}); // 67
		void OpArrayLength(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t structureID, std::uint32_t arraymember); // 68
		void OpGenericPtrMemSemantics(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID); // 69
		void OpInBoundsPtrAccessChain(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t baseID, std::uint32_t elementID, const std::vector<std::uint32_t>& indexesID = {}); // 70
		void OpDecorate(std::uint32_t targetID, EDecoration decoration); // 71
		void OpMemberDecorate(std::uint32_t structureTypeID, std::uint32_t member, EDecoration decoration); // 72
		void OpDecorationGroup(std::uint32_t resultID); // 73
		void OpGroupDecorate(std::uint32_t decorationGroupID, const std::vector<std::uint32_t>& targetsID = {}); // 74
		void OpGroupMemberDecorate(std::uint32_t decorationGroupID, const std::vector<std::pair<std::uint32_t, std::uint32_t>>& targets = {}); // 75
		void OpVectorExtractDynamic(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t vectorID, std::uint32_t indexID); // 77
		void OpVectorInsertDynamic(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t vectorID, std::uint32_t componentID, std::uint32_t indexID); // 78
		void OpVectorShuffle(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t vector1ID, std::uint32_t vector2ID, const std::vector<std::uint32_t>& components = {}); // 79
		void OpCompositeConstruct(std::uint32_t resultTypeID, std::uint32_t resultID, const std::vector<std::uint32_t>& constituentsID = {}); // 80
		void OpCompositeExtract(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t compositeID, const std::vector<std::uint32_t>& indexes = {}); // 81
		void OpCompositeInsert(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t objectID, std::uint32_t compositeID, const std::vector<std::uint32_t>& indexes = {}); // 82
		void OpCopyObject(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operandID); // 83
		void OpTranspose(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t matrixID); // 84
		void OpSampledImage(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageID, std::uint32_t samplerID); // 86
		void OpImageSampleImplicitLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, EImageOperands imageOperands = EImageOperands::None); // 87
		void OpImageSampleExplicitLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, EImageOperands imageOperands); // 88
		void OpImageSampleDrefImplicitLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, std::uint32_t drefID, EImageOperands imageOperands = EImageOperands::None); // 89
		void OpImageSampleDrefExplicitLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, std::uint32_t drefID, EImageOperands imageOperands); // 90
		void OpImageSampleProjImplicitLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, EImageOperands imageOperands = EImageOperands::None); // 91
		void OpImageSampleProjExplicitLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, EImageOperands imageOperands); // 92
		void OpImageSampleProjDrefImplicitLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, std::uint32_t drefID, EImageOperands imageOperands = EImageOperands::None); // 93
		void OpImageSampleProjDrefExplicitLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, std::uint32_t drefID, EImageOperands imageOperands); // 94
		void OpImageFetch(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageID, std::uint32_t coordinateID, EImageOperands imageOperands = EImageOperands::None); // 95
		void OpImageGather(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, std::uint32_t componentID, EImageOperands imageOperands = EImageOperands::None); // 96
		void OpImageDrefGather(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, std::uint32_t drefID, EImageOperands imageOperands = EImageOperands::None); // 97
		void OpImageRead(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageID, std::uint32_t coordinateID, EImageOperands imageOperands = EImageOperands::None); // 98
		void OpImageWrite(std::uint32_t imageID, std::uint32_t coordinateID, std::uint32_t texelID, EImageOperands imageOperands = EImageOperands::None); // 99
		void OpImage(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID); // 100
		void OpImageQueryFormat(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageID); // 101
		void OpImageQueryOrder(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageID); // 102
		void OpImageQuerySizeLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageID, std::uint32_t levelofDetailID); // 103
		void OpImageQuerySize(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageID); // 104
		void OpImageQueryLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID); // 105
		void OpImageQueryLevels(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageID); // 106
		void OpImageQuerySamples(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageID); // 107
		void OpConvertFToU(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t floatValueID); // 109
		void OpConvertFToS(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t floatValueID); // 110
		void OpConvertSToF(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t signedValueID); // 111
		void OpConvertUToF(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t unsignedValueID); // 112
		void OpUConvert(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t unsignedValueID); // 113
		void OpSConvert(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t signedValueID); // 114
		void OpFConvert(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t floatValueID); // 115
		void OpQuantizeToF16(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t valueID); // 116
		void OpConvertPtrToU(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID); // 117
		void OpSatConvertSToU(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t signedValueID); // 118
		void OpSatConvertUToS(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t unsignedValueID); // 119
		void OpConvertUToPtr(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t integerValueID); // 120
		void OpPtrCastToGeneric(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID); // 121
		void OpGenericCastToPtr(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID); // 122
		void OpGenericCastToPtrExplicit(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, EStorageClass storage); // 123
		void OpBitcast(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operandID); // 124
		void OpSNegate(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operandID); // 126
		void OpFNegate(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operandID); // 127
		void OpIAdd(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 128
		void OpFAdd(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 129
		void OpISub(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 130
		void OpFSub(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 131
		void OpIMul(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 132
		void OpFMul(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 133
		void OpUDiv(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 134
		void OpSDiv(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 135
		void OpFDiv(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 136
		void OpUMod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 137
		void OpSRem(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 138
		void OpSMod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 139
		void OpFRem(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 140
		void OpFMod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 141
		void OpVectorTimesScalar(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t vectorID, std::uint32_t scalarID); // 142
		void OpMatrixTimesScalar(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t matrixID, std::uint32_t scalarID); // 143
		void OpVectorTimesMatrix(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t vectorID, std::uint32_t matrixID); // 144
		void OpMatrixTimesVector(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t matrixID, std::uint32_t vectorID); // 145
		void OpMatrixTimesMatrix(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t leftMatrixID, std::uint32_t rightMatrixID); // 146
		void OpOuterProduct(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t vector1ID, std::uint32_t vector2ID); // 147
		void OpDot(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t vector1ID, std::uint32_t vector2ID); // 148
		void OpIAddCarry(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 149
		void OpISubBorrow(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 150
		void OpUMulExtended(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 151
		void OpSMulExtended(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 152
		void OpAny(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t vectorID); // 154
		void OpAll(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t vectorID); // 155
		void OpIsNan(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t xID); // 156
		void OpIsInf(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t xID); // 157
		void OpIsFinite(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t xID); // 158
		void OpIsNormal(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t xID); // 159
		void OpSignBitSet(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t xID); // 160
		void OpLessOrGreater(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t xID, std::uint32_t yID); // 161
		void OpOrdered(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t xID, std::uint32_t yID); // 162
		void OpUnordered(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t xID, std::uint32_t yID); // 163
		void OpLogicalEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 164
		void OpLogicalNotEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 165
		void OpLogicalOr(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 166
		void OpLogicalAnd(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 167
		void OpLogicalNot(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operandID); // 168
		void OpSelect(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t conditionID, std::uint32_t object1ID, std::uint32_t object2ID); // 169
		void OpIEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 170
		void OpINotEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 171
		void OpUGreaterThan(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 172
		void OpSGreaterThan(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 173
		void OpUGreaterThanEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 174
		void OpSGreaterThanEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 175
		void OpULessThan(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 176
		void OpSLessThan(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 177
		void OpULessThanEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 178
		void OpSLessThanEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 179
		void OpFOrdEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 180
		void OpFUnordEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 181
		void OpFOrdNotEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 182
		void OpFUnordNotEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 183
		void OpFOrdLessThan(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 184
		void OpFUnordLessThan(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 185
		void OpFOrdGreaterThan(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 186
		void OpFUnordGreaterThan(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 187
		void OpFOrdLessThanEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 188
		void OpFUnordLessThanEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 189
		void OpFOrdGreaterThanEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 190
		void OpFUnordGreaterThanEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 191
		void OpShiftRightLogical(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t baseID, std::uint32_t shiftID); // 194
		void OpShiftRightArithmetic(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t baseID, std::uint32_t shiftID); // 195
		void OpShiftLeftLogical(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t baseID, std::uint32_t shiftID); // 196
		void OpBitwiseOr(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 197
		void OpBitwiseXor(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 198
		void OpBitwiseAnd(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 199
		void OpNot(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operandID); // 200
		void OpBitFieldInsert(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t baseID, std::uint32_t insertID, std::uint32_t offsetID, std::uint32_t countID); // 201
		void OpBitFieldSExtract(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t baseID, std::uint32_t offsetID, std::uint32_t countID); // 202
		void OpBitFieldUExtract(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t baseID, std::uint32_t offsetID, std::uint32_t countID); // 203
		void OpBitReverse(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t baseID); // 204
		void OpBitCount(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t baseID); // 205
		void OpDPdx(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pID); // 207
		void OpDPdy(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pID); // 208
		void OpFwidth(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pID); // 209
		void OpDPdxFine(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pID); // 210
		void OpDPdyFine(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pID); // 211
		void OpFwidthFine(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pID); // 212
		void OpDPdxCoarse(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pID); // 213
		void OpDPdyCoarse(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pID); // 214
		void OpFwidthCoarse(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pID); // 215
		void OpEmitVertex(); // 218
		void OpEndPrimitive(); // 219
		void OpEmitStreamVertex(std::uint32_t streamID); // 220
		void OpEndStreamPrimitive(std::uint32_t streamID); // 221
		void OpControlBarrier(std::uint32_t executionID, std::uint32_t memoryID, std::uint32_t semanticsID); // 224
		void OpMemoryBarrier(std::uint32_t memoryID, std::uint32_t semanticsID); // 225
		void OpAtomicLoad(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID); // 227
		void OpAtomicStore(std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID, std::uint32_t valueID); // 228
		void OpAtomicExchange(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID, std::uint32_t valueID); // 229
		void OpAtomicCompareExchange(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t equalID, std::uint32_t unequalID, std::uint32_t valueID, std::uint32_t comparatorID); // 230
		void OpAtomicCompareExchangeWeak(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t equalID, std::uint32_t unequalID, std::uint32_t valueID, std::uint32_t comparatorID); // 231
		void OpAtomicIIncrement(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID); // 232
		void OpAtomicIDecrement(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID); // 233
		void OpAtomicIAdd(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID, std::uint32_t valueID); // 234
		void OpAtomicISub(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID, std::uint32_t valueID); // 235
		void OpAtomicSMin(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID, std::uint32_t valueID); // 236
		void OpAtomicUMin(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID, std::uint32_t valueID); // 237
		void OpAtomicSMax(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID, std::uint32_t valueID); // 238
		void OpAtomicUMax(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID, std::uint32_t valueID); // 239
		void OpAtomicAnd(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID, std::uint32_t valueID); // 240
		void OpAtomicOr(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID, std::uint32_t valueID); // 241
		void OpAtomicXor(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID, std::uint32_t valueID); // 242
		void OpPhi(std::uint32_t resultTypeID, std::uint32_t resultID, const std::vector<std::pair<std::uint32_t, std::uint32_t>>& variableParents = {}); // 245
		void OpLoopMerge(std::uint32_t mergeBlockID, std::uint32_t continueTargetID, ELoopControl loopControl); // 246
		void OpSelectionMerge(std::uint32_t mergeBlockID, ESelectionControl selectionControl); // 247
		void OpLabel(std::uint32_t resultID); // 248
		void OpBranch(std::uint32_t targetLabelID); // 249
		void OpBranchConditional(std::uint32_t conditionID, std::uint32_t trueLabelID, std::uint32_t falseLabelID, const std::vector<std::uint32_t>& branchweights = {}); // 250
		void OpSwitch(std::uint32_t selectorID, std::uint32_t defID, const std::vector<std::pair<std::uint32_t, std::uint32_t>>& targets = {}); // 251
		void OpKill(); // 252
		void OpReturn(); // 253
		void OpReturnValue(std::uint32_t valueID); // 254
		void OpUnreachable(); // 255
		void OpLifetimeStart(std::uint32_t pointerID, std::uint32_t size); // 256
		void OpLifetimeStop(std::uint32_t pointerID, std::uint32_t size); // 257
		void OpGroupAsyncCopy(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t destinationID, std::uint32_t sourceID, std::uint32_t numElementsID, std::uint32_t strideID, std::uint32_t eventID); // 259
		void OpGroupWaitEvents(std::uint32_t executionID, std::uint32_t numEventsID, std::uint32_t eventsListID); // 260
		void OpGroupAll(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t predicateID); // 261
		void OpGroupAny(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t predicateID); // 262
		void OpGroupBroadcast(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t valueID, std::uint32_t localIdID); // 263
		void OpGroupIAdd(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID); // 264
		void OpGroupFAdd(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID); // 265
		void OpGroupFMin(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID); // 266
		void OpGroupUMin(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID); // 267
		void OpGroupSMin(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID); // 268
		void OpGroupFMax(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID); // 269
		void OpGroupUMax(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID); // 270
		void OpGroupSMax(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID); // 271
		void OpReadPipe(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pipeID, std::uint32_t pointerID, std::uint32_t packetSizeID, std::uint32_t packetAlignmentID); // 274
		void OpWritePipe(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pipeID, std::uint32_t pointerID, std::uint32_t packetSizeID, std::uint32_t packetAlignmentID); // 275
		void OpReservedReadPipe(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pipeID, std::uint32_t reserveIdID, std::uint32_t indexID, std::uint32_t pointerID, std::uint32_t packetSizeID, std::uint32_t packetAlignmentID); // 276
		void OpReservedWritePipe(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pipeID, std::uint32_t reserveIdID, std::uint32_t indexID, std::uint32_t pointerID, std::uint32_t packetSizeID, std::uint32_t packetAlignmentID); // 277
		void OpReserveReadPipePackets(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pipeID, std::uint32_t numPacketsID, std::uint32_t packetSizeID, std::uint32_t packetAlignmentID); // 278
		void OpReserveWritePipePackets(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pipeID, std::uint32_t numPacketsID, std::uint32_t packetSizeID, std::uint32_t packetAlignmentID); // 279
		void OpCommitReadPipe(std::uint32_t pipeID, std::uint32_t reserveIdID, std::uint32_t packetSizeID, std::uint32_t packetAlignmentID); // 280
		void OpCommitWritePipe(std::uint32_t pipeID, std::uint32_t reserveIdID, std::uint32_t packetSizeID, std::uint32_t packetAlignmentID); // 281
		void OpIsValidReserveId(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t reserveIdID); // 282
		void OpGetNumPipePackets(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pipeID, std::uint32_t packetSizeID, std::uint32_t packetAlignmentID); // 283
		void OpGetMaxPipePackets(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pipeID, std::uint32_t packetSizeID, std::uint32_t packetAlignmentID); // 284
		void OpGroupReserveReadPipePackets(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t pipeID, std::uint32_t numPacketsID, std::uint32_t packetSizeID, std::uint32_t packetAlignmentID); // 285
		void OpGroupReserveWritePipePackets(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t pipeID, std::uint32_t numPacketsID, std::uint32_t packetSizeID, std::uint32_t packetAlignmentID); // 286
		void OpGroupCommitReadPipe(std::uint32_t executionID, std::uint32_t pipeID, std::uint32_t reserveIdID, std::uint32_t packetSizeID, std::uint32_t packetAlignmentID); // 287
		void OpGroupCommitWritePipe(std::uint32_t executionID, std::uint32_t pipeID, std::uint32_t reserveIdID, std::uint32_t packetSizeID, std::uint32_t packetAlignmentID); // 288
		void OpEnqueueMarker(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t queueID, std::uint32_t numEventsID, std::uint32_t waitEventsID, std::uint32_t retEventID); // 291
		void OpEnqueueKernel(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t queueID, std::uint32_t flagsID, std::uint32_t nDRangeID, std::uint32_t numEventsID, std::uint32_t waitEventsID, std::uint32_t retEventID, std::uint32_t invokeID, std::uint32_t paramID, std::uint32_t paramSizeID, std::uint32_t paramAlignID, const std::vector<std::uint32_t>& localSizesID = {}); // 292
		void OpGetKernelNDrangeSubGroupCount(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t nDRangeID, std::uint32_t invokeID, std::uint32_t paramID, std::uint32_t paramSizeID, std::uint32_t paramAlignID); // 293
		void OpGetKernelNDrangeMaxSubGroupSize(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t nDRangeID, std::uint32_t invokeID, std::uint32_t paramID, std::uint32_t paramSizeID, std::uint32_t paramAlignID); // 294
		void OpGetKernelWorkGroupSize(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t invokeID, std::uint32_t paramID, std::uint32_t paramSizeID, std::uint32_t paramAlignID); // 295
		void OpGetKernelPreferredWorkGroupSizeMultiple(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t invokeID, std::uint32_t paramID, std::uint32_t paramSizeID, std::uint32_t paramAlignID); // 296
		void OpRetainEvent(std::uint32_t eventID); // 297
		void OpReleaseEvent(std::uint32_t eventID); // 298
		void OpCreateUserEvent(std::uint32_t resultTypeID, std::uint32_t resultID); // 299
		void OpIsValidEvent(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t eventID); // 300
		void OpSetUserEventStatus(std::uint32_t eventID, std::uint32_t statusID); // 301
		void OpCaptureEventProfilingInfo(std::uint32_t eventID, std::uint32_t profilingInfoID, std::uint32_t valueID); // 302
		void OpGetDefaultQueue(std::uint32_t resultTypeID, std::uint32_t resultID); // 303
		void OpBuildNDRange(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t globalWorkSizeID, std::uint32_t localWorkSizeID, std::uint32_t globalWorkOffsetID); // 304
		void OpImageSparseSampleImplicitLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, EImageOperands imageOperands = EImageOperands::None); // 305
		void OpImageSparseSampleExplicitLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, EImageOperands imageOperands); // 306
		void OpImageSparseSampleDrefImplicitLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, std::uint32_t drefID, EImageOperands imageOperands = EImageOperands::None); // 307
		void OpImageSparseSampleDrefExplicitLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, std::uint32_t drefID, EImageOperands imageOperands); // 308
		void OpImageSparseSampleProjImplicitLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, EImageOperands imageOperands = EImageOperands::None); // 309
		void OpImageSparseSampleProjExplicitLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, EImageOperands imageOperands); // 310
		void OpImageSparseSampleProjDrefImplicitLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, std::uint32_t drefID, EImageOperands imageOperands = EImageOperands::None); // 311
		void OpImageSparseSampleProjDrefExplicitLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, std::uint32_t drefID, EImageOperands imageOperands); // 312
		void OpImageSparseFetch(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageID, std::uint32_t coordinateID, EImageOperands imageOperands = EImageOperands::None); // 313
		void OpImageSparseGather(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, std::uint32_t componentID, EImageOperands imageOperands = EImageOperands::None); // 314
		void OpImageSparseDrefGather(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, std::uint32_t drefID, EImageOperands imageOperands = EImageOperands::None); // 315
		void OpImageSparseTexelsResident(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t residentCodeID); // 316
		void OpNoLine(); // 317
		void OpAtomicFlagTestAndSet(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID); // 318
		void OpAtomicFlagClear(std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID); // 319
		void OpImageSparseRead(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageID, std::uint32_t coordinateID, EImageOperands imageOperands = EImageOperands::None); // 320
		void OpSizeOf(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID); // 321
		void OpTypePipeStorage(std::uint32_t resultID); // 322
		void OpConstantPipeStorage(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t packetSize, std::uint32_t packetAlignment, std::uint32_t capacity); // 323
		void OpCreatePipeFromPipeStorage(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pipeStorageID); // 324
		void OpGetKernelLocalSizeForSubgroupCount(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t subgroupCountID, std::uint32_t invokeID, std::uint32_t paramID, std::uint32_t paramSizeID, std::uint32_t paramAlignID); // 325
		void OpGetKernelMaxNumSubgroups(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t invokeID, std::uint32_t paramID, std::uint32_t paramSizeID, std::uint32_t paramAlignID); // 326
		void OpTypeNamedBarrier(std::uint32_t resultID); // 327
		void OpNamedBarrierInitialize(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t subgroupCountID); // 328
		void OpMemoryNamedBarrier(std::uint32_t namedBarrierID, std::uint32_t memoryID, std::uint32_t semanticsID); // 329
		void OpModuleProcessed(std::string_view process); // 330
		void OpExecutionModeId(std::uint32_t entryPointID, EExecutionMode mode); // 331
		void OpDecorateId(std::uint32_t targetID, EDecoration decoration); // 332
		void OpGroupNonUniformElect(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID); // 333
		void OpGroupNonUniformAll(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t predicateID); // 334
		void OpGroupNonUniformAny(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t predicateID); // 335
		void OpGroupNonUniformAllEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t valueID); // 336
		void OpGroupNonUniformBroadcast(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t valueID, std::uint32_t ID); // 337
		void OpGroupNonUniformBroadcastFirst(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t valueID); // 338
		void OpGroupNonUniformBallot(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t predicateID); // 339
		void OpGroupNonUniformInverseBallot(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t valueID); // 340
		void OpGroupNonUniformBallotBitExtract(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t valueID, std::uint32_t indexID); // 341
		void OpGroupNonUniformBallotBitCount(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t valueID); // 342
		void OpGroupNonUniformBallotFindLSB(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t valueID); // 343
		void OpGroupNonUniformBallotFindMSB(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t valueID); // 344
		void OpGroupNonUniformShuffle(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t valueID, std::uint32_t ID); // 345
		void OpGroupNonUniformShuffleXor(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t valueID, std::uint32_t maskID); // 346
		void OpGroupNonUniformShuffleUp(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t valueID, std::uint32_t deltaID); // 347
		void OpGroupNonUniformShuffleDown(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t valueID, std::uint32_t deltaID); // 348
		void OpGroupNonUniformIAdd(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t valueID, std::uint32_t clusterSizeID = 0); // 349
		void OpGroupNonUniformFAdd(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t valueID, std::uint32_t clusterSizeID = 0); // 350
		void OpGroupNonUniformIMul(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t valueID, std::uint32_t clusterSizeID = 0); // 351
		void OpGroupNonUniformFMul(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t valueID, std::uint32_t clusterSizeID = 0); // 352
		void OpGroupNonUniformSMin(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t valueID, std::uint32_t clusterSizeID = 0); // 353
		void OpGroupNonUniformUMin(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t valueID, std::uint32_t clusterSizeID = 0); // 354
		void OpGroupNonUniformFMin(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t valueID, std::uint32_t clusterSizeID = 0); // 355
		void OpGroupNonUniformSMax(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t valueID, std::uint32_t clusterSizeID = 0); // 356
		void OpGroupNonUniformUMax(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t valueID, std::uint32_t clusterSizeID = 0); // 357
		void OpGroupNonUniformFMax(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t valueID, std::uint32_t clusterSizeID = 0); // 358
		void OpGroupNonUniformBitwiseAnd(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t valueID, std::uint32_t clusterSizeID = 0); // 359
		void OpGroupNonUniformBitwiseOr(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t valueID, std::uint32_t clusterSizeID = 0); // 360
		void OpGroupNonUniformBitwiseXor(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t valueID, std::uint32_t clusterSizeID = 0); // 361
		void OpGroupNonUniformLogicalAnd(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t valueID, std::uint32_t clusterSizeID = 0); // 362
		void OpGroupNonUniformLogicalOr(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t valueID, std::uint32_t clusterSizeID = 0); // 363
		void OpGroupNonUniformLogicalXor(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t valueID, std::uint32_t clusterSizeID = 0); // 364
		void OpGroupNonUniformQuadBroadcast(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t valueID, std::uint32_t indexID); // 365
		void OpGroupNonUniformQuadSwap(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t valueID, std::uint32_t directionID); // 366
		void OpCopyLogical(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operandID); // 400
		void OpPtrEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 401
		void OpPtrNotEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 402
		void OpPtrDiff(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 403
		void OpTerminateInvocation(); // 4416
		void OpSubgroupBallotKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t predicateID); // 4421
		void OpSubgroupFirstInvocationKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t valueID); // 4422
		void OpSubgroupAllKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t predicateID); // 4428
		void OpSubgroupAnyKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t predicateID); // 4429
		void OpSubgroupAllEqualKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t predicateID); // 4430
		void OpGroupNonUniformRotateKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t valueID, std::uint32_t deltaID, std::uint32_t clusterSizeID = 0); // 4431
		void OpSubgroupReadInvocationKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t valueID, std::uint32_t indexID); // 4432
		void OpTraceRayKHR(std::uint32_t accelID, std::uint32_t rayFlagsID, std::uint32_t cullMaskID, std::uint32_t sBTOffsetID, std::uint32_t sBTStrideID, std::uint32_t missIndexID, std::uint32_t rayOriginID, std::uint32_t rayTminID, std::uint32_t rayDirectionID, std::uint32_t rayTmaxID, std::uint32_t payloadID); // 4445
		void OpExecuteCallableKHR(std::uint32_t sBTIndexID, std::uint32_t callableDataID); // 4446
		void OpConvertUToAccelerationStructureKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t accelID); // 4447
		void OpIgnoreIntersectionKHR(); // 4448
		void OpTerminateRayKHR(); // 4449
		void OpSDot(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t vector1ID, std::uint32_t vector2ID, std::optional<EPackedVectorFormat> packedVectorFormat = {}); // 4450
		void OpUDot(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t vector1ID, std::uint32_t vector2ID, std::optional<EPackedVectorFormat> packedVectorFormat = {}); // 4451
		void OpSUDot(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t vector1ID, std::uint32_t vector2ID, std::optional<EPackedVectorFormat> packedVectorFormat = {}); // 4452
		void OpSDotAccSat(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t vector1ID, std::uint32_t vector2ID, std::uint32_t accumulatorID, std::optional<EPackedVectorFormat> packedVectorFormat = {}); // 4453
		void OpUDotAccSat(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t vector1ID, std::uint32_t vector2ID, std::uint32_t accumulatorID, std::optional<EPackedVectorFormat> packedVectorFormat = {}); // 4454
		void OpSUDotAccSat(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t vector1ID, std::uint32_t vector2ID, std::uint32_t accumulatorID, std::optional<EPackedVectorFormat> packedVectorFormat = {}); // 4455
		void OpTypeRayQueryKHR(std::uint32_t resultID); // 4472
		void OpRayQueryInitializeKHR(std::uint32_t rayQueryID, std::uint32_t accelID, std::uint32_t rayFlagsID, std::uint32_t cullMaskID, std::uint32_t rayOriginID, std::uint32_t rayTMinID, std::uint32_t rayDirectionID, std::uint32_t rayTMaxID); // 4473
		void OpRayQueryTerminateKHR(std::uint32_t rayQueryID); // 4474
		void OpRayQueryGenerateIntersectionKHR(std::uint32_t rayQueryID, std::uint32_t hitTID); // 4475
		void OpRayQueryConfirmIntersectionKHR(std::uint32_t rayQueryID); // 4476
		void OpRayQueryProceedKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID); // 4477
		void OpRayQueryGetIntersectionTypeKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID, std::uint32_t intersectionID); // 4479
		void OpGroupIAddNonUniformAMD(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID); // 5000
		void OpGroupFAddNonUniformAMD(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID); // 5001
		void OpGroupFMinNonUniformAMD(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID); // 5002
		void OpGroupUMinNonUniformAMD(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID); // 5003
		void OpGroupSMinNonUniformAMD(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID); // 5004
		void OpGroupFMaxNonUniformAMD(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID); // 5005
		void OpGroupUMaxNonUniformAMD(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID); // 5006
		void OpGroupSMaxNonUniformAMD(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID); // 5007
		void OpFragmentMaskFetchAMD(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageID, std::uint32_t coordinateID); // 5011
		void OpFragmentFetchAMD(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageID, std::uint32_t coordinateID, std::uint32_t fragmentIndexID); // 5012
		void OpReadClockKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t scopeID); // 5056
		void OpImageSampleFootprintNV(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, std::uint32_t granularityID, std::uint32_t coarseID, EImageOperands imageOperands = EImageOperands::None); // 5283
		void OpEmitMeshTasksEXT(std::uint32_t groupCountXID, std::uint32_t groupCountYID, std::uint32_t groupCountZID, std::uint32_t payloadID = 0); // 5294
		void OpSetMeshOutputsEXT(std::uint32_t vertexCountID, std::uint32_t primitiveCountID); // 5295
		void OpGroupNonUniformPartitionNV(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t valueID); // 5296
		void OpWritePackedPrimitiveIndices4x8NV(std::uint32_t indexOffsetID, std::uint32_t packedIndicesID); // 5299
		void OpIgnoreIntersectionNV(); // 5335
		void OpTerminateRayNV(); // 5336
		void OpTraceNV(std::uint32_t accelID, std::uint32_t rayFlagsID, std::uint32_t cullMaskID, std::uint32_t sBTOffsetID, std::uint32_t sBTStrideID, std::uint32_t missIndexID, std::uint32_t rayOriginID, std::uint32_t rayTminID, std::uint32_t rayDirectionID, std::uint32_t rayTmaxID, std::uint32_t payloadIdID); // 5337
		void OpTraceMotionNV(std::uint32_t accelID, std::uint32_t rayFlagsID, std::uint32_t cullMaskID, std::uint32_t sBTOffsetID, std::uint32_t sBTStrideID, std::uint32_t missIndexID, std::uint32_t rayOriginID, std::uint32_t rayTminID, std::uint32_t rayDirectionID, std::uint32_t rayTmaxID, std::uint32_t timeID, std::uint32_t payloadIdID); // 5338
		void OpTraceRayMotionNV(std::uint32_t accelID, std::uint32_t rayFlagsID, std::uint32_t cullMaskID, std::uint32_t sBTOffsetID, std::uint32_t sBTStrideID, std::uint32_t missIndexID, std::uint32_t rayOriginID, std::uint32_t rayTminID, std::uint32_t rayDirectionID, std::uint32_t rayTmaxID, std::uint32_t timeID, std::uint32_t payloadID); // 5339
		void OpExecuteCallableNV(std::uint32_t sBTIndexID, std::uint32_t callableDataIdID); // 5344
		void OpTypeCooperativeMatrixNV(std::uint32_t resultID, std::uint32_t componentTypeID, std::uint32_t executionID, std::uint32_t rowsID, std::uint32_t columnsID); // 5358
		void OpCooperativeMatrixLoadNV(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t strideID, std::uint32_t columnMajorID, EMemoryAccess memoryAccess = EMemoryAccess::None); // 5359
		void OpCooperativeMatrixStoreNV(std::uint32_t pointerID, std::uint32_t objectID, std::uint32_t strideID, std::uint32_t columnMajorID, EMemoryAccess memoryAccess = EMemoryAccess::None); // 5360
		void OpCooperativeMatrixMulAddNV(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t bID, std::uint32_t cID); // 5361
		void OpCooperativeMatrixLengthNV(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t typeID); // 5362
		void OpBeginInvocationInterlockEXT(); // 5364
		void OpEndInvocationInterlockEXT(); // 5365
		void OpDemoteToHelperInvocation(); // 5380
		void OpIsHelperInvocationEXT(std::uint32_t resultTypeID, std::uint32_t resultID); // 5381
		void OpConvertUToImageNV(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operandID); // 5391
		void OpConvertUToSamplerNV(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operandID); // 5392
		void OpConvertImageToUNV(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operandID); // 5393
		void OpConvertSamplerToUNV(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operandID); // 5394
		void OpConvertUToSampledImageNV(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operandID); // 5395
		void OpConvertSampledImageToUNV(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operandID); // 5396
		void OpSamplerImageAddressingModeNV(std::uint32_t bitWidth); // 5397
		void OpSubgroupShuffleINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t dataID, std::uint32_t invocationIdID); // 5571
		void OpSubgroupShuffleDownINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t currentID, std::uint32_t nextID, std::uint32_t deltaID); // 5572
		void OpSubgroupShuffleUpINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t previousID, std::uint32_t currentID, std::uint32_t deltaID); // 5573
		void OpSubgroupShuffleXorINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t dataID, std::uint32_t valueID); // 5574
		void OpSubgroupBlockReadINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t ptrID); // 5575
		void OpSubgroupBlockWriteINTEL(std::uint32_t ptrID, std::uint32_t dataID); // 5576
		void OpSubgroupImageBlockReadINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageID, std::uint32_t coordinateID); // 5577
		void OpSubgroupImageBlockWriteINTEL(std::uint32_t imageID, std::uint32_t coordinateID, std::uint32_t dataID); // 5578
		void OpSubgroupImageMediaBlockReadINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageID, std::uint32_t coordinateID, std::uint32_t widthID, std::uint32_t heightID); // 5580
		void OpSubgroupImageMediaBlockWriteINTEL(std::uint32_t imageID, std::uint32_t coordinateID, std::uint32_t widthID, std::uint32_t heightID, std::uint32_t dataID); // 5581
		void OpUCountLeadingZerosINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operandID); // 5585
		void OpUCountTrailingZerosINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operandID); // 5586
		void OpAbsISubINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 5587
		void OpAbsUSubINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 5588
		void OpIAddSatINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 5589
		void OpUAddSatINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 5590
		void OpIAverageINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 5591
		void OpUAverageINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 5592
		void OpIAverageRoundedINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 5593
		void OpUAverageRoundedINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 5594
		void OpISubSatINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 5595
		void OpUSubSatINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 5596
		void OpIMul32x16INTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 5597
		void OpUMul32x16INTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID); // 5598
		void OpConstantFunctionPointerINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t functionID); // 5600
		void OpFunctionPointerCallINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, const std::vector<std::uint32_t>& operandsID = {}); // 5601
		void OpAsmTargetINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::string_view asmtarget); // 5609
		void OpAsmINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t asmtypeID, std::uint32_t targetID, std::string_view asminstructions, std::string_view constraints); // 5610
		void OpAsmCallINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t asmID, const std::vector<std::uint32_t>& argumentsID = {}); // 5611
		void OpAtomicFMinEXT(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID, std::uint32_t valueID); // 5614
		void OpAtomicFMaxEXT(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID, std::uint32_t valueID); // 5615
		void OpAssumeTrueKHR(std::uint32_t conditionID); // 5630
		void OpExpectKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t valueID, std::uint32_t expectedValueID); // 5631
		void OpVmeImageINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageTypeID, std::uint32_t samplerID); // 5699
		void OpTypeVmeImageINTEL(std::uint32_t resultID, std::uint32_t imageTypeID); // 5700
		void OpTypeAvcImePayloadINTEL(std::uint32_t resultID); // 5701
		void OpTypeAvcRefPayloadINTEL(std::uint32_t resultID); // 5702
		void OpTypeAvcSicPayloadINTEL(std::uint32_t resultID); // 5703
		void OpTypeAvcMcePayloadINTEL(std::uint32_t resultID); // 5704
		void OpTypeAvcMceResultINTEL(std::uint32_t resultID); // 5705
		void OpTypeAvcImeResultINTEL(std::uint32_t resultID); // 5706
		void OpTypeAvcImeResultSingleReferenceStreamoutINTEL(std::uint32_t resultID); // 5707
		void OpTypeAvcImeResultDualReferenceStreamoutINTEL(std::uint32_t resultID); // 5708
		void OpTypeAvcImeSingleReferenceStreaminINTEL(std::uint32_t resultID); // 5709
		void OpTypeAvcImeDualReferenceStreaminINTEL(std::uint32_t resultID); // 5710
		void OpTypeAvcRefResultINTEL(std::uint32_t resultID); // 5711
		void OpTypeAvcSicResultINTEL(std::uint32_t resultID); // 5712
		void OpSubgroupAvcMceGetDefaultInterBaseMultiReferencePenaltyINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sliceTypeID, std::uint32_t qpID); // 5713
		void OpSubgroupAvcMceSetInterBaseMultiReferencePenaltyINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t referenceBasePenaltyID, std::uint32_t payloadID); // 5714
		void OpSubgroupAvcMceGetDefaultInterShapePenaltyINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sliceTypeID, std::uint32_t qpID); // 5715
		void OpSubgroupAvcMceSetInterShapePenaltyINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t packedShapePenaltyID, std::uint32_t payloadID); // 5716
		void OpSubgroupAvcMceGetDefaultInterDirectionPenaltyINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sliceTypeID, std::uint32_t qpID); // 5717
		void OpSubgroupAvcMceSetInterDirectionPenaltyINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t directionCostID, std::uint32_t payloadID); // 5718
		void OpSubgroupAvcMceGetDefaultIntraLumaShapePenaltyINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sliceTypeID, std::uint32_t qpID); // 5719
		void OpSubgroupAvcMceGetDefaultInterMotionVectorCostTableINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sliceTypeID, std::uint32_t qpID); // 5720
		void OpSubgroupAvcMceGetDefaultHighPenaltyCostTableINTEL(std::uint32_t resultTypeID, std::uint32_t resultID); // 5721
		void OpSubgroupAvcMceGetDefaultMediumPenaltyCostTableINTEL(std::uint32_t resultTypeID, std::uint32_t resultID); // 5722
		void OpSubgroupAvcMceGetDefaultLowPenaltyCostTableINTEL(std::uint32_t resultTypeID, std::uint32_t resultID); // 5723
		void OpSubgroupAvcMceSetMotionVectorCostFunctionINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t packedCostCenterDeltaID, std::uint32_t packedCostTableID, std::uint32_t costPrecisionID, std::uint32_t payloadID); // 5724
		void OpSubgroupAvcMceGetDefaultIntraLumaModePenaltyINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sliceTypeID, std::uint32_t qpID); // 5725
		void OpSubgroupAvcMceGetDefaultNonDcLumaIntraPenaltyINTEL(std::uint32_t resultTypeID, std::uint32_t resultID); // 5726
		void OpSubgroupAvcMceGetDefaultIntraChromaModeBasePenaltyINTEL(std::uint32_t resultTypeID, std::uint32_t resultID); // 5727
		void OpSubgroupAvcMceSetAcOnlyHaarINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5728
		void OpSubgroupAvcMceSetSourceInterlacedFieldPolarityINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sourceFieldPolarityID, std::uint32_t payloadID); // 5729
		void OpSubgroupAvcMceSetSingleReferenceInterlacedFieldPolarityINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t referenceFieldPolarityID, std::uint32_t payloadID); // 5730
		void OpSubgroupAvcMceSetDualReferenceInterlacedFieldPolaritiesINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t forwardReferenceFieldPolarityID, std::uint32_t backwardReferenceFieldPolarityID, std::uint32_t payloadID); // 5731
		void OpSubgroupAvcMceConvertToImePayloadINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5732
		void OpSubgroupAvcMceConvertToImeResultINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5733
		void OpSubgroupAvcMceConvertToRefPayloadINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5734
		void OpSubgroupAvcMceConvertToRefResultINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5735
		void OpSubgroupAvcMceConvertToSicPayloadINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5736
		void OpSubgroupAvcMceConvertToSicResultINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5737
		void OpSubgroupAvcMceGetMotionVectorsINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5738
		void OpSubgroupAvcMceGetInterDistortionsINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5739
		void OpSubgroupAvcMceGetBestInterDistortionsINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5740
		void OpSubgroupAvcMceGetInterMajorShapeINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5741
		void OpSubgroupAvcMceGetInterMinorShapeINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5742
		void OpSubgroupAvcMceGetInterDirectionsINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5743
		void OpSubgroupAvcMceGetInterMotionVectorCountINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5744
		void OpSubgroupAvcMceGetInterReferenceIdsINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5745
		void OpSubgroupAvcMceGetInterReferenceInterlacedFieldPolaritiesINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t packedReferenceIdsID, std::uint32_t packedReferenceParameterFieldPolaritiesID, std::uint32_t payloadID); // 5746
		void OpSubgroupAvcImeInitializeINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcCoordID, std::uint32_t partitionMaskID, std::uint32_t sADAdjustmentID); // 5747
		void OpSubgroupAvcImeSetSingleReferenceINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t refOffsetID, std::uint32_t searchWindowConfigID, std::uint32_t payloadID); // 5748
		void OpSubgroupAvcImeSetDualReferenceINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t fwdRefOffsetID, std::uint32_t bwdRefOffsetID, std::uint32_t SearchWindowConfigID, std::uint32_t payloadID); // 5749
		void OpSubgroupAvcImeRefWindowSizeINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t searchWindowConfigID, std::uint32_t dualRefID); // 5750
		void OpSubgroupAvcImeAdjustRefOffsetINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t refOffsetID, std::uint32_t srcCoordID, std::uint32_t refWindowSizeID, std::uint32_t imageSizeID); // 5751
		void OpSubgroupAvcImeConvertToMcePayloadINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5752
		void OpSubgroupAvcImeSetMaxMotionVectorCountINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t maxMotionVectorCountID, std::uint32_t payloadID); // 5753
		void OpSubgroupAvcImeSetUnidirectionalMixDisableINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5754
		void OpSubgroupAvcImeSetEarlySearchTerminationThresholdINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t thresholdID, std::uint32_t payloadID); // 5755
		void OpSubgroupAvcImeSetWeightedSadINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t packedSadWeightsID, std::uint32_t payloadID); // 5756
		void OpSubgroupAvcImeEvaluateWithSingleReferenceINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcImageID, std::uint32_t refImageID, std::uint32_t payloadID); // 5757
		void OpSubgroupAvcImeEvaluateWithDualReferenceINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcImageID, std::uint32_t fwdRefImageID, std::uint32_t bwdRefImageID, std::uint32_t payloadID); // 5758
		void OpSubgroupAvcImeEvaluateWithSingleReferenceStreaminINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcImageID, std::uint32_t refImageID, std::uint32_t payloadID, std::uint32_t streaminComponentsID); // 5759
		void OpSubgroupAvcImeEvaluateWithDualReferenceStreaminINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcImageID, std::uint32_t fwdRefImageID, std::uint32_t bwdRefImageID, std::uint32_t payloadID, std::uint32_t streaminComponentsID); // 5760
		void OpSubgroupAvcImeEvaluateWithSingleReferenceStreamoutINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcImageID, std::uint32_t refImageID, std::uint32_t payloadID); // 5761
		void OpSubgroupAvcImeEvaluateWithDualReferenceStreamoutINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcImageID, std::uint32_t fwdRefImageID, std::uint32_t bwdRefImageID, std::uint32_t payloadID); // 5762
		void OpSubgroupAvcImeEvaluateWithSingleReferenceStreaminoutINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcImageID, std::uint32_t refImageID, std::uint32_t payloadID, std::uint32_t streaminComponentsID); // 5763
		void OpSubgroupAvcImeEvaluateWithDualReferenceStreaminoutINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcImageID, std::uint32_t fwdRefImageID, std::uint32_t bwdRefImageID, std::uint32_t payloadID, std::uint32_t streaminComponentsID); // 5764
		void OpSubgroupAvcImeConvertToMceResultINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5765
		void OpSubgroupAvcImeGetSingleReferenceStreaminINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5766
		void OpSubgroupAvcImeGetDualReferenceStreaminINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5767
		void OpSubgroupAvcImeStripSingleReferenceStreamoutINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5768
		void OpSubgroupAvcImeStripDualReferenceStreamoutINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5769
		void OpSubgroupAvcImeGetStreamoutSingleReferenceMajorShapeMotionVectorsINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID, std::uint32_t majorShapeID); // 5770
		void OpSubgroupAvcImeGetStreamoutSingleReferenceMajorShapeDistortionsINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID, std::uint32_t majorShapeID); // 5771
		void OpSubgroupAvcImeGetStreamoutSingleReferenceMajorShapeReferenceIdsINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID, std::uint32_t majorShapeID); // 5772
		void OpSubgroupAvcImeGetStreamoutDualReferenceMajorShapeMotionVectorsINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID, std::uint32_t majorShapeID, std::uint32_t directionID); // 5773
		void OpSubgroupAvcImeGetStreamoutDualReferenceMajorShapeDistortionsINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID, std::uint32_t majorShapeID, std::uint32_t directionID); // 5774
		void OpSubgroupAvcImeGetStreamoutDualReferenceMajorShapeReferenceIdsINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID, std::uint32_t majorShapeID, std::uint32_t directionID); // 5775
		void OpSubgroupAvcImeGetBorderReachedINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageSelectID, std::uint32_t payloadID); // 5776
		void OpSubgroupAvcImeGetTruncatedSearchIndicationINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5777
		void OpSubgroupAvcImeGetUnidirectionalEarlySearchTerminationINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5778
		void OpSubgroupAvcImeGetWeightingPatternMinimumMotionVectorINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5779
		void OpSubgroupAvcImeGetWeightingPatternMinimumDistortionINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5780
		void OpSubgroupAvcFmeInitializeINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcCoordID, std::uint32_t motionVectorsID, std::uint32_t majorShapesID, std::uint32_t minorShapesID, std::uint32_t directionID, std::uint32_t pixelResolutionID, std::uint32_t sadAdjustmentID); // 5781
		void OpSubgroupAvcBmeInitializeINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcCoordID, std::uint32_t motionVectorsID, std::uint32_t majorShapesID, std::uint32_t minorShapesID, std::uint32_t directionID, std::uint32_t pixelResolutionID, std::uint32_t bidirectionalWeightID, std::uint32_t sadAdjustmentID); // 5782
		void OpSubgroupAvcRefConvertToMcePayloadINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5783
		void OpSubgroupAvcRefSetBidirectionalMixDisableINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5784
		void OpSubgroupAvcRefSetBilinearFilterEnableINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5785
		void OpSubgroupAvcRefEvaluateWithSingleReferenceINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcImageID, std::uint32_t refImageID, std::uint32_t payloadID); // 5786
		void OpSubgroupAvcRefEvaluateWithDualReferenceINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcImageID, std::uint32_t fwdRefImageID, std::uint32_t bwdRefImageID, std::uint32_t payloadID); // 5787
		void OpSubgroupAvcRefEvaluateWithMultiReferenceINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcImageID, std::uint32_t packedReferenceIdsID, std::uint32_t payloadID); // 5788
		void OpSubgroupAvcRefEvaluateWithMultiReferenceInterlacedINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcImageID, std::uint32_t packedReferenceIdsID, std::uint32_t packedReferenceFieldPolaritiesID, std::uint32_t payloadID); // 5789
		void OpSubgroupAvcRefConvertToMceResultINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5790
		void OpSubgroupAvcSicInitializeINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcCoordID); // 5791
		void OpSubgroupAvcSicConfigureSkcINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t skipBlockPartitionTypeID, std::uint32_t skipMotionVectorMaskID, std::uint32_t motionVectorsID, std::uint32_t bidirectionalWeightID, std::uint32_t sadAdjustmentID, std::uint32_t payloadID); // 5792
		void OpSubgroupAvcSicConfigureIpeLumaINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t lumaIntraPartitionMaskID, std::uint32_t intraNeighbourAvailabiltyID, std::uint32_t leftEdgeLumaPixelsID, std::uint32_t upperLeftCornerLumaPixelID, std::uint32_t upperEdgeLumaPixelsID, std::uint32_t upperRightEdgeLumaPixelsID, std::uint32_t sadAdjustmentID, std::uint32_t payloadID); // 5793
		void OpSubgroupAvcSicConfigureIpeLumaChromaINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t lumaIntraPartitionMaskID, std::uint32_t intraNeighbourAvailabiltyID, std::uint32_t leftEdgeLumaPixelsID, std::uint32_t upperLeftCornerLumaPixelID, std::uint32_t upperEdgeLumaPixelsID, std::uint32_t upperRightEdgeLumaPixelsID, std::uint32_t leftEdgeChromaPixelsID, std::uint32_t upperLeftCornerChromaPixelID, std::uint32_t upperEdgeChromaPixelsID, std::uint32_t sadAdjustmentID, std::uint32_t payloadID); // 5794
		void OpSubgroupAvcSicGetMotionVectorMaskINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t skipBlockPartitionTypeID, std::uint32_t directionID); // 5795
		void OpSubgroupAvcSicConvertToMcePayloadINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5796
		void OpSubgroupAvcSicSetIntraLumaShapePenaltyINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t packedShapePenaltyID, std::uint32_t payloadID); // 5797
		void OpSubgroupAvcSicSetIntraLumaModeCostFunctionINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t lumaModePenaltyID, std::uint32_t lumaPackedNeighborModesID, std::uint32_t lumaPackedNonDcPenaltyID, std::uint32_t payloadID); // 5798
		void OpSubgroupAvcSicSetIntraChromaModeCostFunctionINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t chromaModeBasePenaltyID, std::uint32_t payloadID); // 5799
		void OpSubgroupAvcSicSetBilinearFilterEnableINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5800
		void OpSubgroupAvcSicSetSkcForwardTransformEnableINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t packedSadCoefficientsID, std::uint32_t payloadID); // 5801
		void OpSubgroupAvcSicSetBlockBasedRawSkipSadINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t blockBasedSkipTypeID, std::uint32_t payloadID); // 5802
		void OpSubgroupAvcSicEvaluateIpeINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcImageID, std::uint32_t payloadID); // 5803
		void OpSubgroupAvcSicEvaluateWithSingleReferenceINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcImageID, std::uint32_t refImageID, std::uint32_t payloadID); // 5804
		void OpSubgroupAvcSicEvaluateWithDualReferenceINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcImageID, std::uint32_t fwdRefImageID, std::uint32_t bwdRefImageID, std::uint32_t payloadID); // 5805
		void OpSubgroupAvcSicEvaluateWithMultiReferenceINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcImageID, std::uint32_t packedReferenceIdsID, std::uint32_t payloadID); // 5806
		void OpSubgroupAvcSicEvaluateWithMultiReferenceInterlacedINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcImageID, std::uint32_t packedReferenceIdsID, std::uint32_t packedReferenceFieldPolaritiesID, std::uint32_t payloadID); // 5807
		void OpSubgroupAvcSicConvertToMceResultINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5808
		void OpSubgroupAvcSicGetIpeLumaShapeINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5809
		void OpSubgroupAvcSicGetBestIpeLumaDistortionINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5810
		void OpSubgroupAvcSicGetBestIpeChromaDistortionINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5811
		void OpSubgroupAvcSicGetPackedIpeLumaModesINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5812
		void OpSubgroupAvcSicGetIpeChromaModeINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5813
		void OpSubgroupAvcSicGetPackedSkcLumaCountThresholdINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5814
		void OpSubgroupAvcSicGetPackedSkcLumaSumThresholdINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5815
		void OpSubgroupAvcSicGetInterRawSadsINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID); // 5816
		void OpVariableLengthArrayINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t lenghtID); // 5818
		void OpSaveMemoryINTEL(std::uint32_t resultTypeID, std::uint32_t resultID); // 5819
		void OpRestoreMemoryINTEL(std::uint32_t ptrID); // 5820
		void OpArbitraryFloatSinCosPiINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t fromSign, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5840
		void OpArbitraryFloatCastINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5841
		void OpArbitraryFloatCastFromIntINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t mout, std::uint32_t fromSign, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5842
		void OpArbitraryFloatCastToIntINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5843
		void OpArbitraryFloatAddINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t bID, std::uint32_t m2, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5846
		void OpArbitraryFloatSubINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t bID, std::uint32_t m2, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5847
		void OpArbitraryFloatMulINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t bID, std::uint32_t m2, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5848
		void OpArbitraryFloatDivINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t bID, std::uint32_t m2, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5849
		void OpArbitraryFloatGTINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t bID, std::uint32_t m2); // 5850
		void OpArbitraryFloatGEINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t bID, std::uint32_t m2); // 5851
		void OpArbitraryFloatLTINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t bID, std::uint32_t m2); // 5852
		void OpArbitraryFloatLEINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t bID, std::uint32_t m2); // 5853
		void OpArbitraryFloatEQINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t bID, std::uint32_t m2); // 5854
		void OpArbitraryFloatRecipINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5855
		void OpArbitraryFloatRSqrtINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5856
		void OpArbitraryFloatCbrtINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5857
		void OpArbitraryFloatHypotINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t bID, std::uint32_t m2, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5858
		void OpArbitraryFloatSqrtINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5859
		void OpArbitraryFloatLogINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5860
		void OpArbitraryFloatLog2INTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5861
		void OpArbitraryFloatLog10INTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5862
		void OpArbitraryFloatLog1pINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5863
		void OpArbitraryFloatExpINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5864
		void OpArbitraryFloatExp2INTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5865
		void OpArbitraryFloatExp10INTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5866
		void OpArbitraryFloatExpm1INTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5867
		void OpArbitraryFloatSinINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5868
		void OpArbitraryFloatCosINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5869
		void OpArbitraryFloatSinCosINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5870
		void OpArbitraryFloatSinPiINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5871
		void OpArbitraryFloatCosPiINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5872
		void OpArbitraryFloatASinINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5873
		void OpArbitraryFloatASinPiINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5874
		void OpArbitraryFloatACosINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5875
		void OpArbitraryFloatACosPiINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5876
		void OpArbitraryFloatATanINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5877
		void OpArbitraryFloatATanPiINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5878
		void OpArbitraryFloatATan2INTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t bID, std::uint32_t m2, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5879
		void OpArbitraryFloatPowINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t bID, std::uint32_t m2, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5880
		void OpArbitraryFloatPowRINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t bID, std::uint32_t m2, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5881
		void OpArbitraryFloatPowNINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t bID, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy); // 5882
		void OpLoopControlINTEL(const std::vector<std::uint32_t>& loopControlParameters = {}); // 5887
		void OpAliasDomainDeclINTEL(std::uint32_t resultID, std::uint32_t nameID = 0); // 5911
		void OpAliasScopeDeclINTEL(std::uint32_t resultID, std::uint32_t aliasDomainID, std::uint32_t nameID = 0); // 5912
		void OpAliasScopeListDeclINTEL(std::uint32_t resultID, const std::vector<std::uint32_t>& aliasScopesID = {}); // 5913
		void OpFixedSqrtINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t inputTypeID, std::uint32_t inputID, std::uint32_t s, std::uint32_t i, std::uint32_t rI, std::uint32_t q, std::uint32_t o); // 5923
		void OpFixedRecipINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t inputTypeID, std::uint32_t inputID, std::uint32_t s, std::uint32_t i, std::uint32_t rI, std::uint32_t q, std::uint32_t o); // 5924
		void OpFixedRsqrtINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t inputTypeID, std::uint32_t inputID, std::uint32_t s, std::uint32_t i, std::uint32_t rI, std::uint32_t q, std::uint32_t o); // 5925
		void OpFixedSinINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t inputTypeID, std::uint32_t inputID, std::uint32_t s, std::uint32_t i, std::uint32_t rI, std::uint32_t q, std::uint32_t o); // 5926
		void OpFixedCosINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t inputTypeID, std::uint32_t inputID, std::uint32_t s, std::uint32_t i, std::uint32_t rI, std::uint32_t q, std::uint32_t o); // 5927
		void OpFixedSinCosINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t inputTypeID, std::uint32_t inputID, std::uint32_t s, std::uint32_t i, std::uint32_t rI, std::uint32_t q, std::uint32_t o); // 5928
		void OpFixedSinPiINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t inputTypeID, std::uint32_t inputID, std::uint32_t s, std::uint32_t i, std::uint32_t rI, std::uint32_t q, std::uint32_t o); // 5929
		void OpFixedCosPiINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t inputTypeID, std::uint32_t inputID, std::uint32_t s, std::uint32_t i, std::uint32_t rI, std::uint32_t q, std::uint32_t o); // 5930
		void OpFixedSinCosPiINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t inputTypeID, std::uint32_t inputID, std::uint32_t s, std::uint32_t i, std::uint32_t rI, std::uint32_t q, std::uint32_t o); // 5931
		void OpFixedLogINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t inputTypeID, std::uint32_t inputID, std::uint32_t s, std::uint32_t i, std::uint32_t rI, std::uint32_t q, std::uint32_t o); // 5932
		void OpFixedExpINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t inputTypeID, std::uint32_t inputID, std::uint32_t s, std::uint32_t i, std::uint32_t rI, std::uint32_t q, std::uint32_t o); // 5933
		void OpPtrCastToCrossWorkgroupINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID); // 5934
		void OpCrossWorkgroupCastToPtrINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID); // 5938
		void OpReadPipeBlockingINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t packetSizeID, std::uint32_t packetAlignmentID); // 5946
		void OpWritePipeBlockingINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t packetSizeID, std::uint32_t packetAlignmentID); // 5947
		void OpFPGARegINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t resultID0, std::uint32_t inputID); // 5949
		void OpRayQueryGetRayTMinKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID); // 6016
		void OpRayQueryGetRayFlagsKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID); // 6017
		void OpRayQueryGetIntersectionTKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID, std::uint32_t intersectionID); // 6018
		void OpRayQueryGetIntersectionInstanceCustomIndexKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID, std::uint32_t intersectionID); // 6019
		void OpRayQueryGetIntersectionInstanceIdKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID, std::uint32_t intersectionID); // 6020
		void OpRayQueryGetIntersectionInstanceShaderBindingTableRecordOffsetKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID, std::uint32_t intersectionID); // 6021
		void OpRayQueryGetIntersectionGeometryIndexKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID, std::uint32_t intersectionID); // 6022
		void OpRayQueryGetIntersectionPrimitiveIndexKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID, std::uint32_t intersectionID); // 6023
		void OpRayQueryGetIntersectionBarycentricsKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID, std::uint32_t intersectionID); // 6024
		void OpRayQueryGetIntersectionFrontFaceKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID, std::uint32_t intersectionID); // 6025
		void OpRayQueryGetIntersectionCandidateAABBOpaqueKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID); // 6026
		void OpRayQueryGetIntersectionObjectRayDirectionKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID, std::uint32_t intersectionID); // 6027
		void OpRayQueryGetIntersectionObjectRayOriginKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID, std::uint32_t intersectionID); // 6028
		void OpRayQueryGetWorldRayDirectionKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID); // 6029
		void OpRayQueryGetWorldRayOriginKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID); // 6030
		void OpRayQueryGetIntersectionObjectToWorldKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID, std::uint32_t intersectionID); // 6031
		void OpRayQueryGetIntersectionWorldToObjectKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID, std::uint32_t intersectionID); // 6032
		void OpAtomicFAddEXT(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID, std::uint32_t valueID); // 6035
		void OpTypeBufferSurfaceINTEL(std::uint32_t resultID, EAccessQualifier accessQualifier); // 6086
		void OpTypeStructContinuedINTEL(const std::vector<std::uint32_t>& membersID = {}); // 6090
		void OpConstantCompositeContinuedINTEL(const std::vector<std::uint32_t>& constituentsID = {}); // 6091
		void OpSpecConstantCompositeContinuedINTEL(const std::vector<std::uint32_t>& constituentsID = {}); // 6092
		void OpControlBarrierArriveINTEL(std::uint32_t executionID, std::uint32_t memoryID, std::uint32_t semanticsID); // 6142
		void OpControlBarrierWaitINTEL(std::uint32_t executionID, std::uint32_t memoryID, std::uint32_t semanticsID); // 6143
		void OpGroupIMulKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID); // 6401
		void OpGroupFMulKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID); // 6402
		void OpGroupBitwiseAndKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID); // 6403
		void OpGroupBitwiseOrKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID); // 6404
		void OpGroupBitwiseXorKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID); // 6405
		void OpGroupLogicalAndKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID); // 6406
		void OpGroupLogicalOrKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID); // 6407
		void OpGroupLogicalXorKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID); // 6408

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
		auto& getCapabilities() const { return m_Capabilities; }
		auto& getExtensions() const { return m_Extensions; }

	private:
		void          pushLiteralString(std::string_view name);
		std::uint64_t literalStringLength(std::string_view name);

		void requireCapability(ECapability capability) { m_Capabilities.insert(capability); }
		void requireExtension(std::string_view extension) { m_Extensions.emplace_back(std::string { extension }); }

	private:
		std::vector<std::uint32_t> m_Buffer;
		std::set<ECapability>      m_Capabilities;
		std::set<std::string>      m_Extensions;
	};
} // namespace Frertex::Transpilers::SPIRV